#ifndef __ENTRY_H__
#define __ENTRY_H__

#define S_FRAME_SIZE			272 		// size of all saved registers 
#define S_X0                    0

#define SYNC_INVALID_EL1t		0 
#define IRQ_INVALID_EL1t		1 
#define FIQ_INVALID_EL1t		2 
#define ERROR_INVALID_EL1t		3 

#define SYNC_INVALID_EL1h		4 
#define IRQ_INVALID_EL1h		5 
#define FIQ_INVALID_EL1h		6 
#define ERROR_INVALID_EL1h		7 

#define SYNC_INVALID_EL0_64	    	8 
#define IRQ_INVALID_EL0_64	    	9 
#define FIQ_INVALID_EL0_64		10 
#define ERROR_INVALID_EL0_64		11 

#define SYNC_INVALID_EL0_32		12 
#define IRQ_INVALID_EL0_32		13 
#define FIQ_INVALID_EL0_32		14 
#define ERROR_INVALID_EL0_32		15 
#define SYNC_ERROR 16
#define SYSCALL_ERROR 17
#define DATA_ABORT_ERROR 18

#define N_SYSCALLS 7

#define ESR_EC_SHIFT 26
#define ESR_EC_SVC64 0x15
#define ESR_EC_DABT_LOW 0x24

#endif