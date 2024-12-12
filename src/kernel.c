#include <types.h>
#include <peripherals/io.h>
#include <arm/irq.h>
#include <config.h>
#include <scheduler/scheduler.h>
#include <usr/programs.h>
#include <mm/mm.h>
#include <mm/paging.h>
#include <lib/kmalloc.h>

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
    (void) (p);
    #ifdef UART_DEBUG
    uart_writeByteBlocks(c);
    #else
    kernel_writeChar(c);
    #endif
}

void kernel_task() {
    uart_transmitStr("kernel_task()\n");
    if (move_to_userspace(&user_begin, (uint64_t) &user_end - (uint64_t) &user_begin, (uint64_t) &user_task - (uint64_t) &user_begin, get_current_task()) < 0) {
        printf("Error while moving to userspace \n");
    }
}

void kernel_task1() {
    uart_transmitStr("kernel_task1()");
    while(1) {
        printf("tick");
        wait_time(TICK_INTERVAL*10);
    }
}

int main() {
    uart_init();
    uart_transmitStr("Kernel booting...");
    uart_transmitStr("Removing identity mapping of 0x8000");
    set_page_directory(0x0);

    uart_transmitStr("Initialising framebuffer and printf");
    // framebuffer_init();
    init_printf((void*)0,putc);

    // kernel_writeStr("hello from framebuffer\n");
    printf("Booting...\n");
    irq_vector_init();
    system_timer_init();

    printf("Enabling IRQs...\n");
    enable_interrupt_controller();
    irq_barrier();
    enable_irq();
    irq_barrier();
    
    kernel_fork(TASK_FLAGS_KERNEL_THREAD, (uint64_t) &kernel_task, 0);
    kernel_fork(TASK_FLAGS_KERNEL_THREAD, (uint64_t) &kernel_task1, 0);

    printf("allocating kernel heap...\n");
    uint64_t kernel_heap_phys = get_next_free_page();

    for(uint8_t i = 0; i < KERNEL_HEAP_SIZE_PAGES-1; i++) {
        get_next_free_page();
    }

    void* heap_ptr = (void *) (kernel_heap_phys + VIRTUAL_ADDRESS_START);

    /* kmalloc test */
    kmalloc_init(heap_ptr);

    uint64_t* ptr64 = (uint64_t *) kmalloc(sizeof(uint64_t));
    uint32_t* ptr32 = (uint32_t *) kmalloc(sizeof(uint32_t));
    uint32_t* ptr32_2 = (uint32_t *) kmalloc(sizeof(uint32_t));
    uint64_t* ptr64_2 = (uint64_t *) kmalloc(sizeof(uint64_t));

    printf("ptr32_adr: 0x%x, value: 0x%x\n", ptr32, *ptr32);
    kfree(ptr32);
    printf("free1\n");
    kfree(ptr32_2);
    printf("free2\n");
    uint64_t* ptr_64_3 = (uint64_t *) kmalloc(sizeof(uint64_t));
    printf("ptr64_3_adr: 0x%x\n", ptr_64_3);


    while(1) {
        /* we're here forever */
        schedule();
    }
}