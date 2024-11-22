#ifndef __IRQ_CONTROLLER_H__
#define __IRQ_CONTROLLER_H__

#ifdef USE_ARMC_IRQS

#include "base.h"

#define IRQ_BASE_PERIPHERAL     PERIPHERAL_BASE_ADDR + 0xB000U
// 0xFE000000 + 0xB000
// 0xFE00B740
typedef struct irq_controller_regs_struct {
    volatile uint32_t irq_pending_0;
    volatile uint32_t irq_pending_1;
    volatile uint32_t irq_pending_2;
    volatile uint32_t res0;
    volatile uint32_t irq_set_en_0;
    volatile uint32_t irq_set_en_1;
    volatile uint32_t irq_set_en_2;
    volatile uint32_t res1;
    volatile uint32_t irq_clr_en_0;
    volatile uint32_t irq_clr_en_1;
    volatile uint32_t irq_clr_en_2;
    volatile uint32_t res2;
    volatile uint32_t irq_status_0;
    volatile uint32_t irq_status_1;
    volatile uint32_t irq_status_2;
} irq_controller_regs;

#define IRQ0_OFFSET             0x200U
#define IRQ0_BASE_ADDR          (IRQ_BASE_PERIPHERAL + IRQ0_OFFSET)
#define IRQ0_REGS_PTR           ((irq_controller_regs *) IRQ0_BASE_ADDR)

#define IRQ1_OFFSET             0x240U
#define IRQ1_BASE_ADDR          (IRQ_BASE_PERIPHERAL + IRQ1_OFFSET)
#define IRQ1_REGS_PTR           ((irq_controller_regs *) IRQ1_BASE_ADDR)

#define IRQ2_OFFSET             0x280U
#define IRQ2_BASE_ADDR          (IRQ_BASE_PERIPHERAL + IRQ2_OFFSET)
#define IRQ2_REGS_PTR           ((irq_controller_regs *) IRQ2_BASE_ADDR)

#define IRQ3_OFFSET             0x2C0U
#define IRQ3_BASE_ADDR          (IRQ_BASE_PERIPHERAL + IRQ3_OFFSET)
#define IRQ3_REGS_PTR           ((irq_controller_regs *) IRQ3_BASE_ADDR)

#define SYS_TIMER_IRQ_0         1
#define SYSTEM_TIMER_IRQ_1      2
#define SYS_TIMER_IRQ_2         4
#define SYS_TIMER_IRQ_3         8

void enable_interrupt_controller();

#else
#error "This file has been included when not using armc irqs"
#endif

#endif