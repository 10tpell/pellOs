#ifndef __REG_UTILS_H__
#define __REG_UTILS_H__

inline uint32_t reg_read(uint32_t regAddr) { return *(volatile uint32_t*) regAddr; }
inline void reg_write(uint32_t regAddr, uint32_t val) { *(volatile uint32_t*) regAddr = val; }

#endif