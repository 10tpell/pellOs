#include "../include/usr_syscalls.h"
#include "../include/userlib.h"


void main() {
    // char test_buf[60];
    call_syscall_write("Inside userspace!!\n\r");
    
    uint8_t pid = call_syscall_fork();
    if (pid == 0) {
        call_syscall_write("PID: ");
        char pid_buf[1] = "";
        pid_buf[0] = (char) pid + 48;
        call_syscall_write(pid_buf);
        call_syscall_write("\n");

        call_syscall_write("Attempting to disable isrs (should trap)\n");
        asm ("msr cptr_el3, x1");//
        // should trap here
    } else {
        call_syscall_write("PID: ");
        char pid_buf = '1';
        call_syscall_write(&pid_buf);
        call_syscall_write("\n");
        
        call_syscall_write("Entering delay loop here \r\n");
        // delay_init();
        while(1) {
            call_syscall_write("userspace_tick\n");
            call_syscall_wait(1000);
        }
    }
    
    call_syscall_exit(0);
}