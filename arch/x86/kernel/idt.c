/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <asm/idt.h>
#include <asm/exception.h>
#include <asm/irq.h>

static struct idt_ptr idt_ptr;
static struct idt_entry idt_entries[IDT_ENTRIES];

void idt_set_gate(int gate, void (*handler_address)(void))
{
    unsigned long offset = (unsigned long)handler_address;
    unsigned short low16 = offset & 0xffff;
    unsigned short mid16 = (offset >> 16) & 0xffff;
    unsigned int high32 = (offset >> 32) & 0xffffffff;

    idt_entries[gate] = (struct idt_entry){
        .offset_low = low16,
        .selector = 0x8, // Kernel CS selector.
        .type_attributes = 0x8e,
        .offset_mid = mid16,
        .offset_high = high32
    };
}

void idt_load(void)
{
    idt_ptr.limit = sizeof(idt_entries) - 1u;
    idt_ptr.base = (unsigned long)idt_entries;

    asm volatile ("lidt %0" : : "m"(idt_ptr) : "memory");
}

void idt_init(void)
{
    exception_init();
    irq_init();
    idt_load();
}
