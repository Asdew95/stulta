global gdt
global gdt_load

gdt:
    dw 0
    dd 0

gdt_load:
    lgdt [gdt]
    jmp 0x08:.new_cs
    .new_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
