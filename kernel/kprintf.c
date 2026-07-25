/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <torus/types.h>
#include <kernel/kprintf.h>
#include <kernel/console.h>
#include <lib/string.h>
#include <lib/cmdline.h>
#include <lib/vsprintf.h>
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

__printf(1, 0) int vkprintf(const char *fmt, va_list args)
{
    extern const char saved_command_line[1024];

    // Suppress debug logs unless the "debug" command-line option is present.
    if (strncmp(fmt, PL_DEBUG, 2) == 0 && !cmdline_find_option_bool(saved_command_line, "debug"))
    {
        return 0;
    }

    char buf[1024] = {0};
    int buf_i = 0;

    int i = vsnprintf(buf, sizeof(buf), fmt, args);

    // Adjust the reported characters printed if a log level was passed.
    if (buf[0] == '\1' && buf[1] >= '0' && buf[1] <= '7')
    {
        int log_level = buf[1] - '0';

        i = i - 2 + strlen(log_prefixes[log_level]);
        buf_i += 2;

        console_puts(log_prefixes[log_level]);
    }

    while (buf_i < i)
    {
        console_putchar(buf[buf_i++]);
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
