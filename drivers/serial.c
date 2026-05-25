/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <drivers/serial.h>
#include <torus/types.h>
#include <asm/io.h>
#include <asm/boot_info.h>

static u16 serial_base_port;

static bool serial_ready(void)
{
    return inb(serial_base_port + 5) & 0x20;
}

bool serial_init(const struct boot_info *boot_info)
{
    // Find the first available serial port.
    for (size_t i = 0; i < sizeof(boot_info->com_ports); i++)
    {
        if (boot_info->com_ports[i])
        {
            serial_base_port = boot_info->com_ports[i];
            break;
        }
    }

    // There were none?
    if (!serial_base_port)
    {
        return false;
    }

    outb(serial_base_port + 1, 0x00);
    outb(serial_base_port + 3, 0x80);
    outb(serial_base_port,     0x03);
    outb(serial_base_port + 1, 0x00);
    outb(serial_base_port + 3, 0x03);
    outb(serial_base_port + 2, 0xc7);
    outb(serial_base_port + 4, 0x0b);

    return true;
}

void serial_putchar(int ch)
{
    while (!serial_ready())
    {
        asm volatile ("pause");
    }

    outb(serial_base_port, (unsigned char)ch);
}
