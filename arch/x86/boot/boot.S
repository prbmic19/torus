/* SPDX-License-Identifier: GPL-3.0-or-later */

.intel_syntax noprefix

/* Store boot drive somewhere that's low and known. */
#define BOOT_DRIVE_ADDR 0x600

#define STAGE2_SEGMENT 0x0000
#define STAGE2_OFFSET  0x8000

#define KERNEL_SEGMENT 0x1000
#define KERNEL_OFFSET  0x0000

/* Header layout for stage2 and kernel. */
#define IMAGE_MAGIC_OFFSET 0
#define IMAGE_SIZE_OFFSET  6
#define IMAGE_ENTRY_OFFSET 10

.code16

.global boot
boot:
    cli
    cld
    jmp    0x0, 1f
1:  xor    ax, ax
    mov    ds, ax
    mov    es, ax
    mov    fs, ax
    mov    gs, ax
    mov    ss, ax
    mov    sp, 0x7c00
    sti

    /* Store boot drive. */
    mov    [BOOT_DRIVE_ADDR], dl

    /* Enable A20. */
    in     al, 0x92
    or     al, 1 << 1
    and    al, ~(1 << 0)
    out    0x92, al

    call   verify_edd

    /* Load first sector of stage 2. */
    lea    si, [dap_stage2_1]
    call   disk_read

    /* Read stage2 header. */

    mov    eax, [STAGE2_OFFSET + IMAGE_SIZE_OFFSET]
    /* sectors = ((size + 511) / 512) - 1 */
    add    eax, 511
    shr    eax, 9
    push   eax /* Save */
    dec    eax
    /* Store remaining sector count. */
    mov    [dap_stage2_rest + 2], ax
    /* Load remaining sectors after first. */
    lea    si, [dap_stage2_rest]
    call   disk_read

    /* Do pretty much the same thing for the kernel. */
    
    /* Determine where to read from. */
    pop    eax
    inc    eax
    mov    [.Lklba1], eax
    inc    eax
    mov    [.Lklbarest], eax

    lea    si, [dap_kernel_1]
    call   disk_read

    push   es
    mov    ax, KERNEL_SEGMENT
    mov    es, ax

    /* Read kernel header. */

    mov    eax, es:[KERNEL_OFFSET + IMAGE_SIZE_OFFSET]
    add    eax, 511
    shr    eax, 9
    dec    eax
    mov    [dap_kernel_rest + 2], ax
    lea    si, [dap_kernel_rest]
    call   disk_read

    pop    es

    /* Do we have a valid stage 2? */
    mov    cx, 6
    lea    di, [STAGE2_OFFSET + IMAGE_MAGIC_OFFSET]
    lea    si, [stage2_magic]
    repe cmpsb
    jne    1f

    /* Transfer control to stage 2. */
    call   word ptr [STAGE2_OFFSET + IMAGE_ENTRY_OFFSET]

1:  lea    si, [err_no_stage2]
    call   rm_print

1:  hlt
    jmp    1b

/*
 * Arguments:
 * SI = address of string
 */
rm_print:
    pusha
1:  lodsb
    test   al, al
    jz     2f
    mov    ah, 0x0e
    int    0x10
    jmp    1b
2:  popa
    ret

/*
 * Arguments:
 * DX = integer
 *
 * Implicitly uses buf_hex_out.
 */
rm_print_hex:
    pusha
    xor    cx, cx  /* Use this as our index. */
1:  cmp    cx, 4
    je     3f

    /* Convert last character of DX to ASCII. */
    mov    ax, dx
    and    ax, 0xf
    add    al, '0'
    cmp    al, '9'
    jle    2f
    add    al, 7

    /* Place our character in the correct position. */
2:  mov    bx, buf_hex_out + 5
    sub    bx, cx
    mov    [bx], al
    
    ror    dx, 4
    inc    cx
    jmp    1b

3:  lea    si, [buf_hex_out]
    call   rm_print
    popa
    ret

verify_edd:
    pusha

    mov    ah, 0x41
    mov    dl, [BOOT_DRIVE_ADDR]
    mov    bx, 0x55aa
    int    0x13

    jc     1f
    cmp    bx, 0xaa55
    jne    1f
    test   cl, 0x1
    jz     1f

    popa
    ret

1:  lea    si, [err_no_lba]
    call   rm_print

1:  hlt
    jmp    1b

/*
 * Arguments:
 * DS:SI = disk address packet address
 */
disk_read:
    pusha

    mov    ah, 0x42
    mov    dl, [BOOT_DRIVE_ADDR]
    int    0x13
    
    jc     1f
    test   ah, ah
    jnz    1f

    popa
    ret

1:  lea    si, [err_disk_read]
    call   rm_print
    movzx  dx, ah
    call   rm_print_hex

1:  hlt
    jmp    1b

/* All data below this point. */

err_no_lba:
    .asciz "Drive does not support LBA.\r\n"

err_disk_read:
    .asciz "Disk read error.\r\n"

err_no_stage2:
    .asciz "Invalid 2nd-stage bootloader.\r\n"

buf_hex_out:
    .asciz "0x0000"

stage2_magic:
    .ascii "STAGE2"

dap_stage2_1:
    .byte 0x10
    .byte 0
    .word 1
    .word STAGE2_OFFSET
    .word STAGE2_SEGMENT
    .quad 1

dap_stage2_rest:
    .byte 0x10
    .byte 0
    .word 0 /* Will be set. */
    .word STAGE2_OFFSET + 512
    .word STAGE2_SEGMENT
    .quad 2

dap_kernel_1:
    .byte 0x10
    .byte 0
    .word 1
    .word KERNEL_OFFSET
    .word KERNEL_SEGMENT
.Lklba1: .quad 0 /* Will be set. */

dap_kernel_rest:
    .byte 0x10
    .byte 0
    .word 0 /* Will be set. */
    .word KERNEL_OFFSET + 512
    .word KERNEL_SEGMENT
.Lklbarest: .quad 0 /* Will be set. */

.org 510
.word 0xaa55
