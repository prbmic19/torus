/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <kernel/timer.h>
#include <kernel/kprintf.h>
#include <kernel/irq.h>
#include <kernel/memmap.h>
#include <kernel/acpi.h>
#include <drivers/fb.h>

__noreturn void kmain(void)
{
    extern const char saved_command_line[CMDLINE_SIZE];

    timer_init(100);

    pr_notice("Command line: '%s'.\n", saved_command_line);

    memmap_init();
    acpi_init();
    // fb_init();

    while (1)
    {
        cpu_safe_halt();
    }
}
