#include <scheduler/scheduler.h>
#include <mm.h>
#include <utils/printf.h>
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

uint8_t kernel_fork(void* fn, void* args) {
    preempt_disable();

    task_struct* new_task = (task_struct*) get_next_free_page();
    if (!new_task) 
        return 0xFF; /* couldn't get a free page */

    /* copy values from current task */
    new_task->cpu_context = current->cpu_context;
    new_task->preempt_count = 1; // disable preemption until enabled during ret_from_fork
    new_task->priority = current->priority;
    new_task->counter = new_task->priority;
    new_task->state = TASK_STATE_RUNNING;

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

    /* set registers for new process */
    new_task->cpu_context.x19 = (uint64_t) fn;
    new_task->cpu_context.x20 = (uint64_t) args;
    new_task->cpu_context.pc = (uint64_t) ret_from_fork;
    new_task->cpu_context.sp = (uint64_t) new_task + TASK_SIZE;

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

void terminate_task(uint8_t taskId);
void wait_on_task(uint8_t taskId);