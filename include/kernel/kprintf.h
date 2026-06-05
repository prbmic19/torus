/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_KPRINTF_H
#define KERNEL_KPRINTF_H

#include <torus/compiler.h>
#include <stdarg.h>

#define PL_EMERG  "\0010"
// 1 and 2 are reserved.
#define PL_ERR    "\0013"
#define PL_WARN   "\0014"
#define PL_NOTICE "\0015"
#define PL_INFO   "\0016"
#define PL_DEBUG  "\0017"

#define pr_emerg(fmt, ...)  kprintf(PL_EMERG fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...)    kprintf(PL_ERR fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...)   kprintf(PL_WARN fmt, ##__VA_ARGS__)
#define pr_notice(fmt, ...) kprintf(PL_NOTICE fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...)   kprintf(PL_INFO fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...)  kprintf(PL_DEBUG fmt, ##__VA_ARGS__)

extern __printf(1, 0) int vkprintf(const char *fmt, va_list args);
extern __printf(1, 2) int kprintf(const char *fmt, ...);

#endif // KERNEL_KPRINTF_H
