#include "idt.h"
#include "interrupt.h"

#define IDT_GATE_DEFAULT(i) idt_set_gate(i, (uint32_t) interrupt_handler_ ## i, 0x08, IDT_ATTR_PRESENT | IDT_ATTR_DPL_0 | IDT_ATTR_TRAP_GATE);

struct idt_entry idt_entries[256];

void idt_init(void)
{
    IDT_GATE_DEFAULT(0)
    IDT_GATE_DEFAULT(1)
    IDT_GATE_DEFAULT(2)
    IDT_GATE_DEFAULT(3)
    IDT_GATE_DEFAULT(4)
    IDT_GATE_DEFAULT(5)
    IDT_GATE_DEFAULT(6)
    IDT_GATE_DEFAULT(6)
    IDT_GATE_DEFAULT(7)
    IDT_GATE_DEFAULT(8)
    IDT_GATE_DEFAULT(9)
    IDT_GATE_DEFAULT(10)
    IDT_GATE_DEFAULT(11)
    IDT_GATE_DEFAULT(12)
    IDT_GATE_DEFAULT(13)
    IDT_GATE_DEFAULT(14)
    IDT_GATE_DEFAULT(15)
    IDT_GATE_DEFAULT(16)
    IDT_GATE_DEFAULT(17)
    IDT_GATE_DEFAULT(18)
    IDT_GATE_DEFAULT(32)
    IDT_GATE_DEFAULT(33)
    IDT_GATE_DEFAULT(34)
    IDT_GATE_DEFAULT(35)
    IDT_GATE_DEFAULT(36)
    IDT_GATE_DEFAULT(37)
    IDT_GATE_DEFAULT(38)
    IDT_GATE_DEFAULT(39)
    IDT_GATE_DEFAULT(40)
    IDT_GATE_DEFAULT(41)
    IDT_GATE_DEFAULT(42)
    IDT_GATE_DEFAULT(43)
    IDT_GATE_DEFAULT(44)
    IDT_GATE_DEFAULT(45)
    IDT_GATE_DEFAULT(46)
    IDT_GATE_DEFAULT(47)
    idt_set_gate(0x80, (uint32_t) interrupt_handler_128, 0x08, IDT_ATTR_PRESENT | IDT_ATTR_DPL_3 | IDT_ATTR_TRAP_GATE);

    idt.address = (uint32_t) idt_entries;
    idt.size = 8 * 0x81 - 1;
    idt_load();
}

void idt_set_gate(uint32_t i, uint32_t handler, uint16_t cs, uint8_t attr)
{
    idt_entries[i].offset_low = (uint16_t) (handler & 0x0000ffff);
    idt_entries[i].cs = cs;
    idt_entries[i].unused = 0;
    idt_entries[i].attr = attr;
    idt_entries[i].offset_high = (uint16_t) ((handler >> 16) & 0x0000ffff);
}
