#ifndef __MULDIV_ARM_H_INCLUDE__
#define __MULDIV_ARM_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

#ifdef CONFIG_ARM_THUMB2
static inline uint16_t div16u(uint16_t x, uint16_t y)
{
        uint16_t val;
        __asm__ __volatile__ ("udiv     %0, %1, %2\n"
                              : "=r"(val)
                              : "r"(x),
                                "r"(y));
        return val;
}
#define ARCH_HAVE_DIV16U 1

static inline uint32_t div32u(uint32_t x, uint32_t y)
{
	uint32_t val;
	__asm__ __volatile__ ("udiv	%0, %1, %2	\n\t"
			      : "=r"(val)
			      :	"r"(x),
			      	"r"(y));
	return val;
}
#define ARCH_HAVE_DIV32U 1

static inline uint32_t mod32u(uint32_t x, uint32_t y)
{
        uint32_t val;
        __asm__ __volatile__ ("udiv     %0, %1, %2	\n\t"
                              "mls	%0, %2, %0, %1	\n\t"
                              : "=r"(val)
                              : "r"(x),
                                "r"(y));
        return val;
}
#define ARCH_HAVE_MOD32U 1
#endif

static inline uint32_t mul16u(uint16_t x, uint16_t y)
{
        uint32_t val;
        __asm__ __volatile__ ("mul	%0, %1, %2	\n\t"
                              : "=r"(val)
                              : "r"(x),
                                "r"(y));
        return val;
}
#define ARCH_HAVE_MUL16U 1

static inline uint64_t mul32u(uint32_t x, uint32_t y)
{
        uint64_t lo, hi;
        __asm__ __volatile__ ("umull	%0, %1, %2, %3	\n\t"
                              : "=r"(lo),
				"=r"(hi)
                              : "r"(x),
                                "r"(y));
        return ((uint64_t)hi << 32) | lo;
}
#define ARCH_HAVE_MUL32U 1

#endif /* __MULDIV_ARM_H_INCLUDE__ */
