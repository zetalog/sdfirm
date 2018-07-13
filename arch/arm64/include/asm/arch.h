#ifndef __ARCH_ARM64_H_INCLUDE__
#define __ARCH_ARM64_H_INCLUDE__

#include <target/config.h>
#include <asm/mach/arch.h>

#define nop()		__asm__ __volatile__ ("mov\tr0,r0\t@ nop\n\t");
#define mb()		__asm__ __volatile__ ("" : : : "memory")
#define rmb()		__asm__ __volatile__ ("" : : : "memory")
#define wmb()		__asm__ __volatile__ ("" : : : "memory")
#define wait_irq()	__asm__ __volatile__ ("wfi")

#endif /* __ARCH_ARM64_H_INCLUDE__ */
