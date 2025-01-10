#include "peripherals/uart.h"
#include "reg_utils.h"
#include <scheduler/scheduler.h>

void uart_init() {
    uint32_t alt = 0;
    #if UART == UART1
    reg_write((uint32_t *) (AUX_ENABLES_ADDR), 1); // enable mini uart
    reg_write((uint32_t *) (AUX_MU_IER_REG_ADDR), 1); // enable uart rx irq
    reg_write((uint32_t *) (AUX_MU_CNTL_REG_ADDR), 0); // effectively disables uart rx/tx
    reg_write((uint32_t *) (AUX_MU_LCR_REG_ADDR), 3); // 8 data bits
    reg_write((uint32_t *) (AUX_MU_MCR_REG_ADDR), 0);
    reg_write((uint32_t *) (AUX_MU_BAUD_REG_ADDR), AUX_MU_BAUD(115200));

    alt = GPIO_FUNC_ALT_5;
    #else
    UART_REGS->cr = 0; // disable uart before changing config

    uint32_t lcrh = UART_REGS->lcrh;
    lcrh |= UART_LCRH_WL_8BITS;
    lcrh &= ~(UART_LCRH_FEN_MASK); // disable FIFOs
    UART_REGS->lcrh = lcrh;

    /* for baudrate
    115200

    BAUDDIV = (FUARTCLK/(16 * 115200))
        = (48000000/(16*115200))
        = 26.04166666

    => ibrd = 26
    => fbrd = 3
    */
    UART_REGS->ibrd = 26;
    UART_REGS->fbrd = 3;

    alt = GPIO_FUNC_ALT_0;
    #endif

    gpio_useAlt(14, alt);
    gpio_useAlt(15, alt);

    #if UART == UART1
    reg_write((uint32_t *) (AUX_MU_CNTL_REG_ADDR), 3); // Enable rx/tx
    #else
    uint32_t cr = UART_REGS->cr;
    cr |= UART_CR_RXE_MASK; // enable uart rx
    cr |= UART_CR_TXE_MASK; // enable uart tx
    cr |= UART_CR_UARTEN_MASK; // enable uart
    UART_REGS->cr = cr;
    #endif
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

void uart_writeByteBlocks(char byte) { 
    while (!uart_txReady()) {}
    reg_write((uint32_t *) AUX_MU_IO_REG_ADDR, byte);
}

uint8_t uart_readByte() {
    return reg_read((uint32_t *) AUX_MU_IO_REG_ADDR);
}

uint8_t uart_txReady() { 
    return reg_read((uint32_t *) AUX_MU_LSR_REG_ADDR) & 0x20; 
}

uint8_t uart_rxReady() {
    return reg_read((uint32_t *) AUX_MU_LSR_REG_ADDR) & 0x01;
}

static char c = 0;
static sint16_t listener = -1;
void uart_handle_rx_irq(char byte) {
    if (listener < 0) return;

    c = byte;
    wake_task(listener);
}

uint8_t uart_listen(uint8_t pid, char* buf) {
    if (listener == -1) {
        listener = pid;

        task_struct* task = get_current_task();
        task->state = TASK_STATE_BLOCKED;
        schedule();

        *buf = c; 
        listener = -1;
        return 1;
    } else {
        #if EXTRA_DEBUG == DEBUG_ON
        printf("UART: Not setting listener as already set");
        #endif
        return 0;
    }
}