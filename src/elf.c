#include "debug.h"
#include "elf.h"
#include "libk/liballoc.h"
#include "libk/util.h"
#include <stddef.h>
#include "stulta.h"
#include "task.h"
#include "vmm.h"

struct task *elf_create_task(void *buf, size_t size)
{
    struct elf_header *elf = (struct elf_header*) buf;
    
    // Sanity checks
    
    if (elf->ei_magic != ELF_MAGIC) {
        dbg_print("Wrong ELF magic! Got: ");
        dbg_print_uint32(elf->ei_magic);
        dbg_print("\n");
        return NULL;
    }

    if (elf->ei_class != 1) {
        dbg_print("Wrong ELF ei_class! Got: ");
        dbg_print_uint32(elf->ei_class);
        dbg_print("\n");
        return NULL;
    }

    if (elf->ei_data != 1) {
        dbg_print("Wrong ELF ei_data! Got: ");
        dbg_print_uint32(elf->ei_data);
        dbg_print("\n");
        return NULL;
    }

    if (elf->ei_version != 1) {
        dbg_print("Wrong ELF version! Got ei_version: ");
        dbg_print_uint32(elf->ei_version);
        dbg_print("\n");
        return NULL;
    }

    if (elf->e_type != 2) {
        dbg_print("Not an ELF executable! Got e_type: ");
        dbg_print_uint32(elf->e_type);
        dbg_print("\n");
        return NULL;
    }

    if (elf->e_machine != 3) {
        dbg_print("Wrong ELF architecture! Got: ");
        dbg_print_uint32(elf->e_machine);
        dbg_print("\n");
        return NULL;
    }

    if (elf->e_version != 1) {
        dbg_print("Wrong ELF version! Got e_version: ");
        dbg_print_uint32(elf->e_version);
        dbg_print("\n");
        return NULL;
    }

    if (elf->e_entry == 0) {
        dbg_print("Invalid entrypoint!\n");
        return NULL;
    }

    // Sanity checks done

    struct task *task = malloc(sizeof(struct task));

    task->cpu.eax = 0;
    task->cpu.ebx = 0;
    task->cpu.ecx = 0;
    task->cpu.edx = 0;
    task->cpu.ebp = 0;
    task->cpu.esi = 0;
    task->cpu.edi = 0;
    task->cpu.eflags = 0x202;
    task->cpu.esp = 0; // Will be set in task_switch if 0

    task->pd = vmm_alloc_pages(kernel.pd, 0, 1, 0);
    task->ppd = vmm_vtop(NULL, task->pd);

    task->ring = 3;

    memset(task->pd, 0, 4096);
    memcpy((void*) ((uint32_t) task->pd + 4 * 960),
            (void*) ((uint32_t) kernel.pd + 4 * 960), 64 * 4);

    task->kernel_stack = (uint32_t) vmm_alloc_pages(kernel.pd, 0, 1, 0)
        + 0x1000;

    struct elf_pheader *ph = (struct elf_pheader*) ((uint32_t) elf
            + elf->e_phoff);

    for (int i = 0; i < elf->e_phnum; i++) {
        if (ph[i].p_type == 1) {
            dbg_print("Program header\n");
            dbg_print("Type: ");
            dbg_print_uint32(ph[i].p_type);
            dbg_print("\n");
            dbg_print("Virtual address: ");
            dbg_print_uint32(ph[i].p_vaddr);
            dbg_print("\n");
            dbg_print("p_memsz: ");
            dbg_print_uint32(ph[i].p_memsz);
            dbg_print("\n");
            dbg_print("p_filesz: ");
            dbg_print_uint32(ph[i].p_filesz);
            dbg_print("\n");

            void *p = vmm_alloc_pages(kernel.pd, 0, (ph[i].p_memsz + 4095)
                    / 4096, 1);
            memcpy(p, (void*) ((uint32_t) elf + ph[i].p_offset),
                    ph[i].p_filesz);
            memset(p + ph[i].p_filesz, 0, ph[i].p_memsz - ph[i].p_filesz);
            vmm_copy_mapping(task->pd, p, ph[i].p_vaddr / 4096,
                    (ph[i].p_memsz + 4095) / 4096, 1, 1);
            vmm_unmap(p, (ph[i].p_memsz + 4095) / 4096, 0);
        }
    }

    task->cpu.eip = elf->e_entry;

    if (kernel.task == NULL) {
        kernel.task = task;
        task->next = task;
    } else {
        task->next = kernel.task->next;
        kernel.task->next = task;
    }

    return task;
}
