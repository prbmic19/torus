/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

#include <torus/types.h>

void timer_init(unsigned int hz);
u64 timer_get_ticks(void);

#endif // KERNEL_TIMER_H
