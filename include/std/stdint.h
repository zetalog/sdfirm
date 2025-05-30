#ifndef __STD_STDINT_H_INCLUDE__
#define __STD_STDINT_H_INCLUDE__

#define INT8_MIN	(-1-0x7f)
#define INT16_MIN	(-1-0x7fff)
#define INT32_MIN	(-1-0x7fffffff)
#define INT64_MIN	(-1-0x7fffffffffffffff)

#define INT8_MAX	(0x7f)
#define INT16_MAX	(0x7fff)
#define INT32_MAX	(0x7fffffff)
#define INT64_MAX	(0x7fffffffffffffff)

#define UINT8_MAX	(0xff)
#define UINT16_MAX	(0xffff)
#define UINT32_MAX	(0xffffffff)
#define UINT64_MAX	(0xffffffffffffffff)

#define INTMAX_MIN	INT64_MIN
#define INTMAX_MAX	INT64_MAX
#define UINTMAX_MAX	UINT64_MAX

#define INT8_C(c)	c
#define INT16_C(c)	c
#define INT32_C(c)	c

#define UINT8_C(c)	c
#define UINT16_C(c)	c
#define UINT32_C(c)	U(c)

#if UINTPTR_MAX == UINT64_MAX
#define INT64_C(c)	L(c)
#define UINT64_C(c)	UL(c)
#define INTMAX_C(c)	L(c)
#define UINTMAX_C(c)	UL(c)
#else
#define INT64_C(c)	LL(c)
#define UINT64_C(c)	ULL(c)
#define INTMAX_C(c)	LL(c)
#define UINTMAX_C(c)	ULL(c)
#endif

#ifndef __ASSEMBLY__
typedef signed char		int8_t;
typedef unsigned char		uint8_t;
#ifdef SDCC
typedef signed short int	int16_t;
typedef unsigned short int	uint16_t;
#else
typedef signed short		int16_t;
typedef unsigned short		uint16_t;
#endif
#ifndef ARCH_HAVE_UINT32_T
#ifdef SDCC
typedef signed long int		int32_t;
typedef unsigned long int	uint32_t;
#else
typedef signed int		int32_t;
typedef unsigned int		uint32_t;
#endif
#endif
#ifndef ARCH_HAVE_UINT64_T
#ifdef SDCC
typedef signed long long int	int64_t;
typedef unsigned long long int	uint64_t;
#else
typedef signed long long	int64_t;
typedef unsigned long long	uint64_t;
#endif
#endif
typedef unsigned long long	uintmax_t;
#endif /* __ASSEMBLY__ */

#include <target/types.h>

#endif /* __STD_STDINT_H_INCLUDE__ */
