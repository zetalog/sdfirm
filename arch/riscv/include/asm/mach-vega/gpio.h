/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)gpio.h: RV32M1 (VEGA) specific GPIO pin defintions
 * $Id: gpio.h,v 1.1 2019-08-14 15:59:00 zhenglv Exp $
 */

#ifndef __GPIO_VEGA_H_INCLUDE__
#define __GPIO_VEGA_H_INCLUDE__

#include <target/arch.h>

#ifdef CONFIG_GPIO
#ifndef ARCH_HAVE_GPIO
#define ARCH_HAVE_GPIO		1
#else
#error "Multiple GPIO controller defined"
#endif
#endif

#ifdef CONFIG_VEGA_RI5CY
#define GPIO_PORTA		0
#define GPIO_PORTB		1
#define GPIO_PORTC		2
#define GPIO_PORTD		3
#endif
#ifdef CONFIG_VEGA_0RISCY
#define GPIO_PORTE		4
#endif

/* Chapter 25 General-Purpose Input/Output (GPIO) */
#ifdef CONFIG_VEGA_RI5CY
#define GPIOA_BASE		UL(0x48020000)
#define GPIOB_BASE		UL(0x48020040)
#define GPIOC_BASE		UL(0x48020080)
#define GPIOD_BASE		UL(0x480200C0)
#define GPIO_BASE(port)		((GPIOA_BASE + (port) * 0x40))
#endif
#ifdef CONFIG_VEGA_0RISCY
#define GPIOE_BASE		UL(0x4100F000)
#define GPIO_BASE(port)		((GPIOE_BASE + ((port) - 4) * 0x40))
#endif
#define GPIO_REG(port, offset)	((caddr_t)(GPIO_BASE(port)+(offset)))

#define PDOR(port)		GPIO_REG(port, 0x00)
#define PSOR(port)		GPIO_REG(port, 0x04)
#define PCOR(port)		GPIO_REG(port, 0x08)
#define PTOR(port)		GPIO_REG(port, 0x0C)
#define PDIR(port)		GPIO_REG(port, 0x10)
#define PDDR(port)		GPIO_REG(port, 0x14)

#define gpio_get_output(port, pin)		\
	((__raw_readl(PDOR(port)) >> (pin)) & 1)
#define gpio_write_output(port, pin, val)	\
	((val) ?				\
	 __raw_setl(_BV(pin), PDOR(port)) :	\
	 __raw_clearl(_BV(pin), PDOR(port)))
#define gpio_set_output(port, pin)		\
	__raw_writel(_BV(pin), PSOR(port))
#define gpio_clear_output(port, pin)		\
	__raw_writel(_BV(pin), PCOR(port))
#define gpio_toggle_output(port, pin)		\
	__raw_writel(_BV(pin), PTOR(port))
#define gpio_get_input(port, pin)		\
	((__raw_readl(PDIR(port)) >> (pin)) & 1)
#define gpio_read_input(port, pin)		\
	gpio_get_input(port, pin)
#define gpio_direct_input(port, pin)		\
	__raw_clearl(_BV(pin), PDDR(port))
#define gpio_direct_output(port, pin)		\
	__raw_setl(_BV(pin), PDDR(port))
#define gpio_is_output(port, pin)		\
	((__raw_readl(PDDR(port)) >> (pin)) & 1)

/* Chapter 24 Port Control and Interrupts (PORT) */
#ifdef CONFIG_VEGA_RI5CY
#define PORTA_BASE		UL(0x40046000)
#define PORTB_BASE		UL(0x40047000)
#define PORTC_BASE		UL(0x40048000)
#define PORTD_BASE		UL(0x40049000)
#define PORT_BASE(port)		(PORTA_BASE + (port) * 0x1000)
#endif
#ifdef CONFIG_VEGA_0RISCY
#define PORTE_BASE		UL(0x41037000)
#define PORT_BASE(port)		((PORTE_BASE + ((port) - 4) * 0x1000))
#endif
#define PORT_REG(port, offset)	((caddr_t)(PORT_BASE(port)+(offset)))

#define PCR(port, pin)		PORT_REG(port, 4 * (pin))
#define GPCLR(port)		PORT_REG(port, 0x80)
#define GPCHR(port)		PORT_REG(port, 0x84)
#define GICLR(port)		PORT_REG(port, 0x88)
#define GICHR(port)		PORT_REG(port, 0x8C)
#define IFSR(port)		PORT_REG(port, 0xA0)

#define PCR_ISF			_BV(24)
#define PCR_IRQC_OFFSET		16
#define PCR_IRQC_MASK		REG_4BIT_MASK
#define PCR_IRQC(value)		_SET_FV(PCR_IRQC, value)
#define PCR_IRQC_DISABLED	0
/* ISF DMA request */
#define PCR_IRQC_DMA_HIGH	1
#define PCR_IRQC_DMA_LOW	2
#define PCR_IRQC_DMA_BOTH	3
/* IRQC flags */
#define PCR_IRQC_FLAG_HIGH	5
#define PCR_IRQC_FLAG_LOW	6
#define PCR_IRQC_FLAG_BOTH	7
/* ISF IRQ */
#define PCR_IRQC_IRQ_LEVEL_LOW	8
#define PCR_IRQC_IRQ_EDGE_HIGH	9
#define PCR_IRQC_IRQ_EDGE_LOW	10
#define PCR_IRQC_IRQ_EDGE_BOTH	11
#define PCR_IRQC_IRQ_LEVEL_HIGH	12
/* IRQ trigger */
#define PCR_IRQC_ACTIVE_HIGH	13
#define PCR_IRQC_ACTIVE_LOW	14
#define PCR_LK			_BV(15)
#define PCR_MUX_OFFSET		8
#define PCR_MUX_MASK		REG_3BIT_MASK
#define PCR_MUX(value)		_SET_FV(PCR_MUX, value)

#define PCR_MUX_DISABLED	0
#define PCR_MUX_ANALOG		0
#define PCR_MUX_GPIO		1

