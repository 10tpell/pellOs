#include <scheduler/task.h>
#include <mm.h>
#include <utils/memutils.h>

task_pt_regs* get_task_pt_regs(task_struct* task)
{
    uint64_t* ptr = (uint64_t *)task + TASK_SIZE - sizeof(task_struct);
    return (task_pt_regs *)ptr; 
}

sint8_t move_to_userspace(void* pc, task_struct* task)
{
    task_pt_regs* regs = get_task_pt_regs(task);
    memzero((void *) regs, sizeof(*regs));
    
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t;

    uint64_t stack = (uint64_t) get_next_free_page();
    if (!stack) return -1;
    regs->sp = stack + PAGE_SIZE;
    task->stack = stack;
    
    return 0;
}