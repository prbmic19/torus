/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_CLOCKSOURCE_H
#define KERNEL_CLOCKSOURCE_H

#include <torus/types.h>

void clocksource_init(void);
u64 clocksource_read(void);

#endif // KERNEL_CLOCKSOURCE_H
