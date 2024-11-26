#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <scheduler/task.h>

#define N_TASKS    64

uint8_t kernel_fork(void* fn, void* args);
void terminate_task(uint8_t taskId);
void wait_on_task(uint8_t taskId);
void preempt_enable();
void preempt_disable();
void schedule_tail();
void scheduler_tick();
void schedule();
uint64_t get_current_task_preempt();

#endif