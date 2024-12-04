#ifndef __PAGING_H__
#define __PAGING_H__

#include <peripherals/base.h>
#include <scheduler/task.h>
#include <mm/mm.h>

#define PHYSICAL_MEMORY_SIZE 0xFFFFFFFF	

#define PAGE_MASK 0xfffffffffffff000
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2* SECTION_SIZE)
#define HIGH_MEMORY DEVICE_BASE

#define PTRS_PER_TABLE (1 << TABLE_SHIFT)

#define PGD_SHIFT PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE (3 * PAGE_SIZE)

#ifndef ASM

void* allocate_user_page(task_struct* task, void* virt_adr);
void* allocate_kernel_page();
void set_page_directory(uint64_t page_directory);
sint8_t copy_virtual_memory(task_struct* task);

#endif

#endif