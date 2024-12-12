#ifndef __CONFIG_H__
#define __CONFIG_H__

/* IRQ Controller selection */
#define USE_ARMC_IRQS 0
#define USE_GIC_IRQS 1
#define USE_TUT_IRQS 2

#define IRQ_CONTROLLER USE_GIC_IRQS

/* UART_DEBUG enables printf over uart. Commenting this out prints it over display buffer*/
#define UART_DEBUG

/* Which UART peripheral should be used */
#define UART0 0 // PL011
#define UART1 1 // MiniUart
#define UART UART1

/* Enable extra debug messages */
#define DEBUG_OFF 0
#define DEBUG_ON 1
#define EXTRA_DEBUG DEBUG_OFF

/* USE_BITMAP uses a more compressed method to store which pages have been used */
#define USE_BITMAP

#endif