/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <torus/types.h>
#include <asm/idt.h>

struct idt_ptr idt_ptr;
struct idt_entry idt_entries[IDT_ENTRIES];

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

void idt_set_gate(int gate, void (*handler_address)(void))
{
    unsigned long offset = (unsigned long)handler_address;
    u16 low16 = (u16)(offset & 0xffff);
    u16 mid16 = (u16)((offset >> 16) & 0xffff);
    u32 high32 = (u32)((offset >> 32) & 0xffffffff);

    idt_entries[gate] = (struct idt_entry){
        .offset_low = low16,
        .cs_selector = 0x8, // Kernel CS selector.
        .type_attributes = 0x8e,
        .offset_mid = mid16,
        .offset_high = high32
    };
}

void idt_load(void)
{
    idt_ptr.limit = sizeof(idt_entries) - 1u;
    idt_ptr.base = (unsigned long)idt_entries;

    asm volatile ("lidt %0" : : "m"(idt_ptr));
}

void idt_init(void)
{
    idt_set_gate(0, isr0);
    idt_set_gate(1, isr1);
    idt_set_gate(2, isr2);
    idt_set_gate(3, isr3);
    idt_set_gate(4, isr4);
    idt_set_gate(5, isr5);
    idt_set_gate(6, isr6);
    idt_set_gate(7, isr7);
    idt_set_gate(8, isr8);
    idt_set_gate(9, isr9);
    idt_set_gate(10, isr10);
    idt_set_gate(11, isr11);
    idt_set_gate(12, isr12);
    idt_set_gate(13, isr13);
    idt_set_gate(14, isr14);
    idt_set_gate(15, isr15);
    idt_set_gate(16, isr16);
    idt_set_gate(17, isr17);
    idt_set_gate(18, isr18);
    idt_set_gate(19, isr19);
    idt_set_gate(20, isr20);
    idt_set_gate(21, isr21);
    idt_set_gate(22, isr22);
    idt_set_gate(23, isr23);
    idt_set_gate(24, isr24);
    idt_set_gate(25, isr25);
    idt_set_gate(26, isr26);
    idt_set_gate(27, isr27);
    idt_set_gate(28, isr28);
    idt_set_gate(29, isr29);
    idt_set_gate(30, isr30);
    idt_set_gate(31, isr31);

    idt_load();
}
