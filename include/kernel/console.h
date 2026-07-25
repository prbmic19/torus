/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_CONSOLE_H
#define KERNEL_CONSOLE_H

struct console
{
    void (*putchar)(int ch);
};

void console_init(struct console *console);
void console_putchar(int ch);
void console_puts(const char *str);

#endif // KERNEL_CONSOLE_H
