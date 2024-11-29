#include <scheduler/task.h>
#include <mm/mm.h>
#include <utils/memutils.h>

task_pt_regs* get_task_pt_regs(task_struct* task)
{
    uint64_t* ptr = (uint64_t *)task + TASK_SIZE - sizeof(task_struct);
    return (task_pt_regs *)ptr; 
}

sint8_t move_to_userspace(void* start, uint64_t size, void* pc, task_struct* task)
{
    task_pt_regs* regs = get_task_pt_regs(task);
    
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t;
    regs->sp = 2 * PAGE_SIZE; // for now program can only be one page long (second page is for stack)

    /* reserve a page and populate page tables */
    void* code_page = allocate_user_page(task, 0);
    if(code_page == 0) return -1;

    memcpy(code_page, start, size);
    set_page_directory(task->mm.pagedirectory);
    
    return 0;
}