/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

.intel_syntax noprefix

.section .kernel.start, "a", @progbits
SYM_BEGIN_LOCAL_U(kernel_start)
    .ascii "KERNEL"                 /* Signature */
    .long kernel_end - kernel_start /* Size */
    .quad kentry                    /* Entry point */
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

    /* Set up stack. */
    lea    rsp, [rip + __stack_top]
    and    rsp, -8
    mov    rbp, rsp

    /* Clear BSS. */
    lea    rdi, [rip + __bss_start]
    lea    rcx, [rip + __bss_end]
    sub    rcx, rdi
    xor    eax, eax
    rep stosb

    /*
     * In addition to preserving the physical address of the boot_info
     * structure in R15, preserve it in a variable for easy access in C
     * code.
     */
    mov    [rip + boot_info], r15
    
    call   arch_kmain

1:  lea    rdi, [rip + bad_bootloader]
    call   panic
FUNC_END(kentry)

.section .rodata

OBJ_BEGIN(bad_bootloader)
    .asciz "Bootloader is not compliant with the Torus boot protocol."
OBJ_END(bad_bootloader)

.section .bss, "aw", @nobits

OBJ_BEGIN(__stack)
    .space 0x4000
OBJ_END(__stack)
SYM_BEGIN_U(__stack_top)

OBJ_BEGIN(boot_info)
    .space 8
OBJ_END(boot_info)
