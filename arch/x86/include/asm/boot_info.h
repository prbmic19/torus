/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_X86_BOOT_INFO_H
#define ASM_X86_BOOT_INFO_H

#include <torus/compiler.h>
#include <torus/types.h>

#define MAX_E820_ENTRIES 256

struct e820_entry
{
    u64 base;
    u64 length;
    u32 type;
    u32 extended_attributes;
} __packed;

struct boot_info
{
    u8 signature[4];
    u8 e820_entry_count;
    u8 disk_drive_count;
    u16 base_mem_size;
    u32 timer_ticks;
    u8 keyboard_flags;
    u8 video_mode;
    u16 video_columns;
    u16 com_ports[4];
    u16 lpt_ports[3];
    u32 ebda_address;
    u32 rsd_pointer;
    struct e820_entry e820_entries[MAX_E820_ENTRIES];
} __packed;

#endif // ASM_X86_BOOT_INFO_H
