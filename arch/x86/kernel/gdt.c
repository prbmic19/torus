/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <kernel/kprintf.h>
#include <kernel/build_bug.h>
#include <asm/gdt.h>

static struct gdt_ptr gdt_ptr;
static unsigned long gdt_entries[5]; // Just reinterpret this for gdt_entry and gdt_sys_entry.
static struct tss tss;
static unsigned int descriptors_no;

void gdt_set_entry(int entry, unsigned int limit, unsigned int base, unsigned char access, unsigned char flags)
{
    entry = entry > (int)(sizeof(gdt_entries) - 1) ? (int)(sizeof(gdt_entries) - 1) : entry;
    limit &= 0xfffff;
    flags &= 0xf;

    unsigned short l_low16 = limit & 0xffff;
    unsigned char l_high4 = (limit >> 16) & 0xf;

    unsigned short b_low16 = base & 0xffff;
    unsigned char b_mid8 = (base >> 16) & 0xff;
    unsigned char b_high8 = (base >> 24) & 0xff;

    *((struct gdt_entry *)&gdt_entries[entry]) = (struct gdt_entry){
        .limit_low = l_low16,
        .base_low = b_low16,
        .base_mid = b_mid8,
        .access = access,
        .limit_high = l_high4,
        .flags = flags,
        .base_high = b_high8
    };

    descriptors_no++;
}

void gdt_set_sys_entry(int entry, unsigned int limit, unsigned long base, unsigned char access, unsigned char flags)
{
    // System descriptors take up 2 entries (16 bytes), so ensure there is enough space.
    entry = entry > (int)(sizeof(gdt_entries) - 2) ? (int)(sizeof(gdt_entries) - 2) : entry;
    limit &= 0xfffff;
    flags &= 0xf;

    unsigned short l_low16 = limit & 0xffff;
    unsigned char l_high4 = (limit >> 16) & 0xf;

    unsigned short b_low16 = base & 0xffff;
    unsigned char b_mid8 = (base >> 16) & 0xff;
    unsigned char b_high8 = (base >> 24) & 0xff;
    unsigned int b_upper32 = (base >> 32) & 0xffffffff;

    *((struct gdt_sys_entry *)&gdt_entries[entry]) = (struct gdt_sys_entry){
        .limit_low = l_low16,
        .base_low = b_low16,
        .base_mid = b_mid8,
        .access = access,
        .limit_high = l_high4,
        .flags = flags,
        .base_high = b_high8,
        .base_upper = b_upper32
    };

    descriptors_no++;
}

static inline void tss_load(int selector)
{
    asm volatile ("ltr %0" : : "rm"((unsigned short)selector) : "memory"); 
}

void gdt_load(int tss_selector)
{
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base = (unsigned long)gdt_entries;

    asm volatile ("lgdt %0" : : "m"(gdt_ptr) : "memory");

    tss_load(tss_selector);
}

static void tss_init(void)
{
    extern unsigned long __stack_top[];
    extern unsigned long __stack_df_top[];
    extern unsigned long __stack_ss_top[];

    tss.rsp0 = (unsigned long)__stack_top;
    tss.ist[0] = (unsigned long)__stack_df_top;
    tss.ist[1] = (unsigned long)__stack_ss_top;
    tss.io_map_base = sizeof(tss);
}

void gdt_init(void)
{
    tss_init();

    gdt_set_entry(0, 0x0, 0x0, 0x0, 0x0);
    gdt_set_entry(1, 0xfffff, 0x0, 0x9a, 0xa); // 64-bit kernel code.
    gdt_set_entry(2, 0xfffff, 0x0, 0x92, 0xc); // Kernel data.
    gdt_set_sys_entry(                         // TSS.
        3,
        sizeof(tss) - 1,
        (unsigned long)&tss,
        0x89,
        0x0
    );

    gdt_load(3 * 8);

    pr_notice("GDT: %u descriptors.\n", descriptors_no);
}
