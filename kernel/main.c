/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <kernel/timer.h>
#include <kernel/irq.h>

__noreturn void kmain(void)
{
    timer_init(100);

    while (1)
    {
        native_safe_halt();
    }
}
