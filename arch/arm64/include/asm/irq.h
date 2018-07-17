#ifndef __IRQ_ARM64_H_INCLUDE__
#define __IRQ_ARM64_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

typedef uint64_t irq_flags_t;

#define irq_hw_flags_save(x)					\
	__asm__ __volatile__(					\
	"mrs	%0, daif		// local_irq_save\n"	\
	"msr	daifset, #2"					\
	: "=r" (x) : : "memory")
#define irq_hw_flags_restore(x)					\
	__asm__ __volatile__(					\
	"msr	daif, %0		// local_irq_restore\n"	\
	: : "r" (x) : "memory")
#define irq_hw_flags_enable()					\
	__asm__ __volatile__(					\
	"msr	daifclr, #2		// local_irq_enable\n"	\
	: : : "memory")
#define irq_hw_flags_disable()					\
	__asm__ __volatile__(					\
	"msr	daifset, #2		// local_irq_disable\n"	\
	: : : "memory")
#define irq_hw_ctrl_init()

#include <asm/mach/irq.h>

#endif /* __IRQ_ARM64_H_INCLUDE__ */
