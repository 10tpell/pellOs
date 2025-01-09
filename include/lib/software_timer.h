#ifndef __SOFTWARE_TIMER_H__
#define __SOFTWARE_TIMER_H__

#include <types.h>
#include <scheduler/task.h>

#define SOFTWARE_TIMER_PRESCALER 100
#define SOFTWARE_TIMER_MS_TO_TICKS(t) (t / SOFTWARE_TIMER_PRESCALER) 

typedef struct onehit_timer_struct {
    uint32_t finish_time;
    task_struct* task_ptr;
} onehit_timer_t;

void register_one_hit(uint32_t delay_in_ms);
void check_timers();
void increment_tick();

#endif