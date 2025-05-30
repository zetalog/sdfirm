#ifndef __BITOPS_H_INCLUDE__
#define __BITOPS_H_INCLUDE__

#include <target/types.h>

#if defined(CONFIG_ARCH_HAS_BITS_PER_UNIT_64)
typedef uint64_t bits_t;
#define BITS_PER_UNIT		64
#define __ffs_unit		__ffs64
#define __fls_unit		__fls64
#define __ffz_unit		__ffz64
#elif defined(CONFIG_ARCH_HAS_BITS_PER_UNIT_32)
typedef uint32_t bits_t;
#define BITS_PER_UNIT		32
#define __ffs_unit		__ffs32
#define __fls_unit		__fls32
#define __ffz_unit		__ffz32
#elif defined(CONFIG_ARCH_HAS_BITS_PER_UINT_16)
typedef uint8_t bits_t;
#define BITS_PER_UNIT		16
#define __ffs_unit		__ffs16
#define __fls_unit		__fls16
#define __ffz_unit		__ffz16
#else
typedef uint8_t bits_t;
#define BITS_PER_UNIT		8
#define __ffs_unit		__ffs8
#define __fls_unit		__fls8
#define __ffz_unit		__ffz8
#endif
#define BITS_PER_BYTE		8

#define BITS_TO_UNITS(bits)		\
	(((bits_t)((bits)+BITS_PER_UNIT-1))/((uint8_t)BITS_PER_UNIT))
#define DECLARE_BITMAP(name, bits)	\
	bits_t name[BITS_TO_UNITS(bits)]
#define BITOP_MASK(nr)		((bits_t)1 << ((nr) % BITS_PER_UNIT))
#define BITOP_WORD(nr)		((bits_t)(nr) / (uint8_t)BITS_PER_UNIT)
#define BIT_MASK(nr)		((bits_t)1 << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((bits_t)(nr) / (uint8_t)BITS_PER_LONG)

void __set_bit(bits_t nr, volatile bits_t *addr);
void __clear_bit(bits_t nr, volatile bits_t *addr);
bool __test_and_set_bit(bits_t nr, volatile bits_t *addr);
bool __test_and_clear_bit(bits_t nr, volatile bits_t *addr);
#ifdef CONFIG_SMP
void set_bit(bits_t nr, volatile bits_t *addr);
void clear_bit(bits_t nr, volatile bits_t *addr);
bool test_and_set_bit(bits_t nr, volatile bits_t *addr);
bool test_and_clear_bit(bits_t nr, volatile bits_t *addr);
#else
#define set_bit(nr, addr)		__set_bit(nr, addr)
#define clear_bit(nr, addr)		__clear_bit(nr, addr)
#define test_and_set_bit(nr, addr)	__test_and_set_bit(nr, addr)
#define test_and_clear_bit(nr, addr)	__test_and_clear_bit(nr, addr)
#endif
boolean test_bit(bits_t nr, const bits_t *addr);
#define find_first_set_bit(addr, size)		\
	find_next_set_bit((addr), (size), 0)
#define find_first_clear_bit(addr, size)	\
	find_next_clear_bit((addr), (size), 0)
bits_t find_next_set_bit(const bits_t *addr,
			 bits_t size, bits_t offset);
bits_t find_next_clear_bit(const bits_t *addr,
			   bits_t size, bits_t offset);

#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))
#define ALIGN_UP(x, a)		__ALIGN_MASK(x, (a)-1)
#define ALIGN_DOWN(x, a)	__ALIGN_MASK((x)-((a)-1), (a)-1)
#define ALIGN(x, a)		ALIGN_UP(x, a)

#define IS_ALIGNED(x, a)	(((x) & ((a) - 1)) == 0)

#define __round_mask(x, y) 	((y)-1)
#define round_up(x, y)		((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y)	((x) & ~__round_mask(x, y))

