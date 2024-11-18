#include "peripherals/uart.h"
#include "reg_utils.h"

void uart_init() {
    reg_write(AUX_ENABLES_ADDR, 1); // enable mini uart
    reg_write(AUX_MU_IER_REG_ADDR, 0); // disable uart irqs
    reg_write(AUX_MU_CNTL_REG_ADDR, 0); // effectively disables uart rx/tx
    reg_write(AUX_MU_LCR_REG_ADDR, 1); // 8 data bits
    reg_write(AUX_MU_MCR_REG_ADDR, 0);
    reg_write(AUX_MU_BAUD_REG_ADDR, AUX_MU_BAUD(115200));
    gpio_useAlt(14, GPIO_FUNC_ALT_5);
    gpio_useAlt(15, GPIO_FUNC_ALT_5);
    reg_write(AUX_MU_CNTL_REG_ADDR, 3); // Enable rx/tx
}

void uart_transmit(char* buff, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        uart_writeByteBlocks(buff[i]);
    }
    uart_writeByteBlocks('\r');
    uart_writeByteBlocks('\n');
}

void uart_transmitToEndLine(char* buff) {
    uint32_t i = 0;
    while(buff[i] != '\n') {
        uart_writeByteBlocks(buff[i]);
        i++;
    }
    uart_writeByteBlocks(buff[i]);
    uart_writeByteBlocks('\r');
}

void uart_transmitStr(char* buff) {
    uint32_t i = 0;
    while(buff[i] != 0) {
        uart_writeByteBlocks(buff[i]);
        i++;
    }
    uart_writeByteBlocks('\n');
    uart_writeByteBlocks('\r');
}

void _putchar(char character) {
    uart_writeByteBlocks(character);
}