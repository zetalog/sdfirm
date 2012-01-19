#ifndef __TYPES_AVR_H_INCLUDE__
#define __TYPES_AVR_H_INCLUDE__

#define __text__	__attribute__((__progmem__))
#define __near__
#define __far__

#define __TEXT_TYPE__(type, name)	\
	typedef type name
/* this can not work with structures, and need special wrappers using
 * LPM/ELPM instructions accessing program space
 * #define __TEXT_TYPE__(type, name)	\
 *	typedef type name __text__
 */

typedef uint16_t	caddr_t;
#define ARCH_HAVE_CADDR_T 1
typedef uint16_t	size_t;
#define ARCH_HAVE_SIZE_T 1

#endif /* __TYPES_AVR_H_INCLUDE__ */
