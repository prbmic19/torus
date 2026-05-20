/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef X86_ASM_ISR_H
#define X86_ASM_ISR_H

#include <torus/types.h>

struct regs
{
    // Callee-saved registers.
    u64 r15, r14, r13, r12, rbp, rbx;

    // Caller-saved registers.
    u64 r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;

    // Entry-specific value: syscall number for syscalls, interrupt vector for
    // hardware interrupts, or error code for CPU exceptions. (Just preparing.)
    u64 orig_rax;

    // Return frame for iret.
    u64 rip, cs, rflags, rsp, ss;
};

#endif // X86_ASM_ISR_H
