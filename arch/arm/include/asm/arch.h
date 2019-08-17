#ifndef __ARCH_ARM_H_INCLUDE__
#define __ARCH_ARM_H_INCLUDE__

#include <target/config.h>
#include <asm/mach/arch.h>

#define mb()		__asm__ __volatile__ ("" : : : "memory")
#define rmb()		__asm__ __volatile__ ("" : : : "memory")
#define wmb()		__asm__ __volatile__ ("" : : : "memory")

#endif /* __ARCH_ARM_H_INCLUDE__ */
