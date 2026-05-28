/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

#include <torus/compiler.h>
#include <stdarg.h>

extern __noreturn __printf(1, 0) void vpanic(const char *fmt, va_list args);
extern __noreturn __printf(1, 2) void panic(const char *fmt, ...);

#endif // KERNEL_PANIC_H
