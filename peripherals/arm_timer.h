#ifndef __ARM_TIMER_H__
#define __ARM_TIMER_H__

#include "base.h"

#define ARM_TIMER_BASE_PERIPHERAL   PERIPHERAL_BASE_ADDR + 0xB000U

#define ARM_TIMER_LOAD_OFFSET       0x400U
#define ARM_TIMER_LOAD_ADDR         (ARM_TIMER_BASE_PERIPHERAL + ARM_TIMER_LOAD_OFFSET)

typedef struct arm_timer_regs_struct {
    volatile uint32_t load;
    volatile uint32_t value;
    volatile uint32_t cntrl;
    volatile uint32_t irqcntrl;
    volatile uint32_t rawirq;
    volatile uint32_t mskirq;
    volatile uint32_t reload;
    volatile uint32_t prediv;
    volatile uint32_t freecnt;
} arvolatile m_timer_regs;

#define ARM_TIMER_REGS_PTR          ((timer_regs *) ARM_TIMER_LOAD_OFFSET)

#endif