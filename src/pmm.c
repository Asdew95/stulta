#include "debug.h"
#include <multiboot.h>
#include "pmm.h"
#include <stdint.h>
#include "stulta.h"

static uint32_t align_ptr_up(uint32_t ptr);
static uint32_t align_ptr_down(uint32_t ptr);
static int is_page_kreserved(uint32_t address);

void pmm_init()
{
    dbg_print("Memory map:\n");

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)
        kernel.mb_info->mmap_addr;
    int len = kernel.mb_info->mmap_length / sizeof(multiboot_memory_map_t);

    // Pages of 4096 bytes
    uint32_t total_memory = 0;
    uint32_t free_memory = 0;

    // Debug print memory map and find amount of memory
    for (int i = 0; i < len; i++) {
        dbg_print_uint32((uint32_t) mmap[i].addr);
        dbg_print("-");
        uint32_t end = (uint32_t) (mmap[i].addr + mmap[i].len);
        dbg_print_uint32(end > mmap[i].addr ? end : 0xffffffff);
        dbg_print(": ");
        switch (mmap[i].type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                dbg_print("available");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                dbg_print("reserved");
                break;
            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                dbg_print("ACPI reclaimable");
                break;
            case MULTIBOOT_MEMORY_NVS:
                dbg_print("NVS");
                break;
            case MULTIBOOT_MEMORY_BADRAM:
                dbg_print("bad RAM");
                break;
            default:
                dbg_print("unknown (unusable)");
                break;
        }
        dbg_print("\n");

        uint32_t page_start = align_ptr_up(mmap[i].addr);
        uint32_t page_end = align_ptr_down(mmap[i].addr + mmap[i].len);
        total_memory += (page_end - page_start) / 4096;

        if (mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            for(uint32_t j = page_start; j < page_end; j += 4096) {
                if (!is_page_kreserved(j)) {
                    free_memory++;
                }
            }
        }
    }

    kernel.memory.total_memory = total_memory;
    kernel.memory.usable_memory = free_memory;

    // Find enough space for memory stack
    for (int i = 0; i < len; i++) {
        uint32_t page_start = align_ptr_up(mmap[i].addr);
        uint32_t page_end = align_ptr_down(mmap[i].addr + mmap[i].len);

        if (mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t current_block_size = 0;
            uint32_t current_block = page_start;
            for(uint32_t j = page_start; j < page_end; j += 4096) {
                if (!is_page_kreserved(j)) {
                    current_block_size++;
                    if (current_block_size * 4096 >= free_memory * 4) {
                        kernel.memory.free_pages = (uint32_t*) (current_block
                            + 0xf0000000); // TODO: map virtual memory
                        goto stack_found;
                    }
                } else {
                    current_block_size = 0;
                    current_block = j + 4096;
                }
            }
        }
    }
stack_found:

    // Add free pages to page stack
    for (int i = 0; i < len; i++) {
        uint32_t page_start = align_ptr_up(mmap[i].addr);
        uint32_t page_end = align_ptr_down(mmap[i].addr + mmap[i].len);

        if (mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            for(uint32_t j = page_start; j < page_end; j += 4096) {
                if (!is_page_kreserved(j)
                        && (j < (uint32_t) kernel.memory.free_pages
                            || j > (uint32_t) kernel.memory.free_pages
                            + free_memory * 4)) {
                    kernel.memory.free_pages[kernel.memory.free_memory++] = j;
                }
            }
        }
    }

    dbg_print("\n");
    dbg_print("Total memory (in pages): ");
    dbg_print_uint32(kernel.memory.total_memory);
    dbg_print("\nTotal usable memory (in pages): ");
    dbg_print_uint32(kernel.memory.usable_memory);
    dbg_print("\nFree memory (in pages): ");
    dbg_print_uint32(kernel.memory.free_memory);
    dbg_print("\n");
}

static uint32_t align_ptr_up(uint32_t ptr)
{
    return ((ptr + (4096 - 1)) / 4096) * 4096;
}

static uint32_t align_ptr_down(uint32_t ptr)
{
    return (ptr - (ptr % 4096));
}

static int is_page_kreserved(uint32_t address)
{
    address = align_ptr_down(address);
    if (address >= align_ptr_down(kernel.kpstart) && address <= kernel.kpend) {
        return 1;
    }

    if (address >= align_ptr_down((uint32_t) kernel.mb_info)
            && address <= (uint32_t) kernel.mb_info
                + sizeof(multiboot_info_t)) {
        return 1;
    }

    if (address >= align_ptr_down(kernel.mb_info->mmap_addr)
            && address <= kernel.mb_info->mmap_addr
                + kernel.mb_info->mmap_length) {
        return 1;
    }

    return 0;
}

uint32_t pmm_page_alloc(void)
{
    if (kernel.memory.free_memory == 0) {
        return INVALID_PADDRESS;
    }

    return kernel.memory.free_pages[--kernel.memory.free_memory];
}

void pmm_page_free(uint32_t addr)
{
    if (addr == INVALID_PADDRESS) {
        return;
    }

    kernel.memory.free_pages[kernel.memory.free_memory++] =
        align_ptr_down(addr);
}
