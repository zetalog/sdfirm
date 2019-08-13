#ifndef __TYPES_RISCV_H_INCLUDE__
#define __TYPES_RISCV_H_INCLUDE__

#include <stdbool.h>
#define ARCH_HAS_BOOL	1

#define __text__	const
#define __near__
#define __far__

#ifdef CONFIG_32BIT
#define __WORDSIZE	32
#define UINTPTR_MAX	UINT32_MAX
#endif
#ifdef CONFIG_64BIT
#define __WORDSIZE	64
#define UINTPTR_MAX	UINT64_MAX
#endif

#define __TEXT_TYPE__(type, name)	\
	typedef type name

typedef unsigned long	caddr_t;
#define ARCH_HAVE_CADDR_T 1
typedef unsigned long	size_t;
#define ARCH_HAVE_SIZE_T 1
typedef caddr_t		uintptr_t;
#define ARCH_HAVE_UINTPTR_T 1

#endif /* __TYPES_RISCV_H_INCLUDE__ */
