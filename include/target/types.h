#ifndef __TYPES_H_INCLUDE__
#define __TYPES_H_INCLUDE__

#include <target/compiler.h>
#include <target/const.h>

#ifndef __ASSEMBLY__
#include <asm/types.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef ARCH_HAS_BOOL
typedef bool				boolean;
#else
typedef uint8_t				boolean;
typedef boolean				bool;
#define false				(boolean)0
#define true				(!false)
#endif
typedef uint16_t			wchar_t;
#ifndef ARCH_HAVE_CADDR_T
typedef unsigned long			caddr_t;
#endif
#ifndef ARCH_HAVE_UINTPTR_T
typedef unsigned long			uintptr_t;
#endif
#ifndef ARCH_HAVE_SIZE_T
typedef unsigned long			size_t;
#endif
#ifndef ARCH_HAVE_LOFF_T
typedef unsigned long			loff_t;
#endif

#ifndef NULL
#define	NULL				((void *)0L)
#endif /* NULL */

#ifndef offsetof
#define offsetof(s, m)			((size_t)&((s *)0)->m)
#endif
#ifndef container_of
#define container_of(p, s, m)			\
	({					\
		void *__t = (void *)p;		\
		(s *)(__t - offsetof(s, m));	\
	})
#endif
#endif /* __ASSEMBLY__ */

