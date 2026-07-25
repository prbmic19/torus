/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <kernel/clocksource.h>
#include <kernel/irq.h>
#include <kernel/kprintf.h>
#include <drivers/pit.h>
#include <asm/regs.h>

static volatile u64 clock_ticks;
static const int clock_frequency = 100;

static void clock_handler(struct regs *regs)
{
    (void)regs;
    clock_ticks++;
}

u64 clocksource_read(void)
{
    return clock_ticks;
}

void clocksource_init(void)
{
    pit_set_freq(clock_frequency);
    irq_register_handler(IRQ_TIMER, clock_handler);
    pr_notice("clocksource: PIT (%d Hz).\n", clock_frequency);
}
