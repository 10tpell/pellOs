#include <lib/software_timer.h>
#include <peripherals/generic_timer.h>
#include <scheduler/scheduler.h>
#include <utils/printf.h>
#include <config.h>

static onehit_timer_t timer_arr[5] = { {0, 0 } };
static uint64_t current_tick = 0;

void register_one_hit(uint32_t delay_in_ms) {
    #if EXTRA_DEBUG == DEBUG_ON
    printf("Current tick: %u\n", current_tick);
    #endif
    for (uint8_t i = 0; i < 5; i++) {
        if (timer_arr[i].finish_time == 0) {
            timer_arr[i].finish_time = current_tick + SOFTWARE_TIMER_MS_TO_TICKS(delay_in_ms);

            task_struct* task = get_current_task();
            timer_arr[i].task_ptr = task;
            task->state = TASK_STATE_BLOCKED;   

            #if EXTRA_DEBUG == DEBUG_ON
            printf("Moving process %u into BLOCKED state. waiting until %u\n", get_pid(), timer_arr[i].finish_time);
            #endif

            schedule();
            return;
        }
    }
}

void increment_tick() {
    current_tick++;
    #if EXTRA_DEBUG == DEBUG_ON
    printf("\n=================================tick:%d===============================\n", current_tick);
    #endif
}

void check_timers() {
    for (uint8_t i = 0; i < 5; i++) {
        if ((timer_arr[i].task_ptr != 0)) {
            if (timer_arr[i].finish_time <= current_tick) {
                #if EXTRA_DEBUG == DEBUG_ON
                printf("Timer elapsed after %d ticks, moving process %d into RUNNING state.\n", current_tick, get_pid_from_ptr(timer_arr[i].task_ptr));
                #endif

                timer_arr[i].task_ptr->state = TASK_STATE_RUNNING;
                timer_arr[i].task_ptr = (task_struct *) 0;
                timer_arr[i].finish_time = 0;
            }
        } 
    }
}