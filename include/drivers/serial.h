/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef DRIVERS_SERIAL_H
#define DRIVERS_SERIAL_H

#include <torus/types.h>

// Forward declaration.
struct boot_info;

extern bool serial_init(const struct boot_info *boot_info);
extern void serial_putchar(int ch);

#endif // DRIVERS_SERIAL_H
