#ifndef __MM_H__
#define __MM_H__

#include "peripherals/base.h"

#ifndef ASM

#include "types.h"

uint64_t get_next_free_page();
void free_page(uint64_t ptr);

#endif

#define PHYSICAL_MEMORY_SIZE 0xFFFFFFFF	

#define DEVICE_BASE PERIPHERAL_BASE_ADDR
#define VIRTUAL_ADDRESS_START 0xffff000000000000
#define DEVICE_BASE_PHYS (0xFE000000U)
#define BOOT_CODE_START 0x0
#define VC_BASE_PHYS 0x3C000000
#define PAGE_SHIFT 12

#define LOW_MEMORY (2* SECTION_SIZE)
#define HIGH_MEMORY VC_BASE_PHYS

#define PAGE_SIZE               (1 << PAGE_SHIFT)

#define MM_AVAILABLE_PAGE_MIN   (1 << 22)
#define MM_AVAILABLE_PAGE_MAX   DEVICE_BASE_PHYS

#define MM_PAGE_MEMORY_SIZE     (MM_AVAILABLE_PAGE_MAX - MM_AVAILABLE_PAGE_MIN)
#define MM_NUM_OF_PAGES         (MM_PAGE_MEMORY_SIZE / PAGE_SIZE)

// 0xffff00003e00b898

#endif