#include <config.h>
#if IRQ_CONTROLLER == USE_ARMC_IRQS
#include <peripherals/irq_armc.h>
#else
    #if IRQ_CONTROLLER == USE_GIC_IRQS
    #include <peripherals/irq_gic.h>
    #endif
#endif

#include <peripherals/system_timer.h>
#include <peripherals/uart.h>
#include <utils/printf.h>

#if IRQ_CONTROLLER < USE_TUT_IRQS
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
	"ERROR_INVALID_EL0_32",
    "SYNC_ERROR",
    "SYSCALL_ERROR",
    "DATA_ABORT_ERROR"
};

#if IRQ_CONTROLLER == USE_ARMC_IRQS

void enable_interrupt_controller()
{
    irq_controller_regs* ptr = IRQ0_REGS_PTR;
    ptr->irq_set_en_0 = (uint32_t) SYSTEM_TIMER_IRQ_1;
}

#else

/*
assigns a cpu target to handle an isr.
4 ISRs fit in one target reg.
8 possible cpus/targets to handle ISR
bit 0 is cpu0, bit 7 is cpu7
*/
void assignTargets(uint32_t isrId, uint32_t targetId) {
    uint32_t isrBlock = isrId / 4;
    uint32_t targetOffset = ((isrId % 4) * 8) + targetId;
    volatile uint32_t* targetReg = (volatile uint32_t *) IRQ_GICD_IRQ_TARGET + (isrBlock);
    *targetReg |= 1 << targetOffset;
}


void enableISR(uint32_t isrId)
{
    uint32_t isrBlock = isrId / 32;
    uint32_t isrOffset = isrId % 32;
    volatile uint32_t* enableReg = (volatile uint32_t*) IRQ_GICD_ISR_ENABLE + (isrBlock); 
    *enableReg |= 1 << isrOffset;
}

void enable_interrupt_controller()
{
    assignTargets(SYSTEM_TIMER_IRQ_1, 0);
    enableISR(SYSTEM_TIMER_IRQ_1);
}
/*
#define IRQ_GIC_BASE_PERIPHERAL         0xFF840000U

#define IRQ_GIC_DIST_OFFSET             0x1000U
#define IRQ_GIC_DIST                    (IRQ_GIC_BASE_PERIPHERAL + IRQ_GIC_DIST_OFFSET)
#define IRQ_GICD_BASE                   IRQ_GIC_DIST

#define IRQ_GIC_CPU_OFFSET              0x2000U
#define IRQ_GIC_CPU                     (IRQ_GIC_BASE_PERIPHERAL + IRQ_GIC_CPU_OFFSET)
#define IRQ_GICC_BASE                   IRQ_GIC_CPU
#define IRQ_GICC_CTLR_OFFSET            0x0U
#define IRQ_GICC_CTLR                   (IRQ_GICC_BASE + IRQ_GICC_CTLR_OFFSET)
#define IRQ_GICC_PMR_OFFSET             0x04U
#define IRQ_GICC_PMR                    (IRQ_GICC_BASE + IRQ_GICC_PMR_OFFSET)
#define IRQ_GICC_BPR_OFFSET             0x08U
#define IRQ_GICC_BPR                    (IRQ_GICC_BASE + IRQ_GICC_BPR_OFFSET)
#define IRQ_GICC_IAR_OFFSET             0x0CU
#define IRQ_GICC_IAR                    (IRQ_GICC_BASE + IRQ_GICC_IAR_OFFSET)  // 0xFF84200C
#define IRQ_GICC_EOIR_OFFSET            0x10U
#define IRQ_GICC_EOIR                   (IRQ_GICC_BASE + IRQ_GICC_EOIR_OFFSET)
#define IRQ_GICC_RPR_OFFSET             0x14U
#define IRQ_GICC_RPR                    (IRQ_GICC_BASE + IRQ_GICC_RPR_OFFSET)
#define IRQ_GICC_HPPIR_OFFSET           0x18U
#define IRQ_GICC_HPPIR                  (IRQ_GICC_BASE + IRQ_GICC_HPPIR_OFFSET)
*/