/* 23.3.1 RV32M1 Subfamily Pinout (176 VFBGA) */
/* PTA0: PIN B10 */
#define PTA0_MUX_DEFAULT		PTA0_MUX_NMI_b
/* PTA1: PIN E12 */
#define PTA1_MUX_DEFAULT		PTA1_MUX_JTAG_TCLK
/* PTA2: PIN F11 */
#define PTA2_MUX_DEFAULT		PTA2_MUX_JTAG_TDI
/* PTA3: PIN D11 */
#define PTA3_MUX_DEFAULT		PTA3_MUX_JTAG_TDO
/* PTA4: PIN B9 */
#define PTA4_MUX_DEFAULT		PTA4_MUX_JTAG_TMS
/* PTA9: PIN K9 */
#define PTA9_MUX_DEFAULT		PTA9_MUX_RV_JTAG_TCLK
/* PTA10: PIN A9 */
#define PTA10_MUX_DEFAULT		PTA10_MUX_RV_JTAG_TDI
/* PTA14: PIN E8 */
#define PTA14_MUX_DEFAULT		PTA14_MUX_RV_JTAG_TDO
/* PTA15: PIN A7 */
#define PTA15_MUX_DEFAULT		PTA15_MUX_RV_JTAG_TMS
/* PTA17: PIN F7 */
#define PTA17_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA18: PIN D8 */
#define PTA18_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA19: PIN D7 */
#define PTA19_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA20: PIN C7 */
#define PTA20_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA21: PIN B7 */
#define PTA21_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA22: PIN B6 */
#define PTA22_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA23: PIN E6 */
#define PTA23_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA24: PIN D6 */
#define PTA24_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA25: PIN B5 */
#define PTA25_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA26: PIN A5 */
#define PTA26_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA27: PIN A3 */
#define PTA27_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA28: PIN A2 */
#define PTA28_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA30: PIN A1, PIN B2? */
#define PTA30_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTA31: PIN C4 */
#define PTA31_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB0: PIN B3 */
#define PTB0_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB1: PIN C3 */
#define PTB1_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB2: PIN B1 */
#define PTB2_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB3: PIN C1 */
#define PTB3_MUX_DEFAULT		PTB3_MUX_LPADC0_SE0
/* PTB4: PIN C2 */
#define PTB4_MUX_DEFAULT		PTB4_MUX_LPADC0_SE1
/* PTB5: PIN D2 */
#define PTB5_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB6: PIN E1 */
#define PTB6_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB7: PIN E2 */
#define PTB7_MUX_DEFAULT		PTB7_MUX_LPADC0_SE2
/* PTB8: PIN F5 */
#define PTB8_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB9: PIN F4 */
#define PTB9_MUX_DEFAULT		PCR_MUX_LPADC0_SE3
/* PTB11: PIN G6 */
#define PTB11_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB12: PIN G4 */
#define PTB12_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB13: PIN G3 */
#define PTB13_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB14: PIN G2 */
#define PTB14_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB15: PIN G1 */
#define PTB15_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB16: PIN H5 */
#define PTB16_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB17: PIN K5 */
#define PTB17_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB18: PIN H2 */
#define PTB18_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB19: PIN K4 */
#define PTB19_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB20: PIN J1 */
#define PTB20_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB21: PIN J2 */
#define PTB21_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB22: PIN L1 */
#define PTB22_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB24: PIN L2 */
#define PTB24_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB25: PIN L6 */
#define PTB25_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB26: PIN L4 */
#define PTB26_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB28: PIN M4 */
#define PTB28_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB29: PIN L3 */
#define PTB29_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB30: PIN M5 */
#define PTB30_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTB31: PIN M7 */
#define PTB31_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTC0: PIN N1 */
#define PTC0_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTC1: PIN M2 */
#define PTC1_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTC7: PIN N2 */
#define PTC7_MUX_DEFAULT		PTC7_MUX_LPCMP0_IN0
/* PTC8: PIN P3 */
#define PTC8_MUX_DEFAULT		PTC8_MUX_LPCMP0_IN1
/* PTC9: PIN R1 */
#define PTC9_MUX_DEFAULT		PTC9_MUX_LPADC0_SE4
/* PTC10: PIN R2 */
#define PTC10_MUX_DEFAULT		PTC10_MUX_LPADC0_SE5
/* PTC11: PIN T1 */
#define PTC11_MUX_DEFAULT		PTC11_MUX_LPADC0_SE6
/* PTC12: PIN R3 */
#define PTC12_MUX_DEFAULT		PTC12_MUX_LPADC0_SE7
/* PTC27: PIN P6 */
#define PTC27_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTC28: PIN U5 */
#define PTC28_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTC29: PIN N6 */
#define PTC29_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTC30: PIN R7 */
#define PTC30_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTD0: PIN T7 */
#define PTD0_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTD1: PIN P7 */
#define PTD1_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTD2: PIN U7 */
#define PTD2_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTD3: PIN T8 */
#define PTD3_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTD4: PIN N8 */
#define PTD4_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTD5: PIN N10 */
#define PTD5_MUX_DEFAULT		PTD5_MUX_LPADC0_SE8
/* PTD6: PIN U9 */
#define PTD6_MUX_DEFAULT		PTD6_MUX_LPADC0_SE9
/* PTD7: PIN P10 */
#define PTD7_MUX_DEFAULT		PTD7_MUX_LPADC0_SE10
/* PTD8: PIN T9 */
#define PTD8_MUX_DEFAULT		PTD8_MUX_LPADC0_SE11
/* PTD9: PIN U11 */
#define PTD9_MUX_DEFAULT		PTD9_MUX_LPADC0_SE12
/* PTD10: PIN P11 */
#define PTD10_MUX_DEFAULT		PTD10_MUX_LPADC0_SE13
/* PTD11: PIN R11 */
#define PTD11_MUX_DEFAULT		PTD11_MUX_LPADC0_SE14
/* PTE0: PIN R14 */
#define PTE0_MUX_DEFAULT		PTE0_MUX_LPCMP1_IN4
/* PTE1: PIN R16 */
#define PTE1_MUX_DEFAULT		PTE1_MUX_LPADC0_SE18
/* PTE2: PIN P12 */
#define PTE2_MUX_DEFAULT		PTE2_MUX_LPADC0_SE19
/* PTE3: PIN N12 */
#define PTE3_MUX_DEFAULT		PTE3_MUX_LPADC0_SE20
/* PTE4: PIN M11 */
#define PTE4_MUX_DEFAULT		PTE4_MUX_LPADC0_SE21
/* PTE5: PIN R17 */
#define PTE5_MUX_DEFAULT		PTE5_MUX_LPCMP1_IN2
/* PTE8: PIN P16 */
#define PTE8_MUX_DEFAULT		PTE8_MUX_LPADC0_SE22
/* PTE9: PIN N16 */
#define PTE9_MUX_DEFAULT		PTE9_MUX_LPADC0_SE23
/* PTE10: PIN M13 */
#define PTE10_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE11: PIN M14 */
#define PTE11_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE12: PIN L12 */
#define PTE12_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE13: PIN N17 */
#define PTE13_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE14: PIN L16 */
#define PTE14_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE15: PIN L17 */
#define PTE15_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE16: PIN L14 */
#define PTE16_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE17: PIN L15 */
#define PTE17_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE18: PIN K13 */
#define PTE18_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE19: PIN K16 */
#define PTE19_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE21: PIN J17 */
#define PTE21_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE22: PIN J16 */
#define PTE22_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE27: PIN H14 */
#define PTE27_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE28: PIN G14 */
#define PTE28_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE29: PIN G15 */
#define PTE29_MUX_DEFAULT		PCR_MUX_DISABLED
/* PTE30: PIN G17 */
#define PTE30_MUX_DEFAULT		PCR_MUX_DISABLED

