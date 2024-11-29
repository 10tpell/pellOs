#ifndef __TASK_H__
#define __TASK_H__


#define TASK_FLAGS_KERNEL_THREAD 0x2
#define MAX_PROCESS_PAGES 16

/*
 * PSR bits
 */
#define PSR_MODE_EL0t	0x00000000
#define PSR_MODE_EL1t	0x00000004
#define PSR_MODE_EL1h	0x00000005
#define PSR_MODE_EL2t	0x00000008
#define PSR_MODE_EL2h	0x00000009
#define PSR_MODE_EL3t	0x0000000c
#define PSR_MODE_EL3h	0x0000000d

#define INIT_TASK { /*cpu context: */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},\
                    /* state :*/ 0,\
                    /* counter:*/ 0,\
                    /* priority:*/ 1,\
                    /* preempt_count:*/ 0,\
                    /* stack:*/ 0,\
                    /* flags:*/ TASK_FLAGS_KERNEL_THREAD, \
                    /* mm:*/ {0, 0, {{0}}, 0, {0}} }

#define THREAD_CPU_CONTEXT 0

#define TASK_SIZE   4096

#ifndef ASM

#include <types.h>

enum TaskState {
    TASK_STATE_RUNNING = 0,
    TASK_STATE_BLOCKED = 1,
    TASK_STATE_SUSPENDED = 2,
    TASK_STATE_WAITING = 3,
    TASK_STATE_ZOMBIE = 4
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
    // double d16; /* floating point regs (can only store 64bit value atm)*/
    // double d17;
    // double d18;
    // double d19;
    // double d20;
    // double d21;
    // double d22;
    // double d23;
    // double d24;
    // double d25;
    // double d26;
    // double d27;
    // double d28;
    // double d29;
    // double d30;
    // double d31;
} cpu_context;

typedef struct userpage_struct {
    uint64_t physical_adr;
    void* virtual_adr;
} user_page;

typedef struct mm_struct {
    uint64_t pagedirectory;
    sint32_t userpages_count;
    user_page userpages[MAX_PROCESS_PAGES];
    sint32_t kernelpages_count;
    uint64_t kernelpages[MAX_PROCESS_PAGES];
} mm_t;

typedef struct task_struct_struct {
    cpu_context cpu_context;
    enum TaskState state;
    sint64_t counter;
    sint64_t priority;
    sint64_t preempt_count;
    uint64_t stack;
    uint64_t flags;
    mm_t mm;
} task_struct;

typedef struct pt_regs_struct {
    uint64_t regs[31];
    uint64_t sp;
    uint64_t pc;
    uint64_t pstate;
} task_pt_regs;

void ret_from_fork( void );
void cpu_switch_to(void* prev, void* next);
void end_of_sched( void );
task_pt_regs* get_task_pt_regs(task_struct* task);
sint8_t move_to_userspace(void* start, uint64_t size, void* pc, task_struct* task);

#endif /* ifdef ASM */

#endif