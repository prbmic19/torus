/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef X86_ASM_IO_H
#define X86_ASM_IO_H

#include <torus/types.h>
#include <torus/compiler.h>

static __always_inline u8 inb(u16 port)
{
    u8 value;
    asm volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static __always_inline void outb(u16 port, u8 value)
{
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static __always_inline u16 inw(u16 port)
{
    u16 value;
    asm volatile ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static __always_inline void outw(u16 port, u16 value)
{
    asm volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static __always_inline u32 inl(u16 port)
{
    u32 value;
    asm volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static __always_inline void outl(u16 port, u32 value)
{
    asm volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

#endif // X86_ASM_IO_H
