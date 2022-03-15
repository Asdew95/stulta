OBJ = boot.s.o kentry.c.o
OUT = stulta.iso

AS = nasm
CC = clang
LD = ld.lld
ASFLAGS = -f elf32
CFLAGS = -c -ffreestanding -fno-builtin -nostdlib -Wall -Wextra -Werror \
		 --target=i686-pc-none-elf -march=i686
LDFLAGS = -T link.ld -melf_i386

.PHONY: clean run

all: $(OUT)

$(OUT): iso/boot/stulta.elf
	grub-mkrescue -o $@ iso/

iso/boot/stulta.elf: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

run: $(OUT)
	rm -f $(OUT).lock
	bochs -f bochsrc -q

clean:
	rm -f $(OUT) iso/boot/stulta.elf $(OBJ)
