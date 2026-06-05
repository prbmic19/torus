/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

/* 
 * Get boot information while we have access to the BIOS.
 * Then get the kernel command line. Hardcode it for now.
 */

/*
 * Layout of the boot info structure:
 *  Offset      Size (B)    Description
 *  0x00        4           Signature "INFO"
 *  0x04        4           Implemented Torus boot protocol version
 *  0x08        4           Padding
 *  0x0c        4           E820 map entry count
 *  0x10        8           E820 map pointer
 *  0x18        8           RSD pointer
 *  0x20        8           Framebuffer info pointer
 *  0x28        8           Kernel command line pointer
 */

#define BI_SIGNATURE   0x00
#define BI_BOOTPRTCL   0x04
#define BI_E820_COUNT  0x0c
#define BI_E820_PTR    0x10
#define BI_RSDP        0x18
#define BI_FB_INFO_PTR 0x20
#define BI_CMDLINE_PTR 0x28

#define BOOT_INFO_SIZE 48

/*
 * Layout of an E820 entry:
 *  Offset      Size (B)    Description
 *  0x00        8           Base address
 *  0x08        8           Length
 *  0x10        4           Type
 */

#define E820_BASE     0x00
#define E820_LEN      0x08
#define E820_TYPE     0x10

#define E820_MAX_ENTRIES  128
#define E820_MAX_SIZE     (E820_MAX_ENTRIES * 24)

/*
 * Layout of the framebuffer info structure:
 *  Offset      Size (B)    Description
 *  0x00        8           Address
 *  0x08        4           Pitch
 *  0x0c        4           Width
 *  0x10        4           Height
 *  0x14        4           Bits per pixel
 *  0x18        4           Red size
 *  0x1c        4           Red shift
 *  0x20        4           Green size
 *  0x24        4           Green shift
 *  0x28        4           Blue size
 *  0x2c        4           Blue shift
 *  0x30        4           Reserved size
 *  0x34        4           Reserved shift
 */

#define FB_ADDR        0x00
#define FB_PITCH       0x08
#define FB_WIDTH       0x0c
#define FB_HEIGHT      0x10
#define FB_BPP         0x14
#define FB_RED_SIZE    0x18
#define FB_RED_SHIFT   0x1c
#define FB_GREEN_SIZE  0x20
#define FB_GREEN_SHIFT 0x24
#define FB_BLUE_SIZE   0x28
#define FB_BLUE_SHIFT  0x2c
#define FB_RSVD_SIZE   0x30
#define FB_RSVD_SHIFT  0x34

#define FB_INFO_SIZE 52

.intel_syntax noprefix

.section .stage2.start, "a", @progbits
SYM_BEGIN_LOCAL_U(stage2_start)
    .ascii "STAGE2"                 /* Signature */
    .long stage2_end - stage2_start /* Size */
    .word  stage2_entry             /* Pad entry point to 8 bytes. */
    .word  0
    .long  0
SYM_END(stage2_start)

.section .stage2.end, "", @nobits   /* Placed at the end by the linker. */
SYM_BEGIN_LOCAL_U(stage2_end)

.code16

.section .text.entry, "ax", @progbits

FUNC_BEGIN(stage2_entry)
    lea    sp, [__stack_top]
    and    sp, -8

    cld

    /* Clear BSS. */
    lea    di, [__bss_start]
    lea    cx, [__bss_end]
    sub    cx, di
    xor    ax, ax
    rep stosb

    /* Verify a few things first. */
    call   verify_cpuid
    call   verify_long

    call   get_boot_info

    /* Store the pointer to our boot_info strucuture. */
    xor    edi, edi
    lea    di, [boot_info]
    mov    [boot_info_ptr], edi

    call   enter_protected_mode
FUNC_END(stage2_entry)

/*
 * Redefine rm_print because we lost access to it.
 * Same arguments as in the first stage.
 */
FUNC_BEGIN_LOCAL(rm_print)
    pusha
1:  lodsb
    test   al, al
    jz     2f
    mov    ah, 0x0e
    int    0x10
    jmp    1b
2:  popa
    ret
FUNC_END(rm_print)

FUNC_BEGIN_LOCAL(verify_cpuid)
    push   ax

    /* Code taken from https://wiki.osdev.org/CPUID */
    pushfd
    pushfd
    xor    dword ptr [esp], 1 << 21
    popfd
    pushfd
    pop    eax
    xor    eax, [esp]
    popfd
    test   eax, 1 << 21
    jz     1f

    pop    ax
    ret

