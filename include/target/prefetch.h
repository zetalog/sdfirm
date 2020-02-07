#ifndef __PREFETCH_H_INCLUDE__
#define __PREFETCH_H_INCLUDE__

#include <target/arch.h>

#ifndef ARCH_HAS_PREFETCH
#define prefetch(x) __builtin_prefetch(x)
#endif

#ifndef ARCH_HAS_PREFETCHW
#define prefetchw(x) __builtin_prefetch(x,1)
#endif

#endif /* __PREFETCH_H_INCLUDE__ */
