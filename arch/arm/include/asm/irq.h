#ifndef __IRQ_ARM_H_INCLUDE__
#define __IRQ_ARM_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

typedef uint32_t irq_flags_t;

#ifdef __LINUX_ARM_M_ARCH__
#define irq_hw_flags_save(x)					\
	({							\
	__asm__ __volatile__(					\
	"mrs	%0, primask		@ local_irq_save\n"	\
	"cpsid	i"						\
	: "=r" (x) : : "memory", "cc");				\
	})

#define irq_hw_flags_restore(x)					\
	__asm__ __volatile__(					\
	"msr	primask, %0		@ local_irq_restore\n"	\
	:							\
	: "r" (x)						\
	: "memory", "cc")
#else
#if __LINUX_ARM_ARCH__ >= 6
#define irq_hw_flags_save(x)					\
	({							\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_save\n"	\
	"cpsid	i"						\
	: "=r" (x) : : "memory", "cc");				\
	})
#else
#define irq_hw_flags_save(x)					\
	({							\
		unsigned long temp;				\
		(void) (&temp == &x);				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_save\n"	\
"	orr	%1, %0, #128\n"					\
"	msr	cpsr_c, %1"					\
	: "=r" (x), "=r" (temp)					\
	:							\
	: "memory", "cc");					\
	})
#endif
#define irq_hw_flags_restore(x)					\
	__asm__ __volatile__(					\
	"msr	cpsr_c, %0		@ local_irq_restore\n"	\
	:							\
	: "r" (x)						\
	: "memory", "cc")
#endif

#if __LINUX_ARM_ARCH__ >= 6
#define sei()  __asm__("cpsie i	@ __sti" : : : "memory", "cc")
#define cli() __asm__("cpsid i	@ __cli" : : : "memory", "cc")
#else
#define sei()							\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_enable\n"	\
"	bic	%0, %0, #128\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory", "cc");					\
	})
#define cli()							\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_disable\n"	\
"	orr	%0, %0, #128\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory", "cc");					\
	})
#endif

#ifndef CONFIG_CC_ISR_VECTOR
#define _VECTOR(N)	(N)
#define DEFINE_ISR(vector)	\
	void vector##_isr(void)
#endif

#include <asm/mach/irq.h>

#endif /* __IRQ_ARM_H_INCLUDE__ */
