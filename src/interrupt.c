#include "debug.h"
#include "interrupt.h"
#include "io.h"
#include "pic.h"
#include <stdint.h>
#include "stulta.h"
#include "task.h"

void interrupt(struct cpu_state cpu, uint32_t interrupt,
        struct stack_state stack)
{
    int is_irq = interrupt >= 0x20 && interrupt <= 0x2f ? 1 : 0;
    uint8_t irq = interrupt - 0x20;

    if (is_irq) {
        pic_eoi(irq);

        if (irq == 0) { // PIT
            if (kernel.task != NULL) {
                kernel.task->cpu.eax = cpu.eax;
                kernel.task->cpu.ebx = cpu.ebx;
                kernel.task->cpu.ecx = cpu.ecx;
                kernel.task->cpu.edx = cpu.edx;
                kernel.task->cpu.edi = cpu.edi;
                kernel.task->cpu.esi = cpu.esi;
                kernel.task->cpu.ebp = cpu.ebp;
                kernel.task->cpu.esp = cpu.esp;
                kernel.task->cpu.eflags = stack.eflags;
                kernel.task->cpu.eip = stack.eip;

                if (stack.cs == 0x08) {
                    kernel.task->ring = 0;
                } else {
                    kernel.task->ring = 3;
                }

                task_switch(kernel.task->next);
            }
        } else if (irq == 1) { // Keyboard
            char *fb = (char*) 0xf00b8000;
            fb[0] = inb(0x60);
        }
    } else {
        dbg_print("INT: ");
        dbg_print_uint32(interrupt);
        dbg_print("\n");
    }
}

void noint_start(void)
{
    __asm__("cli");
    kernel.int_disable++;
}

void noint_end(void)
{
    kernel.int_disable--;

    if (kernel.int_disable == 0) {
        __asm__("sti");
    }
}
