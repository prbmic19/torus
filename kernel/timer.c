/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <kernel/timer.h>
#include <kernel/irq.h>
#include <kernel/kprintf.h>
#include <drivers/pit.h>

static volatile u64 timer_ticks;

static void timer_handler(struct regs *unused)
{
    (void)unused;
    timer_ticks++;
}

u64 timer_get_ticks(void)
{
    return timer_ticks;
}

void timer_init(unsigned int hz)
{
    pit_set_freq(hz);
    irq_register_handler(IRQ_TIMER, timer_handler);
    kprintf("[NOTICE] Timer initialized.\n");
}
