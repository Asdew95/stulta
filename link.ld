ENTRY(asentry)

SECTIONS {
    . = 0xfe100000;

    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xfe000000)
    {
        *(.text)
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xfe000000)
    {
        *(.rodata*)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xfe000000)
    {
        *(.data)
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xfe000000)
    {
        *(COMMON)
        *(.bss)
    }
}
