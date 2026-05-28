/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <torus/types.h>
#include <kernel/kprintf.h>
#include <kernel/console.h>
#include <stdarg.h>

static void print_num(unsigned int num, unsigned int base, bool is_signed)
{
    char buf[256] = {0};
    int i = 0;

    if (!num)
    {
        console_putchar('0');
        return;
    }

    if (is_signed && (int)num < 0)
    {
        console_putchar('-');
        num = -(int)num; // Beware for INT_MIN.
        base = 10; // Force decimal.
    }

    while (num > 0)
    {
        unsigned int digit = num % base;

        if (digit < 10)
        {
            buf[i++] = '0' + digit;
        }
        else
        {
            buf[i++] = 'a' + (digit - 10);
        }

        num /= base;
    }

    while (i > 0)
    {
        console_putchar(buf[--i]);
    }
}

__format(printf, 1, 2) int kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int i = 0;

    while (fmt[i])
    {
        if (fmt[i] != '%')
        {
            console_putchar(fmt[i++]);
            continue;
        }

        i++;

        switch (fmt[i])
        {
            case 's':
            {
                const char *str = va_arg(args, const char *);

                if (!str)
                {
                    str = "(null)";
                }

                console_puts(str);
                break;
            }
            case 'c':
                console_putchar(va_arg(args, int));
                break;
            case 'd':
                print_num(va_arg(args, int), 10, true);
                break;
            case 'u':
                print_num(va_arg(args, unsigned int), 10, false);
                break;
            case 'x':
                print_num(va_arg(args, unsigned int), 16, false);
                break;
            case '%':
                console_putchar('%');
                break;
            default:
                console_putchar('%');
                console_putchar(fmt[i]);
                break;
        }

        i++;
    }

    va_end(args);
    return i;
}