/* NMI */
#define PTA0_MUX_NMI_b			7

/* CM JTAG */
#define PTA1_MUX_JTAG_TCLK		7
#define PTA2_MUX_JTAG_TDI		7
#define PTA3_MUX_JTAG_TDO		7
#define PTA4_MUX_JTAG_TMS		7

/* SWD */
#define PTA1_MUX_SWD_CLK		7
#define PTA3_MUX_SWD_SWO		7
#define PTA4_MUX_SWD_DIO		7

/* RV JTAG */
#define PTA9_MUX_RV_JTAG_TCLK		7
#define PTA10_MUX_RV_JTAG_TDI		7
#define PTA14_MUX_RV_JTAG_TDO		7
#define PTA15_MUX_RV_JTAG_TMS		7

/* CLKOUT */
#define PTB0_MUX_CLKOUT			5
#define PTD11_MUX_CLKOUT		5
#define PTE4_MUX_CLKOUT			5

/* TRACE */
#define PTD6_MUX_TRACE_D3		5
#define PTD7_MUX_TRACE_D2		5
#define PTD8_MUX_TRACE_D1		5
#define PTD9_MUX_TRACE_D0		5
#define PTD10_MUX_TRACE_CLKOUT		5

/* LPTMR0 */
#define PTB8_MUX_LPTMR0_ALT1		7
#define PTC9_MUX_LPTMR0_ALT2		7
#define PTE3_MUX_LPTMR0_ALT3		7

/* LPTMR1 */
#define PTC7_MUX_LPTMR1_ALT1		7
#define PTE1_MUX_LPTMR1_ALT2		7
#define PTB1_MUX_LPTMR1_ALT3		7

/* LPTMR2 */
#define PTE8_MUX_LPTMR2_ALT1		7
#define PTA30_MUX_LPTMR2_ALT2		7
#define PTE10_MUX_LPTMR2_ALT3		7

/* SPM */
#define PTB9_MUX_SPM_LPREQ		PCR_MUX_GPIO

/* RF */
#define PTA1_MUX_RF_ANT_B		PCR_MUX_GPIO
#define PTA2_MUX_RF_ANT_A		PCR_MUX_GPIO
#define PTA3_MUX_RF0_TX_SWITCH		PCR_MUX_GPIO
#define PTA4_MUX_RF0_RX_SWITCH		PCR_MUX_GPIO
#define PTB2_MUX_RF0_RFOSC_EN		PCR_MUX_GPIO
#define PTB3_MUX_RF0_EXT_OSC_EN		PCR_MUX_GPIO
#define PTB4_MUX_RF0_RF_OFF		PCR_MUX_GPIO
#define PTB4_MUX_RF0_DFT_RESET		PCR_MUX_GPIO
#define PTB5_MUX_RF0_ACTIVE		PCR_MUX_GPIO
#define PTB6_MUX_RF0_BSM_FRAME		7
#define PTB7_MUX_RF0_BSM_DATA		7
#define PTB8_MUX_RF0_EARLY_WARNING	PCR_MUX_GPIO
#define PTB26_MUX_RF0_BSM_CLK		7
#define PTE4_MUX_RF0_DTM_RX		7
#define PTE5_MUX_RF0_DTM_TX		7

/* EMVSIM */
#define PTA17_MUX_EMVSIM0_CLK		4
#define PTA18_MUX_EMVSIM0_RST		4
#define PTA19_MUX_EMVSIM0_VCCEN		4
#define PTA20_MUX_EMVSIM0_IO		4
#define PTA21_MUX_EMVSIM0_PD		4

#define PTD3_MUX_EMVSIM0_CLK		4
#define PTD4_MUX_EMVSIM0_RST		4
#define PTD5_MUX_EMVSIM0_VCCEN		4
#define PTD6_MUX_EMVSIM0_IO		4
#define PTD7_MUX_EMVSIM0_PD		4

/* USB */
#define PTB0_MUX_USB0_SOF_OUT		4
#define PTB26_MUX_USB0_SOF_OUT		2
#define PTD11_MUX_USB0_SOF_OUT		3
#define PTE21_MUX_USB0_SOF_OUT		3

/* SDHC */
#define PTD2_MUX_SDHC0_D7		2
#define PTD3_MUX_SDHC0_D6		2
#define PTD4_MUX_SDHC0_D5		2
#define PTD5_MUX_SDHC0_D4		2
#define PTD6_MUX_SDHC0_D1		2
#define PTD7_MUX_SDHC0_D0		2
#define PTD8_MUX_SDHC0_DCLK		2
#define PTD9_MUX_SDHC0_CMD		2
#define PTD10_MUX_SDHC0_D3		2
#define PTD11_MUX_SDHC0_D2		2

