/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

#include <torus/compiler.h>
#include <stdarg.h>

// Must be defined somewhere in 'arch/*'.
struct regs;

// regs is nullable for both.
extern __noreturn __printf(2, 0) void vpanic(const struct regs *restrict regs, const char *restrict fmt, va_list args);
extern __noreturn __printf(2, 3) void panic(const struct regs *restrict regs, const char *restrict fmt, ...);

#endif // KERNEL_PANIC_H
