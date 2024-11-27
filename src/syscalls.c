#include <utils/printf.h>
#include <scheduler/scheduler.h>

void syscall_write(char* buf) {
    printf(buf);
}

void syscall_clone(void* stack) {
    return kernel_fork(0, 0, 0, stack);
}

void syscall_exit(uint8_t ret) {
    printf("Thread exiting with retcode: %d\n", ret);
    exit_task();
}

void * const sys_call_table[] = {&syscall_write, &syscall_clone, &syscall_exit};