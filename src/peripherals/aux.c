#include <peripherals/aux.h>
#include <peripherals/uart.h>

void handle_aux_irq() {
    if ((*((uint32_t *) AUX_IRQ_ADDR) & 1 )== 1) {
        /* Is an IRQ from UART_1 */
        if ((*((uint32_t *) AUX_MU_IIR_REG_ADDR) & 4) == 4) {
            /* is RX */
            *((uint32_t *) AUX_MU_IIR_REG_ADDR) |= 2;
            uart_handle_rx_irq(uart_readByte());
        } else if ((*((uint32_t *) AUX_MU_IIR_REG_ADDR) & 2) == 2) {
            /* TX */
            *((uint32_t *) AUX_MU_IIR_REG_ADDR) |= 1;
        }
    }
}