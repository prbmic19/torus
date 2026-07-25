/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <kernel/kprintf.h>
#include <asm/regs.h>

static bool is_valid_stack(unsigned long sp)
{
    extern unsigned long __stack[];
    extern unsigned long __stack_top[];
    
    unsigned long stack_bottom = (unsigned long)__stack;
    unsigned long stack_top = (unsigned long)__stack_top;

    return sp >= stack_bottom && sp < stack_top && (sp & 0x7) == 0;
}

void backtrace_dump_warn(unsigned long frame)
{
    pr_warn("Backtrace:\n");
    for (unsigned int i = 0; is_valid_stack(frame); i++)
    {
        pr_warn("  #%d: %p\n", i, (void *)*(unsigned long *)(frame + 8));
        frame = *(unsigned long *)frame;
    }
}

void backtrace_dump(unsigned long frame)
{
    pr_emerg("Backtrace:\n");
    for (unsigned int i = 0; is_valid_stack(frame); i++)
    {
        pr_emerg("  #%d: %p\n", i, (void *)*(unsigned long *)(frame + 8));
        frame = *(unsigned long *)frame;
    }
}

void context_dump(const struct regs *regs)
{
    // This is really used only when the system has died. pr_emerg() seems appropriate.

    // Dump GPRs.
    pr_emerg("rax: 0x%lx\n", regs->rax);
    pr_emerg("rbx: 0x%lx\n", regs->rbx);
    pr_emerg("rcx: 0x%lx\n", regs->rcx);
    pr_emerg("rdx: 0x%lx\n", regs->rcx);
    pr_emerg("rsi: 0x%lx\n", regs->rsi);
    pr_emerg("rdi: 0x%lx\n", regs->rdi);
    pr_emerg("rbp: 0x%lx\n", regs->rbp);
    pr_emerg("rsp: 0x%lx\n", regs->rsp);
    pr_emerg(" r8: 0x%lx\n", regs->r8);
    pr_emerg(" r9: 0x%lx\n", regs->r9);
    pr_emerg("r10: 0x%lx\n", regs->r10);
    pr_emerg("r11: 0x%lx\n", regs->r11);
    pr_emerg("r12: 0x%lx\n", regs->r12);
    pr_emerg("r13: 0x%lx\n", regs->r13);
    pr_emerg("r14: 0x%lx\n", regs->r14);
    pr_emerg("r15: 0x%lx\n", regs->r15);

    pr_emerg("rip: 0x%lx\n", regs->rip);
    pr_emerg("rflags: 0x%lx\n", regs->rflags);

    pr_emerg("ss: 0x%lx\n", regs->ss);
    pr_emerg("cs: 0x%lx\n", regs->cs);

    pr_emerg("cr0: 0x%lx\n", read_cr0());
    pr_emerg("cr2: 0x%lx\n", read_cr2());
    pr_emerg("cr3: 0x%lx\n", read_cr3());
    pr_emerg("cr4: 0x%lx\n", read_cr4());
    pr_emerg("cr8: 0x%lx\n", read_cr8());

    // Dump backtrace.
    backtrace_dump(regs->rbp);
}
