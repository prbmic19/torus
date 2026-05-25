/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <torus/compiler.h>

#define KERNEL_SRC  ((void *)0x10000)
#define KERNEL_DEST ((void *)0x100000)

u64 pml4[512] __aligned(0x1000);
u64 pdpt[512] __aligned(0x1000);
u64 pd[512] __aligned(0x1000);

// const char kernel_command_line[] = KERNEL_CMDLINE;

extern __noreturn void prepare_kernel_jump(void);

static void *pm_memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    unsigned char *restrict d = dest;
    const unsigned char *restrict s = src;
    
    while (n--)
    {
        *d++ = *s++;
    }

    return dest;
}

static void page_tables_init(void)
{
    // For now, build an identity-mapped table.

    for (int i = 0; i < 512; i++)
    {
        pd[i] = ((u64)i * 0x200000) | 0x83;
    }

    pdpt[0] = (u64)(unsigned long)pd | 0x03;
    pml4[0] = (u64)(unsigned long)pdpt | 0x03;
}

static void relocate_kernel(size_t kernel_size)
{
    pm_memcpy(KERNEL_DEST, KERNEL_SRC, kernel_size);
}

__noreturn void pm_main(void)
{
    relocate_kernel(*(u32 *)(KERNEL_SRC + 6)); // Skip 6 magic bytes.

    page_tables_init();

    prepare_kernel_jump();
}
