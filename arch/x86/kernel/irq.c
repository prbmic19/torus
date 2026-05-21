/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm/regs.h>
#include <asm/idt.h>
#include <kernel/irq.h>
#include <drivers/pic.h>

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

void irq_init(void)
{
    pic_init();

    idt_set_gate(32, irq0);
    idt_set_gate(33, irq1);
    idt_set_gate(34, irq2);
    idt_set_gate(35, irq3);
    idt_set_gate(36, irq4);
    idt_set_gate(37, irq5);
    idt_set_gate(38, irq6);
    idt_set_gate(39, irq7);
    idt_set_gate(40, irq8);
    idt_set_gate(41, irq9);
    idt_set_gate(42, irq10);
    idt_set_gate(43, irq11);
    idt_set_gate(44, irq12);
    idt_set_gate(45, irq13);
    idt_set_gate(46, irq14);
    idt_set_gate(47, irq15);
}

void irq_main_handler(struct regs *regs)
{
    irq_dispatch(regs->orig_rax, regs);
    pic_eoi(regs->orig_rax);
}
