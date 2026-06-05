/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <torus/compiler.h>
#include <drivers/fb.h>
#include <asm/boot_info.h>

#define KERNEL_SRC  ((void *)0x10000)

#define PAGE_P  ((u64)1 << 0)
#define PAGE_RW ((u64)1 << 1)
#define PAGE_PS ((u64)1 << 7)

#define MIB(_n) ((u64)(_n) << 20)
#define GIB(_n) ((u64)(_n) << 30)

extern struct boot_info *boot_info_ptr;

u64 pml4[512] __aligned(0x1000);
u64 pdpt[512] __aligned(0x1000);
u64 pd_first_gib[512] __aligned(0x1000);
u64 pd_fb[512] __aligned(0x1000);

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

static void map_2mib(u64 pd[static 512], u64 pd_base, u64 phys_start, u64 phys_end)
{
    phys_start &= ~(MIB(2) - 1);
    phys_end = (phys_end + MIB(2) - 1) & ~(MIB(2) - 1);

    for (u64 a = phys_start; a < phys_end; a += MIB(2))
    {
        u64 i = (a - pd_base) / MIB(2);

        if (unlikely(i >= 512))
        {
            asm volatile ("hlt");
        }

        pd[i] = a | PAGE_P | PAGE_RW | PAGE_PS;
    }
}

static void build_page_tables(void)
{
    // Identity map first 1 GiB.
    map_2mib(pd_first_gib, 0, 0 * MIB(2), 512 * MIB(2));
    pdpt[0] = (u64)(unsigned long)pd_first_gib | PAGE_P | PAGE_RW;

    // Identity map framebuffer region.

    struct fb_info *__fb_info = (struct fb_info *)(unsigned long)boot_info_ptr->fb_info_addr;

    if (likely(__fb_info && __fb_info->phys_addr))
    {
        u64 fb_size = (u64)__fb_info->pitch * (u64)__fb_info->height;
        map_2mib(pd_fb, (__fb_info->phys_addr >> 30) << 30, __fb_info->phys_addr, __fb_info->phys_addr + fb_size);

        u64 pdpt_i = __fb_info->phys_addr >> 30;
        pdpt[pdpt_i] = (u64)(unsigned long)pd_fb | PAGE_P | PAGE_RW;
    }

    pml4[0] = (u64)(unsigned long)pdpt | PAGE_P | PAGE_RW;
}

static void relocate_kernel(void)
{
    // Walk the E820 map until we find an entry with a base that is greater than or
    // equal to 0x100000 (must be usable RAM) that could fit the entire kernel,
    // and relocate the kernel there.
   
    void *kernel_reloc_dest = NULL;
    size_t kernel_size = *(u32 *)(KERNEL_SRC + 6);

    for (int i = 0; i < (int)boot_info_ptr->e820_entry_count; i++)
    {
        struct e820_entry *entry = &((struct e820_entry *)(unsigned long)boot_info_ptr->e820_map_addr)[i];
        
        if (entry->base >= 0x100000 && entry->len >= kernel_size && entry->type == E820_TYPE_USABLE)
        {
            kernel_reloc_dest = (void *)(unsigned long)entry->base;
            break;
        }
    }

    // We didn't find anything? Uh-oh.
    if (unlikely(!kernel_reloc_dest))
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
    build_page_tables();
    relocate_kernel();
    prepare_kernel_jump();
}
