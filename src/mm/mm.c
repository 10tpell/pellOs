#include <mm/mm.h>
#include <mm/paging.h>
#include <utils/printf.h>
#include <utils/memutils.h>
#include <config.h>

static char kernel_memmap[MM_NUM_OF_PAGES] = {0, };

uint64_t get_next_free_page() 
{
    for (uint32_t i = 0; i < MM_NUM_OF_PAGES; i++) {
        if (kernel_memmap[i] == 0) {
            kernel_memmap[i] = 1;
            uint64_t new_page = (uint64_t) (LOW_MEMORY + (i * PAGE_SIZE));
            memzero((uint8_t* ) (new_page + VIRTUAL_ADDRESS_START), PAGE_SIZE);

            #if EXTRA_DEBUG == DEBUG_ON 
            printf("I[%d] new page allocated: 0x%08x%08x\n", i, new_page >> 32, ((new_page << 32) >> 32));
            #endif
            return new_page;
        }
    }
    return 0;
}

void free_page(uint32_t ptr)
{
    kernel_memmap[(ptr - MM_AVAILABLE_PAGE_MIN) / PAGE_SIZE] = 0;
}