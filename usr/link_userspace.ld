SECTIONS
{
    . = 0x0000;                             /* userspace programs start at 0 */
    ENTRY(main)
    /* code */
    .text : { *(.text .text.* .gnu.linkonce.t*) }

    /* data */
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { 
        *(.data .data.* .gnu.linkonce.d*) 
        /* ramdisk */
        __ramdisk_start = .;
        *(.ramdisk)
        __ramdisk_end = .;
    }

    . = ALIGN(0x8);
	__bss_start = .;
	.bss : { *(.bss*) } 
	__bss_end = .;
	. = ALIGN(0x00001000);

    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;