#ifndef __PREFETCH_H_INCLUDE__
#define __PREFETCH_H_INCLUDE__

#include <target/arch.h>

#ifndef prefetch
#define prefetch(x) __builtin_prefetch(x)
#endif

#ifndef prefetchw
#define prefetchw(x) __builtin_prefetch(x,1)
#endif

#endif /* __PREFETCH_H_INCLUDE__ */
