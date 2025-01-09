#include <peripherals/generic_timer.h>
#include <scheduler/scheduler.h>

void generic_timer_init() {
    set_timer();
}

void handle_generic_timer_irq() {
    set_timer();
    
    scheduler_tick();
}