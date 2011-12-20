#ifndef __BITOPS_H_INCLUDE__
#define __BITOPS_H_INCLUDE__

#include <host/types.h>

typedef uint32_t bits_t;

#define BITS_PER_UNIT			32

#define BITS_TO_UNITS(bits)		\
	(((uint8_t)((bits)+BITS_PER_UNIT-1))/((uint8_t)BITS_PER_UNIT))
#define DECLARE_BITMAP(name, bits)	\
	bits_t name[BITS_TO_UNITS(bits)]

#define BITOP_MASK(nr)		((uint8_t)1 << ((nr) % BITS_PER_UNIT))
#define BITOP_WORD(nr)		((uint8_t)(nr) / (uint8_t)BITS_PER_UNIT)

#define ALIGN(x,a)		__ALIGN_MASK(x,(a)-1)
#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))

void set_bit(uint8_t nr, bits_t *addr);
void clear_bit(uint8_t nr, bits_t *addr);
int test_bit(int nr, const bits_t *addr);

uint8_t __ffs8(uint8_t byte);
#define __ffz8(x)	__ffs8((uint8_t)(~(x)))
uint8_t __ffs32(uint32_t word);
#define __ffz32(x)	__ffs32((uint32_t)(~(x)))
uint8_t __ffs16(uint16_t word);
uint8_t __fls16(uint16_t word);
uint8_t __fls32(uint32_t word);
uint32_t __rounddown32(uint32_t n);
uint16_t __roundup16(uint16_t n);
uint32_t __roundup32(uint32_t n);
uint8_t hweight16(uint16_t word);
uint8_t hweight8(uint8_t byte);
uint8_t bitrev8(uint8_t byte);
uint16_t bitrev16(uint16_t x);
uint32_t bitrev32(uint32_t x);

#ifndef offsetof
#define offsetof(s, m)			(uint16_t)&(((s *)0)->m)
#endif

#define swap16(x)			\
	((((x) >> 8) & 0x00FF) | (((x) << 8) &) 0xFF00)
#define swap32(x)			\
	((((x) >> 24) & 0x000000FF) |	\
	 (((x) >>  8) & 0x0000FF00) |	\
	 (((x) << 24) & 0xFF000000) |	\
	 (((x) <<  8) & 0x00FF0000))
#define swapb(d, bit)			\
	 ((d) ^ ((uint8_t)0x01 << (bit)))

#define ___constant_swab16(x) ((uint16_t)(			\
	(((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |		\
	(((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))
#define ___constant_swab32(x) ((uint32_t)(			\
	(((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |	\
	(((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |	\
	(((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |	\
	(((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))
#define __swab32(x)						\
	___constant_swab32(x)

#define LOHALF(b)			(b & 0x0F)
#define HIHALF(b)			(b >> 4)
#ifndef LOBYTE
#define LOBYTE(w)			((uint8_t)w)
#endif
#ifndef HIBYTE
#define HIBYTE(w)			((uint8_t)((w)>>8))
#endif
#ifndef LOWORD
#define LOWORD(w)			((uint16_t)w)
#endif
#ifndef HIWORD
#define HIWORD(w)			((uint16_t)((w)>>16))
#endif
#define MAKEBYTE(a, b)			((uint8_t)(((a) & 0x0F) | (((b) & 0x0F) << 4))) 
#ifndef MAKEWORD
#define MAKEWORD(a, b)			((uint16_t)((a) | ((uint16_t)(b)) << 8)) 
#endif
#ifndef MAKELONG
#define MAKELONG(a, b)			((uint32_t)((a) | ((uint32_t)(b)) << 16))
#endif
#define MAKELLONG(a, b)			((uint64_t)((a) | ((uint64_t)(b)) << 32)) 

#define raise_bits(i, bits)		((i) |= (bits))
#define unraise_bits(i, bits)		((i) &= ~(bits))
#define bits_raised(i, bits)		(((i) & (bits)) == (bits))
#define bits_raised_any(i, bits)	((i) & (bits))

bits_t find_next_set_bit(const bits_t *addr,
			 bits_t size,
			 bits_t offset);
bits_t find_next_clear_bit(const bits_t *addr,
			   bits_t size,
			   bits_t offset);

#define find_first_set_bit(addr, size)		\
	find_next_set_bit((addr), (size), 0)
#define find_first_clear_bit(addr, size)	\
	find_next_clear_bit((addr), (size), 0)

#endif
