#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <scheduler/task.h>

#define N_TASKS    64

uint8_t kernel_fork(uint64_t clone_flags, void* fn, void* args, void* stack);
void exit_task();
void wait_on_task(uint8_t taskId);
void set_task_priority(uint8_t taskId, uint8_t priority);
void preempt_enable();
void preempt_disable();
void schedule_tail();
void scheduler_tick();
void schedule();
uint64_t get_current_task_preempt();
task_struct* get_current_task();

#endif