/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <torus/types.h>
#include <kernel/kprintf.h>
#include <kernel/console.h>
#include <kernel/panic.h>
#include <lib/string.h>
#include <lib/cmdline.h>
#include <stdarg.h>

const char *const log_prefixes[8] = {
    "[EMERG ] ",
    "[(rsvd)] ",
    "[(rsvd)]",
    "[ERR   ] ",
    "[WARN  ] ",
    "[NOTICE] ",
    "[INFO  ] ",
    "[DEBUG ] "
};

static void print_num_int(unsigned int num, unsigned int base, bool is_signed)
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
        num = -(int)num;
        base = 10;
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

static void print_num_long(unsigned long num, unsigned long base, bool is_signed)
{
    char buf[256] = {0};
    int i = 0;

    if (num == 0)
    {
        console_putchar('0');
        return;
    }

    if (is_signed && (long)num < 0)
    {
        console_putchar('-');
        num = -(long)num;
        base = 10;
    }

    while (num > 0)
    {
        unsigned long digit = num % base;

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

__printf(1, 0) int vkprintf(const char *fmt, va_list args)
{
    extern const char saved_command_line[1024];
    int log_level = -1;
    int i = 0;

    // Suppress debug logs unless the "debug" command-line option is present.
    if (strncmp(fmt, PL_DEBUG, 2) == 0 && !cmdline_find_option_bool(saved_command_line, "debug"))
    {
        return 0;
    }

    if (fmt[0] == '\1' && fmt[1] >= '0' && fmt[1] <= '7')
    {
        log_level = fmt[1] - '0';
        i += 2;
    }

    if (log_level != -1)
    {
        console_puts(log_prefixes[log_level]);
    }

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
                print_num_int(va_arg(args, int), 10, true);
                break;
            case 'u':
                print_num_int(va_arg(args, unsigned int), 10, false);
                break;
            case 'x':
                print_num_int(va_arg(args, unsigned int), 16, false);
                break;
            case 'l':
            {
                switch (fmt[++i])
                {
                    case 'd':
                        print_num_long(va_arg(args, long), 10, true);
                        break;
                    case 'u':
                        print_num_long(va_arg(args, unsigned long), 10, false);
                        break;
                    case 'x':
                        print_num_long(va_arg(args, unsigned long), 16, false);
                        break;
                    default:
                        console_putchar('%');
                        console_putchar('l');
                        console_putchar(fmt[i]);
                        break;
                }
                break;
            }
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

    return i;
}

__printf(1, 2) int kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int i = vkprintf(fmt, args);

    va_end(args);
    return i;
}
