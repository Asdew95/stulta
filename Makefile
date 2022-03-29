SRC_FILES = boot.s gdt.c gdt.s idt.c idt.s interrupt.c interrupt.s io.s \
			kentry.c libk/liballoc.c libk/util.c pic.c pmm.c task.c task.s \
			vmm.c vmm.s
DEPS = $(addprefix build/, $(addsuffix .d, $(SRC_FILES)))
OBJ = $(addprefix build/, $(addsuffix .o, $(SRC_FILES)))
SRC = $(addprefix src/, $(SRC_FILES))
KERNEL_ELF = build/iso/boot/stulta.elf
OUT = build/stulta.iso

AS = nasm
CC = clang
LD = ld.lld
ASFLAGS = -f elf32 -Werror
CFLAGS = -c -ffreestanding -fno-builtin -nostdlib -mno-red-zone -Wall -Wextra \
		 -Werror -Wno-unused-parameter -Iinclude/ -std=c99 \
		 --target=i686-pc-none-elf -march=i686
LDFLAGS = -T link.ld -melf_i386

ifdef DEBUG
	CFLAGS += -DDEBUG=$(DEBUG)
endif

.PHONY: clean run

all: $(OUT)

$(OUT): $(KERNEL_ELF) build/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/iso/

build/iso/boot/grub/grub.cfg: grub.cfg
	@mkdir -p build/iso/boot/grub
	cp grub.cfg build/iso/boot/grub/

$(KERNEL_ELF): $(OBJ) link.ld
	@mkdir -p build/iso/boot
	$(LD) $(LDFLAGS) $(OBJ) -o $@

-include $(DEPS)

build/%.s.o: src/%.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -MD -MP -MF build/$*.s.d $< -o $@

build/%.c.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP $< -o $@

run: $(OUT)
	rm -f $(OUT).lock
	bochs -f bochsrc -q

clean:
	rm -rf build/
