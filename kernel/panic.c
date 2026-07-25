/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <torus/types.h>
#include <kernel/panic.h>
#include <kernel/kprintf.h>
#include <kernel/irq.h>
#include <lib/vsprintf.h>

// Must define struct regs and context_dump().
#include <asm/regs.h>

#include <stdarg.h>

static volatile bool panic_in_progress = false;

__noreturn __printf(2, 0) void vpanic(const struct regs *restrict regs, const char *restrict fmt, va_list args)
{
    local_irq_disable();

    if (panic_in_progress)
    {
        pr_emerg("Recursive kernel panic!!\n");
        pr_emerg("Halting execution.\n");

        goto hcf;
    }

    panic_in_progress = true;

    char panic_message[256];
    vsnprintf(panic_message, sizeof(panic_message), fmt, args);

    pr_emerg("                                   .::!!!!!!!:.\n");
    pr_emerg(" .!!!!!:.                        .:!!!!!!!!!!!!\n");
    pr_emerg(" ~~~~!!!!!!.                 .:!!!!!!!!!UWWW$$$\n");
    pr_emerg("     :$$NWX!!:           .:!!!!!!XUWW$$$$$$$$$P\n");
    pr_emerg("     $$$$$##WX!:      .<!!!!UW$$$$\"  $$$$$$$$#\n");
    pr_emerg("     $$$$$  $$$UX   :!!UW$$$$$$$$$   4$$$$$*\n");
    pr_emerg("     ^$$$B  $$$$\\     $$$$$$$$$$$$   d$$R\"\n");
    pr_emerg("       \"*$bd$$$$      '*$$$$$$$$$$$o+#\"\n");
    pr_emerg("            \"\"\"\"          \"\"\"\"\"\"\"\n");
    pr_emerg("Kernel panic! Message: %s\n", panic_message);

    if (regs)
    {
        context_dump(regs);
    }

    pr_emerg("Halting execution.\n");

hcf:

    while (1)
    {
        cpu_halt();
    }
}

__noreturn __printf(2, 3) void panic(const struct regs *restrict regs, const char *restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vpanic(regs, fmt, args);

    va_end(args);
}
