/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm/isr.h>

void isr_handler(struct regs *regs)
{
    (void)regs;
    // For now, do nothing.
}
