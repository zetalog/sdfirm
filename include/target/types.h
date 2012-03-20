#ifndef __TYPES_H_INCLUDE__
#define __TYPES_H_INCLUDE__

typedef signed char		int8_t;
typedef unsigned char		uint8_t;
typedef signed short		int16_t;
typedef unsigned short		uint16_t;
typedef signed long		int32_t;
typedef unsigned long		uint32_t;

#include <asm/types.h>

typedef uint8_t			boolean;
typedef unsigned short		wchar_t;
#ifndef ARCH_HAVE_UINT64_T
typedef unsigned long long	uint64_t;
#endif
#ifndef ARCH_HAVE_CADDR_T
typedef uint16_t		caddr_t;
#endif
#ifndef ARCH_HAVE_SIZE_T
typedef uint32_t		size_t;
#endif

#ifndef NULL
#define	NULL			((void *)0L)
#endif /* NULL */

#define false			(unsigned char)0
#define true			!false

#define _BV(bit)		(1 << (bit))

__TEXT_TYPE__(char, text_char_t);
__TEXT_TYPE__(const uint8_t, text_byte_t);
__TEXT_TYPE__(uint16_t, text_word_t);
__TEXT_TYPE__(uint32_t, text_quad_t);

#endif /* __TYPES_H_INCLUDE__ */
