#include <arm/mmu.h>
#include <mm/paging.h>
#include <mm/mm.h>
#include <scheduler/scheduler.h>
#include <utils/memutils.h>
#include <utils/printf.h>

uintphysptr_t map_table(uint64_t* table, uint64_t shift, void* virt_adr, sint32_t* new_table) {
    uint64_t idx = ((uintptr_t) virt_adr) >> shift;
    idx &= (PTRS_PER_TABLE - 1);

    if (!table[idx]) {
        *new_table = 1;
        uintphysptr_t next_level_table = get_next_free_page();
        uint64_t entry = next_level_table | MM_TYPE_PAGE_TABLE;
        table[idx] = entry;
        return next_level_table;
    } else {
        *new_table = 0;
    }
    return table[idx] & PAGE_MASK;
}

void map_table_entry(uint64_t* table_entry, void* virt_adr, uintphysptr_t phys_adr) {
    uint64_t idx = ((uintptr_t) virt_adr) >> PAGE_SHIFT;
    idx &= PTRS_PER_TABLE - 1;

    uint64_t entry = phys_adr | MMU_PTE_FLAGS;
    table_entry[idx] = entry;
}

void map_page(task_struct* task, void* virt_adr, uintphysptr_t page)
{
   uintphysptr_t page_directory;
    
    if (!task->mm.pagedirectory) {
        task->mm.pagedirectory = get_next_free_page();
        task->mm.kernelpages[++task->mm.kernelpages_count] = task->mm.pagedirectory;
    }

    page_directory = task->mm.pagedirectory;
    sint32_t new_table;

    uintphysptr_t pud = map_table((uint64_t *)(page_directory + VIRTUAL_ADDRESS_START), PGD_SHIFT, virt_adr, &new_table);
    if(new_table) {
        task->mm.kernelpages[++task->mm.kernelpages_count] = pud;
    }

    uintphysptr_t pmd = map_table((uint64_t *)(pud + VIRTUAL_ADDRESS_START), PUD_SHIFT, virt_adr, &new_table);
    if(new_table) {
        task->mm.kernelpages[++task->mm.kernelpages_count] = pmd;
    }

    uintphysptr_t pte = map_table((uint64_t *)(pmd + VIRTUAL_ADDRESS_START), PMD_SHIFT, virt_adr, &new_table);
    if(new_table) {
        task->mm.kernelpages[++task->mm.kernelpages_count] = pte;
    }

    map_table_entry((uint64_t *)(pte + VIRTUAL_ADDRESS_START), virt_adr, page);
    user_page p = {page, virt_adr};
    task->mm.userpages[task->mm.userpages_count++] = p;
}

void* allocate_kernel_page() {
    uintphysptr_t page = get_next_free_page();
    if (!page) return 0;
    return (void*) (page + VIRTUAL_ADDRESS_START);
}

void* allocate_user_page(task_struct* task, void* virt_adr)
{
    uintphysptr_t page = get_next_free_page();
    if (!page) return 0;

    map_page(task, virt_adr, page);
    return (void*) (page + VIRTUAL_ADDRESS_START);
}

sint8_t copy_virtual_memory(task_struct* task) {
    task_struct* src = get_current_task();
    for (sint32_t i = 0; i < src->mm.userpages_count; i++) {
        void* kernel_virtual_adr = allocate_user_page(task, src->mm.userpages[i].virtual_adr);
        if (kernel_virtual_adr == 0) {
            return -1;
        }
        memcpy(kernel_virtual_adr, src->mm.userpages[i].virtual_adr, PAGE_SIZE);
    }
    return 0;
}

sint8_t do_mem_abort(void* addr, uint64_t esr) {
    uint64_t dfs = esr & 63; //0b111111
    printf("do_mem_abort\n");
    if ((dfs & 60 /*0b111100*/) == 4 /*0b100*/) {
        uintphysptr_t page = get_next_free_page();
        if (!page) return -1;

        printf("about to call map_page();\n");
        map_page(get_current_task(), (void*) (((uintptr_t) addr) & PAGE_MASK), page);
                
        return 0;
    }
    return -1;
}
