#ifndef __MM_H__
#define __MM_H__

#include "types.h"
#include "peripherals/base.h"

#define PAGE_SIZE               (1 << 12)

#define MM_AVAILABLE_PAGE_MIN   (1 << 22)
#define MM_AVAILABLE_PAGE_MAX   PERIPHERAL_BASE_ADDR

#define MM_PAGE_MEMORY_SIZE     (MM_AVAILABLE_PAGE_MAX - MM_AVAILABLE_PAGE_MIN)
#define MM_NUM_OF_PAGES         (MM_PAGE_MEMORY_SIZE / PAGE_SIZE)

uint32_t get_next_free_page();
void free_page(uint32_t ptr);

#endif