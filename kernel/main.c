/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <kernel/timer.h>

__noreturn void kmain(void)
{
    timer_init(100u);

    while (1)
    {
        asm volatile ("hlt");
    }
}
