#include <mm/mm.h>
#include <utils/printf.h>

static char kernel_memmap[MM_NUM_OF_PAGES] = {0, };

void* get_next_free_page() 
{
    for (uint8_t i = 0; i < MM_NUM_OF_PAGES; i++) {
        if (kernel_memmap[i] == 0) {
            kernel_memmap[i] = 1;
            return (void *) (MM_AVAILABLE_PAGE_MIN + i * PAGE_SIZE);
        }
    }
    printf("get_next_free_page(): return (void *) 0;");
    return (void *) 0;
}

void free_page(uint32_t ptr)
{
    kernel_memmap[(ptr - MM_AVAILABLE_PAGE_MIN) / PAGE_SIZE] = 0;
}