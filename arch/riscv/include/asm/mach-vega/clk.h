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
 * @(#)clk.h: RV32M1 (VEGA) specific clock tree defintions
 * $Id: clk.h,v 1.1 2019-08-14 15:59:00 zhenglv Exp $
 */

#ifndef __CLK_VEGA_H_INCLUDE__
#define __CLK_VEGA_H_INCLUDE__

#include <asm/mach/scg.h>
#include <asm/mach/pcc.h>
#include <asm/mach/msmc.h>

#define NR_FREQPLANS		3
#define FREQPLAN_RUN		0
#define FREQPLAN_VLPR		1
#define FREQPLAN_HSRUN		2
#define INVALID_FREQPLAN	NR_FREQPLANS

#define invalid_clk		clkid(0xFF, 0xFF)

/* Padring reference clocks */
/* clk_cat_t */
#define CLK_CONST		((clk_cat_t)0)
/* clk_clk_t */
#define LPOSC_CLK		((clk_clk_t)0)
#define SAI_MCLK		((clk_clk_t)1)
#define LPADCIRC_CLK		((clk_clk_t)2)
#define NR_CONST_CLKS		(LPADCIRC_CLK + 1)
#define FREQ_LPOSC_CLK		1000
#define FREQ_LPADCIRC_CLK	2000000
#define FREQ_IRC48_CLK		48000000
#define lposc_clk		clkid(CLK_CONST, LPOSC_CLK)
#define sai_mclk		clkid(CLK_CONST, SAI_MCLK)
#define lpadcirc_clk		clkid(CLK_CONST, LPADCIRC_CLK)

#define CLK_INPUT		((clk_cat_t)1)
#define SOSC_CLK		((clk_clk_t)0)
#define SIRC_CLK		((clk_clk_t)1)
#define FIRC_CLK		((clk_clk_t)2)
#define ROSC_CLK		((clk_clk_t)3)
#define LPFLL_CLK		((clk_clk_t)4)
#define NR_INPUT_CLKS		(LPFLL_CLK + 1)
#define CLK_SCG_SCS(clk)	((clk) + 1)
#define XTAL_CLK		SOSC_CLK
#define XTAL32_CLK		ROSC_CLK
#define RFOSC_CLK		SOSC_CLK
#define RTCOSC_CLK		ROSC_CLK
#define IRC48M_CLK		FIRC_CLK
#define sosc_clk		clkid(CLK_INPUT, SOSC_CLK)
#define sirc_clk		clkid(CLK_INPUT, SIRC_CLK)
#define firc_clk		clkid(CLK_INPUT, FIRC_CLK)
#define rosc_clk		clkid(CLK_INPUT, ROSC_CLK)
#define lpfll_clk		clkid(CLK_INPUT, LPFLL_CLK)
#define xtal_clk		sosc_clk
#define xtal32_clk		rosc_clk
#define rfosc_clk		sosc_clk
#define rtcosc_clk		rosc_clk
#define irc48m_clk		firc_clk

#define CLK_SYSTEM		((clk_cat_t)2)
#define SYS_CLK_SRC		((clk_clk_t)0)
#define CLKOUT_CLK		((clk_clk_t)1)
#define NR_SYSTEM_CLKS		(CLKOUT_CLK + 1)
#define CLK_SCG_CCR(clk)	SCG_REG(SCG_RCCR + 4 * (clk))
#define sys_clk_src		clkid(CLK_SYSTEM, SYS_CLK_SRC)
#define clkout_clk		clkid(CLK_SYSTEM, CLKOUT_CLK)

#define CLK_OUTPUT		((clk_cat_t)3)
/* CCR clocks */
#define SLOW_CLK		((clk_clk_t)0)
#define BUS_CLK			((clk_clk_t)1)
#define EXT_CLK			((clk_clk_t)2)
#define CORE_CLK		((clk_clk_t)3)
#define NR_OUTPUT_CCRS		4
#define CLK_CCR_DIVID(clk)	((clk) == CORE_CLK ? 4 : (clk))
/* DIV clocks */
#define SOSCDIV1_CLK		((clk_clk_t)4)
#define SOSCDIV2_CLK		((clk_clk_t)5)
#define SOSCDIV3_CLK		((clk_clk_t)6)
#define SIRCDIV1_CLK		((clk_clk_t)8)
#define SIRCDIV2_CLK		((clk_clk_t)9)
#define SIRCDIV3_CLK		((clk_clk_t)10)
#define FIRCDIV1_CLK		((clk_clk_t)12)
#define FIRCDIV2_CLK		((clk_clk_t)13)
#define FIRCDIV3_CLK		((clk_clk_t)14)
#define LPFLLDIV1_CLK		((clk_clk_t)20)
#define LPFLLDIV2_CLK		((clk_clk_t)21)
#define LPFLLDIV3_CLK		((clk_clk_t)22)
#define NR_OUTPUT_CLKS		23
#define CLK_OUTPUT_VALID	UL(0x0070F77F)
#define CLK_DIV_CLK(clk)	((clk_clk_t)(((clk) >> 2) - 1))
#define CLK_DIV_CLKID(clk)	clkid(CLK_INPUT, CLK_DIV_CLK(clk))
#define CLK_DIV_SCS(clk)	CLK_SCG_SCS(CLK_DIV_CLK(clk))
#define CLK_DIV_DIVID(clk)	((clk) & 0x03)
#define SYS_CLK			CORE_CLK
#define PLAT_CLK		CORE_CLK
#define SDHC_DCLK		BUS_CLK
#define slow_clk		clkid(CLK_OUTPUT, SLOW_CLK)
#define bus_clk			clkid(CLK_OUTPUT, BUS_CLK)
#define ext_clk			clkid(CLK_OUTPUT, EXT_CLK)
#define core_clk		clkid(CLK_OUTPUT, CORE_CLK)
#define soscdiv1_clk		clkid(CLK_OUTPUT, SOSCDIV1_CLK)
#define soscdiv2_clk		clkid(CLK_OUTPUT, SOSCDIV2_CLK)
#define soscdiv3_clk		clkid(CLK_OUTPUT, SOSCDIV3_CLK)
#define sircdiv1_clk		clkid(CLK_OUTPUT, SIRCDIV1_CLK)
#define sircdiv2_clk		clkid(CLK_OUTPUT, SIRCDIV2_CLK)
#define sircdiv3_clk		clkid(CLK_OUTPUT, SIRCDIV3_CLK)
#define fircdiv1_clk		clkid(CLK_OUTPUT, FIRCDIV1_CLK)
#define fircdiv2_clk		clkid(CLK_OUTPUT, FIRCDIV2_CLK)
#define fircdiv3_clk		clkid(CLK_OUTPUT, FIRCDIV3_CLK)
#define lpflldiv1_clk		clkid(CLK_OUTPUT, LPFLLDIV1_CLK)
#define lpflldiv2_clk		clkid(CLK_OUTPUT, LPFLLDIV2_CLK)
#define lpflldiv3_clk		clkid(CLK_OUTPUT, LPFLLDIV3_CLK)
#define sys_clk			core_clk
#define plat_clk		core_clk
#define sdhc_dclk		bus_clk

/* peripheral interface clock, controlled by CGC */
#define CLK_INTERFACE		((clk_cat_t)4)
#define MSMC_CLK		((clk_clk_t)0)
#define AXBS_CLK		((clk_clk_t)1)
#define DMA0_CLK		((clk_clk_t)2)
#define XRDC_MGR_CLK		((clk_clk_t)3)
#define XRDC_PAC_CLK		((clk_clk_t)4)
#define XRDC_MRC_CLK		((clk_clk_t)5)
#define SEMA42_0_CLK		((clk_clk_t)6)
#define DMAMUX0_CLK		((clk_clk_t)7)
#define EWM_CLK			((clk_clk_t)8)
#define MUA_CLK			((clk_clk_t)9)
#define CRC_CLK			((clk_clk_t)10)
#define PORTA_CLK		((clk_clk_t)11)
#define PORTB_CLK		((clk_clk_t)12)
#define PORTC_CLK		((clk_clk_t)13)
#define PORTD_CLK		((clk_clk_t)14)
#define LPDAC_CLK		((clk_clk_t)15)
#define VREF_CLK		((clk_clk_t)16)
#define DMA1_CLK		((clk_clk_t)17)
#define GPIOE_CLK		((clk_clk_t)18)
#define SEMA42_1_CLK		((clk_clk_t)19)
#define DMAMUX1_CLK		((clk_clk_t)20)
#define INTMUX1_CLK		((clk_clk_t)21)
#define MUB_CLK			((clk_clk_t)22)
#define CAU3_CLK		((clk_clk_t)23)
#define TRNG_CLK		((clk_clk_t)24)
#define PORTE_CLK		((clk_clk_t)25)
#define MTB_CLK			((clk_clk_t)26)
#define EXT_CLK_CLK		((clk_clk_t)27)
#define NR_INTERFACE_CLKS	(EXT_CLK_CLK + 1)
#define msmc_clk		clkid(CLK_INTERFACE, MSMC_CLK)
#define axbs_clk		clkid(CLK_INTERFACE, AXBS_CLK)
#define dma0_clk		clkid(CLK_INTERFACE, DMA0_CLK)
#define xrdc_mgr_clk		clkid(CLK_INTERFACE, XRDC_MGR_CLK)
#define xrdc_pac_clk		clkid(CLK_INTERFACE, XRDC_PAC_CLK)
#define xrdc_mrc_clk		clkid(CLK_INTERFACE, XRDC_MRC_CLK)
#define sema42_0_clk		clkid(CLK_INTERFACE, SEMA42_0_CLK)
#define dmamux0_clk		clkid(CLK_INTERFACE, DMAMUX0_CLK)
#define ewm_clk			clkid(CLK_INTERFACE, EWM_CLK)
#define mua_clk			clkid(CLK_INTERFACE, MUA_CLK)
#define crc_clk			clkid(CLK_INTERFACE, CRC_CLK)
#define porta_clk		clkid(CLK_INTERFACE, PORTA_CLK)
#define portb_clk		clkid(CLK_INTERFACE, PORTB_CLK)
#define portc_clk		clkid(CLK_INTERFACE, PORTC_CLK)
#define portd_clk		clkid(CLK_INTERFACE, PORTD_CLK)
#define lpdac_clk		clkid(CLK_INTERFACE, LPDAC_CLK)
#define vref_clk		clkid(CLK_INTERFACE, VREF_CLK)
#define dma1_clk		clkid(CLK_INTERFACE, DMA1_CLK)
#define gpioe_clk		clkid(CLK_INTERFACE, GPIOE_CLK)
#define sema42_1_clk		clkid(CLK_INTERFACE, SEMA42_1_CLK)
#define dmamux1_clk		clkid(CLK_INTERFACE, DMAMUX1_CLK)
#define intmux1_clk		clkid(CLK_INTERFACE, INTMUX1_CLK)
#define mub_clk			clkid(CLK_INTERFACE, MUB_CLK)
#define cau3_clk		clkid(CLK_INTERFACE, CAU3_CLK)
#define trng_clk		clkid(CLK_INTERFACE, TRNG_CLK)
#define porte_clk		clkid(CLK_INTERFACE, PORTE_CLK)
#define mtb_clk			clkid(CLK_INTERFACE, MTB_CLK)
#define ext_clk_clk		clkid(CLK_INTERFACE, EXT_CLK_CLK)

/* peripheral functional clock, controlled by PCS */
#define CLK_FUNCTIONAL		((clk_cat_t)5)
#define LPIT0_CLK		((clk_clk_t)0)
#define TPM0_CLK		((clk_clk_t)1)
#define TPM1_CLK		((clk_clk_t)2)
#define TPM2_CLK		((clk_clk_t)3)
#define EMVSIM_CLK		((clk_clk_t)4)
#define FLEXIO_CLK		((clk_clk_t)5)
#define LPI2C0_CLK		((clk_clk_t)6)
#define LPI2C1_CLK		((clk_clk_t)7)
#define LPI2C2_CLK		((clk_clk_t)8)
#define LPI2S_CLK		((clk_clk_t)9)
#define USDHC_CLK		((clk_clk_t)10)
#define LPSPI0_CLK		((clk_clk_t)11)
#define LPSPI1_CLK		((clk_clk_t)12)
#define LPSPI2_CLK		((clk_clk_t)13)
#define LPUART0_CLK		((clk_clk_t)14)
#define LPUART1_CLK		((clk_clk_t)15)
#define LPUART2_CLK		((clk_clk_t)16)
#define USB_CLK			((clk_clk_t)17)
#define LPADC_CLK		((clk_clk_t)18)
#define TRACE_CLK		((clk_clk_t)19)
#define LPIT1_CLK		((clk_clk_t)20)
#define TPM3_CLK		((clk_clk_t)21)
#define LPI2C3_CLK		((clk_clk_t)22)
#define LPSPI3_CLK		((clk_clk_t)23)
#define LPUART3_CLK		((clk_clk_t)24)
#define NR_FUNCTIONAL_CLKS	(LPUART3_CLK + 1)
#define lpit0_clk		clkid(CLK_FUNCTIONAL, LPIT0_CLK)
#define tpm0_clk		clkid(CLK_FUNCTIONAL, TPM0_CLK)
#define tpm1_clk		clkid(CLK_FUNCTIONAL, TPM1_CLK)
#define tpm2_clk		clkid(CLK_FUNCTIONAL, TPM2_CLK)
#define emvsim_clk		clkid(CLK_FUNCTIONAL, EMVSIM_CLK)
#define flexio_clk		clkid(CLK_FUNCTIONAL, FLEXIO_CLK)
#define lpi2c0_clk		clkid(CLK_FUNCTIONAL, LPI2C0_CLK)
#define lpi2c1_clk		clkid(CLK_FUNCTIONAL, LPI2C1_CLK)
#define lpi2c2_clk		clkid(CLK_FUNCTIONAL, LPI2C2_CLK)
#define lpi2s_clk		clkid(CLK_FUNCTIONAL, LPI2S_CLK)
#define usdhc_clk		clkid(CLK_FUNCTIONAL, USDHC_CLK)
#define lpspi0_clk		clkid(CLK_FUNCTIONAL, LPSPI0_CLK)
#define lpspi1_clk		clkid(CLK_FUNCTIONAL, LPSPI1_CLK)
#define lpspi2_clk		clkid(CLK_FUNCTIONAL, LPSPI2_CLK)
#define lpuart0_clk		clkid(CLK_FUNCTIONAL, LPUART0_CLK)
#define lpuart1_clk		clkid(CLK_FUNCTIONAL, LPUART1_CLK)
#define lpuart2_clk		clkid(CLK_FUNCTIONAL, LPUART2_CLK)
#define usb_clk			clkid(CLK_FUNCTIONAL, USB_CLK)
#define lpadc_clk		clkid(CLK_FUNCTIONAL, LPADC_CLK)
#define trace_clk		clkid(CLK_FUNCTIONAL, TRACE_CLK)
#define lpit1_clk		clkid(CLK_FUNCTIONAL, LPIT1_CLK)
#define tpm3_clk		clkid(CLK_FUNCTIONAL, TPM3_CLK)
#define lpi2c3_clk		clkid(CLK_FUNCTIONAL, LPI2C3_CLK)
#define lpspi3_clk		clkid(CLK_FUNCTIONAL, LPSPI3_CLK)
#define lpuart3_clk		clkid(CLK_FUNCTIONAL, LPUART3_CLK)

#define CORE_CLK_FREQ_RUN	48000000
#define CORE_CLK_FREQ_VLPR	4000000
#define CORE_CLK_FREQ_HSRUN	72000000

clk_t freqplan_scs2clk(void);
uint8_t freqplan_clk2scs(void);
#ifdef CONFIG_VEGA_BOOT_CPU
void freqplan_config_boot(void);
void freqplan_config_run(void);
void freqplan_config_vlpr(void);
void freqplan_config_hsrun(void);
#else
#define freqplan_config_boot()		do { } while (0)
#define freqplan_config_run()		do { } while (0)
#define freqplan_config_vlpr()		do { } while (0)
#define freqplan_config_hsrun()		do { } while (0)
#endif

int clk_hw_ctrl_init(void);

#endif /* __CLK_VEGA_H_INCLUDE__ */
