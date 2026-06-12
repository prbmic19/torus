/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <torus/types.h>
#include <kernel/panic.h>
#include <kernel/kprintf.h>
#include <kernel/irq.h>

// Must define struct regs, context_save(), and context_dump().
#include <asm/regs.h>

#include <stdarg.h>

static bool panic_in_progress = false;

__noreturn void vpanic(const char *fmt, va_list args)
{
    local_irq_disable();

    if (panic_in_progress)
    {
        pr_emerg("Recursive kernel panic!!\n");
        pr_emerg("Halting execution.\n");

        goto hcf;
    }

    panic_in_progress = true;

    struct regs regs;
    context_store(&regs);
   
    pr_emerg("Kernel panic!\n");
    pr_emerg("Message: ");
    vkprintf(fmt, args);
    kprintf("\n");

    context_dump(&regs);

    pr_emerg("Halting execution.\n");

hcf:

    while (1)
    {
        cpu_halt();
    }
}

__noreturn void panic(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vpanic(fmt, args);

    va_end(args);
}
