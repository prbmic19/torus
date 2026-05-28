/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <drivers/pit.h>
#include <asm/io.h>

void pit_set_freq(unsigned int hz)
{
    unsigned int divisor = PIT_TICK_RATE / hz;

    outb(PIT_MODE, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xff);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xff);
}
