#ifndef __IRQ_MCS51_H_INCLUDE__
#define __IRQ_MCS51_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

#ifndef CONFIG_TICK
typedef uint8_t irq_flags_t;

#define irq_hw_flags_save(__flags__)		\
	do {					\
		irq_flags_t oflags = EA;	\
		EA = 0;				\
		flags = oflags;			\
	} while (0)
#define irq_hw_flags_restore(__flags__)		\
	do {					\
		EA = (__flags__);		\
	} while (0)
#endif

#include <asm/mach/irq.h>

#endif /* __IRQ_MCS51_H_INCLUDE__ */
