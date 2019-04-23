#ifndef __TYPES_H_INCLUDE__
#define __TYPES_H_INCLUDE__

#ifndef __ASSEMBLY__
typedef signed char		int8_t;
typedef unsigned char		uint8_t;
typedef signed short		int16_t;
typedef unsigned short		uint16_t;

#include <errno.h>
#include <asm/types.h>
#include <limits.h>
#include <ctype.h>

#ifdef ARCH_HAS_BOOL
typedef bool			boolean;
#else
typedef uint8_t			boolean;
typedef boolean			bool;
#define false			(boolean)0
#define true			(!FALSE)
#endif
typedef uint16_t		wchar_t;
#ifndef ARCH_HAVE_UINT32_T
typedef signed int		int32_t;
typedef unsigned int		uint32_t;
#endif
#ifndef ARCH_HAVE_UINT64_T
typedef signed long long	int64_t;
typedef unsigned long long	uint64_t;
#endif
#ifndef ARCH_HAVE_CADDR_T
typedef unsigned long		caddr_t;
#endif
#ifndef ARCH_HAVE_SIZE_T
typedef unsigned long		size_t;
#endif
#ifndef ARCH_HAVE_LOFF_T
typedef unsigned long		loff_t;
#endif

#ifndef NULL
#define	NULL			((void *)0L)
#endif /* NULL */

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER)	__compiler_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER)	((size_t)&((TYPE *)0)->MEMBER)
#endif
#endif /* __ASSEMBLY__ */

#define _BV(bit)		(1 << (bit))
#define _SET_FV(name, value)	\
	(((value) & (name##_MASK)) << (name##_OFFSET))
#define _GET_FV(name, value)	\
	(((value) >> (name##_OFFSET)) & (name##_MASK))
/* Default to _SET_FV() as by default _FV() is used to generate field
 * values to be written to the registers.
 */
#define _FV(name, value)	_SET_FV(name, value)

#define REG_1BIT_MASK		0x001
#define REG_2BIT_MASK		0x003
#define REG_3BIT_MASK		0x007
#define REG_4BIT_MASK		0x00F
#define REG_5BIT_MASK		0x01F
#define REG_6BIT_MASK		0x03F
#define REG_7BIT_MASK		0x07F
#define REG_8BIT_MASK		0x0FF
#define REG_9BIT_MASK		0x1FF
#define REG_10BIT_MASK		0x3FF
#define REG_11BIT_MASK		0x7FF
#define REG_12BIT_MASK		0xFFF
#define REG_13BIT_MASK		0x1FFF
#define REG_16BIT_MASK		0xFFFF
#define REG_17BIT_MASK		0x1FFFF
#define REG_20BIT_MASK		0xFFFFF
#define REG_24BIT_MASK		0xFFFFFF

#ifndef __ASSEMBLY__
__TEXT_TYPE__(char, text_char_t);
__TEXT_TYPE__(const uint8_t, text_byte_t);
__TEXT_TYPE__(uint16_t, text_word_t);
__TEXT_TYPE__(uint32_t, text_quad_t);
#endif /* __ASSEMBLY__ */

#endif /* __TYPES_H_INCLUDE__ */
