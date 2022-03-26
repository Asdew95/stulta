ENTRY(asentry)

SECTIONS {
    . = 0xf0100000;

    kvstart = .;
    kpstart = . - 0xf0000000;

    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xf0000000)
    {
        *(.text)
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xf0000000)
    {
        *(.rodata*)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xf0000000)
    {
        *(.data)
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xf0000000)
    {
        *(COMMON)
        *(.bss)
    }

    kvend = .;
    kpend = . - 0xf0000000;
}
