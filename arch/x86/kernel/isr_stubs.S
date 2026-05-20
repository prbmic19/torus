/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

.intel_syntax noprefix

.section .text, "ax", @progbits

.macro PUSH_REGS
    push   rdi
    push   rsi
    push   rdx
    push   rcx
    push   rax
    push   r8
    push   r9
    push   r10
    push   r11
    push   rbx
    push   rbp
    push   r12
    push   r13
    push   r14
    push   r15
.endm

.macro POP_REGS
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    rbp
    pop    rbx
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    pop    rax
    pop    rcx
    pop    rdx
    pop    rsi
    pop    rdi
.endm

.macro ISR_NOERRCODE int_no
FUNC_BEGIN(isr\int_no)
    cli
    push   0
    push   \int_no
    jmp    isr_common_stub
FUNC_END(isr\int_no)
.endm

.macro ISR_ERRCODE int_no
FUNC_BEGIN(isr\int_no)
    cli
    push   \int_no
    jmp    isr_common_stub
FUNC_END(isr\int_no)
.endm

FUNC_BEGIN(isr_common_stub)
    PUSH_REGS    

    /* 
     * Setup arguments for ISR handler.
     * RDI = pointer to a struct regs
     */
    mov    rdi, rsp
    call   isr_handler

    POP_REGS
    add    rsp, 16
    iretq
FUNC_END(isr_common_stub)

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
