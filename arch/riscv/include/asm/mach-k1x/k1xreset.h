#ifndef __K1XRESET_K1MAX_H_INCLUDE__
#define __K1XRESET_K1MAX_H_INCLUDE__

#include <asm/mach/reg.h>

#ifndef __ASSEMBLY__
#ifdef CONFIG_K1M_K1X
void k1x_cpu_reset(void);
#else
#define k1x_cpu_reset()		do { } while (0)
#endif
#endif

#endif /* __K1XRESET_K1MAX_H_INCLUDE__ */