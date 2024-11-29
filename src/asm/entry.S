#include "entry.h"

.macro handle_invalid_exception el type
    push_regs \el
    mov	x0, #\type
    mrs	x1, esr_el1
    mrs	x2, elr_el1
    bl	show_invalid_exception_message
    b	err_hang
.endm

.macro	vtable_entry    vector_handler
    .align	7
    b	\vector_handler
.endm

.macro	push_regs el
    sub	sp, sp, #S_FRAME_SIZE
    stp	x0, x1, [sp, #16 * 0]
    stp	x2, x3, [sp, #16 * 1]
    stp	x4, x5, [sp, #16 * 2]
    stp	x6, x7, [sp, #16 * 3]
    stp	x8, x9, [sp, #16 * 4]
    stp	x10, x11, [sp, #16 * 5]
    stp	x12, x13, [sp, #16 * 6]
    stp	x14, x15, [sp, #16 * 7]
    stp	x16, x17, [sp, #16 * 8]
    stp	x18, x19, [sp, #16 * 9]
    stp	x20, x21, [sp, #16 * 10]
    stp	x22, x23, [sp, #16 * 11]
    stp	x24, x25, [sp, #16 * 12]
    stp	x26, x27, [sp, #16 * 13]
    stp	x28, x29, [sp, #16 * 14]
    
    .if \el == 0
    mrs x21, sp_el0
    .else
    add x21, sp, #S_FRAME_SIZE
    .endif

    mrs x22, elr_el1
    mrs x23, spsr_el1

    stp x30, x21, [sp, #16 * 15]
    stp	x22, x23, [sp, #16 * 16] 
.endm

.macro	pop_regs el
    ldp x22, x23, [sp, #16 * 16]
    ldp x30, x21, [sp, #16 * 15]

    .if \el == 0
    msr sp_el0, x21
    .endif

    msr elr_el1, x22
    msr spsr_el1, x23

    ldp	x0, x1, [sp, #16 * 0]
    ldp	x2, x3, [sp, #16 * 1]
    ldp	x4, x5, [sp, #16 * 2]
    ldp	x6, x7, [sp, #16 * 3]
    ldp	x8, x9, [sp, #16 * 4]
    ldp	x10, x11, [sp, #16 * 5]
    ldp	x12, x13, [sp, #16 * 6]
    ldp	x14, x15, [sp, #16 * 7]
    ldp	x16, x17, [sp, #16 * 8]
    ldp	x18, x19, [sp, #16 * 9]
    ldp	x20, x21, [sp, #16 * 10]
    ldp	x22, x23, [sp, #16 * 11]
    ldp	x24, x25, [sp, #16 * 12]
    ldp	x26, x27, [sp, #16 * 13]
    ldp	x28, x29, [sp, #16 * 14]
    add	sp, sp, #S_FRAME_SIZE		
.endm

#ifndef ASM_MACRO

/*
 * Exception vectors.
 */
.align	11
.globl vectors 
vectors:
	vtable_entry	sync_invalid_el1t			// Synchronous EL1t
	vtable_entry	irq_invalid_el1t			// IRQ EL1t
	vtable_entry	fiq_invalid_el1t			// FIQ EL1t
	vtable_entry	error_invalid_el1t			// Error EL1t

	vtable_entry	sync_invalid_el1h			// Synchronous EL1h
	vtable_entry	el1_irq					    // IRQ EL1h
	vtable_entry	fiq_invalid_el1h			// FIQ EL1h
	vtable_entry	error_invalid_el1h			// Error EL1h

	vtable_entry	el0_sync			        // Synchronous 64-bit EL0
	vtable_entry	irq_invalid_el0_64			// IRQ 64-bit EL0
	vtable_entry	fiq_invalid_el0_64			// FIQ 64-bit EL0
	vtable_entry	error_invalid_el0_64			// Error 64-bit EL0

	vtable_entry	sync_invalid_el0_32			// Synchronous 32-bit EL0
	vtable_entry	irq_invalid_el0_32			// IRQ 32-bit EL0
	vtable_entry	fiq_invalid_el0_32			// FIQ 32-bit EL0
	vtable_entry	error_invalid_el0_32	    // Error 32-bit EL0

sync_invalid_el1t:
	handle_invalid_exception 1, SYNC_INVALID_EL1t

irq_invalid_el1t:
	handle_invalid_exception 1, IRQ_INVALID_EL1t

fiq_invalid_el1t:
	handle_invalid_exception 1, FIQ_INVALID_EL1t

error_invalid_el1t:
	handle_invalid_exception 1, ERROR_INVALID_EL1t

sync_invalid_el1h:
	handle_invalid_exception 1, SYNC_INVALID_EL1h

fiq_invalid_el1h:
	handle_invalid_exception 1, FIQ_INVALID_EL1h

error_invalid_el1h:
	handle_invalid_exception 1, ERROR_INVALID_EL1h

sync_invalid_el0_64:
    /* handling exception by exiting thread */
	mov x0, #5
    bl syscall_exit

irq_invalid_el0_64:
	handle_invalid_exception 0, IRQ_INVALID_EL0_64

fiq_invalid_el0_64:
	handle_invalid_exception 0, FIQ_INVALID_EL0_64

error_invalid_el0_64:
	handle_invalid_exception 0, ERROR_INVALID_EL0_64

sync_invalid_el0_32:
	handle_invalid_exception 0, SYNC_INVALID_EL0_32

irq_invalid_el0_32:
	handle_invalid_exception 0, IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
	handle_invalid_exception 0, FIQ_INVALID_EL0_32

error_invalid_el0_32:
	handle_invalid_exception 0, ERROR_INVALID_EL0_32

el1_irq:
	push_regs 1
	bl	handle_irq
	pop_regs 1
    eret

el0_sync:
    push_regs 0
    mrs x25, esr_el1
    lsr x24, x25, #ESR_EC_SHIFT
    cmp x24, #ESR_EC_SVC64
    b.eq el0_sys_call
    cmp x24, #ESR_EC_DABT_LOW
    b.eq el0_da
    b sync_invalid_el0_64

el0_sys_call:
    adr x27, sys_call_table /* get sys call table ptr into x27 */
    uxtw x26, w8            /* put sys call number into x26 */
    mov x25, #N_SYSCALLS    /* gey num of sys calls */
    bl enable_irq
    cmp x26, x25
    b.hs invalid_sys_call

    ldr x16, [x27, x26, lsl #3]
    blr x16
    b ret_from_syscall

/* When accessing unmapped address */
el0_da:
    bl enable_irq
    mrs x0, far_el1
    mrs x1, esr_el1
    bl do_mem_abort
    cmp x0, 0
    b.eq 1f
    handle_invalid_exception 0, DATA_ABORT_ERROR
1:
    bl disable_irq
    pop_regs 0
    eret

invalid_sys_call:
    handle_invalid_exception 0, SYSCALL_ERROR

ret_from_syscall:
    bl disable_irq
    str x0, [sp, #S_X0]
    pop_regs 0
    eret

.globl err_hang
err_hang: b err_hang

#endif
