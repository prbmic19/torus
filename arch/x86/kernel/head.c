/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/compiler.h>
#include <kernel/console.h>
#include <kernel/kprintf.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/acpi.h>
#include <kernel/memmap.h>
#include <drivers/serial.h>
#include <drivers/fb.h>
#include <lib/string.h>
#include <asm/idt.h>
#include <asm/boot_info.h>
#include <asm/paging.h>
#include <asm/gdt.h>

extern struct boot_info *boot_info;

static struct console serial_console = {
    .putchar = serial_putchar
};

// CMDLINE_SIZE is defined by the build system.
char boot_command_line[CMDLINE_SIZE];
char saved_command_line[CMDLINE_SIZE]; // TODO: __ro_after_init

struct fb_info fb_info;

struct rsdp rsdp;

static struct e820_entry *e820_map;
struct memmap_entry memmap[MEMMAP_MAX_ENTRIES]; // TODO: __ro_after_init
unsigned int memmap_entry_count; // TODO: __ro_after_init

extern __noreturn void kmain(void);

static void verify_boot_info(void)
{
    // Verify the pointer.

    if (unlikely(!(boot_info - KERNEL_VIRT_BASE)))
    {
        panic(NULL, "boot: Boot info pointer is NULL.");
    }
    
    // Verify the boot info structure itself.

    if (unlikely(memcmp(boot_info->signature, "INFO", 4) != 0))
    {
        panic(NULL, "boot: Bad boot info signature.");
    }

    if (unlikely(boot_info->boot_protocol_version != TPRTCL_SUPPORTED_VERSION))
    {
        panic(NULL, "boot: Unsupported Torus boot protocol version: %u", boot_info->boot_protocol_version);
    }

    // Verify the command line.

    if (unlikely(!boot_info->cmdline_addr))
    {
        panic(NULL, "boot: Command line pointer is NULL.");
    }

    // Verify the framebuffer.

    if (unlikely(!boot_info->fb_info_addr))
    {
        panic(NULL, "fb: Framebuffer info pointer is NULL.");
    }

    struct fb_info *__fb_info = vaddr(boot_info->fb_info_addr);

    if (unlikely(!__fb_info->phys_addr))
    {
        panic(NULL, "fb: No framebuffer.");
    }

    if (unlikely(__fb_info->bpp != 32))
    {
        panic(NULL, "fb: Unsupported framebuffer bpp: %u", __fb_info->bpp);
    }

    // Verify the E820 map.

    if (unlikely(!boot_info->e820_map_addr))
    {
        panic(NULL, "e820-memmap: E820 memory map pointer is NULL.");
    }

    if (unlikely(!boot_info->e820_entry_count))
    {
        panic(NULL, "e820-memmap: No E820 memory map.");
    }

    if (unlikely(boot_info->e820_entry_count > E820_MAX_ENTRIES))
    {
        panic(NULL, "e820-memmap: Too many E820 entries: %u", boot_info->e820_entry_count);
    }

    // Verify the RSDP.

    if (unlikely(!boot_info->rsdp_addr))
    {
        panic(NULL, "ACPI: No RSDP.");
    }

    struct rsdp *__rsdp = vaddr(boot_info->rsdp_addr);
    u8 *raw___rsdp = (u8 *)__rsdp;
    u8 sum = 0;

    if (unlikely(memcmp(__rsdp->signature, "RSD PTR ", 8) != 0))
    {
        panic(NULL, "ACPI: Bad RSDP signature.");
    }

    for (int i = 0; i < 20; i++)
    {
        sum += raw___rsdp[i];
    }

    if (unlikely(sum))
    {
        panic(NULL, "ACPI: Bad RSDP checksum.");
    }

    // We support only ACPI 1.0 for now.
    if (unlikely(__rsdp->revision != 0))
    {
        panic(NULL, "ACPI: Unsupported ACPI revision: %u", __rsdp->revision);
    }
}

// This copies only the contents of individual pointers inside boot_info.
// For e820_map, we copy only the pointer. copy_memmap() will handle it.
static void copy_boot_info(void)
{
    strncpy(boot_command_line, (char *)vaddr(boot_info->cmdline_addr), sizeof(boot_command_line) - 1);
    strncpy(saved_command_line, boot_command_line, sizeof(saved_command_line) - 1);

    boot_command_line[sizeof(boot_command_line) - 1] = '\0';
    saved_command_line[sizeof(saved_command_line) - 1] = '\0';

    memcpy(&fb_info, vaddr(boot_info->fb_info_addr), sizeof(fb_info));
    fb_info.virt_addr = (unsigned long)vaddr(fb_info.phys_addr);

    memcpy(&rsdp, vaddr(boot_info->rsdp_addr), sizeof(rsdp));

    e820_map = vaddr(boot_info->e820_map_addr);
}

static void copy_memmap(void)
{
    for (unsigned int i = 0; i < boot_info->e820_entry_count; i++)
    {
        struct e820_entry *entry = &e820_map[i];

        memmap[i].base = entry->base;
        memmap[i].len = entry->len;

        switch (entry->type)
        {
            case E820_TYPE_USABLE:
                memmap[i].type = MEMMAP_TYPE_USABLE;
                break;
            case E820_TYPE_RESERVED:
                memmap[i].type = MEMMAP_TYPE_RESERVED;
                break;
            case E820_TYPE_ACPI:
                memmap[i].type = MEMMAP_TYPE_ACPI_RECLAIMABLE;
                break;
            case E820_TYPE_NVS:
                memmap[i].type = MEMMAP_TYPE_ACPI_NVS;
                break;
            case E820_TYPE_UNUSABLE:
                memmap[i].type = MEMMAP_TYPE_UNUSABLE;
                break;
            default:
                memmap[i].type = MEMMAP_TYPE_UNKNOWN;
        }

        memmap_entry_count++;
    }
}

__noreturn void arch_kmain(void)
{
    /*
     * At this point:
     * - The entry stub has removed the identity mapping; we're now on the higher half.
     * - Interrupts are disabled.
     * - Pointers passed by the bootloader haven't been patched. vaddr() must be used.
     */

    // Initialize the console so we have output right away.
    serial_init();
    console_init(&serial_console);

    gdt_init();

    idt_init();
    local_irq_enable();

    // Verify boot info and copy it into our space.
    verify_boot_info();
    copy_boot_info();
    copy_memmap();

    kmain();
}
