#define ASM
#include <mm/paging.h>
#include <arm/mmu.h>
#undef ASM

/*
Allocates and inits a new page table
* tbl - ptr to location of new table
* virtual_adr - Virtual address that will be mapped
* shift - Index of current page table index (39 for PGD, 30 for PUD)
* tmpx - temporary regs
*/
.macro create_table_entry tbl, virtual_adr, shift, tmp1, tmp2
    /* first shift to the right and then AND the virtual address to get the tbl index */
    lsr \tmp1, \virtual_adr, #\shift
    and \tmp1, \tmp1, #PTRS_PER_TABLE - 1

    /* get location of next page table */
    add \tmp2, \tbl, #PAGE_SIZE

    /* Convert pointer to table descriptor (or with 0b11) */
    orr \tmp2, \tmp2, #MM_TYPE_PAGE_TABLE

    /* store table descriptor in page table (use index calculated at beginning of macro) */
    str \tmp2, [\tbl, \tmp1, lsl #3]

    /* increment \tbl for convenience */
    add \tbl, \tbl, #PAGE_SIZE
.endm

/*
Populates PMD table
* tbl - ptr to PMD table 
* phys_adr - start of physical address that will be mapped
* virt_start - Start of virtual memory to be mapped
* virt_end - End of virtual memory to be mappyed
* flags - flags that will be added to block descriptor
* tmpx - temp regs 
*/
.macro create_block_map tbl, phys_adr, virt_start, virt_end, flags, tmp1
    /* Get table index from virtual start address */
    lsr \virt_start, \virt_start, #SECTION_SHIFT
    and \virt_start, \virt_start, #PTRS_PER_TABLE - 1

    /* Get table index from virtual end address */
    lsr \virt_end, \virt_end, #SECTION_SHIFT
    and \virt_end, \virt_end, #PTRS_PER_TABLE - 1

    /* Create block descriptor by orring with \flags and shifting phys_adr right and then left to clear first 21 bits */
    mov \tmp1, #\flags
    lsr \phys_adr, \phys_adr, #SECTION_SHIFT
    orr \phys_adr, \tmp1, \phys_adr, lsl #SECTION_SHIFT

9999:
    /* Store block descriptor in PMD table */
    str \phys_adr, [\tbl, \virt_start, lsl #3]

    /* iterate index and physical address */
    add \virt_start, \virt_start, #1
    add \phys_adr, \phys_adr, #SECTION_SIZE

    /* loop if not completed block */
    cmp \virt_start, \virt_end
    b.ls 9999b
.endm

/*
Create page directory entry for virtual address
* tbl - ptr to PMD table
* virt - virtual address to map
* tmpx - temp regs
*/
.macro create_pgd_entry, tbl, virt, tmp1, tmp2
    create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2 // this will increment \tbl so that it is ready for next table
    create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2     
.endm

.globl __create_page_tables
__create_page_tables:
    mov x29, x30        /* Save link address */
    adrp x0, pg_dir

    mov x1, #PG_DIR_SIZE
    bl memzero          /* Clear page table area */

    /* ========================= Identity mapping for boot code ==================== */
    /* Creating initial page directory */
    adrp x0, pg_dir
    mov x25, x0         // Copy x0 to return later (for ttbr0)
    mov x1, #BOOT_CODE_START //b *0x840c4
    create_pgd_entry x0, x1, x2, x3

    /* Identity mapping for 0x0 -> DEVICE_BASE_PHYS */
    mov x1, xzr
    mov x2, #BOOT_CODE_START
    ldr x3, =(BOOT_CODE_START + DEVICE_BASE_PHYS - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_FLAGS, x4


    // don't need to map device
    /* ========================= Creating kernel space mapping ===================== */
    /* Create page directory at VIRTUAL_ADDRESS_START */
    // adrp x0, pg_dir //b *0x84134
    mov x1, #VIRTUAL_ADDRESS_START //b *0x84134
    mov x26, x0
    create_pgd_entry x0, x1, x2, x3

    /* Map from 0 -> DEVICE_BASE_PHYS (Where device registers begin) */
    mov x1, xzr
    mov x2, #VIRTUAL_ADDRESS_START
    ldr x3, =(VIRTUAL_ADDRESS_START + VC_BASE_PHYS - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

    /* Map from VC_BASE_PHYS -> DEVICE_BASE_PHYS */
    mov x1, #VC_BASE_PHYS
    ldr x2, =(VIRTUAL_ADDRESS_START + VC_BASE_PHYS)
    ldr x3, =(VIRTUAL_ADDRESS_START + DEVICE_BASE_PHYS - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    /* Map from DEVICE_BASE -> PHYSICAL_MEMORY_SIZE (map device registers) */
    mov x1, #DEVICE_BASE_PHYS
    ldr x2, =(VIRTUAL_ADDRESS_START + DEVICE_BASE_PHYS)
    ldr x3, =(VIRTUAL_ADDRESS_START + PHYSICAL_MEMORY_SIZE - SECTION_SIZE)
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    mov x30, x29 // restore return address
    mov x1, x26
    mov x0, x25
    ret //b *0x841e4

/* void set_page_directory(uint64_t page_directory) */
.globl set_page_directory
set_page_directory:
    msr ttbr0_el1, x0
    tlbi vmalle1is
    dsb ish
    isb
    ret

/* void* get_page_directory() */
.globl get_page_directory
get_page_directory:
    mov x1, 0
    ldr x0, [x1]
    mov x0, 0x1000
    msr ttbr0_el1, x0
    ldr x0, [x1]
    ret



