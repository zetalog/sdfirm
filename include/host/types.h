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

#endif /* __HOST_TYPES_H_INCLUDE__ */
