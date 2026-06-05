/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <drivers/serial.h>
#include <torus/types.h>
#include <asm/io.h>

static const u16 serial_port = 0x3f8;

static bool serial_ready(void)
{
    return inb(serial_port + 5) & 0x20;
}

void serial_init(void)
{
    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, 0x80);
    outb(serial_port + 0, 0x03);
    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, 0x03);
    outb(serial_port + 2, 0xc7);
    outb(serial_port + 4, 0x0b);
}

void serial_putchar(int ch)
{
    while (!serial_ready())
    {
        asm volatile ("pause");
    }

    outb(serial_port, (unsigned char)ch);
}
