/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_KPRINTF_H
#define KERNEL_KPRINTF_H

#include <torus/compiler.h>

extern __format(printf, 1, 2) int kprintf(const char *fmt, ...);

#endif // KERNEL_KPRINTF_H
