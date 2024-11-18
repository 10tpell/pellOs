#define ASM
#include "peripherals/irq_gic.h"
#undef ASM

#define GICC_CTRLR	0x0
#define GICC_PMR	0x4
#define IT_NR		0x8	// Number of interrupt enable registers (256 total irqs)
#define GICD_CTRLR	0x0
#define GICD_IGROUPR	0x80

.globl setup_gic
setup_gic:				// Called from secure mode - set all interrupts to group 1 and enable.
	mrs	x0, MPIDR_EL1
	ldr	x2, =IRQ_GICD_BASE
	tst	x0, #0x3
	b.eq	2f			// primary core

	mov	w0, #3			// Enable group 0 and 1 IRQs from distributor
	str	w0, [x2, #GICD_CTRLR]
2:
	add	x1, x2, #(IRQ_GICC_BASE - IRQ_GICD_BASE)
	mov	w0, #0x1e7
	str	w0, [x1, #GICC_CTRLR]	// Enable group 1 IRQs from CPU interface
	mov	w0, #0xff
	str	w0, [x1, #GICC_PMR]	// priority mask
	add	x2, x2, #GICD_IGROUPR
	mov	x0, #(IT_NR * 4)
	mov	w1, #~0			// group 1 all the things
3:
	subs	x0, x0, #4
	str	w1, [x2, x0]
	b.ne	3b
	ret