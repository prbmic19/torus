ENTRY(kentry)

KERNEL_VIRT_BASE = 0xffffffff80000000;
KERNEL_PHYS_BASE = 0x100000;

PHDRS
{
    text PT_LOAD FLAGS(5);
    rodata PT_LOAD FLAGS(4);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = KERNEL_VIRT_BASE + KERNEL_PHYS_BASE;

    __kernel_start = .;
    
    .text ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.text) - KERNEL_VIRT_BASE)
    {
        *(.kernel.start)
        *(.text.entry)
        *(.text .text.*)
    } :text

    .rodata ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.rodata) - KERNEL_VIRT_BASE)
    {
        *(.rodata .rodata.*)
    } :rodata

    .data ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.data) - KERNEL_VIRT_BASE)
    {
        *(.data .data.*)
    } :data

    .bss ALIGN(CONSTANT(COMMONPAGESIZE)) : AT(ADDR(.bss) - KERNEL_VIRT_BASE)
    {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
        *(.kernel.end)
    } :data

    __kernel_end = .;

    /DISCARD/ :
    {
        *(.eh_frame*)
        *(.comment*)
    }
}
