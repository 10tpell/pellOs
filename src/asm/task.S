#define ASM
#include <scheduler/task.h>
#include <entry.h>
#undef ASM

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

.globl ret_from_fork
ret_from_fork:
    bl end_of_sched
    cbz x19, ret_to_user // of x19 is empty then we need to return to user space
    mov x0, x20 // x20 has args for fn that we will branch to
    blr x19     // x19 contains address of fn
ret_to_user:
    bl disable_irq
    pop_regs 0
    eret

.globl cpu_switch_to
cpu_switch_to:
    /* get address to store regs of current task (in cpu_context) */
    mov    x10, #THREAD_CPU_CONTEXT
    add    x8, x0, x10
    
    /* store registers */
    mov    x9, sp
    stp    x19, x20, [x8], #16 
    stp    x21, x22, [x8], #16
    stp    x23, x24, [x8], #16
    stp    x25, x26, [x8], #16
    stp    x27, x28, [x8], #16
    stp    x29, x9, [x8], #16
    str    x30, [x8], #8

    /* TODO: store float registers *
    stp    d16, d17, [x8], #32
    stp    d18, d19, [x8], #32
    stp    d20, d21, [x8], #32
    stp    d22, d23, [x8], #32
    stp    d24, d25, [x8], #32
    stp    d26, d27, [x8], #32
    stp    d28, d29, [x8], #32
    stp    d30, d31, [x8], #32
    */

    /* get address to load regs from of new task */
    add    x8, x1, x10

    /* load integer registers */
    ldp    x19, x20, [x8], #16       
    ldp    x21, x22, [x8], #16
    ldp    x23, x24, [x8], #16
    ldp    x25, x26, [x8], #16
    ldp    x27, x28, [x8], #16
    ldp    x29, x9, [x8], #16
    ldr    x30, [x8], #8

    /* load float registers *
    stp    d16, d17, [x8], #32
    stp    d18, d19, [x8], #32
    stp    d20, d21, [x8], #32
    stp    d22, d23, [x8], #32
    stp    d24, d25, [x8], #32
    stp    d26, d27, [x8], #32
    stp    d28, d29, [x8], #32
    stp    d30, d31, [x8], #32
     */

    /* Set stack pointer to value of pc in cpu_context (on first run this will be ret_from_fork) */
    mov    sp, x9

    /* returns to ret_from_fork if new task, if not return to value of pc in cpu_context */
    ret