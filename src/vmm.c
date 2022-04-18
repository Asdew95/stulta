#include "interrupt.h"
#include "libk/util.h"
#include "pmm.h"
#include <stddef.h>
#include "stulta.h"
#include "vmm.h"

union pde kernel_pd[1024] __attribute__((aligned(4096)));

static uint32_t find_vspace(union pde *pd, size_t pages, int user,
        size_t *size);
static void change_recursive_pd(uint32_t pd);

void vmm_init(void)
{
    // Recursive paging
    kernel_pd[1023].small.addr = ((uint32_t) kernel_pd - 0xf0000000) >> 12;
    kernel_pd[1023].small.present = 1;

    // Create page tables for kernel (and leave two page tables for recursive
    // paging)
    for (int i = 961; i < 1022; i++) {
        uint32_t pt = pmm_page_alloc();

        kernel_pd[i].small.addr = pt >> 12;
        kernel_pd[i].small.present = 1;
        void *vpt = (void*) (0xffc00000 + i * 4096);
        memset(vpt, 0, 4096);
    }

    flush_tlb();

    // Change the first 4 MiB of the higher-half kernel to use 4-KiB pages
    struct pte *pt0 = vmm_alloc_pages(kernel.pd, 0, 1, 0);
    memset(pt0, 0, 4096);
    for (int i = 0; i < 1024; i++) {
        pt0[i].addr = (i * 4096) >> 12;
        pt0[i].supervisor = 0;
        pt0[i].present = 1;
    }

    union pde pde;
    pde.small.addr = vmm_vtop(NULL, pt0) >> 12;
    pde.small.present = 1;
    kernel.pd[960] = pde;

    flush_tlb();
}

static uint32_t find_vspace(union pde *pd, size_t pages, int user,
        size_t *size)
{
    // Find pages amount of pages not mapped to anything
    size_t cursize = 0;
    // curstart is the page number of the currently checked free block of pages
    uint32_t curstart = 0;

    change_recursive_pd(vmm_vtop(NULL, pd));

    for (int i = user ? 1 : 960; i < (user ? 960 : 1022); i++) {
        union pde *pde = &pd[i];

        if (pde->small.present) {
            if (pde->huge.pagesize) {
                cursize = 0;
            } else {
                struct pte *pt = (struct pte*) (0xffc00000 + i * 4096);
                for (int j = 0; j < 1024; j++) {
                    if (pt[j].present) {
                        cursize = 0;
                    } else {
                        if (cursize == 0) {
                            curstart = i * 1024 + j;
                        }

                        cursize++;

                        if (cursize >= pages) {
                            if (size) {
                                *size = cursize;
                            }

                            return curstart;
                        }
                    }
                }
            }
        } else {
            if (cursize == 0) {
                curstart = i * 1024;
            }

            cursize += 1024;

            if (cursize >= pages) {
                if (size) {
                    *size = cursize;
                }

                return curstart;
            }
        }
    }

    if (size) {
        *size = 0;
    }

    return 0;
}

void *vmm_copy_mapping(union pde *tgt, void *ptr, uint32_t start, size_t pages,
        int deep, int user)
{
    noint_start();

    size_t size = pages;

    if (deep) {
        if (start == 0) {
            start = find_vspace(tgt, pages, user, &size);
        }

        if (size == 0) {
            noint_end();

            return NULL;
        }

    } else if (start == 0) {
        size = 0;

        for (int i = (user ? 1 : 960); i < (user ? 960 : 1022); i++) {
            if (!tgt[i].huge.present) {
                if (size == 0) {
                    start = i * 1024;
                }

                size += 1024;

                if (size >= pages) {
                    break;
                }
            }
        }
    }

    if (size == 0) {
        noint_end();

        return NULL;
    }

    change_recursive_pd(vmm_vtop(NULL, tgt));

    uint32_t srcpage = (uint32_t) ptr / 4096;
    for (int i = 0; (size_t) i < size / 1024 + ((pages % 1024) > 0) ? 1 : 0; i++)
    {
        if (deep) {
            struct pte *tgtpt =
                (struct pte*) (0xff800000 + (i + start / 1024) * 4096);
            union pde *pde = &tgt[i + start / 1024];

            if (!pde->small.present) {
                uint32_t paddr = pmm_page_alloc();
                if (paddr == INVALID_PADDRESS) {
                    // Out of memory
                    noint_end();

                    return NULL;
                }

                pde->small.addr = paddr >> 12;
                pde->small.present = 1;
                pde->small.readwrite = user;
                invlpg((uint32_t) tgtpt);

                // Zero out the memory
                memset(tgtpt, 0, 4096);
            }

            pde->small.supervisor = user;

            for (int j = (i == 0) ? start % 1024 : 0; j < 1024 && pages > 0;
                    j++, pages--) {
                struct pte *srcpte =
                    (struct pte*) (0xffc00000 + (srcpage * 4));
                tgtpt[j] = *srcpte;
                tgtpt[j].supervisor = user;
                tgtpt[j].readwrite = user;
                srcpage++;
            }
        } else {
            union pde *srcpde = &kernel.pd[i + (uint32_t) ptr / (4096 * 1024)];
            union pde *tgtpde = &tgt[i + start / 1024];

            *tgtpde = *srcpde;
            tgtpde->small.supervisor = user;
            tgtpde->small.readwrite = user;
        }
    }

    noint_end();

    return (void*) (start * 4096);
}

