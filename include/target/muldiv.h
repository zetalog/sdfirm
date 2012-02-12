#ifndef __MULDIV_H_INCLUDE__
#define __MULDIV_H_INCLUDE__

#include <target/types.h>
#include <asm/muldiv.h>

#ifndef ARCH_HAVE_DIV16U
uint16_t div16u(uint16_t x, uint16_t y);
#endif
#ifndef ARCH_HAVE_MOD16U
uint16_t mod16u(uint16_t x, uint16_t y);
#endif
#ifndef ARCH_HAVE_DIV32U
uint32_t div32u(uint32_t x, uint32_t y);
#endif
#ifndef ARCH_HAVE_MOD32U
uint32_t mod32u(uint32_t x, uint32_t y);
#endif
#ifndef ARCH_HAVE_MUL16U
uint32_t mul16u(uint16_t x, uint16_t y);
#endif

#ifdef ARCH_HAVE_UINT64_T
uint64_t mul32u(uint32_t x, uint32_t y);
uint64_t mod64u(uint64_t x, uint64_t y);
uint64_t div64u(uint64_t x, uint64_t y);
#endif

#ifndef min
#define min(x, y)	((x)<(y)?(x):(y))
#endif
#ifndef max
#define max(x, y)	((x)>(y)?(x):(y))
#endif

#endif /* __MULDIV_H_INCLUDE__ */
