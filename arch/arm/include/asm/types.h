#ifndef __TYPES_ARM_H_INCLUDE__
#define __TYPES_ARM_H_INCLUDE__

#define __text__	const
#define __near__
#define __far__

#define __WORDSIZE	64

#define __TEXT_TYPE__(type, name)	\
	typedef type name

typedef uint32_t	caddr_t;
#define ARCH_HAVE_CADDR_T 1
typedef uint32_t	size_t;
#define ARCH_HAVE_SIZE_T 1

#endif /* __TYPES_ARM_H_INCLUDE__ */
