/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <asm/idt.h>

extern __noreturn void kmain(void);

__noreturn void arch_kmain(void)
{
    idt_init();
    asm volatile ("sti");

    kmain();
}
