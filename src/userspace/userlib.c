#include <usr/userlib.h>
#include <usr/usr_syscalls.h>
#include <types.h>

/*
.globl memcpy
memcpy:
	ldr x3, [x1], #8
	str x3, [x0], #8
	subs x2, x2, #8
	b.gt memcpy
	ret

.globl memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret
*/

// void memset(uint8_t* dest, uint8_t pattern, uint64_t size) {
//     for(uint64_t i = 0; i < size; i++) {
//         dest[i] = pattern;
//     }
// }

#define SYSTEM_TIMER_DEVICE_ID 0


static uint8_t hasRead = 0;
void delay(uint32_t delay_ms) {
    uint32_t currentTime = call_syscall_read(SYSTEM_TIMER_DEVICE_ID);
	if(!hasRead) {
		call_syscall_write("one read");
		hasRead = 1;
	}
    uint64_t targetTime = currentTime + delay_ms;
	while(currentTime < targetTime) {
		currentTime = call_syscall_read(SYSTEM_TIMER_DEVICE_ID);
	}
    return;
}