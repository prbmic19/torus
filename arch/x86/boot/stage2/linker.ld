ENTRY(stage2_entry)

PHDRS
{
    rodata PT_LOAD FLAGS(4);
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0x8000;
    
    .text : ALIGN(CONSTANT(COMMONPAGESIZE))
    {
        *(.stage2.start)
        *(.text.entry)
        *(.text .text.*)
    } :text

    .rodata : ALIGN(CONSTANT(COMMONPAGESIZE))
    {
        *(.rodata .rodata.*)
    } :rodata

    .data : ALIGN(CONSTANT(COMMONPAGESIZE))
    {
        *(.data .data.*)
    } :data

    .bss : ALIGN(CONSTANT(COMMONPAGESIZE))
    {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
        *(.stage2.end)
    } :data

    /DISCARD/ :
    {
        *(.eh_frame*)
        *(.comment*)
    }
}
