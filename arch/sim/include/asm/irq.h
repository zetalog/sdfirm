#ifndef __IRQ_SIM_H_INCLUDE__
#define __IRQ_SIM_H_INCLUDE__

#include <asm/mach/irq.h>

typedef uint32_t irq_flags_t;

#define irq_hw_flags_save(x)		\
	do {				\
		(x) = cpu_flags;	\
		irq_hw_flags_disable();	\
	} while (0)
#define irq_hw_flags_restore(x)		(cpu_flags = (x))
void irq_hw_flags_enable(void);
void irq_hw_flags_disable(void);

irq_flags_t cpu_flags;
#define SIM_CPU_EAI		0x01

#endif /* __IRQ_SIM_H_INCLUDE__ */
