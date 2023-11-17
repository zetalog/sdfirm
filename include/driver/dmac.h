#ifndef __DMAC_DRIVER_H_INCLUDE__
#define __DMAC_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_DMAC
#include <asm/mach/dmac.h>
#endif

#ifndef ARCH_HAVE_DMAC
// #define dmac_hw_ctrl_init()		do { } while (0)
#endif

#endif /* __DMAC_DRIVER_H_INCLUDE__ */