#define PTE1_MUX_SDHC0_D1		2
#define PTE2_MUX_SDHC0_D0		2
#define PTE3_MUX_SDHC0_D7		2
#define PTE4_MUX_SDHC0_D6		2
#define PTE5_MUX_SDHC0_DCLK		2
#define PTE8_MUX_SDHC0_D5		2
#define PTE9_MUX_SDHC0_CMD		2
#define PTE10_MUX_SDHC0_D4		2
#define PTE11_MUX_SDHC0_D3		2
#define PTE12_MUX_SDHC0_D2		2

/* LPUART0 */
#define PTA1_MUX_LPUART0_CTS		2
#define PTA2_MUX_LPUART0_RX		2
#define PTA3_MUX_LPUART0_TX		2
#define PTA4_MUX_LPUART0_RTS		2

#define PTB22_MUX_LPUART0_CTS		3
#define PTB24_MUX_LPUART0_RTS		3
#define PTB25_MUX_LPUART0_RX		3
#define PTB26_MUX_LPUART0_TX		3

#define PTC7_MUX_LPUART0_RX		3
#define PTC8_MUX_LPUART0_TX		3
#define PTC9_MUX_LPUART0_CTS		3
#define PTC10_MUX_LPUART0_RTS		3

/* LPUART1 */
#define PTA1_MUX_LPUART1_CTS		4
#define PTA2_MUX_LPUART1_RX		4
#define PTA3_MUX_LPUART1_TX		4
#define PTA4_MUX_LPUART1_RTS		4

#define PTA25_MUX_LPUART1_RX		2
#define PTA26_MUX_LPUART1_TX		2
#define PTA27_MUX_LPUART1_CTS		2
#define PTA28_MUX_LPUART1_RTS		2

#define PTB2_MUX_LPUART1_RX		3
#define PTB3_MUX_LPUART1_TX		3
#define PTB4_MUX_LPUART1_CTS		3
#define PTB5_MUX_LPUART1_RTS		3

#define PTC29_MUX_LPUART1_RX		2
#define PTC30_MUX_LPUART1_TX		2
#define PTD0_MUX_LPUART1_CTS		2
#define PTD1_MUX_LPUART1_RTS		2

/* LPUART2 */
#define PTA30_MUX_LPUART2_CTS		2
#define PTA31_MUX_LPUART2_RTS		2
#define PTB0_MUX_LPUART2_TX		2
#define PTB1_MUX_LPUART2_RX		2

#define PTB11_MUX_LPUART2_RX		2
#define PTB12_MUX_LPUART2_TX		2
#define PTB13_MUX_LPUART2_CTS		2
#define PTB14_MUX_LPUART2_RTS		2

#define PTB18_MUX_LPUART2_RX		3
#define PTB19_MUX_LPUART2_TX		3
#define PTB20_MUX_LPUART2_CTS		3
#define PTB21_MUX_LPUART2_RTS		3

/* LPUART3 */
#define PTB16_MUX_LPUART3_CTS		3
#define PTB17_MUX_LPUART3_RTS		3
#define PTB28_MUX_LPUART3_RX		3
#define PTB29_MUX_LPUART3_TX		3

#define PTE8_MUX_LPUART3_RX		3
#define PTE9_MUX_LPUART3_TX		3
#define PTE10_MUX_LPUART3_CTS		3
#define PTE11_MUX_LPUART3_RTS		3

#define PTE27_MUX_LPUART3_CTS		2
#define PTE28_MUX_LPUART3_RTS		2
#define PTE29_MUX_LPUART3_RX		2
#define PTE30_MUX_LPUART3_TX		2

/* LPI2C0 */
#define PTA1_MUX_LPI2C0_SDAS		3
#define PTA2_MUX_LPI2C0_SDA		3
#define PTA3_MUX_LPI2C0_SCL		3
#define PTA4_MUX_LPI2C0_SCLS		3

#define PTB11_MUX_LPI2C0_SDA		4
#define PTB12_MUX_LPI2C0_SCL		4
#define PTB13_MUX_LPI2C0_SDAS		4
#define PTB14_MUX_LPI2C0_SCLS		4

#define PTC8_MUX_LPI2C0_HREQ		4
#define PTC9_MUX_LPI2C0_SDA		4
#define PTC10_MUX_LPI2C0_SCL		4
#define PTC11_MUX_LPI2C0_SDAS		4
#define PTC12_MUX_LPI2C0_SCLS		4

#define PTE1_MUX_LPI2C0_SDAS		3
#define PTE2_MUX_LPI2C0_SCLS		3
#define PTE3_MUX_LPI2C0_SDA		3
#define PTE4_MUX_LPI2C0_SCL		3
#define PTE5_MUX_LPI2C0_HREQ		3

/* LPI2C1 */
#define PTB6_MUX_LPI2C1_SDA		3
#define PTB7_MUX_LPI2C1_SDAS		3
#define PTB8_MUX_LPI2C1_SCLS		3
#define PTB9_MUX_LPI2C1_SCL		3

#define PTB11_MUX_LPI2C1_SDAS		3
#define PTB12_MUX_LPI2C1_SCLS		3
#define PTB13_MUX_LPI2C1_SDA		3
#define PTB14_MUX_LPI2C1_SCL		3
#define PTB15_MUX_LPI2C1_HREQ		3

#define PTC7_MUX_LPI2C1_HREQ		4
#define PTC11_MUX_LPI2C1_SDA		3
#define PTC12_MUX_LPI2C1_SCL		3

#define PTD8_MUX_LPI2C1_SDAS		4
#define PTD9_MUX_LPI2C1_SCLS		4
#define PTD10_MUX_LPI2C1_SDA		4
#define PTD11_MUX_LPI2C1_SCL		4

/* LPI2C2 */
#define PTA9_MUX_LPI2C2_SDAS		2
#define PTA10_MUX_LPI2C2_SCLS		2
#define PTA14_MUX_LPI2C2_SDA		2
#define PTA15_MUX_LPI2C2_SCL		2
#define PTA17_MUX_LPI2C2_HREQ		2

