#ifndef __GIC_ARM_H_INCLUDE__
#define __GIC_ARM_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

#ifndef ARCH_HAVE_VIC
#define ARCH_HAVE_VIC			1
#else
#error "Multiple VIC controller defined"
#endif

/* Generic Intrrupt Controller (GIC) registers */

typedef void (*irq_handler)(void);

void vic_hw_register_irq(uint8_t nr, irq_handler h);
void vic_hw_register_trap(uint8_t nr, irq_handler h);
void vic_hw_vectors_init(void);

#define vic_hw_irq_enable(irq)
#define vic_hw_irq_disable(irq)
#define vic_hw_irq_trigger(irq)
void vic_hw_irq_priority(uint8_t irq, uint8_t prio);
void vic_hw_trap_priority(uint8_t trap, uint8_t prio);

#endif /* __GIC_ARM_H_INCLUDE__ */
