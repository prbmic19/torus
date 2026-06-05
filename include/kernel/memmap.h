/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef KERNEL_MEMMAP_H
#define KERNEL_MEMMAP_H

#include <torus/types.h>

#define MEMMAP_MAX_ENTRIES 128

enum memmap_entry_type
{
    MEMMAP_TYPE_USABLE,
    MEMMAP_TYPE_RESERVED,
    MEMMAP_TYPE_ACPI_RECLAIMABLE,
    MEMMAP_TYPE_ACPI_NVS,
    MEMMAP_TYPE_UNUSABLE,
    MEMMAP_TYPE_UNKNOWN
};

struct memmap_entry
{
    u64 base;
    u64 len;
    enum memmap_entry_type type;
};

extern struct memmap_entry memmap[MEMMAP_MAX_ENTRIES];
extern unsigned int memmap_entry_count;

extern void memmap_init(void);
extern const char *memmap_entry_type_to_string(enum memmap_entry_type type);

#endif // KERNEL_MEMMAP_H
