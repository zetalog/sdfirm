#ifndef __LPC_DRIVER_H_INCLUDE__
#define __LPC_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_LPC
#include <asm/mach/lpc.h>
#endif

#ifndef ARCH_HAVE_LPC
#define lpc_hw_ctrl_init()		do { } while (0)
#endif

#endif /* __LPC_DRIVER_H_INCLUDE__ */
