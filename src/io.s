global outb
global inb

global io_wait

outb:
    mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret

inb:
    mov dx, [esp + 4]
    in al, dx
    ret

io_wait:
    mov dx, 0x80
    mov al, 0
    out dx, al
    ret
