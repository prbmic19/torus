/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <kernel/acpi.h>
#include <kernel/kprintf.h>

void acpi_init(void)
{
    pr_debug(
        "ACPI: RSDT at 0x%lx, OEM ID: '%c%c%c%c%c%c'.\n",
        (unsigned long)rsdp.rsdt_addr,
        rsdp.oem_id[0],
        rsdp.oem_id[1],
        rsdp.oem_id[2],
        rsdp.oem_id[3],
        rsdp.oem_id[4],
        rsdp.oem_id[5]
    );
}