#define PTA22_MUX_LPI2C2_HREQ		4
#define PTA23_MUX_LPI2C2_SDA		4
#define PTA24_MUX_LPI2C2_SCL		4
#define PTA25_MUX_LPI2C2_SDAS		4
#define PTA26_MUX_LPI2C2_SCLS		4

#define PTB21_MUX_LPI2C2_HREQ		4
#define PTB22_MUX_LPI2C2_SDA		4
#define PTB24_MUX_LPI2C2_SCL		4
#define PTB25_MUX_LPI2C2_SDAS		4
#define PTB26_MUX_LPI2C2_SCLS		4

/* LPI2C3 */
#define PTB15_MUX_LPI2C3_SCL		4
#define PTB16_MUX_LPI2C3_SDA		4
#define PTB17_MUX_LPI2C3_SCLS		4
#define PTB18_MUX_LPI2C3_SDAS		4

#define PTE10_MUX_LPI2C3_SDA		4
#define PTE11_MUX_LPI2C3_SCL		4
#define PTE12_MUX_LPI2C3_SDAS		4
#define PTE13_MUX_LPI2C3_SCLS		4
#define PTE14_MUX_LPI2C3_HREQ		4

#define PTE22_MUX_LPI2C3_HREQ		3
#define PTE27_MUX_LPI2C3_SDAS		3
#define PTE28_MUX_LPI2C3_SCLS		3
#define PTE29_MUX_LPI2C3_SDA		3
#define PTE30_MUX_LPI2C3_SCL		3

/* LPSPI0 */
#define PTB2_MUX_LPSPI0_PCS1		2
#define PTB3_MUX_LPSPI0_PCS3		2
#define PTB4_MUX_LPSPI0_SCK		2
#define PTB5_MUX_LPSPI0_SOUT		2
#define PTB6_MUX_LPSPI0_PCS2		2
#define PTB7_MUX_LPSPI0_SIN		2
#define PTB8_MUX_LPSPI0_PCS0		2

#define PTB9_MUX_LPSPI0_PCS1		2
#define PTC7_MUX_LPSPI0_PCS3		2
#define PTC8_MUX_LPSPI0_SCK		2
#define PTC9_MUX_LPSPI0_SOUT		2
#define PTC10_MUX_LPSPI0_PCS2		2
#define PTC11_MUX_LPSPI0_SIN		2
#define PTC12_MUX_LPSPI0_PCS0		2

#define PTC28_MUX_LPSPI0_PCS1		3
#define PTC29_MUX_LPSPI0_PCS3		3
#define PTC30_MUX_LPSPI0_SCK		3
#define PTD0_MUX_LPSPI0_SOUT		3
#define PTD1_MUX_LPSPI0_PCS2		3
#define PTD2_MUX_LPSPI0_SIN		3
#define PTD3_MUX_LPSPI0_PCS0		3

/* LPSPI1 */
#define PTA20_MUX_LPSPI1_PCS1		3
#define PTA23_MUX_LPSPI1_PCS3		3
#define PTA24_MUX_LPSPI1_SCK		3
#define PTA30_MUX_LPSPI1_SOUT		3
#define PTA31_MUX_LPSPI1_PCS2		3
#define PTB0_MUX_LPSPI1_SIN		3
#define PTB1_MUX_LPSPI1_PCS0		3
#define PTB20_MUX_LPSPI1_SCK		2

#define PTB18_MUX_LPSPI1_PCS1		2
#define PTB19_MUX_LPSPI1_PCS3		2
#define PTB21_MUX_LPSPI1_SOUT		2
#define PTB22_MUX_LPSPI1_PCS2		2
#define PTB24_MUX_LPSPI1_SIN		2
#define PTB25_MUX_LPSPI1_PCS0		2

/* LPSPI2 */
#define PTA18_MUX_LPSPI2_PCS1		2
#define PTA19_MUX_LPSPI2_PCS3		2
#define PTA20_MUX_LPSPI2_SCK		2
#define PTA21_MUX_LPSPI2_SOUT		2
#define PTA22_MUX_LPSPI2_PCS2		2
#define PTA23_MUX_LPSPI2_SIN		2
#define PTA24_MUX_LPSPI2_PCS0		2

#define PTD4_MUX_LPSPI2_PCS1		3
#define PTD5_MUX_LPSPI2_PCS3		3
#define PTD6_MUX_LPSPI2_SCK		3
#define PTD7_MUX_LPSPI2_SOUT		3
#define PTD8_MUX_LPSPI2_PCS2		3
#define PTD9_MUX_LPSPI2_SIN		3
#define PTD10_MUX_LPSPI2_PCS0		3

/* LPSPI3 */
#define PTA9_MUX_LPSPI3_SCK		3
#define PTA10_MUX_LPSPI3_SOUT		3

#define PTA17_MUX_LPSPI3_PCS1		3
#define PTA18_MUX_LPSPI3_PCS3		3
#define PTA19_MUX_LPSPI3_SCK		3
#define PTA25_MUX_LPSPI3_SOUT		3
#define PTA26_MUX_LPSPI3_PCS2		3
#define PTA27_MUX_LPSPI3_SIN		3
#define PTA28_MUX_LPSPI3_PCS0		3

#define PTE1_MUX_LPSPI3_PCS1		4
#define PTE2_MUX_LPSPI3_PCS3		4
#define PTE3_MUX_LPSPI3_SCK		4
#define PTE4_MUX_LPSPI3_SOUT		4
#define PTE5_MUX_LPSPI3_PCS2		4
#define PTE8_MUX_LPSPI3_SIN		4
#define PTE9_MUX_LPSPI3_PCS0		4

/* TPM0 */
#define PTA3_MUX_TPM0_CLKIN		6
#define PTB2_MUX_TPM0_CH0		6
#define PTB3_MUX_TPM0_CH1		6
#define PTB4_MUX_TPM0_CH2		6
#define PTB5_MUX_TPM0_CH3		6
#define PTB6_MUX_TPM0_CH4		6
#define PTB7_MUX_TPM0_CH5		6

