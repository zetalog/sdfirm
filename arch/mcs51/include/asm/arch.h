#ifndef __ARCH_MCS51_H_INCLUDE__
#define __ARCH_MCS51_H_INCLUDE__

#include <target/config.h>

#ifdef SDCC
#define nop()	\
	_asm	\
	nop	\
	_endasm
#endif
#define mb()
#define rmb()
#define wmb()
#define wait_irq()
#include <asm/mach/arch.h>

#endif /* __ARCH_MCS51_H_INCLUDE__ */
