/*
 * Derived from commit 9a479b11 of https://github.com/thomasloven/mittos64
 *
 * Copyright (c) 2017 Thomas Lovén
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <asm_generic/linkage.h>
#include <asm/paging.h>

#define PGTABLE_ENTRY_COUNT 512

.section .data, "aw", @progbits

.balign PAGE_SIZE
OBJ_BEGIN_U(boot_pml4)
    .quad (boot_pdpt - KERNEL_VIRT_BASE) + (PAGE_P | PAGE_RW)
    .rept PGTABLE_ENTRY_COUNT - 2
        .quad 0
    .endr
    .quad (boot_pdpt - KERNEL_VIRT_BASE) + (PAGE_P | PAGE_RW)
OBJ_END(boot_pml4)

.balign PAGE_SIZE
OBJ_BEGIN_U(boot_pdpt)
    .quad (boot_pd - KERNEL_VIRT_BASE) + (PAGE_P | PAGE_RW)
    .rept PGTABLE_ENTRY_COUNT - 3
        .quad 0
    .endr
    .quad (boot_pd - KERNEL_VIRT_BASE) + (PAGE_P | PAGE_RW)
    .quad 0
OBJ_END(boot_pdpt)

.balign PAGE_SIZE
OBJ_BEGIN_U(boot_pd)
    .set i, 0
    .rept PGTABLE_ENTRY_COUNT
        .quad (i << 21) | PAGE_P | PAGE_RW | PAGE_PS | PAGE_G
        .set i, (i + 1)
    .endr
OBJ_END(boot_pd)
