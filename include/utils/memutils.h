#ifndef __MEM_UTILS_H__
#define __MEM_UTILS_H__

#include <types.h>

void memzero(uint8_t* start, uint64_t size);
void memset(uint8_t* start, uint8_t x, uint64_t size);
void memcpy(uint8_t* src, uint8_t* dest, uint64_t size);

#endif