#define PTB15_MUX_TPM0_CLKIN		6
#define PTC7_MUX_TPM0_CH0		6
#define PTC8_MUX_TPM0_CH1		6
#define PTC9_MUX_TPM0_CH2		6
#define PTC10_MUX_TPM0_CH3		6
#define PTC11_MUX_TPM0_CH4		6
#define PTC12_MUX_TPM0_CH5		6

#define PTC27_MUX_TPM0_CH4		6
#define PTC28_MUX_TPM0_CH3		6
#define PTC29_MUX_TPM0_CH2		6
#define PTC30_MUX_TPM0_CH1		6
#define PTD0_MUX_TPM0_CH0		6
#define PTE3_MUX_TPM0_CLKIN		6

/* TPM1 */
#define PTA30_MUX_TPM1_CH0		6
#define PTA31_MUX_TPM1_CH1		6
#define PTB0_MUX_TPM1_CLKIN		6

#define PTB19_MUX_TPM1_CLKIN		6
#define PTB20_MUX_TPM1_CH0		6
#define PTB21_MUX_TPM1_CH1		6

#define PTE4_MUX_TPM1_CLKIN		6
#define PTE8_MUX_TPM1_CH0		6
#define PTE9_MUX_TPM1_CH1		6

/* TPM2 */
#define PTA19_MUX_TPM2_CH5		6
#define PTA20_MUX_TPM2_CH4		6
#define PTA21_MUX_TPM2_CH3		6
#define PTA22_MUX_TPM2_CH2		6
#define PTA23_MUX_TPM2_CH1		6
#define PTA24_MUX_TPM2_CH0		6
#define PTB23_MUX_TPM2_CLKIN		6

#define PTD3_MUX_TPM2_CLKIN		6
#define PTD6_MUX_TPM2_CH5		6
#define PTD7_MUX_TPM2_CH4		6
#define PTD8_MUX_TPM2_CH3		6
#define PTD9_MUX_TPM2_CH2		6
#define PTD10_MUX_TPM2_CH1		6
#define PTD11_MUX_TPM2_CH0		6

#define PTE16_MUX_TPM2_CH0		6
#define PTE17_MUX_TPM2_CH1		6
#define PTE18_MUX_TPM2_CH2		6
#define PTE19_MUX_TPM2_CH3		6
#define PTE21_MUX_TPM2_CH4		6
#define PTE30_MUX_TPM2_CLKIN		6
#define PTE22_MUX_TPM2_CH5		6

/* TPM3 */
#define PTB12_MUX_TPM3_CLKIN		6
#define PTB13_MUX_TPM3_CH0		6
#define PTB14_MUX_TPM3_CH1		6

#define PTE10_MUX_TPM3_CH0		6
#define PTE11_MUX_TPM3_CH1		6
#define PTE12_MUX_TPM3_CLKIN		6

#define PTE13_MUX_TPM3_CH0		6
#define PTE14_MUX_TPM3_CH1		6
#define PTE15_MUX_TPM3_CLKIN		6

/* I2S */
#define PTB2_MUX_I2S0_TXD0		4
#define PTB3_MUX_I2S0_TX_FS		4
#define PTB4_MUX_I2S0_TX_BCLK		4
#define PTB5_MUX_I2S0_MCLK		4
#define PTB6_MUX_I2S0_RX_BCLK		4
#define PTB7_MUX_I2S0_RX_FS		4
#define PTB8_MUX_I2S0_RXD0		4
#define PTB9_MUX_I2S0_RXD1		4
#define PTB1_MUX_I2S0_TXD1		4

#define PTB28_MUX_I2S0_TXD0		4
#define PTB29_MUX_I2S0_TX_FS		4
#define PTB30_MUX_I2S0_TX_BCLK		4
#define PTB31_MUX_I2S0_RXD0		4
#define PTC0_MUX_I2S0_RX_FS		4
#define PTC1_MUX_I2S0_RX_BCLK		4

#define PTE13_MUX_I2S0_TX_BCLK		2
#define PTE14_MUX_I2S0_TX_FS		2
#define PTE15_MUX_I2S0_TXD0		2
#define PTE16_MUX_I2S0_RX_BCLK		2
#define PTE17_MUX_I2S0_RX_FS		2
#define PTE18_MUX_I2S0_RXD0		2
#define PTE19_MUX_I2S0_MCLK		2
#define PTE21_MUX_I2S0_TXD1		2
#define PTE22_MUX_I2S0_RXD1		2

/* LPADC */
#define PTB3_MUX_LPADC0_SE0		PCR_MUX_ANALOG
#define PTB4_MUX_LPADC0_SE1		PCR_MUX_ANALOG
#define PTB7_MUX_LPADC0_SE2		PCR_MUX_ANALOG
#define PTB9_MUX_LPADC0_SE3		PCR_MUX_ANALOG
#define PTC9_MUX_LPADC0_SE4		PCR_MUX_ANALOG
#define PTC10_MUX_LPADC0_SE5		PCR_MUX_ANALOG
#define PTC11_MUX_LPADC0_SE6		PCR_MUX_ANALOG
#define PTC12_MUX_LPADC0_SE7		PCR_MUX_ANALOG
#define PTD5_MUX_LPADC0_SE8		PCR_MUX_ANALOG
#define PTD6_MUX_LPADC0_SE9		PCR_MUX_ANALOG
#define PTD7_MUX_LPADC0_SE10		PCR_MUX_ANALOG
#define PTD8_MUX_LPADC0_SE11		PCR_MUX_ANALOG
#define PTD9_MUX_LPADC0_SE12		PCR_MUX_ANALOG
#define PTD10_MUX_LPADC0_SE13		PCR_MUX_ANALOG
#define PTD11_MUX_LPADC0_SE14		PCR_MUX_ANALOG
#define PTE1_MUX_LPADC0_SE18		PCR_MUX_ANALOG
#define PTE2_MUX_LPADC0_SE19		PCR_MUX_ANALOG
#define PTE3_MUX_LPADC0_SE20		PCR_MUX_ANALOG
#define PTE4_MUX_LPADC0_SE21		PCR_MUX_ANALOG
#define PTE8_MUX_LPADC0_SE22		PCR_MUX_ANALOG
#define PTE9_MUX_LPADC0_SE23		PCR_MUX_ANALOG

