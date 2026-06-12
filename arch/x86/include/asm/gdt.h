/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_X86_GDT_H
#define ASM_X86_GDT_H

#include <torus/compiler.h>

struct gdt_ptr
{
    unsigned short limit;
    unsigned long base;
} __packed;

struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_mid;
    unsigned char access;
    unsigned char limit_high : 4;
    unsigned char flags : 4;
    unsigned char base_high;
} __packed;

struct gdt_sys_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_mid;
    unsigned char access;
    unsigned char limit_high : 4;
    unsigned char flags : 4;
    unsigned char base_high;
    unsigned int base_upper;
    unsigned int _rsvd;
} __packed;

struct tss
{
    unsigned int _rsvd0;
    unsigned long rsp0;
    unsigned long rsp1;
    unsigned long rsp2;
    unsigned long _rsvd1;
    unsigned long ist[7];
    unsigned long _rsvd2;
    unsigned short _rsvd3;
    unsigned short io_map_base;
} __packed;

extern void gdt_set_entry(int desc, unsigned int limit, unsigned int base, unsigned char access, unsigned char flags);
extern void gdt_set_sys_entry(int desc, unsigned int limit, unsigned long base, unsigned char access, unsigned char flags);
extern void gdt_load(int tss_selector);
extern void gdt_init(void);

#endif // ASM_X86_GDT_H
