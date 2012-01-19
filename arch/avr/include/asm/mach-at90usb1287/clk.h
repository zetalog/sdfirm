#ifndef __CLK_AT90USB1287_H_INCLUDE__
#define __CLK_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

/*
 * PLL Control and Status Register
 */
#define PLLCSR	_SFR_IO8(0x29)
#define PLLP2	4
#define PLLP1	3
#define PLLP0	2
#define PLLE	1
#define PLOCK	0

/*
 * Oscillator Calibration Register
 */
#define OSCCAL  _SFR_MEM8(0x66)

/*
 * Clock Prescale Register
 */
#define CLKPR   _SFR_MEM8(0x61)
#define CLKPCE	7
#define CLKPS3	3
#define CLKPS2	2
#define CLKPS1	1
#define CLKPS0	0

#define CLK_OSC		8000

#if CONFIG_CPU_AT90USB1287_FREQ_8M
#define CLK_CPU		CLK_OSC
#define CLK_DIVISOR	0
#define PLL_DIVISOR	(_BV(PLLP1)|_BV(PLLP0))
#endif

#define CLK_IO		CLK_CPU
#define CLK_FLASH	CLK_CPU

#define CLK_USB		48000

/* To avoid unintentional changes of clock frequency, a special write
 * procedure must be followed to change the CLKPS bits:
 *
 * 1. Write the Clock Prescaler Change Enable (CLKPCE) bit to one and all
 *    other bits in CLKPR to zero.
 * 2. Within four cycles, write the desired value to CLKPS while write a
 *    zero to CLKPCE.
 */
#define __clk_hw_set_prescale(x)		\
	do {					\
		CLKPR = _BV(CLKPCE);		\
		CLKPR = (x);			\
	} while (0)

#define clk_hw_set_pll(x)			\
	do {					\
		PLLCSR = (_BV(PLLE)|x);		\
		while (!(PLLCSR & _BV(PLOCK)));	\
	} while (0)

void clk_hw_set_cpu(void);
#define clk_hw_set_usb()		clk_hw_set_pll(PLL_DIVISOR)

#endif /* __CLK_AT90USB1287_H_INCLUDE__ */
