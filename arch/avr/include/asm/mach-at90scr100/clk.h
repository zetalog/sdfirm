#ifndef __CLK_AT90SCR100_H_INCLUDE__
#define __CLK_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

/* Oscillator Calibration Register */
#define OSCCAL  _SFR_MEM8(0x66)

/*
 * PLL Control Register
 */
#define PLLCR	_SFR_MEM8(0x62)
#define PLLMUX	7
#define PLOCK	1
#define PON	0

/*
 * Clock Prescale Register
 */
#define CLKPR   _SFR_MEM8(0x61)
#define CLKPS3	3
#define CLKPS2	2
#define CLKPS1	1
#define CLKPS0	0

#define CLK_OSC		8000
#define CLK_PLL		96000

#if CONFIG_CPU_AT90SCR100_FREQ_8M
#define CLK_CPU		8000
#define CLK_CLKPR	5
#endif

#if CONFIG_CPU_AT90SCR100_FREQ_16M
#define CLK_CPU		16000
#define CLK_CLKPR	3
#endif

#if CONFIG_CPU_AT90SCR100_FREQ_24M
#define CLK_CPU		24000
#define CLK_CLKPR	1
#endif

#define CLK_IO		CLK_CPU
#define CLK_FLASH	CLK_CPU

#define CLK_USB		48000

/* XXX: 7.1.8 USB modules can only work under CLK_PLL. */
#define __clk_hw_set_prescale(x)		\
	do {					\
		clk_hw_set_pll(_BV(PLLMUX));	\
		CLKPR = (x);			\
	} while (0)

#define clk_hw_set_pll(mux)			\
	do {					\
		PLLCR = (_BV(PON));		\
		while (!(PLLCR & _BV(PLOCK)));	\
		PLLCR |= (mux);			\
	} while (0)

void clk_hw_set_cpu(void);
#define clk_hw_set_usb()

#endif /* __CLK_AT90SCR100_H_INCLUDE__ */
