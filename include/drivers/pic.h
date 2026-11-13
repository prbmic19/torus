/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef DRIVERS_PIC_H
#define DRIVERS_PIC_H

#include <torus/compiler.h>
#include <torus/types.h>
#include <asm/io.h>

#define PIC1_CMD  0x20
#define PIC2_CMD  0xa0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xa1

__always_inline static void pic_prim_write_cmd(u8 cmd)
{
    outb(PIC1_CMD, cmd);
}

__always_inline static void pic_secnd_write_cmd(u8 cmd)
{
    outb(PIC2_CMD, cmd);
}

__always_inline static u8 pic_prim_read_data(void)
{
    return inb(PIC1_DATA);
}

__always_inline static u8 pic_secnd_read_data(void)
{
    return inb(PIC2_DATA);
}

__always_inline static void pic_prim_write_data(u8 data)
{
    outb(PIC1_DATA, data);
}

__always_inline static void pic_secnd_write_data(u8 data)
{
    outb(PIC2_DATA, data);
}

extern void pic_init(void);
extern void pic_eoi(int irq);

#endif // DRIVERS_PIC_H
