#include <utils/printf.h>
#include <scheduler/scheduler.h>
#include <peripherals/system_timer.h>

uint64_t syscall_write(uint64_t buf_ptr) {
    char* buf = (char *) buf_ptr;
    printf(buf);
    return 0;
}

uint64_t syscall_fork(uint64_t not_used) {
    (void) (not_used);
    return kernel_fork(0, 0, 0);
}

uint64_t syscall_exit(uint64_t ret) {
    printf("Thread exiting with retcode: %ld\n", ret);
    exit_task();
    
    // should never reach here
    return 0;
}

#define SYSTEM_TIMER_DEVICE_ID 0

uint64_t syscall_read(uint64_t deviceId) {
    if (deviceId == SYSTEM_TIMER_DEVICE_ID) {
        return (uint64_t) get_timer_val();
    }
    return (uint64_t) -1;
}

uint64_t (*sys_call_table[]) (uint64_t) = {&syscall_write, &syscall_fork, &syscall_exit, &syscall_read};