// start is assumed to be valid if start != 0
void *vmm_alloc_pages(union pde *pd, uint32_t start, size_t pages, int user)
{
    noint_start();

    size_t rpages = pages;
    size_t size = rpages;

    if (start == 0) {
        start = find_vspace(pd, pages, user, &size);
    }

    if (size == 0) {
        noint_end();
        return NULL;
    }

    change_recursive_pd(vmm_vtop(NULL, pd));

    for (int i = 0; rpages > 0; i++) {
        union pde *pde = &pd[i + start / 1024];
        struct pte *pt =
            (struct pte*) (0xff800000 + (i + start / 1024) * 4096);

        if (!pde->small.present) {
            uint32_t paddr = pmm_page_alloc();
            if (paddr == INVALID_PADDRESS) {
                // Out of memory
                noint_end();

                return NULL;
            }

            pde->small.addr = paddr >> 12;
            pde->small.supervisor = user;
            pde->small.present = 1;
            pde->small.readwrite = user;

            // Zero out the memory
            memset(pt, 0, 4096);
        }

        for (int j = (i == 0) ? start % 1024 : 0;
                j < 1024 && rpages > 0; j++, rpages--) {
            uint32_t paddr = pmm_page_alloc();
            if (paddr == INVALID_PADDRESS) {
                // Out of memory
                noint_end();

                return NULL;
            }

            pt[j].addr = paddr >> 12;
            pt[j].supervisor = user;
            pt[j].readwrite = user;
            pt[j].present = 1;
            invlpg((i + start / 1024) * 4096 * 1024 + j * 4096);
        }
    }

    noint_end();

    return (void*) (start * 4096);
}

void vmm_unmap(void *ptr, size_t pages, int free)
{
    noint_start();

    uint32_t page = (uint32_t) ptr / 4096;
    size_t rpages = pages;
    for (int i = 0; (size_t) i < page / 1024 + pages % 1024 > 0 ? 1 : 0; i++) {
        struct pte *pt = (struct pte*) (0xffc00000 + (i + page / 1024) * 4096);

        for (int j = (i == 0) ? page % 1024 : 0; j < 1024 && rpages > 0;
                j++, rpages--) {
            pt[j].present = 0;
            invlpg((i + page / 1024) * 4096 * 1024 + j * 4096);
            if (free) {
                pmm_page_free(pt[j].addr << 12);
            }
        }
    }

    noint_end();
}

uint32_t vmm_vtop(union pde *pd, void *addr)
{
    uint32_t page = (uint32_t) addr / 4096;
    struct pte *pt;
    pd = pd == NULL ? kernel.pd : pd;
    union pde *pde = &pd[page / 1024];

    if (!pde->small.present) {
        return INVALID_PADDRESS;
    }

    if (pd == kernel.pd) {
        if (pde->huge.pagesize) {
            return (pde->huge.addr_high << 22) | (pde->huge.addr_low << 12)
                + (uint32_t) addr % (1024 * 4096);
        } else {
            pt = (struct pte*) (0xffc00000 + (page / 1024) * 4096);
        }
    } else {
        change_recursive_pd(vmm_vtop(NULL, pd));
        pt = (struct pte*) (0xff800000 + (page / 1024) * 4096);
    }

    uint32_t ret = (pt[page % 1024].addr << 12) + (uint32_t) addr % 4096;

    return ret;
}

static void change_recursive_pd(uint32_t pd)
{
    if (kernel.pd[1022].small.addr == pd >> 12) {
        return;
    }

    kernel.pd[1022].small.addr = pd >> 12;
    kernel.pd[1022].small.pagesize = 0;
    kernel.pd[1022].small.present = 1;
    for (int i = 0; i < 1024; i++) {
        invlpg(0xff800000 + 4096 * i);
    }
}
