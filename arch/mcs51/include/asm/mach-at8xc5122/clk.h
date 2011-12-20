#ifndef __CLK_AT8XC5122_H_INCLUDE__
#define __CLK_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>

/* Clock Selection Register */
Sfr(CKSEL,	0x85);
#define CKS	0

/* Clock Configuration Register 0 */
Sfr(CKCON0,	0x8F);
/* Clock Configuration Register 1 */
Sfr(CKCON1,	0xAF);
#define X2	0

/* PLL Control Register */
Sfr(PLLCON,	0xA3);
#define EXT48	2
#define PLLEN	1
#define PLOCK	0

/* Clock Reload Register */
Sfr(CKRL,	0x97);

/* PLL Divider Register */
Sfr(PLLDIV,	0xA4);

/* variables used for clk_hw_resume_dev/clk_hw_suspend_dev */
/* CKCON0 */
#define DEV_GPT0		0x01
#define DEV_GPT1		0x02
#define DEV_UART		0x04
#define DEV_WDT			0x06
/* CKCON1 */
#define DEV_SPI			0x08

/* CLK_XTAL & CLK_CPU definitions */
/* PLLDIV_RN will be used for PLL programming (96MHz), its value is
 * CLK_XTAL1 dependent.  Thus it is defined here along with the CLK_XTAL1
 * value definition.
 */
#ifdef CONFIG_CPU_AT8XC5122_PLL
#define CLK_XTAL1		8	/* M */
#define PLLDIV_RN		0xB0
#ifdef CONFIG_CPU_AT8XC5122_FREQ_24M
#define CLK_CPU			24	/* M */
#endif
#ifdef CONFIG_CPU_AT8XC5122_FREQ_48M
#define CLK_CPU			48	/* M */
#endif
#endif
#ifdef CONFIG_CPU_AT8XC5122_OSC
#ifdef CONFIG_OSC_AT8XC5122_FREQ_24M
#define CLK_XTAL1		24	/* M */
#define PLLDIV_RN		0x20
#endif
#ifdef CONFIG_OSC_AT8XC5122_FREQ_48M
#define CLK_XTAL1		48	/* M */
#define PLLDIV_RN		0x10
#endif
#ifdef CONFIG_CPU_AT8XC5122_X2
#define CLK_CPU			CLK_XTAL1
#else
#define CLK_CPU			(CLK_XTAL1/2)
#endif
#endif

/* CLK_PLL & CLK_PERIPH definitions */
#define CLK_PLL			96				/* M */
/* TODO: Variable CLK_PERIPH Value
 * Actual CLK_PERIPH should be diveded by tow if:
 * 1. CPU is in the suspended mode, or
 * 2. CPU is in the resumed X2 mode and the specific device is in the
 *    suspended mode.
 */
#define CLK_PERIPH		CLK_CPU				/* M */
#define PLL_FREQ		(uint32_t)(CLK_PLL*1000)	/* K */
#define CPU_FREQ		(uint32_t)(CLK_CPU*1000)	/* K */

/* PLLDIV Program Formula:
 * CLK_PLL = CLK_XTAL * ((R+1)/(N+1)) = 96
 * thus:
 * (R+1)/(N+1) = 96/CLK_XTAL
 */
#define clk_hw_enable_pll()			\
	do {					\
		PLLDIV = PLLDIV_RN;		\
		PLLCON = _BV(PLLEN);		\
		while (!(PLLCON & _BV(PLOCK)));	\
	} while (0)

/* PR1 Prescaler Clock Reload Formula:
 * CLK_CPU = (1/(2*(16-CKRL))) * CLK_PR1
 * thus:
 * CLRL = 16 - (CLK_PR1 / (CLK_CPU*2))
 * where:
 * CLK_PR1 is CLK_PLL for PLL fed systems, or
 * CLK_PR1 is CLK_XTAL1 for on-chip oscillator fed systems.
 */
#define clk_hw_config_cpu_x1(ck_cpu)			\
	do {						\
		CKCON0 = 0;				\
		CKCON1 = 0;				\
		CKRL = 16-(CLK_PR1/((ck_cpu)<<1));	\
	} while (0)
#define clk_hw_config_cpu_x2()				\
	do {						\
		clk_hw_config_cpu_x1(CLK_CPU);		\
		CKCON0 |= _BV(X2);			\
	} while (0)

#ifdef CONFIG_CPU_AT8XC5122_X2
#define clk_hw_resume_cpu()		clk_hw_config_cpu_x2()
#define clk_hw_suspend_cpu()		clk_hw_config_cpu_x1(CLK_CPU)
#else
#define clk_hw_resume_cpu()		clk_hw_config_cpu_x1(CLK_CPU)
#define clk_hw_suspend_cpu()		clk_hw_config_cpu_x1(CLK_CPU/2)
#endif

#if defined(CONFIG_CPU_AT8XC5122_X2)
void clk_hw_resume_dev(uint8_t dev);
void clk_hw_suspend_dev(uint8_t dev);
#else
#define clk_hw_resume_dev(dev)
#define clk_hw_suspend_dev(dev)
#endif

#ifdef CONFIG_CPU_AT8XC5122_PLL
#define CLK_PR1				CLK_PLL
#define clk_hw_enable_cpu()		\
	do {				\
		CKSEL |= _BV(CKS);	\
		clk_hw_resume_cpu();	\
	} while (0)
#define clk_hw_enable_usb()		(PLLCON &= ~_BV(EXT48))
#endif
#ifdef CONFIG_CPU_AT8XC5122_OSC
#define CLK_PR1				CLK_XTAL1
#define clk_hw_enable_cpu()		\
	do {				\
		CKSEL &= ~_BV(CKS);	\
		clk_hw_resume_cpu();	\
	} while (0)
#define clk_hw_enable_usb()		(PLLCON |= _BV(EXT48))
#endif

#endif /* __CLK_AT8XC5122_H_INCLUDE__ */
