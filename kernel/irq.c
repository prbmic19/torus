/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <kernel/irq.h>

static irq_handler_t irq_handlers[IRQ_MAX];

void irq_register_handler(int irq, irq_handler_t handler)
{
    if (irq < 0 || irq >= IRQ_MAX)
    {
        return;
    }

    irq_handlers[irq] = handler;
}

void irq_unregister_handler(int irq)
{
    if (irq < 0 || irq >= IRQ_MAX)
    {
        return;
    }

    irq_handlers[irq] = (irq_handler_t)0;
}

void irq_dispatch(int irq, struct regs *regs)
{
    if (irq < 0 || irq >= IRQ_MAX)
    {
        return;
    }

    if (irq_handlers[irq])
    {
        irq_handlers[irq](regs);
    }
}
