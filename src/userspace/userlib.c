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


static file_desc_t timer_fd = -1;

/*
block on task for a set amount of time:
* delay_ms: time in us to wait
*/
void delay(uint32_t delay_ms) {
	uint32_t buf = 0;
    call_syscall_read(timer_fd, (char *) &buf, sizeof(uint32_t));
    uint64_t targetTime = buf + delay_ms;
	while(buf < targetTime) {
		call_syscall_read(timer_fd, (char *) &buf, sizeof(uint32_t));
	}
    return;
}

void delay_init() {
	if (timer_fd < 0) {
		timer_fd = call_syscall_open("/dev/timer", 0);
	}
}