/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <drivers/pic.h>
#include <asm/io.h>

void pic_init(void)
{
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

void pic_eoi(int irq)
{
    if (irq >= 8)
    {
        outb(PIC2_CMD, 0x20);
    }

    outb(PIC1_CMD, 0x20);
}
