/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <kernel/kprintf.h>
#include <asm/idt.h>
#include <asm/exception.h>
#include <asm/irq.h>

static struct idt_ptr idt_ptr;
static struct idt_entry idt_entries[IDT_ENTRIES];

void idt_set_gate(int gate, void (*handler_addr)(void), unsigned short selector, unsigned char ist, unsigned char type_attr)
{
    gate = gate > (int)(sizeof(idt_entries) - 1) ? (int)(sizeof(idt_entries) - 1) : gate;
    ist &= 0x7;

    unsigned long offset = (unsigned long)handler_addr;
    unsigned short o_low16 = offset & 0xffff;
    unsigned short o_mid16 = (offset >> 16) & 0xffff;
    unsigned int o_high32 = (offset >> 32) & 0xffffffff;

    idt_entries[gate] = (struct idt_entry){
        .offset_low = o_low16,
        .selector = selector,
        .ist = ist,
        .type_attr = type_attr,
        .offset_mid = o_mid16,
        .offset_high = o_high32
    };
}

void idt_load(void)
{
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base = (unsigned long)idt_entries;

    asm volatile ("lidt %0" : : "m"(idt_ptr) : "memory");
}

void idt_init(void)
{
    exception_init();
    irq_init();
    idt_load();
    pr_notice("IDT initialized.\n");
}
