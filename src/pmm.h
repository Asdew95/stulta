#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define INVALID_PADDRESS 0xffffffff

void pmm_init();

uint32_t pmm_page_alloc(void);
void pmm_page_free(uint32_t addr);

#endif
