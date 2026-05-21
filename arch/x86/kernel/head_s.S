/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

.intel_syntax noprefix

.section .kernel.start, "a", @progbits
SYM_BEGIN_LOCAL_U(kernel_start)
    .ascii "KERNEL"                 /* Magic */
    .long kernel_end - kernel_start /* Size */
    .quad kentry                    /* Entry point */
SYM_END(kernel_start)

.section .kernel.end, "", @nobits
SYM_BEGIN_LOCAL_U(kernel_end)       /* Placed at the end by the linker. */

.section .text.entry, "ax", @progbits

FUNC_BEGIN(kentry)
    endbr64

    lea    rsp, [rip + stack_top]
    and    rsp, -16
    mov    rbp, rsp

    /* Clear BSS. */
    lea    rdi, [rip + __bss_start]
    lea    rcx, [rip + __bss_end]
    sub    rcx, rdi
    xor    eax, eax
    rep stosb

    call   arch_kmain
FUNC_END(kentry)

.section .bss, "aw", @nobits

SYM_BEGIN_LOCAL(stack_bottom)
.space 0x8000
SYM_BEGIN_LOCAL_U(stack_top)
