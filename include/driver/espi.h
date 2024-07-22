#ifndef __ESPI_DRIVER_H_INCLUDE__
#define __ESPI_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_ESPI
#include <asm/mach/espi.h>
#endif

#ifndef ARCH_HAVE_ESPI
#define espi_hw_ctrl_init()		do { } while (0)
#endif

#endif /* __ESPI_DRIVER_H_INCLUDE__ */