/* LPCMP0 */
#define PTA4_MUX_LPCMP0_OUT		6
#define PTA14_MUX_LPCMP0_OUT		6
#define PTB26_MUX_LPCMP0_OUT		6
#define PTC7_MUX_LPCMP0_IN0		PCR_MUX_ANALOG
#define PTC8_MUX_LPCMP0_IN1		PCR_MUX_ANALOG
#define PTC9_MUX_LPCMP0_IN2		PCR_MUX_ANALOG

/* LPCMP1 */
#define PTE0_MUX_LPCMP1_IN4		PCR_MUX_ANALOG
#define PTE2_MUX_LPCMP1_OUT		6
#define PTE3_MUX_LPCMP1_IN0		PCR_MUX_ANALOG
#define PTE4_MUX_LPCMP1_IN1		PCR_MUX_ANALOG
#define PTE5_MUX_LPCMP1_IN2		PCR_MUX_ANALOG
#define PTE5_MUX_LPCMP1_OUT		6

/* EWM */
#define PTB24_MUX_EWM_IN		6
#define PTB25_MUX_EWM_OUT_b		6
#define PTC11_MUX_EWM_IN		7
#define PTC12_MUX_EWM_OUT_b		7
#define PTD1_MUX_EWM_IN			6
#define PTD3_MUX_EWM_OUT_b		6
#define PTE0_MUX_EWM_IN			6
#define PTE1_MUX_EWM_OUT_b		6

/* LLWU */
#define PTA22_MUX_LLWU_P2		PCR_MUX_GPIO
#define PTA30_MUX_LLWU_P3		PCR_MUX_GPIO
#define PTA1_MUX_LLWU_P0		PCR_MUX_GPIO
#define PTA2_MUX_LLWU_P1		PCR_MUX_GPIO
#define PTB1_MUX_LLWU_P4		PCR_MUX_GPIO
#define PTB2_MUX_LLWU_P5		PCR_MUX_GPIO
#define PTB4_MUX_LLWU_P6		PCR_MUX_GPIO
#define PTB6_MUX_LLWU_P7		PCR_MUX_GPIO
#define PTB7_MUX_LLWU_P8		PCR_MUX_GPIO
#define PTB8_MUX_LLWU_P9		PCR_MUX_GPIO
#define PTB16_MUX_LLWU_P10		PCR_MUX_GPIO
#define PTB20_MUX_LLWU_P11		PCR_MUX_GPIO
#define PTB22_MUX_LLWU_P12		PCR_MUX_GPIO
#define PTB25_MUX_LLWU_P13		PCR_MUX_GPIO
#define PTB28_MUX_LLWU_P14		PCR_MUX_GPIO
#define PTC7_MUX_LLWU_P15		PCR_MUX_GPIO
#define PTC9_MUX_LLWU_P16		PCR_MUX_GPIO
#define PTC11_MUX_LLWU_P17		PCR_MUX_GPIO
#define PTC12_MUX_LLWU_P18		PCR_MUX_GPIO
#define PTD8_MUX_LLWU_P19		PCR_MUX_GPIO
#define PTD10_MUX_LLWU_P20		PCR_MUX_GPIO
#define PTE1_MUX_LLWU_P21		PCR_MUX_GPIO
#define PTE3_MUX_LLWU_P22		PCR_MUX_GPIO
#define PTE8_MUX_LLWU_P23		PCR_MUX_GPIO
#define PTE9_MUX_LLWU_P24		PCR_MUX_GPIO
#define PTE10_MUX_LLWU_P25		PCR_MUX_GPIO
#define PTE12_MUX_LLWU_P26		PCR_MUX_GPIO

/* FB */
#define PTA9_MUX_FB_A23			5
#define PTA10_MUX_FB_A22		5
#define PTA14_MUX_FB_AD23		5
#define PTA15_MUX_FB_AD22		5
#define PTA17_MUX_FB_AD21		5
#define PTA18_MUX_FB_AD20		5
#define PTA19_MUX_FB_AD19		5
#define PTA20_MUX_FB_AD18		5
#define PTA21_MUX_FB_AD17		5
#define PTA22_MUX_FB_AD16		5
#define PTA23_MUX_FB_AD15		5
#define PTA24_MUX_FB_OE_b		5
#define PTA25_MUX_FB_AD31		5
#define PTA26_MUX_FB_AD30		5
#define PTA27_MUX_FB_AD29		5
#define PTA28_MUX_FB_AD28		5
#define PTA30_MUX_FB_AD14		5
#define PTA31_MUX_FB_AD13		5
#define PTB1_MUX_FB_AD12		5
#define PTB2_MUX_FB_AD11		5
#define PTB3_MUX_FB_AD10		5
#define PTB4_MUX_FB_AD9			5
#define PTB5_MUX_FB_AD8			5
#define PTB6_MUX_FB_AD7			5
#define PTB7_MUX_FB_AD6			5
#define PTB8_MUX_FB_AD5			5
#define PTB9_MUX_FB_RW_b		5
#define PTB11_MUX_FB_AD27		5
#define PTB12_MUX_FB_AD26		5
#define PTB13_MUX_FB_AD25		5
#define PTB14_MUX_FB_AD24		5
#define PTB15_MUX_FB_CS5_b		5
#define PTB15_MUX_FB_TSIZ1		5
#define PTB15_MUX_FB_BE23_16_b		5
#define PTB16_MUX_FB_CS4_b		5
#define PTB16_MUX_FB_TSIZ0		5
#define PTB16_MUX_FB_BE31_24_b		5
#define PTB17_MUX_FB_TBST_b		5
#define PTB17_MUX_FB_CS2_b		5
#define PTB17_MUX_FB_BE15_8_b		5
#define PTB18_MUX_FB_CS3_b		5
#define PTB18_MUX_FB_BE7_0_b		5
#define PTB18_MUX_FB_TA_b		6
#define PTB19_MUX_FB_ALE		5
#define PTB19_MUX_FB_CS1_b		5
#define PTB19_MUX_FB_TS_b		5
#define PTB20_MUX_FB_CS0_b		5
#define PTB21_MUX_FB_AD4		5
#define PTB22_MUX_FB_AD3		5
#define PTB24_MUX_FB_AD2		5
#define PTB25_MUX_FB_AD1		5
#define PTB26_MUX_FB_AD0		5
#define PTB28_MUX_FB_A16		5
#define PTB29_MUX_FB_A17		5
#define PTB30_MUX_FB_A18		5
#define PTB31_MUX_FB_A19		5
#define PTC0_MUX_FB_A20			5
#define PTC1_MUX_FB_A21			5

