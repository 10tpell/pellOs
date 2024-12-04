#define ASM
#include <usr/usr_syscalls.h>
#undef ASM

.section ".text.user"

/* void call_syscall_exit(uint64_t ret) */
.globl call_syscall_exit
call_syscall_exit:
    mov x8, #SYS_CALL_EXIT
    svc 0x0
    ret

/* void call_syscall_write(char* buf) */
.globl call_syscall_write
call_syscall_write:
    mov x8, #SYS_CALL_WRITE
    svc 0x0
    ret

/* uint8_t call_syscall_fork() */
.globl call_syscall_fork
call_syscall_fork:
    mov x8, #SYS_CALL_FORK
    svc 0x0
    ret

/* uint32_t call_syscall_read(uint64_t device_id) */
.globl call_syscall_read
call_syscall_read:
    mov x8, #SYS_CALL_READ
    svc 0x0
    ret