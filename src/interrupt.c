#include "interrupt.h"
#include "io.h"
#include "pic.h"
#include <stdint.h>

void interrupt(struct cpu_state cpu, uint32_t interrupt,
        struct stack_state stack)
{
    int is_irq = interrupt >= 0x20 && interrupt <= 0x2f ? 1 : 0;
    uint8_t irq = interrupt - 0x20;
    if (is_irq) {
        pic_eoi(irq);

        if (irq == 1) { // Keyboard
            char *fb = (char*) 0xb8000;
            fb[0] = inb(0x60);
        }
    }


}
