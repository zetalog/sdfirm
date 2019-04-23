#ifndef __TYPES_ARM64_H_INCLUDE__
#define __TYPES_ARM64_H_INCLUDE__

#include <stdbool.h>
#define ARCH_HAS_BOOL	1

#define __text__	const
#define __near__
#define __far__

#define __WORDSIZE	64
#define UINTPTR_MAX	UINT64_MAX

#define __TEXT_TYPE__(type, name)	\
	typedef type name

typedef unsigned long	caddr_t;
#define ARCH_HAVE_CADDR_T 1
typedef unsigned long	size_t;
#define ARCH_HAVE_SIZE_T 1
typedef caddr_t		uintptr_t;

#endif /* __TYPES_ARM64_H_INCLUDE__ */
