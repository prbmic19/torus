/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <torus/compiler.h>
#include <asm/boot_info.h>

#define KERNEL_SRC  ((void *)0x10000)

struct boot_info *boot_info = (struct boot_info *)0x680;

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

static void relocate_kernel(void)
{
    // Walk the E820 map until we find an entry with a base that is greater than or
    // equal to 0x100000 (must be usable RAM) that could fit the entire kernel,
    // and relocate the kernel there.
    
    void *kernel_reloc_dest = NULL;
    size_t kernel_size = *(u32 *)(KERNEL_SRC + 6);

    for (size_t i = 0; i < MAX_E820_ENTRIES; i++)
    {
        struct e820_entry *entry = &boot_info->e820_entries[i];
        
        // If these are all zero (except ext_attr, it might be nonzero), we've reached the end.
        if (entry->base == 0 && entry->length == 0 && entry->type == 0)
        {
            break;
        }

        if (entry->base >= 0x100000 && entry->length >= kernel_size && entry->type == 1)
        {
            kernel_reloc_dest = (void *)(unsigned long)entry->base;
            break;
        }
    }

    // We didn't find anything? Uh-oh.
    if (!kernel_reloc_dest)
    {
        // At this point, we have no way of reporting an error besides halting.
        while (1)
        {
            asm volatile ("hlt");
        }
    }

    pm_memcpy(kernel_reloc_dest, KERNEL_SRC, kernel_size);
}

__noreturn void pm_main(void)
{
    relocate_kernel();

    page_tables_init();

    prepare_kernel_jump();
}
