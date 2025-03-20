#ifndef __WDT_DRIVER_H_INCLUDE__
#define __WDT_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_WDT
#include <asm/mach/wdt.h>
#endif

#ifndef ARCH_HAVE_WDT
#define wdt_hw_con_bark()		do { } while (0)
#define wdt_hw_con_init()		do { } while (0)
#endif

#endif /* __WDT_DRIVER_H_INCLUDE__ */
