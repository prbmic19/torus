/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <kernel/console.h>

static struct console *current_console;

void console_init(struct console *console)
{
    current_console = console;
    console_puts("[OK] Console initialized.\n");
}

void console_putchar(int ch)
{
    if (!current_console || !current_console->putchar)
    {
        return;
    }

    // Normalize newlines.
    if (ch == '\n')
    {
        current_console->putchar('\r');
    }

    current_console->putchar(ch);
}

void console_puts(const char *string)
{
    while (*string)
    {
        console_putchar(*string++);
    }
}
