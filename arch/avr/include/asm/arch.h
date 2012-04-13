#ifndef __ARCH_AVR_H_INCLUDE__
#define __ARCH_AVR_H_INCLUDE__

#include <target/config.h>
#include <asm/mach/arch.h>

#define nop()		__asm__ __volatile__ ("nop" ::)
#define wait_irq()	__asm__ __volatile__ ("sleep" ::)
#define mb()		__asm__ __volatile__ ("" : : : "memory")
#define rmb()		__asm__ __volatile__ ("" : : : "memory")
#define wmb()		__asm__ __volatile__ ("" : : : "memory")

#endif /* __ARCH_AVR_H_INCLUDE__ */
