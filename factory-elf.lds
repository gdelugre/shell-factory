/* 
 * Linker script for generating shell-factory targets with -N.
 */
SECTIONS
{
    PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x400000));
    . = SEGMENT_START("text-segment", 0x400000) + SIZEOF_HEADERS;

    /*
     * The executable code.
     */
    .text               : { *(.text .text.* .gnu.linkonce.t.*) }
    PROVIDE(__etext = .);
    PROVIDE(_etext = .);
    PROVIDE(etext = .);

    /* 
     * Constant data.
     * Merge into the text section.
     */
    .rodata             : { *(.rodata .rodata.* .gnu.linkonce.r.*) }

    /*
     * Non-constant data.
     * Pushed inside the data segment which should be adjacent to text.
     */

    .data               : { *(.data .data.* .gnu.linkonce.d.*) }
    PROVIDE(edata = .);

    /*
     * Uninitialized data. Bound to the data segment, should not take any space.
     */
    __bss_start = .;
    .bss                : { *(.bss .bss.* .gnu.linkonce.b.*) *(COMMON) }

    PROVIDE (ebss = .);

    _end = ALIGN(4096);
    PROVIDE (eend = _end);

    /DISCARD/           : { *(.note.*) }
}
