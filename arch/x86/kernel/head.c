/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <kernel/console.h>
#include <drivers/serial.h>
#include <lib/string.h>
#include <asm/idt.h>
#include <asm/boot_info.h>

static struct boot_info boot_info;

static struct console serial_console = {
    .putchar = serial_putchar
};

extern __noreturn void kmain(void);

__noreturn void arch_kmain(void)
{
    // Copy boot info provided by the bootloader at 0x680.
    memcpy(&boot_info, (void *)0x680, sizeof(boot_info));

    if (!serial_init(&boot_info))
    {
        // Halt and catch fire!
        asm volatile ("cli; hlt");
    }

    console_init(&serial_console);

    idt_init();
    asm volatile ("sti" : : : "memory");

    kmain();
}
