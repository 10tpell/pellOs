#include <reg_utils.h>

uint32_t reg_read(volatile uint32_t* regAddr) { return *(volatile uint32_t*) regAddr; }
void reg_write(volatile uint32_t* regAddr, uint32_t val) { *(volatile uint32_t*) regAddr = val; }
