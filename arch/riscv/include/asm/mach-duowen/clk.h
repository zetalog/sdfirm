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
 * @(#)clk.h: DUOWEN clock controller definitions
 * $Id: clk.h,v 1.1 2019-11-06 10:36:00 zhenglv Exp $
 */

#ifndef __CLK_DUOWEN_H_INCLUDE__
#define __CLK_DUOWEN_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/crcntl.h>

#ifdef CONFIG_CRCNTL
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#define NR_FREQPLANS		1
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	NR_FREQPLANS

#define clk_freq_t		uint64_t
#define invalid_clk		clkid(0xFF, 0xFF)

#ifdef CONFIG_DUOWEN_PLL_REAL
#define SOC_VCO_FREQ		ULL(4000000000) /* 4GHz */
#define DDR_BUS_VCO_FREQ	ULL(3200000000) /* 3.2GHz */
#define DDR_VCO_FREQ		ULL(3200000000) /* 3.2GHz */
#define COHFAB_VCO_FREQ		ULL(4000000000) /* 4GHz */
#define CL_VCO_FREQ		ULL(3000000000) /* 3GHz */
#define ETH_VCO_FREQ		ULL(2500000000) /* 2.5GHz */
#endif

#include <dt-bindings/clock/sbi-clock-duowen.h>

#ifdef CONFIG_DUOWEN_PLL_NONE
#define XO_CLK_FREQ		25000000
#define SOC_PLL_FREQ		XO_CLK_FREQ
#define DDR_BUS_PLL_FREQ	XO_CLK_FREQ
#define DDR_PLL_FREQ		XO_CLK_FREQ
#define CFAB_PLL_FREQ		XO_CLK_FREQ
#define CL_PLL_FREQ		XO_CLK_FREQ
#define ETH_PLL_FREQ		XO_CLK_FREQ
#define SFAB_PLL_FREQ		XO_CLK_FREQ
#define SGMII_PLL_FREQ		XO_CLK_FREQ

#define SOC_CLK_FREQ		SOC_PLL_FREQ
#define IMC_CLK_FREQ		SOC_CLK_FREQ /* Not div2 */
#define APC_CLK_FREQ		CL_PLL_FREQ
#define SFAB_CLK_FREQ		SFAB_PLL_FREQ
#define CFAB_CLK_FREQ		CFAB_PLL_FREQ
#define APB_CLK_FREQ		SFAB_CLK_FREQ
#define AHB_CLK_FREQ		SFAB_CLK_FREQ

#define gpio0_clk		GPIO0_CLK
#define gpio1_clk		GPIO1_CLK
#define gpio2_clk		GPIO2_CLK
#define uart0_clk		UART0_CLK
#define uart1_clk		UART1_CLK
#define uart2_clk		UART2_CLK
#define uart3_clk		UART3_CLK
#define i2c0_clk		I2C0_CLK
#define i2c1_clk		I2C1_CLK
#define i2c2_clk		I2C2_CLK
#define i2c3_clk		I2C3_CLK
#define i2c4_clk		I2C4_CLK
#define i2c5_clk		I2C5_CLK
#define i2c6_clk		I2C6_CLK
#define i2c7_clk		I2C7_CLK
#define i2c8_clk		I2C8_CLK
#define i2c9_clk		I2C9_CLK
#define i2c10_clk		I2C10_CLK
#define i2c11_clk		I2C11_CLK
#define spi_flash_clk		SPI_FLASH_CLK
#define spi0_clk		SPI0_CLK
#define spi1_clk		SPI1_CLK
#define spi2_clk		SPI2_CLK
#define spi3_clk		SPI3_CLK
#define spi4_clk		SPI4_CLK
#define sd_clk			SD_CLK
#define eth_clk			ETH_CLK
#define tsensor_clk		TSENSOR_CLK
#define clk_hw_enable(clk)		do { } while (0)
#define clk_hw_get_frequency(clk)	XO_CLK_FREQ
#define clk_pll_init()			do { } while (0)
#define clk_pll_dump()			do { } while (0)
#define duowen_clk_init()		do { } while (0)
#endif /* CONFIG_DUOWEN_PLL_NONE */

#ifdef CONFIG_CLK
/* Clock flags, used by clk drivers to indicate clock features */
#define CLK_CLK_SEL_F		_BV(0)
#define CLK_CLK_EN_F		_BV(1)
#define CLK_SW_RST_F		_BV(2)
#define CLK_COHFAB_CFG_F	_BV(3) /* cohfab/cluster clock/reset */
#define CLK_CLUSTER_CFG_F	_BV(4) /* APC internal clock/reset */
/* XXX: CLK_PLL_SEL_F/CLK_DIV_SEL_F:
 *
 * Some select clocks marked by this flag are controlled internally by the
 * PLL clocks.
 */
#define CLK_PLL_SEL_F		_BV(5) /* CLK_SEL for PLL P/R clkout */
#define CLK_DIV_SEL_F		_BV(6) /* CLK_SEL for PLL DIV */
#define CLK_MUX_SEL_F		(CLK_PLL_SEL_F | CLK_DIV_SEL_F)
#define CLK_CR			(CLK_CLK_EN_F | CLK_SW_RST_F)
#define CLK_C			CLK_CLK_EN_F
#define CLK_R			CLK_SW_RST_F

#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_DEC_FLAGS					\
	uint8_t flags0;					\
	uint8_t flags1;
#define CLK_DEF_FLAGS(__flags)				\
	.flags0 = __flags,				\
	.flags1 = __flags,
#define CLK_DEC_FLAGS_RO				\
	uint8_t flags;
#define CLK_DEF_FLAGS_RO(__flags)			\
	.flags = __flags,
#define clk_read_flags(soc, clk)			\
	((soc) ? (clk).flags1 : (clk).flags0)
#define clk_write_flags(soc, clk, __flags)		\
	do {						\
		if (soc)				\
			(clk).flags1 = (__flags);	\
		else					\
			(clk).flags0 = (__flags);	\
	} while (0)
#define clk_set_flags(soc, clk, __flags)		\
	do {						\
		if (soc)				\
			(clk).flags1 |= (__flags);	\
		else					\
			(clk).flags0 |= (__flags);	\
	} while (0)
#define clk_clear_flags(soc, clk, __flags)		\
	do {						\
		if (soc)				\
			(clk).flags1 &= ~(__flags);	\
		else					\
			(clk).flags0 &= ~(__flags);	\
	} while (0)
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define CLK_DEC_FLAGS					\
	uint8_t flags;
#define CLK_DEF_FLAGS(__flags)				\
	.flags = __flags,
#define CLK_DEC_FLAGS_RO				\
	uint8_t flags;
#define CLK_DEF_FLAGS_RO(__flags)			\
	.flags = __flags,
#define clk_read_flags(soc, clk)		((clk).flags)
#define clk_write_flags(soc, clk, __flags)	((clk).flags = (__flags))
#define clk_set_flags(soc, clk, __flags)	((clk).flags |= (__flags))
#define clk_clear_flags(soc, clk, __flags)	((clk).flags &= ~(__flags))
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#ifdef CONFIG_CONSOLE_COMMAND
void clk_pll_dump(void);
#else /* CONFIG_CONSOLE_COMMAND */
#define clk_pll_dump()				do { } while (0)
#endif /* CONFIG_CONSOLE_COMMAND */
void clk_pll_init(void);
/* Enable clock tree core */
void clk_hw_ctrl_init(void);
#ifdef CONFIG_MMU
void clk_hw_mmu_init(void);
#endif /* CONFIG_MMU */
void duowen_clk_init(void);
#endif /* CONFIG_CLK */

#ifdef CONFIG_DUOWEN_PLL_REAL
void clk_apply_vco(clk_clk_t vco, clk_clk_t clk, clk_freq_t freq);
void clk_apply_pll(clk_clk_t pll, clk_clk_t clk, clk_freq_t freq);
/* XXX: Protect Dynamic PLL Change
 *
 * For ASIC environment, CLK_SELECT is managed by the CLK_PLL and CLK_DIV.
 */
#define clk_select_mux(clk)			\
	do {					\
		if (clk != invalid_clk)		\
			clk_enable(clk);	\
	} while (0)
#define clk_deselect_mux(clk)			\
	do {					\
		if (clk != invalid_clk)		\
			clk_disable(clk);	\
	} while (0)
#else /* CONFIG_DUOWEN_PLL_REAL */
#define clk_apply_vco(vco, clk, freq)		do { } while (0)
#define clk_apply_pll(pll, clk, freq)		do { } while (0)
#define clk_select_mux(clk)			do { } while (0)
#define clk_deselect_mux(clk)			do { } while (0)
#endif /* CONFIG_DUOWEN_PLL_REAL */

void duowen_clk_imc_init(void);
void duowen_clk_apc_init(void);

#endif /* __CLK_DUOWEN_H_INCLUDE__ */
