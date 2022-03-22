#ifndef DEBUG_H
#define DEBUG_H

#include "io.h"

// This is fine! This is fin--
//
// This is so not right...
#define _dbg_print_char(n) n ## _print_char
#define __dbg_print_char(n) _dbg_print_char(n)

static inline void bochs_print_char(char c);
static inline void dbg_print_char(char c);
static inline void dbg_print(char *s);
static inline void dbg_print_uint32(uint32_t d);

static inline void bochs_print_char(char c)
{
    outb(0xe9, c);
}

static inline void dbg_print_char(char c)
{
#ifdef DEBUG
    __dbg_print_char(DEBUG)(c);
#endif
}

static inline void dbg_print(char *s)
{
#ifdef DEBUG
    char c;
    int i = 0;
    while ((c = s[i++]) != '\0') {
        bochs_print_char(c);
    }
#endif
}

static inline void dbg_print_uint32(uint32_t d)
{
#ifdef DEBUG
    dbg_print("0x");

    for (int i = 0; i < 32; i += 4) {
        dbg_print_char("0123456789abcdef"[(d >> (28 - i)) & 0x0f]);
    }
#endif
}

#endif
