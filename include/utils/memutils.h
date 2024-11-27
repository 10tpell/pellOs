#ifndef __MEM_UTILS_H__
#define __MEM_UTILS_H__

#include <types.h>

inline void memzero(void* start, uint64_t size) {
    if(size % 8 == 0) {
        uint64_t* p = (uint64_t*) start;
        for (uint64_t i = 0; i < size; i++) {
            p[i] = 0;
        }
    } else if (size % 4 == 0) {
        uint32_t* p = (uint32_t*) start;
        for (uint32_t i = 0; i < size; i++) {
            p[i] = 0;
        }
    } else if (size % 2 == 0) {
        uint16_t* p = (uint16_t*) start;
        for (uint64_t i = 0; i < size; i++) {
            p[i] = 0;
        }
    } else {
        uint8_t* p = (uint8_t*) start;
        for (uint64_t i = 0; i < size; i++) {
            p[i] = 0;
        }
    }
}

inline void memset(void* start, uint8_t x, uint64_t size) {
    for (uint64_t i = 0; i < size; i++) {
        ((uint8_t *) start)[i] = x; 
    }
}

#endif