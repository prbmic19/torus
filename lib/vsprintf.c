/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <lib/vsprintf.h>
#include <lib/string.h>
#include <torus/types.h>
#include <torus/compiler.h>
#include <stdarg.h>
#include <limits.h>

#include <kernel/panic.h>

// Returns number of digits converted.
static int int_to_str(char *buf, size_t buf_size, u64 num, int base, bool uppercase)
{
    if (unlikely(!buf_size))
    {
        return 0;
    }

    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char temp[70]; // This is enough to fit a 64-bit binary string.
    int i = 0;

    if (!num)
    {
        temp[i++] = '0';
    }
    else
    {
        while (num > 0 && i < (int)sizeof(temp))
        {
            temp[i++] = digits[num % base];
            num /= base;
        }
    }

    const int limit = buf_size - 1; // Account for the null terminator.
    int written = 0;

    for (int j = i - 1; j >= 0; j--)
    {
        if (written >= limit)
        {
            break;
        }

        buf[written++] = temp[j];
    }

    buf[written] = '\0';
    return written;
}

__printf(3, 0) int vsnprintf(char *restrict buf, size_t buf_size, const char *restrict fmt, va_list args)
{
    if (unlikely(!buf || !fmt))
    {
        return -1;
    }

    if (unlikely(!buf_size))
    {
        return 0;
    }

    if (unlikely(buf_size > INT_MAX))
    {
        return -1;
    }

    int i = 0;
    int j = 0;
    const int limit = buf_size - 1;

    while (fmt[i])
    {
        if (j >= limit)
        {
            break;
        }

        if (fmt[i] != '%')
        {
            buf[j++] = fmt[i++];
            continue;
        }
        
        i++;

        bool is_long_long = false;
        bool is_long = false;
        bool is_size_t = false;

        if (fmt[i] == 'l')
        {
            i++;
            is_long = true;

            if (fmt[i] == 'l')
            {
                i++;
                is_long_long = true;
            }
        }
        else if (fmt[i] == 'z')
        {
            i++;
            is_size_t = true;
        }

        switch (fmt[i])
        {
            case 'c':
                buf[j++] = (char)va_arg(args, int);
                break;
            case 's':
            {
                const char *str = va_arg(args, const char *);
                
                if (!str)
                {
                    str = "(null)";
                }

                while (*str && j < limit)
                {
                    buf[j++] = *str++;
                }

                break;
            }
            case 'i':
            case 'd':
            {
                long long num;

                if (is_size_t)
                {
                    num = va_arg(args, ssize_t);
                }
                else if (is_long_long)
                {
                    num = va_arg(args, long long);
                }
                else if (is_long)
                {
                    num = va_arg(args, long);
                }
                else
                {
                    num = va_arg(args, int);
                }

                if (num < 0)
                {
                    buf[j++] = '-';
                    num = -num;
                }

                char num_buf[64];
                int len = int_to_str(num_buf, sizeof(num_buf), num, 10, false);

                for (int k = 0; k < len; k++)
                {
                    buf[j++] = num_buf[k];
                }

                break;
            }
            case 'u':
            case 'x':
            case 'X':
            {
                unsigned long long num;

                if (is_size_t)
                {
                    num = va_arg(args, size_t);
                }
                else if (is_long_long)
                {
                    num = va_arg(args, unsigned long long);
                }
                else if (is_long)
                {
                    num = va_arg(args, unsigned long);
                }
                else
                {
                    num = va_arg(args, unsigned int);
                }

                int base = fmt[i] == 'u' ? 10 : 16;
                bool upper = fmt[i] == 'X';

                char num_buf[64];
                int len = int_to_str(num_buf, sizeof(num_buf), num, base, upper);

                for (int k = 0; k < len; k++)
                {
                    buf[j++] = num_buf[k];
                }

                break;
            }
            case 'p':
            {
                void *ptr = va_arg(args, void *);

                buf[j++] = '0';
                buf[j++] = 'x';

                char num_buf[64];
                int len = int_to_str(num_buf, sizeof(num_buf), (u64)(unsigned long)ptr, 16, false);

                for (int k = 0; k < len; k++)
                {
                    buf[j++] = num_buf[k];
                }

                break;
            }
            case '%':
                buf[j++] = '%';
                break;
            default:
                buf[j++] = '%';
                buf[j++] = fmt[i];
                break;
        }

        i++;
    }

    buf[j] = '\0';
    return j;
}

__printf(2, 0) int vsprintf(char *restrict buf, const char *restrict fmt, va_list args)
{
    return vsnprintf(buf, INT_MAX, fmt, args);
}

__printf(3, 4) int snprintf(char *restrict buf, size_t buf_size, const char *restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int i = vsnprintf(buf, buf_size, fmt, args);

    va_end(args);
    return i;
}

__printf(2, 3) int sprintf(char *restrict buf, const char *restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int i = vsprintf(buf, fmt, args);

    va_end(args);
    return i;
}
