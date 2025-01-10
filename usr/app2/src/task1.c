#include "../include/usr_syscalls.h"
#include "../include/userlib.h"


void intToStr(uint32_t num, char* str) {
    if (!str) return;

    uint8_t count = 0;
    uint32_t num_tmp = num;
    while (num_tmp >= 1) {
        num_tmp /= 10;
        count++;
    }

    uint8_t i = 1;
    while (num >= 1) {
        str[count - i] = (char) ((uint8_t)(num % 10) + 48);
        num /= 10;
        i++;
    }
}

void main() {
    // char test_buf[60];
    char c;
    call_syscall_write("Inside app2!!\n\r");

    file_desc_t uart = call_syscall_open("/dev/uart1", 0);

    if (uart < 0) {
        call_syscall_write("Couldn't open uart\n");
    } else {
        for(;;) {
            call_syscall_read(uart, &c, 1);
            call_syscall_write(&c);
        }
    }
    call_syscall_exit(0);
}