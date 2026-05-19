/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>

__noreturn void kmain(void)
{
    while (1)
    {
        asm volatile ("hlt");
    }
}
