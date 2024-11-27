#include <types.h>
#include <peripherals/io.h>
#include <arm/irq.h>
#include <config.h>
#include <scheduler/scheduler.h>
#include <usr/usr_syscalls.h>

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
#include <utils/memutils.h>

void putc(void* p, char c) {
    #ifdef UART_DEBUG
    uart_writeByteBlocks(c);
    #else
    kernel_writeChar(c);
    #endif
}

void user_task() {
    char test_buf[30];
    memset(test_buf, 0, 30);

    tfp_sprintf(test_buf, "Inside userspace!!\n\r");
    call_syscall_write(test_buf);
    call_syscall_exit(0);
}

void kernel_task() {
    printf("entering task2\n");
    if (move_to_userspace(&user_task, get_current_task()) < 0) {
        printf("Error while moving to userspace \n");
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

    printf("Enabling IRQs...\n");
    enable_interrupt_controller();
    irq_barrier();
    enable_irq();
    irq_barrier();
    
    kernel_fork(TASK_FLAGS_KERNEL_THREAD, &kernel_task, 0, 0);

    while(1) {
        /* we're here forever */
        schedule();
    }
}