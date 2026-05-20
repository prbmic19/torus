/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef X86_ASM_IO_H
#define X86_ASM_IO_H

#include <torus/types.h>
#include <torus/compiler.h>

__always_inline u8 inb(u16 port)
{
    u8 data;
    asm volatile ("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

__always_inline void outb(u16 port, u8 data)
{
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

__always_inline u16 inw(u16 port)
{
    u16 data;
    asm volatile ("inw %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

__always_inline void outw(u16 port, u16 data)
{
    asm volatile ("outw %0, %1" : : "a"(data), "Nd"(port));
}

__always_inline u32 inl(u16 port)
{
    u32 data;
    asm volatile ("inl %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

__always_inline void outl(u16 port, u32 data)
{
    asm volatile ("outl %0, %1" : : "a"(data), "Nd"(port));
}

#endif // X86_ASM_IO_H
