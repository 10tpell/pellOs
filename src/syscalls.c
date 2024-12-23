#include <utils/printf.h>
#include <scheduler/scheduler.h>
#include <peripherals/system_timer.h>
#include <fs/vfs.h>

uint64_t syscall_write(char* buf_ptr) {
    char* buf = (char *) buf_ptr;
    printf(buf);
    return 0;
}

uint64_t syscall_fork(void) {
    return kernel_fork(0, 0, 0);
}

uint64_t syscall_exit(uint64_t ret) {
    printf("Thread exiting with retcode: %ld\n", ret);
    exit_task();
    
    // should never reach here
    return 0;
}

#define SYSTEM_TIMER_DEVICE_ID 0

sint64_t syscall_read(file_desc_t fd, char* buf, uint32_t size) {
    if (!is_valid_fd(fd)) return -1;
    if (!buf) return -1;

    return vfs_read(fd, buf, size);
}

file_desc_t syscall_open(const char* path, uint64_t flags) {
    if (!path) return -1;

    return vfs_open(path, flags);
}

void syscall_close(file_desc_t fd) {
    if (fd < 0) return;
    vfs_close(fd);
}
// uint64_t (*sys_call_table[]) (uint64_t) = {&syscall_write, &syscall_fork, &syscall_exit, &syscall_read};

void* sys_call_table[] = {&syscall_write, &syscall_fork, &syscall_exit, &syscall_read, &syscall_open};