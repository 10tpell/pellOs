#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

#include "base.h"

#define SYSTEM_TIMER_PERIPHERAL_BASE        (PERIPHERAL_BASE_ADDR + 0x3000)
#define CLOCK_RATE 100000

/*
CS System Timer Control/Status
CLO System Timer Counter Lower 32 bits
CHI System Timer Counter Higher 32 bits
C0 System Timer Compare 0
C1 System Timer Compare 1
C2 System Timer Compare 2
C3 System Timer Compare 3
*/
typedef struct system_timer_struct {
    volatile uint32_t cs;
    volatile uint32_t clo;
    volatile uint32_t chi;
    volatile uint32_t c0;
    volatile uint32_t c1;
    volatile uint32_t c2;
    volatile uint32_t c3;
} system_timer_regs;

#define SYSTEM_TIMER_REGS_PTR               ((system_timer_regs *)SYSTEM_TIMER_PERIPHERAL_BASE)

void system_timer_init();
void handle_timer_c0_ISR();

#endif