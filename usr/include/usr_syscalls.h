#ifndef __SYSCALLS_USR_H__
#define __SYSCALLS_USR_H__

#define SYS_CALL_WRITE      0 		// syscal numbers 
#define SYS_CALL_FORK       1 	
#define SYS_CALL_EXIT       2
#define SYS_CALL_READ       3 
#define SYS_CALL_OPEN       4	
#define SYS_CALL_CLOSE      5

#ifndef ASM

#include "types.h"

typedef sint16_t file_desc_t;

uint8_t call_syscall_exit(uint64_t ret);
uint8_t call_syscall_fork();
uint8_t call_syscall_write(char* buf);
sint64_t call_syscall_read(file_desc_t fd, char* buf, uint32_t size);
file_desc_t call_syscall_open(const char* path, uint64_t flags);
void call_syscall_close(file_desc_t fd);

#endif

#endif