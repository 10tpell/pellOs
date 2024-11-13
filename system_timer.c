#include "peripherals/system_timer.h"
#include "arm/time.h"
#include "peripherals/uart.h"

void system_timer_init() {
    uint32_t current_timer_val = SYSTEM_TIMER_REGS_PTR->clo;
    SYSTEM_TIMER_REGS_PTR->c1 = current_timer_val + CLOCK_RATE;
}

void handle_timer_c0_ISR() {
    uint32_t current_timer_val = SYSTEM_TIMER_REGS_PTR->clo;
    SYSTEM_TIMER_REGS_PTR->c1 = current_timer_val + CLOCK_RATE;

    uart_transmitStr("1 second has passed...");
}