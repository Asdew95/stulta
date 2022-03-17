global idt
global idt_load

idt:
    dw 0
    dd 0

idt_load:
    lidt [idt]
    ret
