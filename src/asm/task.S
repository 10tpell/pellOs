#define ASM
#include <scheduler/task.h>
#undef ASM

.globl ret_from_fork
ret_from_fork:
    bl end_of_sched
    mov x0, x20 // x20 has args for fn that we will branch to
    blr x19     // x19 contains address of fn

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
    str    x30, [x8]

    /* get address to load regs from of new task */
    add    x8, x1, x10

    /* load registers */
    ldp    x19, x20, [x8], #16       
    ldp    x21, x22, [x8], #16
    ldp    x23, x24, [x8], #16
    ldp    x25, x26, [x8], #16
    ldp    x27, x28, [x8], #16
    ldp    x29, x9, [x8], #16
    ldr    x30, [x8]
    mov    sp, x9

    /* returns to ret_from_fork if new task, if not return to value of pc in cpu_context */
    ret