#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <types.h>
#include <mm/paging.h>

#define START_OF_HEAP ((free_mem_header_t *) 0)
#define END_OF_HEAP ((free_mem_header_t *) -1)

#define KERNEL_HEAP_SIZE_PAGES 64
#define KERNEL_HEAP_SIZE (KERNEL_HEAP_SIZE_PAGES * PAGE_SIZE)

#define KERNEL_MALLOC_SIZE_SIZE (sizeof(uint64_t))
#define KERNEL_MALLOC_HEADER_SIZE (KERNEL_MALLOC_SIZE_SIZE)

typedef struct free_mem_header_struct free_mem_header_t;

typedef struct free_mem_header_struct {
    free_mem_header_t* prev;
    free_mem_header_t* next;
    uint64_t size;
} free_mem_header_t;

void* kmalloc(uint64_t size);
void kfree(void* ptr);
void kmalloc_init(void* heap_start);

#endif