/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <asm_generic/linkage.h>

/* 
 * Gather all information we can while we have access to the BIOS.
 * Store it somewhere low and safe, at 0x680.
 */

#define BOOT_INFO_ADDR  0x680

/*
 * Layout of the boot_info structure:
 *  Offset      Size (B)    Description
 *  0x00        4           Signature "INFO"
 *  0x04        1           E820 entry count
 *  0x05        1           Disk drive count
 *  0x06        2           Base memory size (KiB)
 *  0x08        4           Timer ticks
 *  0x0c        1           Keyboard flags
 *  0x0d        1           Video mode
 *  0x0e        2           Video columns
 *  0x10        8           COM port addresses, (2 bytes * 4)
 *  0x18        6           LPT port addresses, (2 bytes * 3)
 *  0x1e        4           EBDA physical address
 *  0x22        4           RSD pointer (ACPI)
 *  0x26        6144        E820 entries (24 bytes * 256)
 */

/* Offsets of fields in the boot_info structure */
#define BI_SIGNATURE    0x00
#define BI_E820_ENT_CNT 0x04
#define BI_DISK_CNT     0x05
#define BI_BASE_MEM_SZ  0x06
#define BI_TIMER_TICKS  0x08
#define BI_KBD_FLAGS    0x0c
#define BI_VIDEO_MODE   0x0d
#define BI_VIDEO_COLS   0x0e
#define BI_COM_PORTS    0x10
#define BI_LPT_PORTS    0x18
#define BI_EBDA         0x1e
#define BI_RSDP         0x22
#define BI_E820_START   0x26

/* Offsets of fields in an E820 entry */
#define E820_BASE_LOW   0x00 
#define E820_BASE_HIGH  0x04
#define E820_LEN_LOW    0x08
#define E820_LEN_HIGH   0x0c
#define E820_TYPE       0x10
#define E820_ACPI       0x14

.intel_syntax noprefix

.section .stage2.start, "a", @progbits
SYM_BEGIN_LOCAL_U(stage2_start)
    .ascii "STAGE2"                 /* Magic */
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
    lea    sp, [stack_top]
    and    sp, -16

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

    call   gather_boot_info

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

FUNC_BEGIN_LOCAL(gather_boot_info)
    pusha

    /* Write the signature "INFO". */
    mov    byte ptr [BOOT_INFO_ADDR + BI_SIGNATURE + 0], 'I'
    mov    byte ptr [BOOT_INFO_ADDR + BI_SIGNATURE + 1], 'N'
    mov    byte ptr [BOOT_INFO_ADDR + BI_SIGNATURE + 2], 'F'
    mov    byte ptr [BOOT_INFO_ADDR + BI_SIGNATURE + 3], 'O'

    lea    di, [BOOT_INFO_ADDR + 4]
    mov    cx, 0x2000
    xor    ax, ax
    rep stosb

    /* Gather all the information. */
    call   get_e820_map
    call   get_disk_info
    call   get_mem_info_bda
    call   get_timer_ticks
    call   get_keyboard_flags
    call   get_video_info
    call   get_ports
    call   get_ebda
    call   get_rsdp

    popa
    ret
FUNC_END(gather_boot_info)

/* 
 * E820 memory map. 
 * Presently doesn't check if the loop went over 256 entries.
 */
FUNC_BEGIN_LOCAL(get_e820_map)
    pusha

    lea    di, [BOOT_INFO_ADDR + BI_E820_START]
    xor    ebx, ebx
    mov    edx, 0x534d4150  /* "SMAP" */
    xor    bp, bp           /* Use this as our entry count. */

    mov    eax, 0xe820
    mov    ecx, 24
    mov    dword ptr es:[di + 20], 1
    int    0x15
    jc     4f

    mov    edx, 0x534d4150
    cmp    eax, edx
    jne    5f
    jmp    2f

1:  mov    eax, 0xe820
    mov    ecx, 24
    mov    dword ptr es:[di + 20], 1
    int    0x15
    jc     3f

2:  inc    bp
    add    di, 24
    test   ebx, ebx
    jnz    1b

3:  mov    ax, bp
    mov    [BOOT_INFO_ADDR + BI_E820_ENT_CNT], al
    jmp    5f

4:  mov    byte ptr [BOOT_INFO_ADDR + BI_E820_ENT_CNT], 0

5:  popa
    ret
FUNC_END(get_e820_map)

