#ifndef __CONFIG_H__
#define __CONFIG_H__

#define USE_ARMC_IRQS 0
#define USE_GIC_IRQS 1
#define USE_TUT_IRQS 2

#define IRQ_CONTROLLER USE_GIC_IRQS
// #define UART_DEBUG

#define DEBUG_OFF 0
#define DEBUG_ON 1
#define EXTRA_DEBUG DEBUG_OFF

#endif