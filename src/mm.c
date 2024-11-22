#include "mm.h"

static char kernel_memmap[MM_NUM_OF_PAGES] = {0};

uint32_t get_next_free_page() 
{
    for (uint8_t i = 0; i < MM_NUM_OF_PAGES; i++) {
        if (kernel_memmap[i] == 0) {
            kernel_memmap[i] = 1;
            return MM_AVAILABLE_PAGE_MIN + i * PAGE_SIZE;
        }
    }
    return 0;
}

void free_page(uint32_t ptr)
{
    kernel_memmap[(ptr - MM_AVAILABLE_PAGE_MIN) / PAGE_SIZE];
}