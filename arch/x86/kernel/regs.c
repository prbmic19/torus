/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <kernel/kprintf.h>
#include <asm/regs.h>

static bool is_valid_stack(unsigned long rsp)
{
    extern unsigned long __stack;
    extern unsigned long __stack_top;
    
    unsigned long stack_bottom = (unsigned long)&__stack;
    unsigned long stack_top = (unsigned long)&__stack_top;

    return rsp >= stack_bottom && rsp < stack_top && (rsp & 0x7) == 0;
}

void context_dump(const struct regs *regs)
{
    // This is really only used when the system has died. pr_emerg() seems appropriate.

    pr_emerg("Context:\n");

    // Dump GPRs.
    pr_emerg("    rax: 0x%lx\n", regs->rax);
    pr_emerg("    rbx: 0x%lx\n", regs->rbx);
    pr_emerg("    rcx: 0x%lx\n", regs->rcx);
    pr_emerg("    rdx: 0x%lx\n", regs->rcx);
    pr_emerg("    rsi: 0x%lx\n", regs->rsi);
    pr_emerg("    rdi: 0x%lx\n", regs->rdi);
    pr_emerg("    rbp: 0x%lx\n", regs->rbp);
    pr_emerg("    rsp: 0x%lx\n", regs->rsp);
    pr_emerg("     r8: 0x%lx\n", regs->r8);
    pr_emerg("     r9: 0x%lx\n", regs->r9);
    pr_emerg("    r10: 0x%lx\n", regs->r10);
    pr_emerg("    r11: 0x%lx\n", regs->r11);
    pr_emerg("    r12: 0x%lx\n", regs->r12);
    pr_emerg("    r13: 0x%lx\n", regs->r13);
    pr_emerg("    r14: 0x%lx\n", regs->r14);
    pr_emerg("    r15: 0x%lx\n", regs->r15);

    pr_emerg("    rip: 0x%lx\n", regs->rip);
    pr_emerg("    rflags: 0x%lx\n", regs->rflags);

    // Muehehe... read the control registers here.
    pr_emerg("    cr0: 0x%lx\n", read_cr0());
    pr_emerg("    cr2: 0x%lx\n", read_cr2());
    pr_emerg("    cr3: 0x%lx\n", read_cr3());
    pr_emerg("    cr4: 0x%lx\n", read_cr4());
    pr_emerg("    cr8: 0x%lx\n", read_cr8());

    // Dump backtrace.
    pr_emerg("    Backtrace:\n");
    unsigned long rbp = regs->rbp;
    for (int i = 0; is_valid_stack(rbp); i++)
    {
        pr_emerg("        #%d: 0x%lx\n", i, *(unsigned long *)(rbp + 8));
        rbp = *(unsigned long *)rbp;
    }
}
