/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <torus/compiler.h>

#define KERNEL_SRC  ((void *)0x10000)
#define KERNEL_DEST ((void *)0x100000)

__aligned(0x1000) u64 pml4[512];
__aligned(0x1000) u64 pdpt[512];
__aligned(0x1000) u64 pd[512];

usize kernel_size = 0u;

// const char kernel_command_line[] = KERNEL_CMDLINE;

extern __noreturn void prepare_kernel_jump(void);

static void *_pm_memcpy(void *restrict dest, const void *restrict src, usize n)
{
    u8 *restrict d = (u8 *restrict)dest;
    const u8 *restrict s = (const u8 *restrict)src;
    
    for (usize i = 0u; i < n; i++)
    {
        d[i] = s[i];
    }

    return dest;
}

static void page_tables_init(void)
{
    // We currently just have an identity-mapped page table.

    for (int i = 0; i < 512; i++)
    {
        pd[i] = ((u64)i * (u64)0x200000u) | (u64)0x83;
    }

    pdpt[0] = (u64)(uptr)pd | (u64)0x03;
    pml4[0] = (u64)(uptr)pdpt | (u64)0x03;
}

static void relocate_kernel(void)
{
    _pm_memcpy(KERNEL_DEST, KERNEL_SRC, kernel_size);
}

__noreturn void pm_main(void)
{
    kernel_size = (usize)(*(u32 *)(KERNEL_SRC + 6)); // Skip 6 magic bytes.
    relocate_kernel();

    page_tables_init();

    prepare_kernel_jump();
}
