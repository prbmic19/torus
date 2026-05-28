/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H

#include <torus/compiler.h>

// Must define arch_local_irq_enable(), arch_local_irq_disable(), arch_native_safe_halt(), and arch_native_halt().
#include <asm/irq.h>
// Must define struct regs.
#include <asm/regs.h>

#define IRQ_MAX   256
#define IRQ_TIMER 0

__always_inline static void local_irq_enable(void)
{
    arch_local_irq_enable();
}

__always_inline static void local_irq_disable(void)
{
    arch_local_irq_disable();
}

__always_inline static void native_safe_halt(void)
{
    arch_native_safe_halt();
}

__always_inline static void native_halt(void)
{
    arch_native_halt();
}

typedef void (*irq_handler_t)(struct regs *regs);

extern void irq_register_handler(int irq, irq_handler_t handler);
extern void irq_unregister_handler(int irq);
extern void irq_dispatch(int irq, struct regs *regs);

#endif // KERNEL_IRQ_H
