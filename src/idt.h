#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ATTR_PRESENT 0b10000000
#define IDT_ATTR_DPL_0 0b00100000
#define IDT_ATTR_DPL_3 0b01100000
#define IDT_ATTR_INT_GATE 0b00001110
#define IDT_ATTR_TRAP_GATE 0b00001111

struct idt {
    uint16_t size;
    uint32_t address;
} __attribute__((packed));

struct idt_entry {
    uint16_t offset_low;
    uint16_t cs;
    uint8_t unused;
    uint8_t attr;
    uint16_t offset_high;
} __attribute__((packed));

extern struct idt idt;
extern struct idt_entry idt_entries[256];

void idt_init(void);

void idt_set_gate(uint32_t i, uint32_t handler, uint16_t cs, uint8_t attr);

void idt_load(void);

#endif
