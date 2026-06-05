/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_X86_BOOT_INFO_H
#define ASM_X86_BOOT_INFO_H

#include <torus/compiler.h>
#include <torus/types.h>

#define E820_MAX_ENTRIES 128

#define E820_TYPE_USABLE   1
#define E820_TYPE_RESERVED 2
#define E820_TYPE_ACPI     3
#define E820_TYPE_NVS      4
#define E820_TYPE_UNUSABLE 5

#define TPRTCL_SUPPORTED_VERSION 1

struct e820_entry
{
    u64 base;
    u64 len;
    u32 type;
} __packed;

struct boot_info
{
    char signature[4];
    u32 boot_protocol_version;
    u8 _pad[4];
    u32 e820_entry_count;
    // These are all physical pointers.
    u64 e820_map_addr;
    u64 rsdp_addr;
    u64 fb_info_addr;
    u64 cmdline_addr;
} __packed;

#endif // ASM_X86_BOOT_INFO_H