/* Disk drive count. */
FUNC_BEGIN_LOCAL(get_disk_info)
    pusha

    mov    ah, 0x08
    mov    dl, [0x600]  /* Get boot drive. */
    int    0x13
    
    jc     1f
    test   ah, ah
    jnz    1f

    and    dl, 0x3f     /* Get maximum drive number. */
    inc    dl
    mov    [BOOT_INFO_ADDR + BI_DISK_CNT], dl
    jmp    2f

1:  mov    byte ptr [BOOT_INFO_ADDR + BI_DISK_CNT], 0

2:  popa
    ret
FUNC_END(get_disk_info)

/* Conventional memory size. */
FUNC_BEGIN_LOCAL(get_mem_info_bda)
    pusha
    int    0x12
    mov    [BOOT_INFO_ADDR + BI_BASE_MEM_SZ], ax
    popa
    ret
FUNC_END(get_mem_info_bda)

/* Timer ticks. */
FUNC_BEGIN_LOCAL(get_timer_ticks)
    push   ax
    mov    eax, [0x46c]
    mov    [BOOT_INFO_ADDR + BI_TIMER_TICKS], eax
    pop    ax
    ret
FUNC_END(get_timer_ticks)

/* Keyboard flags. */
FUNC_BEGIN_LOCAL(get_keyboard_flags)
    push   ax
    mov    al, [0x417]
    mov    [BOOT_INFO_ADDR + BI_KBD_FLAGS], al
    pop    ax
    ret
FUNC_END(get_keyboard_flags)

/* Video information. */
FUNC_BEGIN_LOCAL(get_video_info)
    push   ax

    mov    al, [0x449]  /* Current video mode */
    mov    [BOOT_INFO_ADDR + BI_VIDEO_MODE], al

    mov    ax, [0x44a]  /* Number of columns */
    mov    [BOOT_INFO_ADDR + BI_VIDEO_COLS], ax

    pop    ax
    ret
FUNC_END(get_video_info)

/* COM and LPT port addresses. */
FUNC_BEGIN_LOCAL(get_ports)
    pusha

    lea    si, [0x400]
    lea    di, [BOOT_INFO_ADDR + BI_COM_PORTS]
    mov    cx, 4
1:  mov    ax, [si]
    stosw
    add    si, 2
    loop   1b

    lea    si, [0x408]
    lea    di, [BOOT_INFO_ADDR + BI_LPT_PORTS]
    mov    cx, 3
1:  mov    ax, [si]
    stosw
    add    si, 2
    loop   1b

    popa
    ret
FUNC_END(get_ports)

/* EBDA physical address. */
FUNC_BEGIN_LOCAL(get_ebda)
    pusha
    mov    ax, [0x40e]
    movzx  eax, ax
    shl    eax, 4
    mov    [BOOT_INFO_ADDR + BI_EBDA], eax
    popa
    ret
FUNC_END(get_ebda)

/* ACPI RSD pointer. */
FUNC_BEGIN_LOCAL(get_rsdp)
    pusha
    push   es

    mov    ax, [0x40e]  /* EBDA segment */
    test   ax, ax
    jz     1f

    mov    es, ax
    xor    di, di
    mov    ecx, 0x1000
    call   find_rsdp

    test   ax, ax
    jnz    2f

    /* Scan BIOS ROM if we haven't found it yet. */
1:  mov    ax, 0xe000
    mov    es, ax
    xor    di, di
    mov    ecx, 128 * 1024
    call   find_rsdp

    /* Did we find it? */
    test   ax, ax
    jz     3f

2:  mov    ax, es
    movzx  eax, ax
    shl    eax, 4
    movzx  edi, di
    add    eax, edi
    mov    [BOOT_INFO_ADDR + BI_RSDP], eax

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
 * ES:DI = location if found, AX=1, else AX=0
 */
FUNC_BEGIN_LOCAL(find_rsdp)
    push   cx
    push   bp
    push   si
    push   di

    lea    si, [rsdp_signature]
    mov    ebp, ecx

1:  test   ebp, ebp
    jz     2f

    push   si
    push   di
    mov    cx, 8 /* 8 characters in the signature */
    repe cmpsb
    pop    di
    pop    si
    je     2f

    inc    di
    dec    ebp
    jmp    1b

2:  setz   al
    pop    di
    pop    si
    pop    bp
    pop    cx
    ret
FUNC_END(get_rsdp)

.section .rodata, "a", @progbits

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

OBJ_BEGIN_LOCAL(stack)
    .space 0x1000
OBJ_END(stack)
/* Not local because we'll reuse it. */
SYM_BEGIN_U(stack_top)
