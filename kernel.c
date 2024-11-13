#include "types.h"
#include "io.h"

int main() {
    uart_init();
    uart_transmitStr("Kernel booting...");
    for(;;) {
        /* we're here forever */
    }
}