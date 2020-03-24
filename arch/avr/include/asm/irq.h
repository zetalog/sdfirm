#ifndef __IRQ_AVR_H_INCLUDE__
#define __IRQ_AVR_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

#define cli()  __asm__ __volatile__ ("cli" ::)
#define sei()  __asm__ __volatile__ ("sei" ::)

typedef __unused uint8_t irq_flags_t;

#define irq_hw_flags_save(__flags__)		\
	do {					\
		__asm__ __volatile__ (		\
		"	in %0,__SREG__\n"	\
		"	cli"			\
		: "=r" (__flags__)		\
		);				\
	} while (0)
#define irq_hw_flags_restore(__flags__)		\
	do {					\
		__asm__ __volatile__ (		\
		"	sbrs %0,%1\n"		\
		"	rjmp 1f\n"		\
		"	sei\n"			\
		"1:	"			\
		:				\
		: "r" (__flags__),		\
		  "i" (SREG_I)			\
		);				\
	} while (0)

#ifndef CONFIG_CC_ISR_VECTOR
#define _VECTOR(N)	(N)
#define DEFINE_ISR(vector)	\
	void vector##_isr(void)
#endif

/* include mach/irq.h to locate NR_IRQS */
#include <asm/mach/irq.h>

#endif /* __AVR_IRQ_H_INCLUDE__ */
