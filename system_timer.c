#include "peripherals/system_timer.h"
#include "arm/time.h"
#include "peripherals/uart.h"

void system_timer_init() {
    SYSTEM_TIMER_REGS_PTR->c1 = get_timer_val() + CLOCK_RATE;
}

void handle_timer_c0_ISR() {
    uint32_t current_timer_val = SYSTEM_TIMER_REGS_PTR->clo;
    SYSTEM_TIMER_REGS_PTR->c1 = get_timer_val() + CLOCK_RATE;

    uart_transmitStr("ISR: 1 second has passed...");
}

uint32_t get_timer_val() {
    return SYSTEM_TIMER_REGS_PTR->clo;
}

uint32_t wait_time(uint32_t time_us) {
    uint32_t startTime = get_timer_val();
    while (get_timer_val() < startTime + time_us) {}
    return time_us;
}

uint8_t poll_isr(uint8_t timer_id) {
    uint32_t cs = SYSTEM_TIMER_REGS_PTR->cs;
    cs = (cs >> timer_id) & 0x1U;
    return cs;
}