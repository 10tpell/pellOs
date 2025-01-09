#include "arm/regs.h"
#include "arm/mmu.h"
#include "config.h"

#define ASM
#include <mm/paging.h>
#undef ASM

.section ".text.boot"  // Make sure the linker puts this at the start of the kernel image

.global _start  // Execution starts here
_start:
    // Check processor ID is zero (executing on main core), else hang
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // We're not on the main core, so hang in an infinite wait loop
1:  wfe
    b       1b
2:  // We're on the main core!
    
    // Check EL
    bl      get_el
    sub     x6, x0, #3
    ldr     x0, =EL2     
    cbz     x6, switch_to_el2    // Go to EL2 if in EL3
EL2:
    bl      get_el
    sub     x6, x0, #2     
    adr     x0, #8 // TODO: explain
//  ldr     x0, =main_entry      
    cbz     x6, switch_to_el1    // Go to EL1 if in EL3
    
main_entry:  
    /* Now create page tables */
    bl __create_page_tables // will set x1 to ttbr1 and x0 to ttbr0

    /* update ttbr1_el1 to point to PGD table */
    // ldr x0, =pg_dir
    msr ttbr1_el1, x1
    msr ttbr0_el1, x1

    /* update sp to point to virtual address */
    mov x0, #VIRTUAL_ADDRESS_START
    add sp, x0, #LOW_MEMORY

    /* Configure Translation Control Register */
    ldr x0, =(TCR_VALUE)
    msr tcr_el1, x0

    /* Configure mair register */
    ldr x0, =(MAIR_VALUE)
    msr mair_el1, x0

    /* Enable MMU */
    ldr x2, =main
    mov x0, #SCTLR_MMU_ENABLED
    msr sctlr_el1, x0

    tlbi vmalle1is
    dsb ish
    isb

    // Clean the BSS section (after MMU enabled because of kernel mapping)
    ldr     x0, =__bss_start     // Start address
    ldr     x1, =__bss_size      // Size of the section

    bl memzero // b *0x8006c 

    br x2

.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

/* void switch_to_el2(void* el2_entry) */
.globl switch_to_el2
switch_to_el2:
    mov     x1, #0x33ff
    msr     cptr_el3, x1 	 // Disable coprocessor traps to EL3
    mov     x1, #3 << 20
    msr     cpacr_el1, x1	 // Enable FP/SIMD at EL1


#if IRQ_CONTROLLER == USE_GIC_IRQS
    bl      setup_gic
#endif
    // Now get ready to switch from EL3 down to EL2

    ldr     x1, =SCR_VALUE
    msr     scr_el3, x1

    ldr     x1, =SPSR_EL3_VAL
    msr     spsr_el3, x1
    
    msr     elr_el3, x0

    eret

/* void switch_to_el1(void* el1_entry); */
.globl switch_to_el1
switch_to_el1:
    ldr	x1, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x1		

	ldr	x1, =HCR_VALUE
	msr	hcr_el2, x1

	ldr	x1, =SPSR_VALUE
	msr	spsr_el2, x1

	msr	elr_el2, x0

	eret	

			
