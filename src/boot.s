global asentry

extern kentry

MAGIC_NUMBER equ 0x1badb002
FLAGS equ 0
CHECKSUM equ -MAGIC_NUMBER

KERNEL_STACK_SIZE equ 4096

section .bss
align 4

kernel_stack:
resb KERNEL_STACK_SIZE

section .text
align 4

dd MAGIC_NUMBER
dd FLAGS
dd CHECKSUM

asentry:
    xchg bx, bx
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    mov al, '!'
    mov [0xb8000], al
    call kentry
    .loop:
    jmp .loop
