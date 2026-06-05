/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

#define MSR_EFER 0xc0000080
#define KERNEL_ADDRESS 0x100000

.intel_syntax noprefix

.section .text, "ax", @progbits

.code32

FUNC_BEGIN(prepare_kernel_jump)
    /* Is PAE supported? */
    mov    eax, 1
    cpuid
    test   edx, 1 << 6
    jz     1f

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
    and    eax, ~((1 << 0) | (1 << 1))
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

    lea    rsp, [rip + __stack_top]
    and    rsp, -8

    /* Do we have a valid kernel? */
    mov    ecx, 6
    mov    rdi, KERNEL_ADDRESS
    lea    rsi, [rip + kernel_signature]
    repe cmpsb
    jne    1f

    /* Clear IF and VM. */
    pushfq
    and    qword ptr [rsp], ~((1 << 9) | (1 << 17))
    popfq

    xor    ebx, ebx
    xor    ebp, ebp
    xor    r12d, r12d
    xor    r13d, r13d
    xor    r14d, r14d
    xor    r15d, r15d

    /* Magic number that the kernel expects. */
    mov    eax, 0xc0ffee64

    /* Pass the 4-byte boot info pointer and zero-extend it to 8 bytes. */
    mov    edi, [rip + boot_info_ptr]

    /* Now, transfer control to the kernel. */
    call   qword ptr [KERNEL_ADDRESS + 10]

1:  mov    eax, 0xb16ba115
    mov    ecx, 2

1:  hlt
    jmp    1b
FUNC_END(lm_entry)

.section .rodata, "a", @progbits

OBJ_BEGIN_LOCAL(kernel_signature)
    .ascii "KERNEL"
OBJ_END(kernel_signature)
