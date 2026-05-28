/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <torus/types.h>
#include <kernel/console.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <drivers/serial.h>
#include <lib/string.h>
#include <asm/idt.h>
#include <asm/boot_info.h>

static struct boot_info boot_info;

static struct console serial_console = {
    .putchar = serial_putchar
};

static u8 good_boot_info_signature[4] = {'I', 'N', 'F', 'O'};

extern __noreturn void kmain(void);

__noreturn void arch_kmain(void)
{
    // Copy boot info provided by the bootloader at 0x680.
    memcpy(&boot_info, (void *)0x680, sizeof(boot_info));

    if (memcmp(boot_info.signature, good_boot_info_signature, sizeof(boot_info.signature)) != 0)
    {
        panic("Bad boot info signature.");
    }

    if (!serial_init(&boot_info))
    {
        panic("Cannot initialize serial.");
    }

    console_init(&serial_console);

    idt_init();
    local_irq_enable();

    kmain();
}
