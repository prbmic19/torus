/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef LIB_VSPRINTF_H
#define LIB_VSPRINTF_H

#include <torus/types.h>
#include <torus/compiler.h>
#include <stdarg.h>

extern __printf(3, 0) int vsnprintf(char *restrict buf, size_t buf_size, const char *restrict fmt, va_list args);
extern __printf(2, 0) int vsprintf(char *restrict buf, const char *restrict fmt, va_list args);
extern __printf(3, 4) int snprintf(char *restrict buf, size_t buf_size, const char *restrict fmt, ...);
extern __printf(2, 3) int sprintf(char *restrict buf, const char *restrict fmt, ...);

#endif // LIB_VSPRINTF_H
