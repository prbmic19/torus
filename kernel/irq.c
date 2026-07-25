/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <kernel/irq.h>

static irq_handler_t irq_handlers[IRQ_MAX_HANDLERS];

void irq_register_handler(int irq, irq_handler_t handler)
{
    if (irq < 0 || irq >= IRQ_MAX_HANDLERS)
    {
        return;
    }

    irq_handlers[irq] = handler;
}

void irq_unregister_handler(int irq)
{
    if (irq < 0 || irq >= IRQ_MAX_HANDLERS)
    {
        return;
    }

    irq_handlers[irq] = NULL;
}

void irq_dispatch(int irq, struct regs *regs)
{
    if (irq < 0 || irq >= IRQ_MAX_HANDLERS)
    {
        return;
    }

    if (irq_handlers[irq])
    {
        irq_handlers[irq](regs);
    }
}
