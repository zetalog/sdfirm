#ifndef __SYSREG_ARM64_H_INCLUDE__
#define __SYSREG_ARM64_H_INCLUDE__

#include <target/types.h>

#define read_sysreg(r) ({					\
	uint64_t __val;						\
	asm volatile("mrs %0, " #r : "=r" (__val));		\
	__val;							\
})

/*
 * "Z" normally means zero immediate, it means XZR when it is combined with
 * "%x0" template.
 */
#define write_sysreg(v, r) do {					\
	uint64_t __val = (uint64_t)v;				\
	asm volatile("msr " #r ", %x0" : : "rZ" (__val));	\
} while (0)

#endif /* __SYSREG_ARM64_H_INCLUDE__ */
