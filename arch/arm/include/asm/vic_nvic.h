#ifndef __VIC_NVIC_H_INCLUDE__
#define __VIC_NVIC_H_INCLUDE__

#include <asm/nvic.h>
#include <asm/mach/mem.h>
#include <asm/mach/irq.h>
#include <asm/mach/scb.h>

#ifndef ARCH_HAVE_VIC
#define ARCH_HAVE_VIC			1
#else
#error "Multiple VIC controller defined"
#endif

#define __VIC_HW_PRIO_MAX	NVIC_PRIO_MAX
#define __VIC_HW_PRIO_TRAP_MIN	NVIC_PRIO_TRAP_MIN

typedef void (*irq_handler)(void);

#define vic_hw_irq_enable(irq)		nvic_irq_enable_irq(irq)
#define vic_hw_irq_disable(irq)		nvic_irq_disable_irq(irq)
#define vic_hw_irq_trigger(irq)		nvic_irq_trigger_irq(irq)
void vic_hw_irq_priority(uint8_t irq, uint8_t prio);
void vic_hw_trap_priority(uint8_t trap, uint8_t prio);
void vic_hw_register_irq(uint8_t nr, irq_handler h);
void vic_hw_register_trap(uint8_t nr, irq_handler h);
void vic_hw_vectors_init(void);

#endif /* __VIC_NVIC_H_INCLUDE__ */