uint8_t __ffs8(uint8_t word);
uint8_t __ffs16(uint16_t word);
uint8_t __ffs32(uint32_t dword);
uint8_t __ffs64(uint64_t qword);
uint8_t __fls8(uint8_t x);
uint8_t __fls16(uint16_t word);
uint8_t __fls32(uint32_t dword);
uint8_t __fls64(uint64_t quad);
#define __ffz8(x)	__ffs8((uint8_t)(~(x)))
#define __ffz16(x)	__ffs16((uint16_t)(~(x)))
#define __ffz32(x)	__ffs32((uint32_t)(~(x)))
#define __ffz64(x)	__ffs64((uint64_t)(~(x)))

#define __roundup8(n)	((uint8_t)1 << (__fls8((uint8_t)((n)-1))+1))
#define __roundup16(n)	((uint16_t)1 << (__fls16((uint16_t)((n)-1))+1))
#define __roundup32(n)	((uint32_t)1 << (__fls32((uint32_t)((n)-1))+1))
#define __roundup64(n)	((uint64_t)1 << (__fls64((uint32_t)((n)-1))+1))
#define __rounddown32(n)	((uint32_t)1 << __fls32(n))

#define __ilog2_u8(n)	__fls8(n)
#define __ilog2_u16(n)	__fls16(n)
#define __ilog2_u32(n)	__fls32(n)
#define __ilog2_u64(n)	__fls64(n)
extern __noreturn uint8_t ____ilog2_NaN(void);
#define ilog2_const(n)			\
(					\
	(n) < 1 ? ____ilog2_NaN() :	\
	(n) & (ULL(1) << 63) ? 63 :	\
	(n) & (ULL(1) << 62) ? 62 :	\
	(n) & (ULL(1) << 61) ? 61 :	\
	(n) & (ULL(1) << 60) ? 60 :	\
	(n) & (ULL(1) << 59) ? 59 :	\
	(n) & (ULL(1) << 58) ? 58 :	\
	(n) & (ULL(1) << 57) ? 57 :	\
	(n) & (ULL(1) << 56) ? 56 :	\
	(n) & (ULL(1) << 55) ? 55 :	\
	(n) & (ULL(1) << 54) ? 54 :	\
	(n) & (ULL(1) << 53) ? 53 :	\
	(n) & (ULL(1) << 52) ? 52 :	\
	(n) & (ULL(1) << 51) ? 51 :	\
	(n) & (ULL(1) << 50) ? 50 :	\
	(n) & (ULL(1) << 49) ? 49 :	\
	(n) & (ULL(1) << 48) ? 48 :	\
	(n) & (ULL(1) << 47) ? 47 :	\
	(n) & (ULL(1) << 46) ? 46 :	\
	(n) & (ULL(1) << 45) ? 45 :	\
	(n) & (ULL(1) << 44) ? 44 :	\
	(n) & (ULL(1) << 43) ? 43 :	\
	(n) & (ULL(1) << 42) ? 42 :	\
	(n) & (ULL(1) << 41) ? 41 :	\
	(n) & (ULL(1) << 40) ? 40 :	\
	(n) & (ULL(1) << 39) ? 39 :	\
	(n) & (ULL(1) << 38) ? 38 :	\
	(n) & (ULL(1) << 37) ? 37 :	\
	(n) & (ULL(1) << 36) ? 36 :	\
	(n) & (ULL(1) << 35) ? 35 :	\
	(n) & (ULL(1) << 34) ? 34 :	\
	(n) & (ULL(1) << 33) ? 33 :	\
	(n) & (ULL(1) << 32) ? 32 :	\
	(n) & (ULL(1) << 31) ? 31 :	\
	(n) & (ULL(1) << 30) ? 30 :	\
	(n) & (ULL(1) << 29) ? 29 :	\
	(n) & (ULL(1) << 28) ? 28 :	\
	(n) & (ULL(1) << 27) ? 27 :	\
	(n) & (ULL(1) << 26) ? 26 :	\
	(n) & (ULL(1) << 25) ? 25 :	\
	(n) & (ULL(1) << 24) ? 24 :	\
	(n) & (ULL(1) << 23) ? 23 :	\
	(n) & (ULL(1) << 22) ? 22 :	\
	(n) & (ULL(1) << 21) ? 21 :	\
	(n) & (ULL(1) << 20) ? 20 :	\
	(n) & (ULL(1) << 19) ? 19 :	\
	(n) & (ULL(1) << 18) ? 18 :	\
	(n) & (ULL(1) << 17) ? 17 :	\
	(n) & (ULL(1) << 16) ? 16 :	\
	(n) & (ULL(1) << 15) ? 15 :	\
	(n) & (ULL(1) << 14) ? 14 :	\
	(n) & (ULL(1) << 13) ? 13 :	\
	(n) & (ULL(1) << 12) ? 12 :	\
	(n) & (ULL(1) << 11) ? 11 :	\
	(n) & (ULL(1) << 10) ? 10 :	\
	(n) & (ULL(1) <<  9) ?  9 :	\
	(n) & (ULL(1) <<  8) ?  8 :	\
	(n) & (ULL(1) <<  7) ?  7 :	\
	(n) & (ULL(1) <<  6) ?  6 :	\
	(n) & (ULL(1) <<  5) ?  5 :	\
	(n) & (ULL(1) <<  4) ?  4 :	\
	(n) & (ULL(1) <<  3) ?  3 :	\
	(n) & (ULL(1) <<  2) ?  2 :	\
	(n) & (ULL(1) <<  1) ?  1 :	\
	(n) & (ULL(1) <<  0) ?  0 :	\
	____ilog2_NaN()			\
)

