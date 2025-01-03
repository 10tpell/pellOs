#ifndef __GENERIC_TIMER_H__
#define __GENERIC_TIMER_H__

#include <types.h>

uint64_t read_cntrl();
uint64_t read_comparator();
uint64_t read_timer();

void set_cntrl(uint64_t val);
void set_comparator(uint64_t val);
void set_timer();
void set_cntfreq(uint64_t val);

void generic_timer_init();
void handle_generic_timer_irq();

#endif