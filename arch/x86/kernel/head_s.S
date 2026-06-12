/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>
#include <asm/paging.h>

.intel_syntax noprefix

.section .kernel.start, "a", @progbits
SYM_BEGIN_LOCAL_U(kernel_start)
    .ascii "KERNEL"                 /* Signature */
    .long kernel_end - kernel_start /* Size */
    .quad paddr(kentry)             /* Entry point */
SYM_END(kernel_start)

.section .kernel.end, "", @nobits
SYM_BEGIN_LOCAL_U(kernel_end)       /* Placed at the end by the linker. */

.section .text.entry, "ax", @progbits

FUNC_BEGIN(kentry)
    endbr64

    /* Were we loaded by a compliant bootloader? */
    cmp    eax, 0xc0ffee64
    jne    1f

    /*
     * RDI contains the physical address of the boot_info structure
     * provided by the bootloader. Preserve it in R15.
     */
    mov    r15, rdi

    /* Set up temporary stack. */
    mov    rsp, offset paddr(__stack_top)
    and    rsp, -8
    mov    rbp, rsp

    /* Load our page tables. */
    mov    rax, offset paddr(boot_pml4)
    mov    cr3, rax

    mov    rax, offset hh_entry
    jmp    rax

1:  mov    rdi, offset paddr(bad_bootloader)
    call   panic
FUNC_END(kentry)

FUNC_BEGIN(hh_entry)
    endbr64

    /* Move our stack to kernel space. */
    add    rsp, KERNEL_VIRT_BASE
    and    rsp, -8
    mov    rbp, rsp

    /* Remove the identity mapping. */
    xor    eax, eax
    movabs [boot_pml4], rax
    mov    rax, cr3
    mov    cr3, rax

    /* TODO: Map the framebuffer, probably somewhere like 0xffff8000... */

    lgdt   boot_gdt_ptr

    push   0x8
    lea    rax, [rip + .Lreload_cs]
    push   rax
    retfq

.Lreload_cs:
    mov    eax, 0x10
    mov    ss, ax

    xor    eax, eax
    mov    ds, ax
    mov    es, ax
    mov    fs, ax
    mov    gs, ax

    lldt   null_ldt_ptr

    /*
     * Move our boot_info pointer to kernel space and 
     * store it in a variable for easy access in C code.
     */
    mov    rax, r15
    add    rax, KERNEL_VIRT_BASE
    mov    [rip + boot_info], rax

    call   arch_kmain
FUNC_END(hh_entry)

.section .rodata

OBJ_BEGIN(bad_bootloader)
    .asciz "Bootloader is not compliant with the Torus boot protocol."
OBJ_END(bad_bootloader)

OBJ_BEGIN(boot_gdt)
    .quad 0

    /* Kernel code and data. */
    .quad 0x00af9a000000ffff
    .quad 0x00cf92000000ffff
OBJ_END(boot_gdt)

SYM_BEGIN_LOCAL_U(boot_gdt_end)

OBJ_BEGIN(boot_gdt_ptr)
    .word boot_gdt_end - boot_gdt - 1
    .quad boot_gdt
OBJ_END(boot_gdt_ptr)

.section .bss, "aw", @nobits

OBJ_BEGIN(__stack)
    .space 0x4000
OBJ_END(__stack)
SYM_BEGIN_U(__stack_top)

/* Stack for #DF handler. */
OBJ_BEGIN(__stack_df)
    .space 0x400
OBJ_END(__stack_df)
SYM_BEGIN_U(__stack_df_top)

/* Stack for #SS handler. */
OBJ_BEGIN(__stack_ss)
    .space 0x400
OBJ_END(__stack_ss)
SYM_BEGIN_U(__stack_ss_top)

OBJ_BEGIN(boot_info)
    .space 8
OBJ_END(boot_info)

OBJ_BEGIN(null_ldt_ptr)
    .space 10
OBJ_END(null_ldt_ptr)
