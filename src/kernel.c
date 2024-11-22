#include <types.h>
#include <peripherals/io.h>
#include <arm/irq.h>
#include <config.h>

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

int main() {
    framebuffer_init();
    uart_init();
    uart_transmitStr("Kernel booting...");
    init_printf((void*)0,putc);
    printf("Booting...\n");
    irq_vector_init();
    system_timer_init();

    printf("Enabling IRQs...\n");
    enable_interrupt_controller();
    irq_barrier();
    enable_irq();
    irq_barrier();
    
    while(1) {
        /* we're here forever */
    }
}