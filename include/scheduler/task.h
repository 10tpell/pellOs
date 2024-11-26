#ifndef __TASK_H__
#define __TASK_H__

#ifndef ASM
#include <types.h>

enum TaskState {
    TASK_STATE_RUNNING = 0,
    TASK_STATE_BLOCKED = 1,
    TASK_STATE_SUSPENDED = 2,
    TASK_STATE_WAITING = 3
};

typedef struct cpu_context_struct {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;
    uint64_t sp;
    uint64_t pc;
} cpu_context;

typedef struct task_struct_struct {
    cpu_context cpu_context;
    enum TaskState state;
    sint64_t counter;
    sint64_t priority;
    sint64_t preempt_count;
} task_struct;
#endif

#define INIT_TASK { /*cpu context: */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},\
                    /* state :*/ 0,\
                    /* counter:*/ 0,\
                    /* priority:*/ 1,\
                    /* preempt_count:*/ 0 }

#define THREAD_CPU_CONTEXT 0

#define TASK_SIZE   4096

#ifndef ASM
void ret_from_fork( void );
void cpu_switch_to(void* prev, void* next);
void end_of_sched( void );
#endif

#endif