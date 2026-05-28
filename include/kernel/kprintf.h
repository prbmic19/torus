/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_KPRINTF_H
#define KERNEL_KPRINTF_H

#include <torus/compiler.h>
#include <stdarg.h>

#define PL_EMERG  "[EMERG] "
#define PL_ERR    "[ERR] "
#define PL_WARN   "[WARN] "
#define PL_NOTICE "[NOTICE] "
#define PL_INFO   "[INFO] "
#define PL_DEBUG  "[DEBUG] "

#define pr_emerg(fmt, ...)  kprintf(PL_EMERG fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...)    kprintf(PL_ERR fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...)   kprintf(PL_WARN fmt, ##__VA_ARGS__)
#define pr_notice(fmt, ...) kprintf(PL_NOTICE fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...)   kprintf(PL_INFO fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...)  kprintf(PL_DEBUG fmt, ##__VA_ARGS__)

extern __printf(1, 0) int vkprintf(const char *fmt, va_list args);
extern __printf(1, 2) int kprintf(const char *fmt, ...);

#endif // KERNEL_KPRINTF_H