void gic_debug_print() 
{
    printf("============== GIC REGS ===============\n");
    for (uint32_t i = 0; i < GIC_MAX_ISRS; i+=32)
    {
        printf("GICD_ISR_ENABLE_%d (0x%08x): 0x%08x\n", i/32, ((volatile uint32_t *) (IRQ_GICD_ISR_ENABLE + i/8)), *((volatile uint32_t *) (IRQ_GICD_ISR_ENABLE + i/8)));
    }
    for (uint32_t i = 0; i < GIC_MAX_ISRS; i+=4)
    {
        printf("GICD_IRQ_TARGET_%d (0x%08x): 0x%08x\n", i/4, ((volatile uint32_t *) (IRQ_GICD_IRQ_TARGET + i)), *((volatile uint32_t *) (IRQ_GICD_IRQ_TARGET + i)));
    }
    for (uint32_t i = 0; i < GIC_MAX_ISRS; i+=32)
    {
        printf("GICD_ISRPEND_%d (0x%08x): 0x%08x\n", i/32, ((volatile uint32_t *) (IRQ_GICD_ICPEND + i/8)), *((volatile uint32_t *) (IRQ_GICD_ICPEND + i/8)));
    }
    printf("GICC_IAR: 0x%08x\n", *((volatile uint32_t *) IRQ_GICC_IAR));
    printf("GICC_EOIR: 0x%08x\n", *((volatile uint32_t *) IRQ_GICC_EOIR));
    printf("GICC_HPPIR: 0x%08x\n", *((volatile uint32_t *) IRQ_GICC_HPPIR));
}

void gic_debug_print_for_irq(uint32_t irqId)
{
    printf("GICC_CTLR: 0x%08x\nGICD_CTLR: 0x%08x\n", *((volatile uint32_t*) IRQ_GICC_BASE), *((volatile uint32_t*) IRQ_GICD_BASE));
    volatile uint32_t* groupReg = (volatile uint32_t*) IRQ_GICD_ISR_GROUP + (irqId / 32);
    printf("GICD_ISR_GROUP_%d (0x%08x): 0x%08x\n", irqId/32, groupReg, *groupReg);
    volatile uint32_t* enableReg = (volatile uint32_t*) IRQ_GICD_ISR_ENABLE + (irqId / 32);
    printf("GICD_ISR_ENABLE_%d (0x%08x): 0x%08x\n", irqId/32, enableReg, *enableReg);

    volatile uint32_t* targetReg = (volatile uint32_t*) IRQ_GICD_IRQ_TARGET + (irqId / 4);
    printf("GICD_IRQ_TARGET_%d (0x%08x): 0x%08x\n", irqId/4, targetReg, *targetReg);

    volatile uint32_t* pendReg = (volatile uint32_t*) IRQ_GICD_ICPEND + (irqId/32);
    printf("GICD_ISRPEND_%d (0x%08x): 0x%08x\n", irqId/32, pendReg, *pendReg);

    printf("GICC_IAR: 0x%08x\n", *((volatile uint32_t *) IRQ_GICC_IAR));
    printf("GICC_EOIR: 0x%08x\n", *((volatile uint32_t *) IRQ_GICC_EOIR));
    printf("GICC_HPPIR: 0x%08x\n", *((volatile uint32_t *) IRQ_GICC_HPPIR));
}

#endif

void handle_irq(void)
{
    #if IRQ_CONTROLLER == USE_ARMC_IRQS
	unsigned int irq = IRQ0_REGS_PTR->irq_pending_0;
    #else
    uint32_t irqAck = *((volatile uint32_t*) IRQ_GICC_IAR);
    uint32_t irq = irqAck & 0x2FF;
    #endif
	switch (irq) {
		case (SYSTEM_TIMER_IRQ_1):
            #if IRQ_CONTROLLER == USE_GIC_IRQS
            *((volatile uint32_t*)IRQ_GICC_EOIR) |= irqAck;
            #endif
			handle_timer_c0_ISR();
			break;
		default:
			printf("Unknown pending irq: %x\r\n", irq);
            break;
	}
}

void show_invalid_exception_message(int type, unsigned long esr, unsigned long address, unsigned long far)
{
	printf("%s, ESR: %x, address: %lx, far: %lx\r\n", entry_error_messages[type], esr, address, far);
    //backup for lack of long support
    printf("address_upper: %x , address_lower: %x\n", address >> 32, (address << 32) >> 32 );
}

#endif
