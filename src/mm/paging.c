#include <arm/mmu.h>
#include <mm/paging.h>
#include <mm/mm.h>
#include <scheduler/scheduler.h>

void* map_table(uint64_t* table, uint64_t shift, void* virt_adr, sint64_t* new_table) {
    uint64_t idx = ((uint64_t) virt_adr) >> shift;
    idx &= (PTRS_PER_TABLE - 1);

    if (!table[idx]) {
        *new_table = 1;
        void* next_level_table = get_next_free_page();
        uint64_t entry = ((uint64_t) next_level_table) | MM_TYPE_PAGE_TABLE;
        table[idx] = entry;
        return next_level_table;
    } else {
        *new_table = 0;
    }
    return table[idx] & PAGE_MASK;
}

void map_table_entry(uint64_t* table_entry, void* virt_adr, uint64_t phys_adr) {
    uint64_t idx = ((uint64_t) virt_adr) >> PAGE_SHIFT;
    idx &= PTRS_PER_TABLE - 1;

    uint64_t entry = phys_adr | MMU_PTE_FLAGS;
    table_entry[idx] = entry;
}

void map_page(task_struct* task, void* virt_adr, void* page)
{
    void* page_directory;
    
    if (!task->mm.pagedirectory) {
        task->mm.pagedirectory = get_next_free_page();
        task->mm.kernelpages[++task->mm.kernelpages_count] = task->mm.pagedirectory;
    }

    page_directory = task->mm.pagedirectory;
    sint32_t new_table;

    void* pud = map_table((uint64_t *)(page_directory + VIRTUAL_ADDRESS_START), PGD_SHIFT, virt_adr, &new_table);
    if(new_table) {
        task->mm.kernelpages[++task->mm.kernelpages_count] = pud;
    }

    void* pmd = map_table((uint64_t *)(pud + VIRTUAL_ADDRESS_START), PUD_SHIFT, virt_adr, &new_table);
    if(new_table) {
        task->mm.kernelpages[++task->mm.kernelpages_count] = pmd;
    }

    void* pte = map_table((uint64_t *)(pmd + VIRTUAL_ADDRESS_START), PMD_SHIFT, virt_adr, &new_table);
    if(new_table) {
        task->mm.kernelpages[++task->mm.kernelpages_count] = pte;
    }

    map_table_entry((void *)(pte + VIRTUAL_ADDRESS_START), virt_adr, page);
    user_page p = {page, virt_adr};
    task->mm.userpages[task->mm.userpages_count++] = p;
}

void* allocate_kernel_page() {
    void* page = get_next_free_page();
    if (!page) return 0;
    return page + VIRTUAL_ADDRESS_START;
}

void* allocate_user_page(task_struct* task, void* virt_adr)
{
    void* page = get_next_free_page();
    if (!page) return 0;

    map_page(task, virt_adr, page);
    return page + VIRTUAL_ADDRESS_START;
}

sint8_t copy_virtual_memory(task_struct* task) {
    task_struct* src = get_current_task();
    for (uint64_t i = 0; i < src->mm.userpages_count; i++) {
        void* kernel_virtual_adr = allocate_user_page(task, src->mm.userpages[i].virtual_adr);
        if (kernel_virtual_adr == 0) {
            return -1;
        }
        memcpy(kernel_virtual_adr, src->mm.userpages[i].virtual_adr, PAGE_SIZE);
    }
    return 0;
}

static sint8_t ind = 1;
sint8_t do_mem_abort(void* addr, uint64_t esr) {
    uint64_t dfs = esr & 0b111111;
    if ((dfs & 0b111100) == 0b100) {
        void* page = get_next_free_page();
        if (!page) return -1;

        map_page(get_current_task(), ((uint64_t) addr) & PAGE_MASK, page);
        ind++;

        if (ind > 2) return -1;
        
        return 0;
    }
    return -1;
}
