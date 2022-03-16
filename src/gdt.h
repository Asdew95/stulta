#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct gdt {
    uint16_t size;
    uint32_t address;
} __attribute__((packed));

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

extern struct gdt gdt;

extern struct gdt_entry gdt_entries[3];

void gdt_init(void);

struct gdt_entry create_gdt_desc(uint32_t base, uint32_t limit, uint8_t access,
        uint8_t granularity);

extern void gdt_load();

#endif
