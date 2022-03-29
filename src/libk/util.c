#include <stddef.h>
#include "util.h"

void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    char *d = (char*) dest;
    char *s = (char*) src;

    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    char *dst = (char*) s;

    for (size_t i = 0; i < n; i++) {
        dst[i] = c;
    }

    return s;
}