#define _BV_UL(bit)			(UL(1) << (bit))
#define _BV_ULL(bit)			(ULL(1) << (bit))
#define _BV(bit)			_BV_UL(bit)
#define _SET_FV_UL(name, value)		\
	((UL(0) + ((value) & (name##_MASK))) << (name##_OFFSET))
#define _SET_FV_ULL(name, value)	\
	((ULL(0) + ((value) & (name##_MASK))) << (name##_OFFSET))
#define _SET_FV(name, value)		_SET_FV_UL(name, value)
#define _GET_FV_UL(name, value)		\
	((UL(0) + ((value) >> (name##_OFFSET))) & (name##_MASK))
#define _GET_FV_ULL(name, value)	\
	((ULL(0) + ((value) >> (name##_OFFSET))) & (name##_MASK))
#define _GET_FV(name, value)		_GET_FV_UL(name, value)
/* Default to _SET_FV() as by default _FV() is used to generate field
 * values to be written to the registers.
 */
#define _FV(name, value)		_SET_FV(name, value)

#define REG_1BIT_MASK			0x001
#define REG_2BIT_MASK			0x003
#define REG_3BIT_MASK			0x007
#define REG_4BIT_MASK			0x00F
#define REG_5BIT_MASK			0x01F
#define REG_6BIT_MASK			0x03F
#define REG_7BIT_MASK			0x07F
#define REG_8BIT_MASK			0x0FF
#define REG_9BIT_MASK			0x1FF
#define REG_10BIT_MASK			0x3FF
#define REG_11BIT_MASK			0x7FF
#define REG_12BIT_MASK			0xFFF
#define REG_13BIT_MASK			0x1FFF
#define REG_14BIT_MASK			0x3FFF
#define REG_15BIT_MASK			0x7FFF
#define REG_16BIT_MASK			0xFFFF
#define REG_17BIT_MASK			0x1FFFF
#define REG_20BIT_MASK			0xFFFFF
#define REG_23BIT_MASK			0x7FFFFF
#define REG_24BIT_MASK			0xFFFFFF
#define REG_28BIT_MASK			0xFFFFFFF
#define REG_32BIT_MASK			0xFFFFFFFF
#define REG_36BIT_MASK			ULL(0xFFFFFFFFF)
#define REG_40BIT_MASK			ULL(0xFFFFFFFFFF)
#define REG_44BIT_MASK			ULL(0xFFFFFFFFFFF)

#define REG16_1BIT_INDEX(n)		((((uint16_t)(n)) & (~UL(15))) >> 4)
#define REG16_2BIT_INDEX(n)		((((uint16_t)(n)) & (~UL(7 ))) >> 3)
#define REG16_4BIT_INDEX(n)		((((uint16_t)(n)) & (~UL(3 ))) >> 2)
#define REG16_8BIT_INDEX(n)		((((uint16_t)(n)) & (~UL(1 ))) >> 1)
#define REG32_1BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(31))) >> 5)
#define REG32_2BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(15))) >> 4)
#define REG32_4BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(7 ))) >> 3)
#define REG32_8BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(3 ))) >> 2)
#define REG32_16BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(1 ))) >> 1)
#define REG64_1BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(63))) >> 6)
#define REG64_2BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(31))) >> 5)
#define REG64_4BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(15))) >> 4)
#define REG64_8BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(7) )) >> 3)
#define REG64_16BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(3) )) >> 2)
#define REG64_32BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(1) )) >> 1)
#define REG_1BIT_INDEX(n)		REG32_1BIT_INDEX(n)
#define REG_2BIT_INDEX(n)		REG32_2BIT_INDEX(n)
#define REG_4BIT_INDEX(n)		REG32_4BIT_INDEX(n)
#define REG_8BIT_INDEX(n)		REG32_8BIT_INDEX(n)
#define REG_16BIT_INDEX(n)		REG32_16BIT_INDEX(n)

#define REG16_1BIT_OFFSET(n)		((((uint16_t)(n)) & UL(15)) << 0)
#define REG16_2BIT_OFFSET(n)		((((uint16_t)(n)) & UL( 7)) << 1)
#define REG16_4BIT_OFFSET(n)		((((uint16_t)(n)) & UL( 3)) << 2)
#define REG16_8BIT_OFFSET(n)		((((uint16_t)(n)) & UL( 1)) << 3)
#define REG32_1BIT_OFFSET(n)		((((uint32_t)(n)) & UL(31)) << 0)
#define REG32_2BIT_OFFSET(n)		((((uint32_t)(n)) & UL(15)) << 1)
#define REG32_4BIT_OFFSET(n)		((((uint32_t)(n)) & UL( 7)) << 2)
#define REG32_8BIT_OFFSET(n)		((((uint32_t)(n)) & UL( 3)) << 3)
#define REG32_16BIT_OFFSET(n)		((((uint32_t)(n)) & UL( 1)) << 4)
#define REG64_1BIT_OFFSET(n)		((((uint64_t)(n)) & ULL(63)) << 0)
#define REG64_2BIT_OFFSET(n)		((((uint64_t)(n)) & ULL(31)) << 1)
#define REG64_4BIT_OFFSET(n)		((((uint64_t)(n)) & ULL(15)) << 2)
#define REG64_8BIT_OFFSET(n)		((((uint64_t)(n)) & ULL( 7)) << 3)
#define REG64_16BIT_OFFSET(n)		((((uint64_t)(n)) & ULL( 3)) << 4)
#define REG64_32BIT_OFFSET(n)		((((uint64_t)(n)) & ULL( 1)) << 5)
#define REG_1BIT_OFFSET(n)		REG32_1BIT_OFFSET(n)
#define REG_2BIT_OFFSET(n)		REG32_2BIT_OFFSET(n)
#define REG_4BIT_OFFSET(n)		REG32_4BIT_OFFSET(n)
#define REG_8BIT_OFFSET(n)		REG32_8BIT_OFFSET(n)
#define REG_16BIT_OFFSET(n)		REG32_16BIT_OFFSET(n)

#define REG_1BIT_ADDR(base, n)		((base)+(((n) & (~31)) >> 3))
#define REG_2BIT_ADDR(base, n)		((base)+(((n) & (~15)) >> 2))
#define REG_4BIT_ADDR(base, n)		((base)+(((n) & (~7 )) >> 1))
#define REG_8BIT_ADDR(base, n)		((base)+(((n) & (~3 )) >> 0))
#define _GET_FVn(n, name, value)	\
	(((value) >> (name##_OFFSET(n))) & (name##_MASK))
#define _SET_FVn(n, name, value)	\
	(((value) & (name##_MASK)) << (name##_OFFSET(n)))

/* Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l)				\
	(((~UL(0)) - (UL(1) << (l)) + 1) &	\
	 (~UL(0) >> (__WORDSIZE - 1 - (h))))
#define GENMASK_ULL(h, l)			\
	(((~ULL(0)) - (ULL(1) << (l)) + 1) &	\
	 (~ULL(0) >> (__WORDSIZE - 1 - (h))))

#ifndef __ASSEMBLY__
__TEXT_TYPE__(char, text_char_t);
__TEXT_TYPE__(const uint8_t, text_byte_t);
__TEXT_TYPE__(uint16_t, text_word_t);
__TEXT_TYPE__(uint32_t, text_quad_t);
#endif /* __ASSEMBLY__ */

#endif /* __TYPES_H_INCLUDE__ */
