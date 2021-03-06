#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "vmm.h"

struct task {
    union pde *pd;
    uint32_t ppd;

    struct cpu {
        uint32_t esp;
        uint32_t eip;
        uint32_t eflags;
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;
    } __attribute__((packed)) cpu;
    uint32_t kernel_stack;

    int ring;

    struct task *next;
} __attribute__((packed));

struct tss {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap;
} __attribute__((packed));

extern struct tss tss;

void task_init(void);

struct task *task_new(void *code, size_t csize);
void task_switch(struct task *task);

#endif
