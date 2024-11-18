#ifndef __IRQ_GIC_H__
#define __IRQ_GIC_H__

#ifndef USE_ARMC_IRQS
#include "base.h"

#define GIC_MAX_ISRS 256U

#define IRQ_GIC_BASE_PERIPHERAL         0xFF840000U

#define IRQ_GIC_DIST_OFFSET             0x1000U
#define IRQ_GIC_DIST                    (IRQ_GIC_BASE_PERIPHERAL + IRQ_GIC_DIST_OFFSET)
#define IRQ_GICD_BASE                   IRQ_GIC_DIST
#define IRQ_GICD_ISR_ENABLE_OFFSET      0x100U
#define IRQ_GICD_ISR_ENABLE             (IRQ_GICD_BASE + IRQ_GICD_ISR_ENABLE_OFFSET) // 0xFF841100U

#define IRQ_GICD_IRQ_TARGET_OFFSET      0x800U
#define IRQ_GICD_IRQ_TARGET             (IRQ_GICD_BASE + IRQ_GICD_IRQ_TARGET_OFFSET)

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

/*
0x001C

GICC_ABPR

RW

0x00000003

Aliased Binary Point Register[d]

The minimum value of the Aliased Binary Point Register is 0x3.

0x0020

GICC_AIAR

RO

0x000003FF

Aliased Interrupt Acknowledge Register[d]

0x0024

GICC_AEOIR

WO

-

Aliased End of Interrupt Register[d]

0x0028

GICC_AHPPIR	
RO

0x000003FF

Aliased Highest Priority Pending Interrupt Register[c][d]

0x00D0

GICC_APR0

RW

0x00000000

Active Priority Register

0x00E0

GICC_NSAPR0

RW

0x00000000

Non-Secure Active Priority Register[d]

0x00FC

GICC_IIDR	
RO

0x0202143B

CPU Interface Identification Register, GICC_IIDR

0x1000

GICC_DIR	
WO

-	
Deactivate Interrupt Register
*/

/* System timer irq ids */
#define SYSTEM_TIMER_IRQ_0 (0x60U)
#define SYSTEM_TIMER_IRQ_1 (0x61U)
#define SYSTEM_TIMER_IRQ_2 (0x62U)
#define SYSTEM_TIMER_IRQ_3 (0x63U)

#ifndef ASM
/* function declarations */
void enableISR(uint32_t isrId);
void assignTargets(uint32_t isrId, uint32_t targetId);
void setup_gic();
void gic_debug_print();

#endif

#else
#error "This file has been included whilst using ARMC IRQs"
#endif

#endif