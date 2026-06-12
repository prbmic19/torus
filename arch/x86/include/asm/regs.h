/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_X86_REGS_H
#define ASM_X86_REGS_H

#include <torus/compiler.h>
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

// This clears the .orig_rax, .errcode, .cs, and .ss fields.
__always_inline static void context_store(struct regs *regs)
{
    // Save RIP first.
    asm volatile (
        "pushq %%rax\n\t"
        "leaq 0(%%rip), %%rax\n\t"
        "movq %%rax, %0\n\t"
        "popq %%rax"
        : "=m"(regs->rip)
        :
        : "memory"
    );

    // Save RFLAGS.
    asm volatile ("pushfq; popq %0" : "=m"(regs->rflags) : : "cc", "memory");

    // Save GPRs.
    asm volatile ("movq %%rax, %0" : "=m"(regs->rax));
    asm volatile ("movq %%rbx, %0" : "=m"(regs->rbx));
    asm volatile ("movq %%rcx, %0" : "=m"(regs->rcx));
    asm volatile ("movq %%rdx, %0" : "=m"(regs->rdx));
    asm volatile ("movq %%rsi, %0" : "=m"(regs->rsi));
    asm volatile ("movq %%rdi, %0" : "=m"(regs->rdi));
    asm volatile ("movq %%rbp, %0" : "=m"(regs->rbp));
    asm volatile ("movq %%rsp, %0" : "=m"(regs->rsp));
    asm volatile ("movq %%r8, %0" : "=m"(regs->r8));
    asm volatile ("movq %%r9, %0" : "=m"(regs->r9));
    asm volatile ("movq %%r10, %0" : "=m"(regs->r10));
    asm volatile ("movq %%r11, %0" : "=m"(regs->r11));
    asm volatile ("movq %%r12, %0" : "=m"(regs->r12));
    asm volatile ("movq %%r13, %0" : "=m"(regs->r13));
    asm volatile ("movq %%r14, %0" : "=m"(regs->r14));
    asm volatile ("movq %%r15, %0" : "=m"(regs->r15));

    regs->orig_rax = 0;
    regs->errcode = 0;
    regs->cs = 0;
    regs->ss = 0;
}

__always_inline static unsigned long read_cr0(void)
{
    unsigned long cr0;
    asm volatile ("movq %%cr0, %0" : "=r"(cr0));
    return cr0;
}

__always_inline static unsigned long read_cr2(void)
{
    unsigned long cr2;
    asm volatile ("movq %%cr2, %0" : "=r"(cr2));
    return cr2;
}

__always_inline static unsigned long read_cr3(void)
{
    unsigned long cr3;
    asm volatile ("movq %%cr3, %0" : "=r"(cr3));
    return cr3;
}

__always_inline static unsigned long read_cr4(void)
{
    unsigned long cr4;
    asm volatile ("movq %%cr4, %0" : "=r"(cr4));
    return cr4;
}

__always_inline static unsigned long read_cr8(void)
{
    unsigned long cr8;
    asm volatile ("movq %%cr8, %0" : "=r"(cr8));
    return cr8;
}

__always_inline static void write_cr0(unsigned long cr0)
{
    asm volatile ("movq %0, %%cr0" : : "r"(cr0));
}

__always_inline static void write_cr2(unsigned long cr2)
{
    asm volatile ("movq %0, %%cr2" : : "r"(cr2));
}

__always_inline static void write_cr3(unsigned long cr3)
{
    asm volatile ("movq %0, %%cr3" : : "r"(cr3));
}

__always_inline static void write_cr4(unsigned long cr4)
{
    asm volatile ("movq %0, %%cr4" : : "r"(cr4));
}

__always_inline static void write_cr8(unsigned long cr8)
{
    asm volatile ("movq %0, %%cr8" : : "r"(cr8));
}

extern void context_dump(const struct regs *regs);

#endif // ASM_X86_REGS_H
