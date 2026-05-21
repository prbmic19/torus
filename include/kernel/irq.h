/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H

#define IRQ_MAX   256
#define IRQ_TIMER 0

// Forward declaration.
struct regs;

typedef void (*irq_handler_t)(struct regs *regs);

extern void irq_register_handler(int irq, irq_handler_t handler);
extern void irq_unregister_handler(int irq);
extern void irq_dispatch(int irq, struct regs *regs);

#endif // KERNEL_IRQ_H
