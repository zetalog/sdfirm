#ifndef __HOST_TYPES_H_INCLUDE__
#define __HOST_TYPES_H_INCLUDE__

#include <stdint.h>
typedef unsigned char		boolean;

#ifndef NULL
#define	NULL			((void *)0L)
#endif /* NULL */

#define false			(unsigned char)0
#define true			!false

#ifndef inline
#define inline __inline
#endif

#ifdef WIN32
#define ULL(num)	num##UI64
#else
#define ULL(num)	num##ULL
#endif

#endif /* __HOST_TYPES_H_INCLUDE__ */
