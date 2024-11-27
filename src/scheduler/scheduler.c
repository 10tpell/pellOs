#include <scheduler/scheduler.h>
#include <mm.h>
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

uint8_t kernel_fork(uint64_t clone_flags, void* fn, void* args, void* stack) {
    preempt_disable();

    task_struct* new_task = (task_struct*) get_next_free_page();
    if (!new_task) 
        return 0xFF; /* couldn't get a free page */

    /* get location of pt_regs and zero them */
    task_pt_regs* child = get_task_pt_regs(new_task);
    memzero(child, sizeof(task_pt_regs));
    memzero(&(new_task->cpu_context), sizeof(cpu_context));

    /* handle kernel/user mode specific init for thread */
    if (clone_flags & TASK_FLAGS_KERNEL_THREAD) {
        new_task->cpu_context.x19 = fn;
        new_task->cpu_context.x20 = args;
    } else {
        task_pt_regs* cur_regs = get_task_pt_regs(current);
        // *child = *cur_regs;
        child->pc = cur_regs->pc;
        child->pstate = cur_regs->pstate;
        for (uint8_t i = 0; i < 31; i++) {
            child->regs[i] = cur_regs->regs[i];
        }
        child->regs[0] = 0;
        child->sp = stack + PAGE_SIZE;
        new_task->stack = stack;
    }

    /* copy values from current task */
    new_task->preempt_count = 1; // disable preemption until enabled during ret_from_fork
    new_task->flags = clone_flags;
    new_task->priority = current->priority;
    new_task->counter = new_task->priority;
    new_task->state = TASK_STATE_RUNNING;
    
    /* set registers for new process */
    new_task->cpu_context.pc = (uint64_t) ret_from_fork;
    new_task->cpu_context.sp = (uint64_t) child;

    #if EXTRA_DEBUG == DEBUG_ON
    printf("kernel_fork - preempt_count: %d, priority: %d, counter: %d, state: %d\n", new_task->preempt_count, new_task->priority, new_task->counter, new_task->state);
    #endif

    /* get pid of new_task process */
    uint8_t pid = n_tasks;
    #if EXTRA_DEBUG == DEBUG_ON
    printf("task_array[%d] = 0x%08x;\n", pid, new_task);
    #endif
    task_array[pid] = new_task;
    n_tasks++;

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

void exit_task() 
{
    preempt_disable();
    for (uint8_t i = 0; i < N_TASKS; i++) {
        if (task_array[i] == current) {
            task_array[i]->state = TASK_STATE_ZOMBIE;
            break;
        }
    }

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