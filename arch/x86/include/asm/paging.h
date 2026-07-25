/* SDPX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_X86_PAGING_H
#define ASM_X86_PAGING_H

#ifdef __ASSEMBLER__
#   define PAGE_P  (1ul << 0)
#   define PAGE_RW (1ul << 1)
#   define PAGE_PS (1ul << 7)
#   define PAGE_G  (1ul << 8)
#else
#   define PAGE_P  (1 << 0)
#   define PAGE_RW (1 << 1)
#   define PAGE_PS (1 << 7)
#   define PAGE_G  (1 << 8)
#endif

#define PAGE_SIZE 0x1000

#ifdef __ASSEMBLER__
#   define KERNEL_VIRT_BASE 0xffffffff80000000
#   define vaddr(_addr) ((_addr) + KERNEL_VIRT_BASE)
#   define paddr(_addr) ((_addr) - KERNEL_VIRT_BASE)
#else
#   define KERNEL_VIRT_BASE 0xffffffff80000000ul
#   define vaddr(_addr) ((void *)((unsigned long)(_addr) + KERNEL_VIRT_BASE))
#   define paddr(_addr) ((void *)((unsigned long)(_addr) - KERNEL_VIRT_BASE))
#endif // __ASSEMBLER__

#endif // ASM_X86_PAGING_H
