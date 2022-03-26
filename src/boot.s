global asentry

extern kentry
extern kernel_pd
extern paging_enable
extern paging_set_cr3

extern kpstart
extern kvstart
extern kpend
extern kvend

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
    ; Enable PSE
    mov eax, cr4
    or eax, 0x00000010
    mov cr4, eax

    mov esp, kernel_stack + KERNEL_STACK_SIZE - 0xf0000000 ; Lower-half stack
    push ebx

    ; Map 4 MiB from 0xf0000000 and 0x00000000 to 0x00000000
    mov eax, 0x00000083
    mov ebx, kernel_pd - 0xf0000000
    mov [ebx], eax
    add ebx, 3840
    mov [ebx], eax

    push kernel_pd - 0xf0000000
    call paging_set_cr3
    lea eax, [.higher_half]
    add esp, 4
    push eax
    jmp paging_enable

    .higher_half:
    add esp, 0xf0000000 ; Higher-half stack
    push kvend
    push kpend
    push kvstart
    push kpstart
    call kentry
    .loop:
    jmp .loop
