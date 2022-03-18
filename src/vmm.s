global paging_enable
global paging_set_cr3

paging_enable:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

paging_set_cr3:
    mov eax, [esp + 4]
    mov cr3, eax
    ret
