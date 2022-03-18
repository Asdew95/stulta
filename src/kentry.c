#include "gdt.h"
#include "idt.h"
#include "interrupt.h"
#include "pic.h"
#include "vmm.h"

void kentry(void)
{
    char *fb = (char*) 0xb8000;
    fb[0] = 'A';

    gdt_init();
    idt_init();
    pic_init();
    interrupts_enable();
    vmm_init();
}
