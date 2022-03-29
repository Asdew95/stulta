#ifndef LIBK_UTIL_H
#define LIBK_UTIL_H

#include <stddef.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memset(void *s, int c, size_t n);

#endif
