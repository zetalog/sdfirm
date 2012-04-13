#ifndef __WDT_DRIVER_H_INCLUDE__
#define __WDT_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_ARCH_HAS_WDT
#include <asm/mach/wdt.h>
#endif

#ifndef ARCH_HAVE_WDT
#define wdt_hw_ctrl_stop()
#endif

#endif /* __WDT_DRIVER_H_INCLUDE__ */
