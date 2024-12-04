#ifndef __REG_UTILS_H__
#define __REG_UTILS_H__

#include "types.h"

uint32_t reg_read(volatile uint32_t* regAddr);
void reg_write(volatile uint32_t* regAddr, uint32_t val);

#endif