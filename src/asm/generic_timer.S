/* uint64_t read_cntrl(); */
.globl read_cntrl
read_cntrl:
    mrs x0, cntp_ctl_el0
    ret

/* uint64_t read_comparator(); */
.globl read_comparator
read_comparator:
    mrs x0, cntp_cval_el0
    ret

/* uint64_t read_timer(); */
.globl read_timer
read_timer:
    mrs x0, cntp_tval_el0
    ret

/* uint64_t read_timer_freq() */
.globl read_timer_freq
read_timer_freq:
    mrs x0, cntfrq_el0
    ret

.globl set_cntfreq
set_cntfreq:
    msr cntfrq_el0, x0
    ret

/* void set_timer() */
.globl set_timer
set_timer:
    mrs     x0, cntfrq_el0
    mov     x1, 1000                 // 1/100 s
    udiv    x1, x0, x1

    mrs     x0, cntpct_el0
    add     x0, x0, x1                  // 1st interrupt in 10 ms
    msr     cntp_cval_el0, x0

    mrs     x0, cntp_ctl_el0
    orr     x0, x0, 1 << 0    //  1 << 0     Interrupts NOT masked.
    msr     cntp_ctl_el0, x0
    ret