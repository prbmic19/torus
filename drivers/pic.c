/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <drivers/pic.h>
#include <asm/io.h>

void pic_init(void)
{
    pic_prim_write_cmd(0x11);
    pic_secnd_write_cmd(0x11);

    pic_prim_write_data(0x20);
    pic_secnd_write_data(0x28);

    pic_prim_write_data(0x04);
    pic_secnd_write_data(0x02);

    pic_prim_write_data(0x01);
    pic_secnd_write_data(0x01);

    pic_prim_write_data(0x00);
    pic_secnd_write_data(0x00);
}

void pic_eoi(int irq)
{
    if (irq >= 8)
    {
        pic_secnd_write_cmd(0x20);
    }

    pic_prim_write_cmd(0x20);
}
