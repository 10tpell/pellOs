#ifndef __UART_H__
#define __UART_H__

#include "gpio.h"
#include "config.h"
#include "base.h"
#include <reg_utils.h>

#if UART == UART1
#include "aux.h"

uint8_t uart_txReady();
uint8_t uart_rxReady();
void uart_writeByteBlocks(char byte);
uint8_t uart_readByte();
void uart_handle_rx_irq(char byte);
uint8_t uart_listen(uint8_t pid, char* c);

#else

#define UART0_BASE_ADDR PERIPHERAL_BASE_ADDR + 0x201000

#define UART_CR_UARTEN_MASK (1 << 0)
#define UART_CR_RXE_MASK (1 << 9)
#define UART_CR_TXE_MASK (1 << 8)

#define UART_LCRH_WL_8BITS (0b11 << 5)
#define UART_LCRH_FEN_MASK (1 << 4)

#define UART_FR_TXFE_MASK (1 << 7)
#define UART_FR_BUSY_MASK (1 << 3)

typedef struct uart_regs_struct {
    uint32_t dr;            // 0x0              
    uint32_t rsrecr;        // 0x4
    uint64_t res;           // 0x8
    uint64_t res2;          // 0x10
    uint32_t fr;            // 0x18
    uint32_t res3;          // 0x1C
    uint32_t ilpr;          // 0x20
    uint32_t ibrd;          // 0x24
    uint32_t fbrd;          // 0x28
    uint32_t lcrh;          // 0x2c
    uint32_t cr;            // 0x30
    uint32_t ifls;          // 0x34
    uint32_t imsc;          // 0x38
    uint32_t ris;           // 0x3c
    uint32_t mis;           // 0x40
    uint32_t icr;           // 0x44
    uint32_t dmacr;         // 0x48
} uart_regs;

#if UART == UART0
#define UART_REGS ((uart_regs*) (UART0_BASE_ADDR))
#endif

inline uint8_t uart_txReady() { return (reg_read(UART_REGS->fr) & UART_FR_BUSY_MASK) == UART_FR_BUSY_MASK; }
inline void uart_writeByteBlocks(char byte) {
    while(!uart_txReady());
    uint32_t regVal = UART_REGS->dr;
    regVal &= ~(0b11111111);
    regVal |= byte;
    return reg_write((void*) UART_REGS->dr, regVal);
}

#endif

void uart_init();
void uart_transmit(char* buff, uint32_t size);
void uart_transmitToEndLine(char* buff);
void uart_transmitStr(char* buff);

#endif