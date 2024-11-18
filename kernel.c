#include "types.h"
#include "peripherals/io.h"
#include "arm/irq.h"
#include "peripherals/irq_gic.h"
#include "peripherals/system_timer.h"
#include "printf.h"

void putc(void* p, char c) {
    uart_writeByteBlocks(c);
}

int main() {
    uart_init();
    uart_transmitStr("Kernel booting...");
    init_printf((void*)0,putc);
    irq_vector_init();
    system_timer_init();
    uart_transmitStr("Enabling IRQs.");
    enable_interrupt_controller();
    irq_barrier();
    enable_irq();
    for(;;) {
        /* we're here forever */
        wait_time(CLOCK_RATE);
        printf("1 sec passed.\n");
        if(poll_isr(1) == 1) {
            gic_debug_print();
            printf("ISR should be firing\n");
        }
    }
}