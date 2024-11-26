#include <types.h>
#include <peripherals/io.h>
#include <arm/irq.h>
#include <config.h>
#include <scheduler/scheduler.h>

#if IRQ_CONTROLLER == USE_ARMC_IRQS
    #include <peripherals/irq_armc.h>
#else
    #if IRQ_CONTROLLER == USE_GIC_IRQS
        #include <peripherals/irq_gic.h>
    #else
        #include <peripherals/irq_tut.h>
    #endif
#endif

#include <peripherals/system_timer.h>
#include <peripherals/framebuffer.h>
#include <utils/printf.h>
#include <utils/shell.h>

void putc(void* p, char c) {
    #ifdef UART_DEBUG
    uart_writeByteBlocks(c);
    #else
    kernel_writeChar(c);
    #endif
}

void task1() {
    printf("entering task1\n");
    while(1) {
        wait_time(TICK_INTERVAL/2);
        printf("task1\n");
    }
}

void task2() {
    printf("entering task2\n");
    enable_irq();
    while(1) {
        wait_time(TICK_INTERVAL*3);
        printf("task2\n");
    }
}

int main() {
    framebuffer_init();
    uart_init();
    uart_transmitStr("Kernel booting...");
    init_printf((void*)0,putc);
    printf("Booting...\n");
    irq_vector_init();
    system_timer_init();

    kernel_fork(&task1, (void*) 0);
    kernel_fork(&task2, (void*) 0);

    printf("Enabling IRQs...\n");
    enable_interrupt_controller();
    irq_barrier();
    enable_irq();
    irq_barrier();
    
    while(1) {
        /* we're here forever */
        schedule();
    }
}