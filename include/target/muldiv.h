#ifndef __MULDIV_H_INCLUDE__
#define __MULDIV_H_INCLUDE__

#include <target/types.h>
#include <asm/muldiv.h>

/* 16-bit calculations */
#ifndef ARCH_HAVE_DIV16U
uint16_t div16u(uint16_t x, uint16_t y);
#endif
#ifndef ARCH_HAVE_MOD16U
uint16_t mod16u(uint16_t x, uint16_t y);
#endif

/* 32-bit calculations */
#ifndef ARCH_HAVE_DIV32U
uint32_t div32u(uint32_t x, uint32_t y);
#endif
#ifndef ARCH_HAVE_MOD32U
uint32_t mod32u(uint32_t x, uint32_t y);
#endif
#ifndef ARCH_HAVE_MUL16U
uint32_t mul16u(uint16_t x, uint16_t y);
#endif

/* 64-bit calculations */
#ifndef ARCH_HAVE_MUL32U
uint64_t mul32u(uint32_t x, uint32_t y);
#endif
#ifndef ARCH_HAVE_MOD64U
uint64_t mod64u(uint64_t x, uint64_t y);
#endif
#ifndef ARCH_HAVE_DIV64U
uint64_t div64u(uint64_t x, uint64_t y);
#endif

#ifdef CONFIG_MATH_GCD32
uint32_t gcd32u(uint32_t n, uint32_t m);
#else
#define gcd32u(n, m)	1
#endif

#ifndef min
#define min(x, y)		((x)<(y)?(x):(y))
#endif
#ifndef max
#define max(x, y)		((x)>(y)?(x):(y))
#endif
#ifndef clamp
#define clamp(val, lo, hi)	min(max(val, lo), hi)
#endif
#ifndef min3
#define min3(x, y, z)		min(min(x, y), z)
#endif

#define DIV_ROUND_DOWN(l, d)		div32u(l, d)
#define DIV_ROUND_UP(l, d)		DIV_ROUND_DOWN((l) + (d) - 1, (d))
#define DIV_ROUND_DOWN_LL(ll, d)	div64u(ll, d)
#define DIV_ROUND_UP_LL(ll, d)		DIV_ROUND_DOWN_LL((ll) + (d) - 1, (d))

#endif /* __MULDIV_H_INCLUDE__ */
