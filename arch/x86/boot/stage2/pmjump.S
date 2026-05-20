/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

.intel_syntax noprefix

.section .text, "ax", @progbits

.code16

FUNC_BEGIN(enter_protected_mode)
    cli
    lgdt   gdt_ptr
    lidt   null_idt_ptr /* Be careful. */
    
    mov    eax, cr0
    or     eax, 1 << 0
    mov    cr0, eax

    jmp    0x18, pm_entry
FUNC_END(enter_protected_mode)

.code32

FUNC_BEGIN_LOCAL(pm_entry)
    mov    ax, 0x20
    mov    ds, ax
    mov    ss, ax
    mov    es, ax
    mov    fs, ax
    mov    gs, ax

    lea    esp, [stack_top]
    and    esp, -16
    mov    ebp, esp

    /* Remove stale values from real mode. */
    xor    eax, eax
    xor    ecx, ecx
    xor    edx, edx
    xor    ebx, ebx
    xor    esi, esi
    xor    edi, edi

    /* Clear BSS. */
    lea    edi, [__bss_start]
    lea    ecx, [__bss_end]
    sub    ecx, edi
    xor    eax, eax
    rep stosb

    call   pm_main

1:  mov    eax, 0xb16ba115
    xor    ecx, ecx

1:  hlt
    jmp    1b
FUNC_END(pm_entry)

.section .rodata, "a", @progbits

OBJ_BEGIN_LOCAL(gdt)
    .quad 0

    /* 64-bit descriptors (kernel code and data). */
    .quad 0x00af9a000000ffff
    .quad 0x00af92000000ffff

    /* 32-bit descriptors (kernel code and data). */
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
OBJ_END(gdt)

SYM_BEGIN_LOCAL_U(gdt_end)

OBJ_BEGIN_LOCAL(gdt_ptr)
    .word gdt_end - gdt - 1
    .long gdt
    .long 0     /* Workaround */
OBJ_END(gdt_ptr)

OBJ_BEGIN_LOCAL(null_idt_ptr)
    .word 0
    .quad 0
OBJ_END(null_idt_ptr)
