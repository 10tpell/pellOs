#ifndef __IRQ_H__
#define __IRQ_H__

#include "../types.h"

void irq_vector_init( void );
void enable_irq( void );
void disable_irq( void );
void irq_barrier( void );
uint32_t read_isr_el1( void );
uint64_t get_daif( void );

#endif