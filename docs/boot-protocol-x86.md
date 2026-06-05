# Torus Boot Protocol for x86, version 1

The Torus x86 boot protocol defines the interface between a bootloader and the Torus kernel.

## Loading the kernel

The bootloader must load the kernel into the first usable memory region that begins at or above address `0x100000`.

## Kernel image format

The kernel image begins with a header having the following format:

```c
struct kimage_header
{
    char signature[6];
    uint32_t size;
    uint64_t entry;
};
```

The `signature` field contains the ASCII characters "KERNEL" to indicate it is a valid kernel image. The `size` field specifies the size of the kernel image. The `entry` field specifies the absolute virtual address of the kernel entry point.

## Expected machine state

Before transferring control to the kernel, the machine must have the following state:

- `RAX` is set to the magic value `0x00000000C0FFEE64`.
- `RBX`, `RBP`, `R12`, `R13`, `R14`, and `R15` are cleared.
- `RDI` is set to the physical address of the boot_info structure.
- `GDTR` references a valid GDT containing:
    - A null descriptor at selector `0x00`.
    - A 64-bit kernel code descriptor at selector `0x08`.
    - A kernel data descriptor at selector `0x10`.
- `CS` is a 64-bit kernel code segment at selector `0x08`.
- `SS` is a 64-bit kernel data segment at selector `0x10`.
- `A20 gate` is enabled.
- `CR0` bits 31 (`PG`) and 0 (`PE`) are set. All other bits are undefined.
- `CR3` contains the physical address of a valid page table hierarchy. At minimum, the page tables must identity-map:
    - The kernel image.
    - The boot_info structure.
    - All structures referenced by boot_info.
- `CR4` bit 5 (`PAE`) is set. All other bits are undefined.
- `IA32_EFER` (MSR `0xC0000080`) bit 8 (`LME`) is set. Bits 0 (`SCE`) and 11 (`NXE`) are cleared.
- `RFLAGS` bits 17 (`VM`) and 9 (`IF`) are cleared. All other bits are undefined.

The contents of the GDT beyond the descriptors specified above are undefined. The kernel may replace the GDT at any time.

All other processor registers and flag bits are undefined. This includes `RSP`, which the kernel must set up before using the stack, and the IDTR, which the kernel must set up before enabling interrupts.

## Boot information

The boot_info structure passed to the kernel through `RDI` has the format:

```c
struct boot_info
{
    char signature[4];
    uint32_t boot_protocol_version;
    uint8_t _pad[4];
    uint32_t e820_entry_count;
    uint64_t e820_map_addr;
    uint64_t rsdp_addr;
    uint64_t fb_info_addr;
    uint64_t cmdline_addr;
};
```

The boot_info structure provides the kernel with information gathered by the bootloader. The fields ending with `_addr` contain physical addresses and are valid through the identity mapping established by the bootloader.

The `signature` field must contain the ASCII characters "INFO" to indicate that the structure is valid. The `boot_protocol_version` field specifies the version of the Torus boot protocol implemented by the bootloader. The padding field must be ignored and its value is undefined.

### E820 memory map

The `e820_map_addr` field must point to an array of E820 memory map entries provided by the BIOS. The number of entries in the array is specified by the field `e820_entry_count`.

Each entry must have the format:

```c
struct e820_entry
{
    uint64_t base;
    uint64_t len;
    uint32_t type;
};
```

The `type` field may contain one of the following values:

| Value | Meaning |
| --- | --- |
| 1 | Usable RAM |
| 2 | Reserved |
| 3 | ACPI reclaimable |
| 4 | ACPI NVS |
| 5 | Unusable |
| 7 | Persistent memory |

Values other than the ones above for `type` must be treated by the kernel as reserved.

### RSDP

The `rsdp_addr` field must contain the physical address of the ACPI Root System Descriptor Pointer (RSDP).

A value of zero indicates that no RSDP could be located.

### Framebuffer information

The `fb_info_addr` points to a framebuffer information structure. The structure has the format:

```c
struct fb_info
{
    uint64_t phys_addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t red_size;
    uint32_t red_shift;
    uint32_t green_size;
    uint32_t green_shift;
    uint32_t blue_size;
    uint32_t blue_shift;
    uint32_t rsvd_size;
    uint32_t rsvd_shift;
};
```

If a framebuffer is available, the `phys_addr` field contains the physical address of the beginning of the framebuffer. The framebuffer memory region is accessible through the identity-mapping established by the bootloader.

If no framebuffer is available, the `phys_addr` field must be zero.

The framebuffer uses the pixel format described by the color mask fields. 

Individual pixels are located with the formula:

```
pixel_addr = phys_addr + y * pitch + x * (bpp / 8)
```

### Command line

The `cmdline_addr` field points to a NUL-terminated ASCII command-line string. The string may be empty. The command line remains valid for the duration of kernel execution.

## Compliance

A bootloader is considered compliant with version 1 of the Torus boot protocol if it:

- Loads a valid Torus kernel image.
- Enters 64-bit long mode.
- Provides a valid boot_info structure.
- Establishes the required identity mappings.
- Transfers control to the kernel with the machine state defined above.
