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

void dump_context(const struct regs *regs)
{
    kprintf("Context:\n");

    // Dump GPRs.
    kprintf("  rax: 0x%lx\n", regs->rax);
    kprintf("  rbx: 0x%lx\n", regs->rbx);
    kprintf("  rcx: 0x%lx\n", regs->rcx);
    kprintf("  rdx: 0x%lx\n", regs->rcx);
    kprintf("  rsi: 0x%lx\n", regs->rsi);
    kprintf("  rdi: 0x%lx\n", regs->rdi);
    kprintf("  rbp: 0x%lx\n", regs->rbp);
    kprintf("  rsp: 0x%lx\n", regs->rsp);
    kprintf("   r8: 0x%lx\n", regs->r8);
    kprintf("   r9: 0x%lx\n", regs->r9);
    kprintf("  r10: 0x%lx\n", regs->r10);
    kprintf("  r11: 0x%lx\n", regs->r11);
    kprintf("  r12: 0x%lx\n", regs->r12);
    kprintf("  r13: 0x%lx\n", regs->r13);
    kprintf("  r14: 0x%lx\n", regs->r14);
    kprintf("  r15: 0x%lx\n", regs->r15);

    kprintf("  rip: 0x%lx\n", regs->rip);
    kprintf("  rflags: 0x%lx\n", regs->rflags);

    // Muehehe... read the control registers here.
    unsigned long cr;
    
    native_read_cr0(&cr);
    kprintf("  cr0: 0x%lx\n", cr);

    native_read_cr2(&cr);
    kprintf("  cr2: 0x%lx\n", cr);

    native_read_cr3(&cr);
    kprintf("  cr3: 0x%lx\n", cr);

    native_read_cr4(&cr);
    kprintf("  cr4: 0x%lx\n", cr);

    native_read_cr8(&cr);
    kprintf("  cr8: 0x%lx\n", cr);

    // Dump backtrace.
    kprintf(  "Backtrace:\n");
    unsigned long rbp = regs->rbp;
    for (int i = 0; i < 64; i++)
    {
        if (!is_valid_stack(rbp))
        {
            break;
        }

        kprintf("  #%d: 0x%lx\n", i, *(unsigned long *)(rbp + 8));
        rbp = *(unsigned long *)rbp;
    }
}
