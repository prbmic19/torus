/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <asm/idt.h>
#include <asm/io.h>

extern __noreturn void kmain(void);

__noreturn void x86_64_kmain(void)
{
    idt_init();

    // For now, mask all hardware interrupts. Remap the PIC later.
    outb(0x21, 0xff);
    outb(0xa1, 0xff);

    asm volatile ("sti");

    kmain();
}
