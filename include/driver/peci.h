#ifndef __PECI_DRIVER_H_INCLUDE__
#define __PECI_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_PECI
#include <asm/mach/peci.h>
#endif

#ifndef ARCH_HAVE_PECI
#define peci_hw_ctrl_init()		do { } while (0)
#define peci_hw_xfer_submit(wr, rd)	do { } while (0)
#endif

#endif /* __PECI_DRIVER_H_INCLUDE__ */
