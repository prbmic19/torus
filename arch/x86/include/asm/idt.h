/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_X86_IDT_H
#define ASM_X86_IDT_H

#include <torus/compiler.h>

#define IDT_ENTRIES 256

struct idt_ptr
{
    unsigned short limit;
    unsigned long base;
} __packed;

struct idt_entry
{
    unsigned short offset_low;
    unsigned short selector;
    unsigned char ist; // Plus 5 reserved bits.
    unsigned char type_attributes;
    unsigned short offset_mid;
    unsigned int offset_high;
    unsigned int reserved;
} __packed;

extern void idt_set_gate(int gate, void (*handler_address)(void));
extern void idt_load(void);
extern void idt_init(void);

#endif // ASM_X86_IDT_H
