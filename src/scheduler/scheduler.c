#include <scheduler/scheduler.h>
#include <mm/mm.h>
#include <mm/paging.h>
#include <utils/printf.h>
#include <utils/memutils.h>
#include <arm/irq.h>
#include <config.h> 

static task_struct init_task = INIT_TASK;
task_struct* current = &init_task;
task_struct* task_array[N_TASKS] = {&init_task, };
static uint8_t n_tasks = 1;

uint64_t get_current_task_preempt()
{
    return current->preempt_count;
}

void preempt_disable()
{
    current->preempt_count++;
}

void preempt_enable()
{
    current->preempt_count--;
}

uint8_t kernel_fork(uint64_t clone_flags, uint64_t fn, uint64_t args) {
    preempt_disable();

    task_struct* new_task;

    void* page = allocate_kernel_page();
    new_task = (task_struct *) page;

    if (!new_task) 
        return 0xFF; /* couldn't get a free page */

    /* get location of pt_regs and zero them */
    task_pt_regs* child = get_task_pt_regs(new_task);

    /* handle kernel/user mode specific init for thread */
    if (clone_flags & TASK_FLAGS_KERNEL_THREAD) {
        new_task->cpu_context.x19 = fn;
        new_task->cpu_context.x20 = args;
    } else {
        task_pt_regs* cur_regs = get_task_pt_regs(current);
        *child = *cur_regs;
        child->regs[0] = 0;
        copy_virtual_memory(new_task);
    }

    /* copy values from current task */
    new_task->preempt_count = 1; // disable preemption until enabled during ret_from_fork
    new_task->flags = clone_flags;
    new_task->priority = current->priority;
    new_task->counter = new_task->priority;
    new_task->state = TASK_STATE_RUNNING;

    // for now we don't want to copy fd_table (this needs to be changed)
    // TODO: copy fd table and copy fd list across as well
    new_task->fd_table.count = 0;
    new_task->fd_table.file = (file_t *) 0;
    
    /* set registers for new process */
    new_task->cpu_context.pc = (uint64_t) ret_from_fork;
    new_task->cpu_context.sp = (uint64_t) child;

    #if EXTRA_DEBUG == DEBUG_ON
    printf("kernel_fork - preempt_count: %d, priority: %d, counter: %d, state: %d\n", new_task->preempt_count, new_task->priority, new_task->counter, new_task->state);
    #endif

    /* get pid of new_task process */
    uint8_t pid = n_tasks++;
    #if EXTRA_DEBUG == DEBUG_ON
    printf("task_array[%d] = 0x%08x;\n", pid, new_task);
    #endif
    task_array[pid] = new_task;

    preempt_enable();

    return pid;
}

void switch_to(task_struct* next)
{
    if (current == next)
        return;
    
    task_struct* prev = current;
    current = next;
    #if EXTRA_DEBUG == DEBUG_ON
    printf("cpu_switch_to(0x%08x, 0x%08x);\n", prev, next);
    #endif
    cpu_switch_to(prev, next);
}

void end_of_sched()
{
    preempt_enable();
}

void _schedule()
{
    preempt_disable();
    uint32_t next;
    sint32_t c;
    task_struct* task_ptr;

    for(;;) {
        c = -1;
        next = 0;
        for (uint32_t i = 0; i < N_TASKS; i++) {
            task_ptr = task_array[i];
            if ((uint64_t) task_ptr > 0 && task_ptr->state == TASK_STATE_RUNNING && (sint32_t) task_ptr->counter > c) {
                c = task_ptr->counter;
                next = i;
            }
        }

        if (c > 0) {
            break;
        }
        
        for (uint32_t i = 0; i < N_TASKS; i++) {
            task_ptr = task_array[i];
            if (task_ptr)
                task_ptr->counter = (task_ptr->counter >> 1) + task_ptr->priority;
        }
    }
    switch_to(task_array[next]);
    preempt_enable();
}

void schedule()
{
    current->counter = 0;
    _schedule();
}

void scheduler_tick()
{
    current->counter--;
    if (current->counter > 0 || current->preempt_count > 0)
    {
        return;
    }
    current->counter = 0;
    enable_irq();
    _schedule();
    disable_irq();
}

void set_task_priority(uint8_t taskId, uint8_t priority)
{
    task_struct* task_ptr = task_array[taskId];
    if(!task_ptr) return;

    task_ptr->priority = priority;
}

uint8_t get_pid() {
    for (uint8_t i = 0; i < N_TASKS; i++) {
        if (task_array[i] == current) {
            return i;
        }
    }
    return 0;
}

void exit_task() 
{
    preempt_disable();
    task_array[get_pid()]->state = TASK_STATE_ZOMBIE;

    if (current->stack) {
        free_page(current->stack);
    }

    preempt_enable();
    schedule();
}

task_struct* get_current_task() {
    return current;
}

void wait_on_task(uint8_t taskId);