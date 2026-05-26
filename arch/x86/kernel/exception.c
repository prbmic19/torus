/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <kernel/console.h>
#include <asm/exception.h>
#include <asm/idt.h>
#include <asm/regs.h>

extern void exception0(void);
extern void exception1(void);
extern void exception2(void);
extern void exception3(void);
extern void exception4(void);
extern void exception5(void);
extern void exception6(void);
extern void exception7(void);
extern void exception8(void);
extern void exception9(void);
extern void exception10(void);
extern void exception11(void);
extern void exception12(void);
extern void exception13(void);
extern void exception14(void);
extern void exception15(void);
extern void exception16(void);
extern void exception17(void);
extern void exception18(void);
extern void exception19(void);
extern void exception20(void);
extern void exception21(void);
extern void exception22(void);
extern void exception23(void);
extern void exception24(void);
extern void exception25(void);
extern void exception26(void);
extern void exception27(void);
extern void exception28(void);
extern void exception29(void);
extern void exception30(void);
extern void exception31(void);

void exception_init(void)
{
    idt_set_gate(0, exception0);
    idt_set_gate(1, exception1);
    idt_set_gate(2, exception2);
    idt_set_gate(3, exception3);
    idt_set_gate(4, exception4);
    idt_set_gate(5, exception5);
    idt_set_gate(6, exception6);
    idt_set_gate(7, exception7);
    idt_set_gate(8, exception8);
    idt_set_gate(9, exception9);
    idt_set_gate(10, exception10);
    idt_set_gate(11, exception11);
    idt_set_gate(12, exception12);
    idt_set_gate(13, exception13);
    idt_set_gate(14, exception14);
    idt_set_gate(15, exception15);
    idt_set_gate(16, exception16);
    idt_set_gate(17, exception17);
    idt_set_gate(18, exception18);
    idt_set_gate(19, exception19);
    idt_set_gate(20, exception20);
    idt_set_gate(21, exception21);
    idt_set_gate(22, exception22);
    idt_set_gate(23, exception23);
    idt_set_gate(24, exception24);
    idt_set_gate(25, exception25);
    idt_set_gate(26, exception26);
    idt_set_gate(27, exception27);
    idt_set_gate(28, exception28);
    idt_set_gate(29, exception29);
    idt_set_gate(30, exception30);
    idt_set_gate(31, exception31);
}

__noreturn void exception_main_handler(struct regs *regs)
{
    (void)regs;
    
    console_puts("[PANIC] CPU exception occurred. Halting execution.\n"); 
    
    // Halt and catch fire!
    asm volatile ("cli" : : : "memory");
    while (1)
    {
        asm volatile ("hlt");
    }
}
