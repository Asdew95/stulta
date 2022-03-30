global flush_tss
global jump_ring3
global jump_r0_task

flush_tss:
    mov ax, 0x28
    ltr ax
    ret

jump_ring3:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23
    mov eax, [esp + 16]
    push eax
    mov eax, [esp + 28]
    push eax
    push 0x1b
    mov eax, [esp + 32]
    push eax
    add esp, 44
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop ebp
    pop esi
    pop edi
    sub esp, 72
    iret

jump_r0_task:
    mov ebx, [esp + 4]
    add ebx, 8
    mov esp, [ebx]

    mov eax, [ebx + 8]
    push eax
    push 0x08
    mov eax, [ebx + 4]
    push eax

    mov esp, ebx
    add esp, 12
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop ebp
    pop esi
    pop edi

    sub esp, 40
    mov esp, [esp]
    sub esp, 12
    iret
