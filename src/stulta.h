#ifndef STULTA_H
#define STULTA_H

#include <multiboot.h>
#include <stddef.h>
#include <stdint.h>

extern struct kernel_info {
    multiboot_info_t *mb_info;
    struct {
        // total_memory, usable_memory, and free_memory are in 4096-byte pages
        uint32_t total_memory;
        uint32_t usable_memory;
        uint32_t free_memory;
        // free_pages is a stack of the base addresses of free 4096-byte pages
        uint32_t *free_pages;
    } memory;

    uint32_t kpstart;
    uint32_t kvstart;
    uint32_t kpend;
    uint32_t kvend;
} kernel;

#endif
