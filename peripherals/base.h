#ifndef __PERIPHERALS_H__
#define __PERIPHERALS_H__

#ifndef ASM
#include "../types.h"
#endif

#if 0 == 1
#define PERIPHERAL_BASE_ADDR    0x47E000000U //try this for a sec
#else 
#define PERIPHERAL_BASE_ADDR    0xFE000000U
#endif

#endif