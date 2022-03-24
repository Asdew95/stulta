#include <stddef.h>
#include "util.h"

void *memset(void *s, int c, size_t n)
{
    char *dst = (char*) s;

    for (size_t i = 0; i < n; i++) {
        dst[i] = c;
    }

    return s;
}
