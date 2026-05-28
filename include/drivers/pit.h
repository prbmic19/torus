/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef DRIVERS_PIT_H
#define DRIVERS_PIT_H

#define PIT_TICK_RATE 1193182u

#define PIT_MODE     0x43
#define PIT_CHANNEL0 0x40

extern void pit_set_freq(unsigned int hz);

#endif // DRIVERS_PIT_H
