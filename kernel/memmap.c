/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <torus/compiler.h>
#include <kernel/memmap.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>
#include <lib/string.h>

void memmap_init(void)
{
    pr_info("Physical memory map:\n");

    for (unsigned int i = 0; i < memmap_entry_count; i++)
    {
        pr_info(
            "    %p-%p: %s\n",
            (void *)memmap[i].base,
            (void *)(memmap[i].base + memmap[i].len - 1),
            memmap_entry_type_to_string(memmap[i].type)
        );
    }

    // Reserve 0x0-0xfff if it is not reserved yet.
    if (
        memmap[0].base == 0x0 &&
        memmap[0].len >= 0x1000 &&
        memmap[0].type != MEMMAP_TYPE_RESERVED &&
        memmap[0].type != MEMMAP_TYPE_UNUSABLE
    )
    {
        if (unlikely(memmap_entry_count >= MEMMAP_MAX_ENTRIES))
        {
            panic("memmap: No space to split entry.");
        }

        const u64 split = 0x1000;
        u64 orig_len = memmap[0].len;
        enum memmap_entry_type orig_type = memmap[0].type;

        memmove(&memmap[2], &memmap[1], (memmap_entry_count - 1) * sizeof(memmap[0]));
        memmap_entry_count++;

        // Reserve 0x0-0xfff.
        memmap[0].len = split;
        memmap[0].type = MEMMAP_TYPE_RESERVED;

        // Preserve 0x1000..orig_len.
        memmap[1].base = split;
        memmap[1].len = orig_len - split;
        memmap[1].type = orig_type;

        pr_debug(
            "memmap: Updated %p-%p from %s to %s.\n",
            (void *)memmap[0].base,
            (void *)(memmap[0].base + memmap[0].len - 1),
            memmap_entry_type_to_string(orig_type),
            memmap_entry_type_to_string(memmap[0].type)
        );
    }
}

const char *memmap_entry_type_to_string(enum memmap_entry_type type)
{
    switch (type)
    {
        case MEMMAP_TYPE_USABLE:
            return "usable";
        case MEMMAP_TYPE_RESERVED:
            return "reserved";
        case MEMMAP_TYPE_ACPI_RECLAIMABLE:
            return "ACPI reclaimable";
        case MEMMAP_TYPE_ACPI_NVS:
            return "ACPI NVS";
        case MEMMAP_TYPE_UNUSABLE:
            return "unusable";
        case MEMMAP_TYPE_UNKNOWN:
        default:
            return "unknown";
    }
}
