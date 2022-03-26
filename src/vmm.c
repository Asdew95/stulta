#include "debug.h"
#include "libk/util.h"
#include "pmm.h"
#include <stddef.h>
#include "vmm.h"

union pde kernel_pd[1024] __attribute__((aligned(4096)));

void vmm_init(void)
{
    // Recursive paging
    kernel_pd[1023].small.addr = ((uint32_t) kernel_pd - 0xf0000000) >> 12;
    kernel_pd[1023].small.present = 1;
}

void *vmm_alloc_pages(size_t pages)
{
    // Find pages amount of pages not mapped to anything
    size_t cursize = 0;
    // curstart is the page number of the currently checked free block of pages
    uint32_t curstart = 0;
    size_t rpages = pages;

    for (int i = 0; i < 1024; i++) {
        union pde *pde = &kernel_pd[i];

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
                            goto end;
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
                goto end;
            }
        }
    }

    return NULL;

end:
    for (int i = 0; (size_t) i < cursize / 1024 + cursize % 1024 > 0 ? 1 : 0;
            i++) {
        union pde *pde = &kernel_pd[i + curstart / 1024];
        struct pte *pt =
            (struct pte*) (0xffc00000 + (i + curstart / 1024) * 4096);

        if (!pde->small.present) {
            uint32_t paddr = pmm_page_alloc();
            if (paddr == INVALID_PADDRESS) {
                // Out of memory
                return NULL;
            }

            pde->small.addr = paddr >> 12;
            pde->small.present = 1;

            // Zero out the memory
            memset(pt, 0, 4096);
        }

        for (int j = (i == 0) ? curstart % 1024 : 0;
                j < 1024 && rpages > 0; j++, rpages--) {
            uint32_t paddr = pmm_page_alloc();
            if (paddr == INVALID_PADDRESS) {
                // Out of memory
                return NULL;
            }

            pt[j].addr = paddr >> 12;
            pt[j].present = 1;
        }
    }

    return (void*) (curstart * 4096);
}

void vmm_free_pages(void *ptr, size_t pages)
{
    uint32_t page = (uint32_t) ptr / 4096;
    size_t rpages = pages;
    for (int i = 0; (size_t) i < page / 1024 + pages % 1024 > 0 ? 1 : 0; i++) {
        struct pte *pt = (struct pte*) (0xffc00000 + (i + page / 1024) * 4096);

        for (int j = (i == 0) ? page % 1024 : 0; j < 1024 && rpages > 0;
                j++, rpages--) {
            pt[j].present = 0;
            pmm_page_free(pt[j].addr << 12);
        }
    }
}
