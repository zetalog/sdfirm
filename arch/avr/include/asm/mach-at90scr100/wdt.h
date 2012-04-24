#ifndef __WDT_AT90SCR100_H_INCLUDE__
#define __WDT_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

/* AT90SCR100 BSP always requires wdt support */
#ifdef CONFIG_ARCH_AT90SCR100
#ifndef ARCH_HAVE_WDT
#define ARCH_HAVE_WDT		1
#else
#error "Multiple watchdog timer defined"
#endif
#endif

/*
 * Watchdog Control and Status Register
 */
#define WDTCSR  _SFR_MEM8(0x60)
#define WDP0	0
#define WDP1	1
#define WDP2	2
#define WDE	3
#define WDCE	4
#define WDP3	5
#define WDIE	6
#define WDIF	7

/* MCUSR */
#define WDRF	3

#define wdr()  __asm__ __volatile__ ("wdr" ::)

void wdt_hw_ctrl_start(uint8_t value);
void wdt_hw_ctrl_stop(void);

#endif /* __WDT_AT90SCR100_H_INCLUDE__ */
