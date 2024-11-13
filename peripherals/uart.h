#ifndef __UART_H__
#define __UART_H__

#include "gpio.h"
#include "aux.h"

inline uint8_t uart_txReady() { return reg_read(AUX_MU_LSR_REG_ADDR) & 0x20; }
inline void uart_writeByteBlocks(char byte) { 
    while (!uart_txReady()) {}
    return reg_write(AUX_MU_IO_REG_ADDR, byte);
}

void uart_init();
void uart_transmit(char* buff, uint32_t size);
void uart_transmitToEndLine(char* buff);
void uart_transmitStr(char* buff);

#endif