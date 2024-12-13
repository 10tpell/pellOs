SECTIONS
{
    . = 0xffff000000080000;                             /* virtual address for kernel */
    .text.boot : { *(.text.boot) }

    /* userspace while we don't have an ELF loader */
    . = ALIGN (0x1000);
    user_begin = .;
    .text.user : { * (.text.user) }
    .rodata.user : { build/src/userspace* (.rodata) }
    .data.user : { build/src/userspace* (.data) }
    .bss.user : { build/src/userspace* (.bss) }
    user_end = .;

    /* kernel code */
    .text : { *(.text .text.* .gnu.linkonce.t*) }

    /* kernel data */
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

    /* Page directory bits */
    . = ALIGN (0x1000);
    pg_dir = .;
    .data.pgd : { . += (3 * (1 << 12)); }
    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;