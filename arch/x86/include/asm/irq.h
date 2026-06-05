/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_X86_IRQ_H
#define ASM_X86_IRQ_H

#include <torus/compiler.h>

__always_inline static void arch_local_irq_enable(void)
{
    asm volatile ("sti" : : : "memory");
}

__always_inline static void arch_local_irq_disable(void)
{
    asm volatile ("cli" : : : "memory");
}

__always_inline static void arch_cpu_safe_halt(void)
{
    asm volatile ("sti; hlt" : : : "memory");
}

__always_inline static void arch_cpu_halt(void)
{
    asm volatile ("hlt" : : : "memory");
}

extern void irq_init(void);

#endif // ASM_X86_IRQ_H
