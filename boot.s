global asentry

extern kentry

MAGIC_NUMBER equ 0x1badb002
FLAGS equ 0
CHECKSUM equ -MAGIC_NUMBER

section .text:
align 4

dd MAGIC_NUMBER
dd FLAGS
dd CHECKSUM

asentry:
    mov al, '!'
    mov [0xb8000], al
    call kentry
    .loop:
    jmp .loop
