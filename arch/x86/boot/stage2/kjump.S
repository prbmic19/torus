/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

#define MSR_EFER 0xc0000080
#define KERNEL_ADDRESS 0x100000

.intel_syntax noprefix

.section .text, "ax", @progbits

.code32

FUNC_BEGIN(prepare_kernel_jump)
    call   enter_long_mode

1:  mov    eax, 0xb16ba115
    mov    ecx, 1

1:  hlt
    jmp    1b
FUNC_END(prepare_kernel_jump)

FUNC_BEGIN_LOCAL(enter_long_mode)
    /* Point CR3 to our identity-mapped PML4 table. */
    lea    eax, [pml4]
    mov    cr3, eax

    /* Enable PAE. */
    mov    eax, cr4
    or     eax, 1 << 5
    mov    cr4, eax

    /* Enable LME. */
    mov    ecx, MSR_EFER
    rdmsr
    or     eax, 1 << 8
    wrmsr

    /* Enable paging. */
    mov    eax, cr0
    or     eax, 1 << 31
    mov    cr0, eax

    jmp    0x8, lm_entry
FUNC_END(enter_long_mode)

.code64

FUNC_BEGIN_LOCAL(lm_entry)
    mov    ax, 0x10
    mov    ds, ax
    mov    ss, ax
    mov    es, ax

    xor    ax, ax
    mov    fs, ax
    mov    gs, ax

    lea    rsp, [rip + stack_top]
    and    rsp, -16
    mov    rbp, rsp

    /* Remove stale values from protected mode. */
    xor    eax, eax
    xor    ecx, ecx
    xor    edx, edx
    xor    ebx, ebx
    xor    esi, esi
    xor    edi, edi
    /* Clear the newly accessible registers. */
    xor    r8d, r8d
    xor    r9d, r9d
    xor    r10d, r10d
    xor    r11d, r11d
    xor    r12d, r12d
    xor    r13d, r13d
    xor    r14d, r14d
    xor    r15d, r15d

    /* Do we have a valid kernel? */
    mov    ecx, 6
    mov    rdi, KERNEL_ADDRESS
    lea    rsi, [rip + kernel_magic]
    repe cmpsb
    jne    1f

    cld

    /* Now, transfer control to the kernel. */
    mov    rax, [KERNEL_ADDRESS + 10]
    call   rax

1:  mov    eax, 0xb16ba115
    mov    ecx, 2

1:  hlt
    jmp    1b
FUNC_END(lm_entry)

.section .rodata, "a", @progbits

OBJ_BEGIN_LOCAL(kernel_magic)
    .ascii "KERNEL"
OBJ_END(kernel_magic)
