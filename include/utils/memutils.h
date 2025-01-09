#ifndef __MEM_UTILS_H__
#define __MEM_UTILS_H__

#include <types.h>

void memzero(uint8_t* start, uint64_t size);
void memzero_u8(uint8_t* start, uint64_t size);
void memset(uint8_t* start, uint8_t x, uint64_t size);
void memcpy_u64(uint64_t* dest, uint64_t* src, uint64_t size);
void memcpy_u8(uint8_t* dest, uint8_t* src, uint64_t size);
void memcpy(uint8_t* dest, uint8_t* src, uint64_t size);

sint8_t strcmp(const char* a, const char* b);
void strcpy_s(const char* dest, uint64_t destsz, const char* src);

#endif