#define is_power_of_2(n)		\
        (n != 0 && ((n & (n - 1)) == 0))

#define bitrev3(bits)			\
	 ((((bits) & 0x1) << 2) | (((bits)& 0x4) >> 2) | ((bits) & 0x2))
uint8_t bitrev8(uint8_t byte);
uint16_t bitrev16(uint16_t word);
uint8_t hweight8(uint8_t byte);
uint8_t hweight16(uint16_t word);
uint8_t hweight32(uint32_t word);
uint8_t hweight64(uint64_t quad);

#define swap16(x) ((uint16_t)(						\
	(((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |			\
	(((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))
#define swap32(x) ((uint32_t)(						\
	(((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |		\
	(((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |		\
	(((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |		\
	(((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))
#define swap64(x) ((uint64_t)(						\
	(((uint64_t)(x) & (uint64_t)ULL(0x00000000000000ff)) << 56) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x000000000000ff00)) << 40) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x0000000000ff0000)) << 24) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x00000000ff000000)) <<  8) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x000000ff00000000)) >>  8) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x0000ff0000000000)) >> 24) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0x00ff000000000000)) >> 40) |	\
	(((uint64_t)(x) & (uint64_t)ULL(0xff00000000000000)) >> 56)))
#define swapb(d, bit)			\
	 ((d) ^ ((uint8_t)0x01 << (bit)))
uint8_t parity8(uint8_t val);

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
#ifndef LODWORD
#define LODWORD(w)			((uint32_t)w)
#endif
#ifndef HIDWORD
#define HIDWORD(w)			((uint32_t)((w)>>32))
#endif
#define MAKEBYTE(a, b)			((uint8_t)(((a) & 0x0F) | (((b) & 0x0F) << 4))) 
#ifndef MAKEWORD
#define MAKEWORD(a, b)			((uint16_t)(((uint16_t)(a)) | \
						    ((uint16_t)(b)) << 8))
#endif
#ifndef MAKELONG
#define MAKELONG(a, b)			((uint32_t)(((uint32_t)(a)) | \
						    ((uint32_t)(b)) << 16))
#endif
#ifndef MAKELLONG
#define MAKELLONG(a, b)			((uint64_t)(((uint64_t)(a)) | \
						    ((uint64_t)(b)) << 32))
#endif

#define raise_bits(i, bits)		((i) |= (bits))
#define unraise_bits(i, bits)		((i) &= ~(bits))
#define bits_raised(i, bits)		(((i) & (bits)) == (bits))
#define bits_raised_any(i, bits)	((i) & (bits))

#include <target/endian.h>

#endif /* __BITOPS_H_INCLUDE__ */
