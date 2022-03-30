#include "interrupt.h"
#include "libk/liballoc.h"
#include "libk/util.h"
#include <multiboot.h>
#include "pmm.h"
#include "stulta.h"
#include "task.h"
#include "vmm.h"

struct tss tss;

extern void flush_tss(void);
extern void jump_ring3(struct task task);
extern void jump_r0_task(struct task *task);

void task_init(void)
{
    // Zero out the TSS
    memset(&tss, 0, sizeof(tss));

    tss.ss0 = 0x0010;
    flush_tss();

    if (kernel.mb_info->flags & MULTIBOOT_INFO_MODS) {
        multiboot_module_t *modules
            = (multiboot_module_t*) kernel.mb_info->mods_addr;

        for (int i = 0; i < (int) kernel.mb_info->mods_count; i++) {
            multiboot_module_t module = modules[i];
            // Allocate space for the module
            void *modm = vmm_alloc_pages(kernel.pd,
                (module.mod_end - module.mod_start + 4095) / 4096, 0);
            memcpy(modm, (void*) module.mod_start,
                    (module.mod_end - module.mod_start));

            task_new(modm, (module.mod_end - module.mod_start
                        + 4095) / 4096);
        }

        task_switch(kernel.task);
    }
}

// csize is the code size in pages
struct task *task_new(void *code, size_t csize)
{
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

    task->pd = vmm_alloc_pages(kernel.pd, 1, 0);
    task->ppd = vmm_vtop(NULL, task->pd);

    task->ring = 3;

    memset(task->pd, 0, 4096);
    memcpy((void*) ((uint32_t) task->pd + 4 * 960),
            (void*) ((uint32_t) kernel.pd + 4 * 960), 64 * 4);

    task->cpu.eip = (uint32_t) vmm_copy_mapping(task->pd, code, csize, 1, 1);

    task->kernel_stack = (uint32_t) vmm_alloc_pages(kernel.pd, 1, 0) + 0x1000;

    if (kernel.task == NULL) {
        kernel.task = task;
        task->next = task;
    } else {
        task->next = kernel.task->next;
        kernel.task->next = task;
    }

    return task;
}

void task_switch(struct task *task)
{
    noint_start();
    kernel.ppd = task->ppd;
    kernel.pd = task->pd;
    kernel.pd[1023].small.addr = (uint32_t) kernel.ppd >> 12;

    paging_set_cr3(kernel.ppd);
    tss.esp0 = task->kernel_stack;

    if (task->cpu.esp == 0) {
        task->cpu.esp = (uint32_t) vmm_alloc_pages(kernel.pd, 1, 1) + 0x1000;
        memset((void*) (task->cpu.esp - 0x1000), 0, 4096);
    }

    kernel.task = task;

    kernel.int_disable = 0;

    if (task->ring == 3) {
        jump_ring3(*task);
    } else {
        jump_r0_task(task);
    }
}
