#ifndef __REG_UTILS_H__
#define __REG_UTILS_H__

#include "types.h"

inline uint32_t reg_read(void* regAddr) { return *(volatile uint32_t*) regAddr; }
inline void reg_write(void* regAddr, uint32_t val) { *(volatile uint32_t*) regAddr = val; }

#endif