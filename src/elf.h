#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>
#include "task.h"

#define ELF_MAGIC 0x464c457f

struct elf_header {
    uint32_t ei_magic;
    uint8_t ei_class;
    uint8_t ei_data;
    uint8_t ei_version;
    uint8_t unused[9];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} __attribute__((packed));

struct elf_pheader {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} __attribute__((packed));

struct task *elf_create_task(void *buf, size_t size);

#endif
