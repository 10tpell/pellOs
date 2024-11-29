SECTIONS
{
    /*. = 0x80000;  Kernel load address for AArch64 */  
    

    . = 0xffff000000080000;   
    .text.boot : { *(.text.boot) }
    /* userspace while we don't have an ELF loader */
    . = ALIGN (0x1000);
    user_begin = .;
    .text.user : { build/user* (.text) }
    .rodata.user : { build/user* (.rodata) }
    .data.user : { build/user* (.data) }
    .bss.user : { build/user* (.bss) }
    user_end = .;

    .text : { *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
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