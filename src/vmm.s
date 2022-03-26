global paging_enable
global paging_set_cr3
global flush_tlb
global invlpg

paging_enable:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

paging_set_cr3:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

flush_tlb:
    mov eax, cr3
    mov cr3, eax
    ret

invlpg:
    mov eax, [esp + 4]
    invlpg [eax]
    ret
