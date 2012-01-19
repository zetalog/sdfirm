#ifndef __ARCH_ARM_H_INCLUDE__
#define __ARCH_ARM_H_INCLUDE__

#include <target/config.h>
#include <asm/mach/arch.h>

#define nop() __asm__ __volatile__("mov\tr0,r0\t@ nop\n\t");

#endif /* __ARCH_ARM_H_INCLUDE__ */
