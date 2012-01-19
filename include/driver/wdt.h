#ifndef __WDT_DRIVER_H_INCLUDE__
#define __WDT_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_WDT_AT90USB1287
#include <asm/mach-at90usb1287/wdt.h>
#endif

#ifndef ARCH_HAVE_WDT
#define wdt_hw_ctrl_stop()
#endif

#endif /* __WDT_DRIVER_H_INCLUDE__ */
