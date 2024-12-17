#ifndef __USERLIB_H__
#define __USERLIB_H__

#include <types.h>

// void memset(uint8_t* dest, uint8_t pattern, uint64_t size);

void delay(uint32_t delay_ms) __attribute__ ((section(".text.user")));
void delay_init() __attribute__((section(".text.user")));

#endif