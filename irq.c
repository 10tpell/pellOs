#include "peripherals/irq_controller.h"
#include "peripherals/system_timer.h"
#include "peripherals/uart.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

void enable_interrupt_controller()
{
    irq_controller_regs* ptr = IRQ0_REGS_PTR;
    ptr->irq_set_en_0 = (uint32_t) SYS_TIMER_IRQ_1;
}

void show_invalid_exception_message(int type, unsigned long esr, unsigned long address)
{
    uart_transmitStr("Invalid entry hit");
	// printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq(void)
{
	unsigned int irq = IRQ0_REGS_PTR->irq_pending_0;
	switch (irq) {
		case (SYS_TIMER_IRQ_1):
			handle_timer_c0_ISR();
			break;
		default:
            uart_transmitStr("Unknown pending irq");
			// printf("Unknown pending irq: %x\r\n", irq);
	}
}