1:  lea    si, [err_no_cpuid]
    call   rm_print

1:  hlt
    jmp    1b
FUNC_END(verify_cpuid)

FUNC_BEGIN_LOCAL(verify_long)
    pusha

    mov    eax, 0x80000000
    cpuid
    cmp    eax, 0x80000001
    jb     1f
    mov    eax, 0x80000001
    cpuid
    test   edx, 1 << 29
    jz     1f

    popa
    ret

1:  lea    si, [err_no_long]
    call   rm_print

1:  hlt
    jmp    1b
FUNC_END(verify_long)

FUNC_BEGIN_LOCAL(get_boot_info)
    pusha

    /* Write the signature "INFO". */
    mov    byte ptr [boot_info + BI_SIGNATURE + 0], 'I'
    mov    byte ptr [boot_info + BI_SIGNATURE + 1], 'N'
    mov    byte ptr [boot_info + BI_SIGNATURE + 2], 'F'
    mov    byte ptr [boot_info + BI_SIGNATURE + 3], 'O'

    mov    dword ptr [boot_info + BI_BOOTPRTCL], 1

    /* Pass the command line. */
    xor    eax, eax
    lea    ax, [cmdline]
    mov    [boot_info + BI_CMDLINE_PTR], eax

    call   get_e820_map
    call   get_rsdp
    call   get_fb

    popa
    ret
FUNC_END(get_boot_info)

/* E820 memory map. */
FUNC_BEGIN_LOCAL(get_e820_map)
    pusha

    lea    di, [e820_map]
    xor    ebx, ebx
    mov    edx, 0x534d4150  /* "SMAP" */
    xor    ebp, ebp         /* Use this as our entry count. */

    mov    eax, 0xe820
    mov    ecx, 20
    int    0x15
    jc     4f

    mov    edx, 0x534d4150
    cmp    eax, edx
    jne    5f
    jmp    2f

1:  mov    eax, 0xe820
    mov    ecx, 20
    int    0x15
    jc     3f

2:  inc    ebp
    cmp    ebp, E820_MAX_ENTRIES
    je     5f
    add    di, 20
    test   ebx, ebx
    jnz    1b

3:  mov    [boot_info + BI_E820_COUNT], ebp
    jmp    5f

4:  mov    dword ptr [boot_info + BI_E820_COUNT], 0

5:  xor    eax, eax
    lea    ax, [e820_map]
    mov    [boot_info + BI_E820_PTR], eax
    popa
    ret
FUNC_END(get_e820_map)

/* Framebuffer info. */
FUNC_BEGIN_LOCAL(get_fb)
    pusha
    push   fs

    lea    di, [vbe_info]

    mov    byte ptr [di + 0], 'V'
    mov    byte ptr [di + 1], 'B'
    mov    byte ptr [di + 2], 'E'
    mov    byte ptr [di + 3], '2'

    mov    ax, 0x4f00
    int    0x10

    cmp    ax, 0x004f
    jne    4f

    /* Mode list pointer. */
    mov    bx, [vbe_info + 0x0e]
    mov    ax, [vbe_info + 0x10]
    mov    fs, ax

1:  mov    cx, fs:[bx]

    cmp    cx, 0xffff
    je     4f

    /* Get mode information. */

    lea    di, [vbe_mode_info]

    mov    ax, 0x4f01
    int    0x10

    cmp    ax, 0x004f
    jne    3f

    /* Is it a supported and linear framebuffer? */
    test   word ptr [vbe_mode_info + 0x00], (1 << 0) | (1 << 7)
    jz     3f

    /* Is the resolution 1024x768x32? */

    cmp    word ptr [vbe_mode_info + 0x12], 1024
    jne    3f
    cmp    word ptr [vbe_mode_info + 0x14], 768
    jne    3f
    cmp    byte ptr [vbe_mode_info + 0x19], 32
    jne    3f

    mov    ax, 0x4f02
    mov    bx, cx
    or     bx, 1 << 14 /* Linear framebuffer. */
    int    0x10

    cmp    ax, 0x004f
    jne    2f

    mov    eax, [vbe_mode_info + 0x28]
    mov    [fb_info + FB_ADDR], eax

    movzx  eax, word ptr [vbe_mode_info + 0x10]
    mov    [fb_info + FB_PITCH], eax

    movzx  eax, word ptr [vbe_mode_info + 0x12]
    mov    [fb_info + FB_WIDTH], eax

    movzx  eax, word ptr [vbe_mode_info + 0x14]
    mov    [fb_info + FB_HEIGHT], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x19]
    mov    [fb_info + FB_BPP], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x1f]
    mov    [fb_info + FB_RED_SIZE], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x20]
    mov    [fb_info + FB_RED_SHIFT], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x21]
    mov    [fb_info + FB_GREEN_SIZE], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x22]
    mov    [fb_info + FB_GREEN_SHIFT], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x23]
    mov    [fb_info + FB_BLUE_SIZE], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x24]
    mov    [fb_info + FB_BLUE_SHIFT], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x25]
    mov    [fb_info + FB_RSVD_SIZE], eax

    movzx  eax, byte ptr [vbe_mode_info + 0x26]
    mov    [fb_info + FB_RSVD_SHIFT], eax

    xor    eax, eax
    lea    ax, [fb_info]
    mov    [boot_info + BI_FB_INFO_PTR], eax

