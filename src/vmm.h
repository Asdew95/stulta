#ifndef VMM_H
#define VMM_H

#include <stddef.h>
#include <stdint.h>

union pde {
    struct {
        int present : 1;
        int readwrite : 1;
        int supervisor : 1;
        int writethrough : 1;
        int cache_disable : 1;
        int accessed : 1;
        int dirty : 1;
        int pagesize : 1;
        int global : 1;
        int available1 : 3;
        int pat : 1;
        int addr_low : 8;
        int reserved : 1;
        int addr_high : 10;
    } __attribute__((packed)) huge;

    struct {
        int present : 1;
        int readwrite : 1;
        int supervisor : 1;
        int writethrough : 1;
        int cache_disable : 1;
        int accessed : 1;
        int available1 : 1;
        int pagesize : 1;
        int available2 : 4;
        int addr : 20;
    } __attribute__((packed)) small;
};

struct pte {
    int present : 1;
    int readwrite : 1;
    int supervisor : 1;
    int writethrough : 1;
    int cache_disable : 1;
    int accessed : 1;
    int dirty : 1;
    int pat : 1;
    int global : 1;
    int available1 : 3;
    int addr : 20;
} __attribute__((packed));

extern union pde kernel_pd[1024];

void vmm_init(void);

void *vmm_alloc_pages(union pde *pd, uint32_t start, size_t pages, int user);
void vmm_unmap(void *ptr, size_t pages, int free);

void *vmm_copy_mapping(union pde *tgt, void *ptr, uint32_t start, size_t pages,
        int deep, int user);

uint32_t vmm_vtop(union pde *pd, void *addr);

extern void paging_set_cr3(uint32_t cr3);
extern void paging_enable(void);
extern void flush_tlb(void);
extern void invlpg(uint32_t addr);

#endif
