/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef X86_ASM_IDT_H
#define X86_ASM_IDT_H

#include <torus/compiler.h>
#include <torus/types.h>

#define IDT_ENTRIES 256

struct idt_ptr
{
    u16 limit;
    unsigned long base;
} __packed;

struct idt_entry
{
    u16 offset_low;
    u16 cs_selector;
    u8 ist; // Plus 5 reserved bits.
    u8 type_attributes;
    u16 offset_mid;
    u32 offset_high;
    u32 reserved;
} __packed;

extern void idt_set_gate(int gate, void (*handler_address)(void));
extern void idt_load(void);
extern void idt_init(void);

#endif // X86_ASM_IDT_H
