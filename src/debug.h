#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG

#include "io.h"

#define BOCHS_PRINT_CHAR(c) outb(0xe9, c);
#define BOCHS_PRINT(s) for (int i = 0; s[i] != '\0'; i++) { \
    BOCHS_PRINT_CHAR(s[i]); }
#define BOCHS_PRINT_UINT(d) BOCHS_PRINT("0x") for (int i = 0; i < 32; i += 4) \
    { BOCHS_PRINT_CHAR("0123456789abcdef"[((d) >> (32 - i - 4)) & 0x0f]) }

#define BOCHS_BREAKPOINT() __asm__("xchg %bx, %bx");

#else

#define BOCHS_PRINT_CHAR(c)
#define BOCHS_PRINT(s)
#define BOCHS_PRINT_UINT(d)

#define BOCHS_BREAKPOINT()

#endif

#endif
