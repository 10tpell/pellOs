.globl irq_vector_init
irq_vector_init:
    adr     x0, vectors
    msr     vbar_el1, x0
    ret

.globl enable_irq
enable_irq:
    msr     daifclr, #2
    ret

.globl get_daif
get_daif:
    mrs     x0, daif
    ret

.globl disable_irq
disable_irq:
    msr     daifset, #2
    ret

.globl irq_barrier
irq_barrier:
    dsb sy
    isb sy
    ret

/* uint32_t read_isr_el1 ( void ) */
.globl read_isr_el1
read_isr_el1:
    mrs x0, ISR_EL1