2:  jmp    4f

3:  add    bx, 2
    jmp    1b

4:  pop    fs
    popa
    ret
FUNC_END(get_fb)

/* RSDP. */
FUNC_BEGIN_LOCAL(get_rsdp)
    pusha
    push   es

    mov    ax, 0xe000
    mov    es, ax
    xor    di, di
    mov    ecx, 0x10000 /* First 64 KiB. */
    call   find_rsdp

    /* Did we find it? */
    test   al, al
    jnz    2f

    /* Try the second 64 KiB. */
    mov    ax, 0xf000
    mov    es, ax
    xor    di, di
    mov    ecx, 0x10000
    call   find_rsdp

    /* Did we find it? */
    test   al, al
    jz     3f /* Give up. */

2:  mov    ax, es
    movzx  eax, ax
    shl    eax, 4
    movzx  edi, di
    add    eax, edi
    mov    [boot_info + BI_RSDP], eax

3:  pop    es
    popa
    ret
FUNC_END(get_rsdp)

/* 
 * Find RSDP signature "RSD PTR ".
 * Arguments:
 * ES:DI = start
 * ECX = bytes to scan
 * Returns:
 * ES:DI = location if found, AL = 1, else AL = 0
 */
FUNC_BEGIN_LOCAL(find_rsdp)
    push   cx
    push   bp
    push   si

    lea    si, [rsdp_signature]
    mov    ebp, ecx

1:  cmp    ebp, 8
    jb     3f

    push   si
    push   di
    mov    cx, 8 /* 8 characters in the signature. */
    repe cmpsb
    pop    di
    pop    si
    je     2f

    add    di, 16
    sub    ebp, 16
    jmp    1b

2:  mov    al, 1
    jmp    4f

3:  xor    al, al

4:  pop    si
    pop    bp
    pop    cx
    ret
FUNC_END(get_rsdp)

.section .rodata, "a", @progbits

OBJ_BEGIN_LOCAL(cmdline)
    .asciz CMDLINE
OBJ_END(cmdline)

OBJ_BEGIN_LOCAL(rsdp_signature)
    .ascii "RSD PTR "
OBJ_END(rsdp_signature)

OBJ_BEGIN_LOCAL(err_no_cpuid)
    .asciz "Processor does not support CPUID.\r\n"
OBJ_END(err_no_cpuid)

OBJ_BEGIN_LOCAL(err_no_long)
    .asciz "Processor does not support long mode.\r\n"
OBJ_END(err_no_long)

.section .bss, "aw", @nobits

OBJ_BEGIN_LOCAL(__stack)
    .space 0x400
OBJ_END(__stack)
/* Global because we'll reuse it. */
SYM_BEGIN_U(__stack_top)

OBJ_BEGIN(boot_info_ptr)
    .space 4
OBJ_END(boot_info_ptr)

OBJ_BEGIN_LOCAL(boot_info)
    .space BOOT_INFO_SIZE
OBJ_END(boot_info)

OBJ_BEGIN_LOCAL(e820_map)
    .space E820_MAX_SIZE
OBJ_END(e820_map)

OBJ_BEGIN_LOCAL(fb_info)
    .space FB_INFO_SIZE
OBJ_END(fb_info)

OBJ_BEGIN_LOCAL(vbe_info)
    .space 512
OBJ_END(vbe_info)

OBJ_BEGIN_LOCAL(vbe_mode_info)
    .space 256
OBJ_END(vbe_mode_info)
