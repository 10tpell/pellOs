#include "arm/regs.h"
#include "config.h"
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
    // Clean the BSS section
    ldr     x1, =__bss_start     // Start address
    ldr     w2, =__bss_size      // Size of the section
3:  cbz     w2, 4f               // Quit loop if zero
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b               // Loop if non-zero

    // Check EL
4:  bl      get_el
    sub     x6, x0, #3
    ldr     x0, =EL2     
    cbz     x6, switch_to_el2    // Go to EL2 if in EL3
EL2:
    bl      get_el
    sub     x6, x0, #2     
    ldr     x0, =EL1      
    cbz     x6, switch_to_el1    // Go to EL1 if in EL3
EL1:
    bl      main_entry       // go to main 
    
main_entry:  
    // Set stack to start below our code
    ldr     x1, =0x400000
    mov     sp, x1

    bl      main
    // In case it does return, halt the master core too
    b       1b

.section ".text"

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

			
