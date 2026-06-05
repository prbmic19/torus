/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_ACPI_H
#define KERNEL_ACPI_H

#include <torus/types.h>

struct rsdp
{
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_addr;

    /* u8 len;
    u64 xsdt_addr;
    u8 ext_checksum;
    u8 _rsvd[3]; */
};

extern struct rsdp rsdp;

extern void acpi_init(void);

#endif // KERNEL_ACPI_H
