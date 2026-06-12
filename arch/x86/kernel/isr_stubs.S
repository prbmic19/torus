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

.macro EXCEPTION_NOERRCODE except_no, dest_stub
FUNC_BEGIN(exception\except_no)
    push   0
    push   \except_no
    jmp    \dest_stub
FUNC_END(exception\except_no)
.endm

.macro EXCEPTION_ERRCODE except_no, dest_stub
FUNC_BEGIN(exception\except_no)
    push   \except_no
    jmp    \dest_stub
FUNC_END(exception\except_no)
.endm

.macro IRQ irq_no
FUNC_BEGIN(irq\irq_no)
    push   0
    push   \irq_no
    jmp    irq_common_stub
FUNC_END(irq\irq_no)
.endm

FUNC_BEGIN(exception_common_stub)
    PUSH_REGS
    mov    rdi, rsp
    call   exception_main_handler
    POP_REGS
    add    rsp, 16
    iretq
FUNC_END(exception_common_stub)

FUNC_BEGIN(df_stub)
    PUSH_REGS
    mov    rdi, rsp
    call   exception_df_handler
    POP_REGS
    add    rsp, 16
    iretq
FUNC_END(df_stub)

FUNC_BEGIN(ss_stub)
    PUSH_REGS
    mov    rdi, rsp
    call   exception_ss_handler
    POP_REGS
    add    rsp, 16
    iretq
FUNC_END(ss_stub)

FUNC_BEGIN(irq_common_stub)
    PUSH_REGS
    mov    rdi, rsp
    call   irq_main_handler
    POP_REGS
    add    rsp, 16
    iretq
FUNC_END(irq_common_stub)

/* CPU exceptions. */
EXCEPTION_NOERRCODE 0, exception_common_stub 
EXCEPTION_NOERRCODE 1, exception_common_stub
EXCEPTION_NOERRCODE 2, exception_common_stub
EXCEPTION_NOERRCODE 3, exception_common_stub
EXCEPTION_NOERRCODE 4, exception_common_stub
EXCEPTION_NOERRCODE 5, exception_common_stub
EXCEPTION_NOERRCODE 6, exception_common_stub
EXCEPTION_NOERRCODE 7, exception_common_stub
EXCEPTION_ERRCODE   8, df_stub
EXCEPTION_NOERRCODE 9, exception_common_stub
EXCEPTION_ERRCODE   10, exception_common_stub 
EXCEPTION_ERRCODE   11, exception_common_stub
EXCEPTION_ERRCODE   12, ss_stub
EXCEPTION_ERRCODE   13, exception_common_stub
EXCEPTION_ERRCODE   14, exception_common_stub
EXCEPTION_NOERRCODE 15, exception_common_stub
EXCEPTION_NOERRCODE 16, exception_common_stub
EXCEPTION_NOERRCODE 17, exception_common_stub
EXCEPTION_NOERRCODE 18, exception_common_stub
EXCEPTION_NOERRCODE 19, exception_common_stub
EXCEPTION_NOERRCODE 20, exception_common_stub
EXCEPTION_NOERRCODE 21, exception_common_stub
EXCEPTION_NOERRCODE 22, exception_common_stub
EXCEPTION_NOERRCODE 23, exception_common_stub
EXCEPTION_NOERRCODE 24, exception_common_stub
EXCEPTION_NOERRCODE 25, exception_common_stub
EXCEPTION_NOERRCODE 26, exception_common_stub
EXCEPTION_NOERRCODE 27, exception_common_stub
EXCEPTION_NOERRCODE 28, exception_common_stub
EXCEPTION_NOERRCODE 29, exception_common_stub
EXCEPTION_NOERRCODE 30, exception_common_stub
EXCEPTION_NOERRCODE 31, exception_common_stub

/* Hardware interrupts. */
IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
