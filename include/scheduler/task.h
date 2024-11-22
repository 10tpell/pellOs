#ifndef __TASK_H__
#define __TASK_H__

#include "types.h"

struct cpu_context {
    uint32_t x19;
    uint32_t x20;
    uint32_t x21;
    uint32_t x22;
    uint32_t x23;
    uint32_t x24;
    uint32_t x25;
    uint32_t x26;
    uint32_t x27;
    uint32_t x28;
    uint32_t fp;
    uint32_t sp;
    uint32_t pc;
};

struct task_struct {
    struct cpu_context cpu_context;
    uint32_t state;
    uint32_t counter;
    uint32_t priority;
    uint32_t preempt_count;
};

#endif