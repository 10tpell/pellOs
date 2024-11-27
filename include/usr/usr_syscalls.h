#ifndef __SYSCALLS_USR_H__
#define __SYSCALLS_USR_H__

#define SYS_CALL_WRITE    0 		// syscal numbers 
#define SYS_CALL_CLONE    1 	
#define SYS_CALL_EXIT     2 	

#ifndef ASM

#include <types.h>

uint8_t call_syscall_clone(void* fn, void* args, void* stack);
uint8_t call_syscall_exit(uint64_t ret);
uint8_t call_syscall_write(char* buf);

#endif

#endif