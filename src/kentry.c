#include "gdt.h"
#include "idt.h"
#include "interrupt.h"
#include <multiboot.h>
#include "pic.h"
#include "pmm.h"
#include "stulta.h"
#include "vmm.h"

struct kernel_info kernel;

void kentry(uint32_t kpstart, uint32_t kvstart, uint32_t kpend, uint32_t kvend,
        struct multiboot_info *mb_info)
{
    char *fb = (char*) 0xb8000;
    fb[0] = 'A';

    kernel.mb_info = mb_info;
    kernel.kpstart = kpstart;
    kernel.kvstart = kvstart;
    kernel.kpend = kpend;
    kernel.kvend = kvend;

    gdt_init();
    idt_init();
    pic_init();
    pmm_init();
    vmm_init();
    interrupts_enable();
}