/* FXIO */
#define PTB9_MUX_FXIO0_D0		7
#define PTB11_MUX_FXIO0_D1		7
#define PTB12_MUX_FXIO0_D2 		7
#define PTB13_MUX_FXIO0_D3		7
#define PTB14_MUX_FXIO0_D4		7
#define PTB15_MUX_FXIO0_D5		7
#define PTB16_MUX_FXIO0_D6		7
#define PTB17_MUX_FXIO0_D7		7
#define PTB18_MUX_FXIO0_D8		7
#define PTB19_MUX_FXIO0_D9		7
#define PTB20_MUX_FXIO0_D10		7
#define PTB21_MUX_FXIO0_D11		7
#define PTB23_MUX_FXIO0_D12		7
#define PTB24_MUX_FXIO0_D13		7
#define PTB25_MUX_FXIO0_D14		7
#define PTB28_MUX_FXIO0_D15		7
#define PTB29_MUX_FXIO0_D16		7
#define PTC28_MUX_FXIO0_D17		7
#define PTC29_MUX_FXIO0_D18		7
#define PTC30_MUX_FXIO0_D19		7
#define PTD0_MUX_FXIO0_D20		7
#define PTD1_MUX_FXIO0_D21		7
#define PTD3_MUX_FXIO0_D22		7
#define PTD3_MUX_FXIO0_D23		7
#define PTD4_MUX_FXIO0_D24		7
#define PTD5_MUX_FXIO0_D25		7
#define PTD6_MUX_FXIO0_D26		7
#define PTD7_MUX_FXIO0_D27		7
#define PTD8_MUX_FXIO0_D28		7
#define PTD9_MUX_FXIO0_D29		7
#define PTD10_MUX_FXIO0_D30		7
#define PTD11_MUX_FXIO0_D31		7

#define PTE9_MUX_FXIO0_D0		7
#define PTE11_MUX_FXIO0_D1		7
#define PTE12_MUX_FXIO0_D2		7
#define PTE13_MUX_FXIO0_D3		7
#define PTE14_MUX_FXIO0_D4		7
#define PTE15_MUX_FXIO0_D5		7
#define PTE16_MUX_FXIO0_D6		7
#define PTE17_MUX_FXIO0_D7		7
#define PTE18_MUX_FXIO0_D8		7
#define PTE19_MUX_FXIO0_D9		7
#define PTE21_MUX_FXIO0_D10		7
#define PTE22_MUX_FXIO0_D11		7
#define PTE27_MUX_FXIO0_D28		7
#define PTE28_MUX_FXIO0_D29		7
#define PTE29_MUX_FXIO0_D30		7
#define PTE30_MUX_FXIO0_D31		7

#define PCR_DSE			_BV(6) /* Drive Strength Enable */
#define PCR_ODE			_BV(5) /* Open Drain Enable */
#define PCR_PFE			_BV(4) /* Passive Filter Enable */
#define PCR_SRE			_BV(2) /* Slew Rate Enable */
#define PCR_PE			_BV(1) /* Pull Enable */
#define PCR_PS			_BV(0) /* Pull Select */
#define PCR_PAD_MASK		\
	(PCR_DSE | PCR_ODE | PCR_PFE | PCR_SRE | PCR_PE | PCR_PS)

#define port_config_mux(port, pin, mux)			\
	__raw_writel_mask(PCR_MUX((mux) < 2 ?		\
			  1 : (mux)),			\
			  PCR_MUX(REG_3BIT_MASK),	\
			  PCR(port, pin))
#define port_set_irqc(port, pin, irqc)			\
	__raw_writel_mask(PCR_IRQC(irqc),		\
			  PCR_IRQC(PCR_IRQC_MASK),	\
			  PCR(port, pin))
#define port_set_isf(port, pin)			\
	__raw_setl(PCR_ISF, PCR(port, pin))
#define port_get_isf(port, pin)			\
	(!!(__raw_readl(PCR(port, pin)) & PCR_ISF))
#define port_set_lk(port, pin)			\
	__raw_setl(PCR_LK, PCR(port, pin))

/* GPIO abstraction */
#define GPIO_HW_MAX_PORTS	5
#define GPIO_HW_MAX_PINS	32

#define gpio_hw_read_pin(port, pin)		\
	(gpio_is_output(port, pin) ?		\
	 gpio_get_output(port, pin) :		\
	 gpio_get_input(port, pin))
#define gpio_hw_write_pin(port, pin, val)	\
	((val) ?				\
	 gpio_set_output(port, pin) :		\
	 gpio_clear_output(port, pin))
#define gpio_hw_config_mux(port, pin, mux)	\
	port_config_mux(port, pin, mux)
#define gpio_hw_config_pad(port, pin, pad, drv)	\
	port_config_pad(port, pin, pad, drv)
#define gpio_hw_config_irq(port, pin, mode)	\
	port_config_irq(port, pin, mode)
#define gpio_hw_enable_irq(port, pin)
#define gpio_hw_disable_irq(port, pin)		\
	port_set_irqc(port, pin, PCR_IRQC_DISABLED)
#define gpio_hw_clear_irq(port, pin)		\
	port_set_isf(port, pin)
#define gpio_hw_trigger_irq(port, pin)
#define gpio_hw_irq_status(port, pin)		\
	port_get_isf(port, pin)
#define gpio_hw_ctrl_init()		do { } while (0)

void port_config_pad(uint8_t port, uint8_t pin,
		     uint8_t pad, uint8_t drv);
void port_config_irq(uint8_t port, uint8_t pin, uint32_t mode);

#endif /* __GPIO_VEGA_H_INCLUDE__ */
