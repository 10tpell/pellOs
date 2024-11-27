#define ASM
#include <usr/usr_syscalls.h>
#undef ASM

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

/* uint8_t call_syscall_clone(void* fn, void* args, void* stack) */
.globl call_syscall_clone
call_syscall_clone:
    mov x10, x0
    mov x11, x1
    mov x12, x2

    mov x0, x2
    mov x8, #SYS_CALL_CLONE
    svc 0x0

    cmp x0, #0
    beq thread_start

thread_start:
    mov x29, 0

    /* move fn args (as previously saved into x0) */
    mov x0, x11
    blr x10 // jump to function

    /* finished executing thread, now call exit */
    b call_syscall_exit