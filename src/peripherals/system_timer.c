#include <peripherals/system_timer.h>
#include <arm/time.h>
#include <peripherals/uart.h>
#include <peripherals/framebuffer.h>
#include <scheduler/scheduler.h>
#include <utils/printf.h>

uint8_t tick_occured = 0;

void system_timer_init() {
    SYSTEM_TIMER_REGS_PTR->c1 = get_timer_val() + TICK_INTERVAL;
}

void handle_timer_c0_ISR() {
    if (!tick_occured) tick_occured = 1; // So we can sense whether we have system timer interval (QEMU does not)
    
    SYSTEM_TIMER_REGS_PTR->c1 = get_timer_val() + TICK_INTERVAL;
    SYSTEM_TIMER_REGS_PTR->cs |= 2;
    scheduler_tick();
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