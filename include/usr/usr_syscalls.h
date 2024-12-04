#ifndef __SYSCALLS_USR_H__
#define __SYSCALLS_USR_H__

#define SYS_CALL_WRITE    0 		// syscal numbers 
#define SYS_CALL_FORK     1 	
#define SYS_CALL_EXIT     2
#define SYS_CALL_READ     3 	

#ifndef ASM

#include <types.h>

uint8_t call_syscall_exit(uint64_t ret);
uint8_t call_syscall_fork();
uint8_t call_syscall_write(char* buf);
uint32_t call_syscall_read(uint64_t devId);

#endif

#endif