#include "../include/usr_syscalls.h"
#include "../include/userlib.h"


void user_task() {
    // char test_buf[60];
    call_syscall_write("Inside userspace!!\n\r");
    uint8_t pid = 1; //call_syscall_fork();
    if (pid == 0) {
        call_syscall_write("Attempting to disable isrs (should trap)\n");
        asm ("msr daifset, #2");
        // should trap here
    } else {
        call_syscall_write("Entering delay loop here \r\n");
        delay_init();
        while(1) {
            delay(1000000);
            call_syscall_write("userspace tick\r\n");
        }
    }
    
    call_syscall_exit(0);
}