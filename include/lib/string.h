/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef LIB_STRING_H
#define LIB_STRING_H

#include <torus/types.h>
#include <torus/compiler.h>

extern __nonnull(1, 2) void *memcpy(void *restrict dest, const void *restrict src, size_t n);
extern __nonnull(1, 2) void *memmove(void *dest, const void *src, size_t n);
extern __nonnull(1) void *memset(void *dest, int c, size_t n);
extern __nonnull(1, 2) __pure int memcmp(const void *s1, const void *s2, size_t n);
extern __nonnull(1) __pure size_t strlen(const char *s);
extern __nonnull(1) __pure size_t strnlen(const char *s, size_t maxlen);
extern __nonnull(1, 2) char *strcpy(char *restrict dst, const char *src);
extern __nonnull(1, 2) char *strncpy(char *restrict dst, const char *src, size_t dsize);
extern __nonnull(1, 2) __pure int strcmp(const char *s1, const char *s2);
extern __nonnull(1, 2) __pure int strncmp(const char *s1, const char *s2, size_t n);
extern __nonnull(1) __pure char *strchr(const char *s, int c);
extern __nonnull(1) __pure char *strrchr(const char *s, int c);

#endif // LIB_STRING_H
