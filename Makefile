SRC_FILES = boot.s kentry.c
DEPS = $(addprefix build/, $(addsuffix .d, $(SRC_FILES)))
OBJ = $(addprefix build/, $(addsuffix .o, $(SRC_FILES)))
SRC = $(addprefix src/, $(SRC_FILES))
KERNEL_ELF = build/iso/boot/stulta.elf
OUT = build/stulta.iso

AS = nasm
CC = clang
LD = ld.lld
ASFLAGS = -f elf32
CFLAGS = -c -ffreestanding -fno-builtin -nostdlib -Wall -Wextra -Werror \
		 --target=i686-pc-none-elf -march=i686
LDFLAGS = -T link.ld -melf_i386

.PHONY: clean run

all: $(OUT)

$(OUT): $(KERNEL_ELF) build/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/iso/

build/iso/boot/grub/grub.cfg: grub.cfg
	@mkdir -p build/iso/boot/grub
	cp grub.cfg build/iso/boot/grub/

$(KERNEL_ELF): $(OBJ)
	@mkdir -p build/iso/boot
	$(LD) $(LDFLAGS) $(OBJ) -o $@

-include $(DEPS)

build/%.s.o: src/%.s
	@mkdir -p build
	$(AS) $(ASFLAGS) -MD -MP -MF build/$*.s.d $< -o $@

build/%.c.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -MMD -MP $< -o $@

run: $(OUT)
	rm -f $(OUT).lock
	bochs -f bochsrc -q

clean:
	rm -rf build/
