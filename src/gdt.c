#include "gdt.h"
#include <stdint.h>

struct gdt_entry gdt_entries[3];

void gdt_init(void)
{
    gdt_entries[0] = create_gdt_desc(0, 0, 0, 0);
    gdt_entries[1] = create_gdt_desc(0, 0xfffff, 0x9a, 0xcf);
    gdt_entries[2] = create_gdt_desc(0, 0xfffff, 0x92, 0xcf);

    gdt = (struct gdt) { .address = (uint32_t) gdt_entries, .size = 3 * 8 - 1};
    gdt_load();
}

struct gdt_entry create_gdt_desc(uint32_t base, uint32_t limit, uint8_t access,
    uint8_t granularity)
{
    struct gdt_entry gdt;

    gdt.limit_low = (uint16_t) (limit & 0xffff);
    gdt.base_low = (uint16_t) (base & 0xffff);
    gdt.base_middle = (uint8_t) ((base >> 16) & 0xff);
    gdt.access = access;
    gdt.granularity =
        (uint8_t) ((granularity & 0xf0) | ((limit >> 16) & 0x0f));
    gdt.base_high = (uint8_t) ((base >> 24) & 0xff);

    return gdt;
}
