/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef X86_ASM_REGS_H
#define X86_ASM_REGS_H

#include <torus/types.h>

struct regs
{
    // Callee-saved registers.
    unsigned long r15, r14, r13, r12, rbp, rbx;

    // Caller-saved registers.
    unsigned long r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;

    // Entry-specific value: syscall number, hardware interrupt number,
    // or CPU exception number. (Just preparing.)
    unsigned long orig_rax;

    // Error code for CPU exceptions (0 for everything else).
    unsigned long errcode;
    
    // Return frame for iret.
    unsigned long rip, cs, rflags, rsp, ss;
};

#endif // X86_ASM_REGS_H
