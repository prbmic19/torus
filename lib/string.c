/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <lib/string.h>
#include <torus/types.h>
#include <torus/compiler.h>

__nonnull(1, 2) void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    unsigned char *restrict d = dest;
    const unsigned char *restrict s = src;

    while (n--)
    {
        *d++ = *s++;
    }

    return dest;
}

__nonnull(1, 2) void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;

    if (d == s)
    {
        return dest;
    }

    if (d < s)
    {
        while (n--)
        {
            *d++ = *s++;
        }
    }
    else
    {
        d += n;
        s += n;

        while (n--)
        {
            *--d = *--s;
        }
    }

    return dest;
}

__nonnull(1) void *memset(void *dest, int c, size_t n)
{
    unsigned char *d = dest;

    while (n--)
    {
        *d++ = c;
    }

    return dest;
}

__nonnull(1, 2) __pure int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *t1 = s1;
    const unsigned char *t2 = s2;

    while (n--)
    {
        if (*t1 != *t2)
        {
            return *t1 - *t2;
        }

        t1++;
        t2++;
    }

    return 0;
}

__nonnull(1) __pure size_t strlen(const char *s)
{
    const char *t = s;
    
    while (*t != '\0')
    {
        t++;
    }
    
    return t - s;
}

__nonnull(1) __pure size_t strnlen(const char *s, size_t maxlen)
{
    const char *t = s;
    
    while (maxlen-- && *t)
    {
        t++;
    }
    
    return t - s;
}

__nonnull(1, 2) char *strcpy(char *restrict dst, const char *restrict src)
{
    char *orig_dst = dst;
    
    while ((*dst++ = *src++))
    {
    }

    return orig_dst;
}

__nonnull(1, 2) char *strncpy(char *restrict dst, const char *restrict src, size_t dsize)
{
    char *d = dst;

    while (dsize != 0 && *src)
    {
        *d++ = *src++;
        dsize--;
    }

    while (dsize != 0)
    {
        *d++ = '\0';
        dsize--;
    }

    return dst;
}

__nonnull(1, 2) __pure int strcmp(const char *restrict s1, const char *restrict s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

__nonnull(1, 2) __pure int strncmp(const char *restrict s1, const char *restrict s2, size_t n)
{
    while (n && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }

    if (n == 0)
    {
        return 0;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

__nonnull(1) __pure char *strchr(const char *s, int c)
{
    while (*s != '\0')
    {
        if (*(unsigned char *)s == (unsigned char)c)
        {
            return (char *)s;
        }

        s++;
    }

    if (c == '\0')
    {
        return (char *)s;
    }

    return NULL;
}

__nonnull(1) __pure char *strrchr(const char *s, int c)
{
    const char *last = NULL;

    do
    {
        if (*(unsigned char *)s == (unsigned char)c)
        {
            last = s;
        }
    }
    while (*s++);

    return (char *)last;
}
