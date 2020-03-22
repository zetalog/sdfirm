/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)clk.h: DPU specific clock tree definitions
 * $Id: clk.h,v 1.1 2020-03-06 13:14:00 zhenglv Exp $
 */

#ifndef __CLK_DPU_H_INCLUDE__
#define __CLK_DPU_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/pll.h>

#ifdef CONFIG_DPU_PLL
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#ifdef CONFIG_DPU_BOOT
#define NR_FREQPLANS		1
#else
#define NR_FREQPLANS		7
#endif
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	NR_FREQPLANS

#define invalid_clk		clkid(0xFF, 0xFF)

#define CLK_INPUT		((clk_cat_t)0)
#define XIN			((clk_clk_t)0)
#define PCIE_PHY_CLK		((clk_clk_t)1)
#define IO_CLK			((clk_clk_t)2)
#define NR_INPUT_CLKS		(IO_CLK + 1)
#define xin			clkid(CLK_INPUT, XIN)
#define pcie_phy_clk		clkid(CLK_INPUT, PCIE_PHY_CLK)
#define io_clk			clkid(CLK_INPUT, IO_CLK)

#define CLK_VCO			((clk_cat_t)1)
#define PLL0_VCO		((clk_clk_t)0)
#define PLL1_VCO		((clk_clk_t)1)
#define PLL2_VCO		((clk_clk_t)2)
#define PLL3_VCO		((clk_clk_t)3)
#define PLL4_VCO		((clk_clk_t)4)
#define PLL5_VCO		((clk_clk_t)5)
#define NR_VCO_CLKS		(PLL5_VCO + 1)
#define pll0_vco		clkid(CLK_VCO, PLL0_VCO)
#define pll1_vco		clkid(CLK_VCO, PLL1_VCO)
#define pll2_vco		clkid(CLK_VCO, PLL2_VCO)
#define pll3_vco		clkid(CLK_VCO, PLL3_VCO)
#define pll4_vco		clkid(CLK_VCO, PLL4_VCO)
#define pll5_vco		clkid(CLK_VCO, PLL5_VCO)

#define CLK_PLL			((clk_cat_t)2)
#define PLL0_P			((clk_clk_t)0)
#define PLL1_P			((clk_clk_t)1)
#define PLL2_P			((clk_clk_t)2)
#define PLL3_P			((clk_clk_t)3)
#define PLL4_P			((clk_clk_t)4)
#define PLL5_P			((clk_clk_t)5)
#define PLL3_R			((clk_clk_t)6)
#define NR_PLL_CLKS		(PLL3_R + 1)
#define pll0_p			clkid(CLK_PLL, PLL0_P)
#define pll1_p			clkid(CLK_PLL, PLL1_P)
#define pll2_p			clkid(CLK_PLL, PLL2_P)
#define pll3_p			clkid(CLK_PLL, PLL3_P)
#define pll4_p			clkid(CLK_PLL, PLL4_P)
#define pll5_p			clkid(CLK_PLL, PLL5_P)
#define pll3_r			clkid(CLK_PLL, PLL3_R)

#define CLK_SEL			((clk_cat_t)3)
#define IMC_CLK			((clk_clk_t)0) /* PLL0_P */
#define PE_CLK			((clk_clk_t)1) /* PLL1_P */
#define DDR_CLK			((clk_clk_t)2) /* PLL2_P */
#define PCIE_CLK		((clk_clk_t)3) /* PLL3_P */
#define CPU_CLK			((clk_clk_t)4) /* PLL4_P */
#define PCIE_REF_CLK		((clk_clk_t)5) /* PLL5_P */
#define APB_CLK			((clk_clk_t)6) /* PLL3_R */
#define NR_SEL_CLKS		(APB_CLK + 1)
#define imc_clk			clkid(CLK_SEL, IMC_CLK)
#define pe_clk			clkid(CLK_SEL, PE_CLK)
#define ddr_clk			clkid(CLK_SEL, DDR_CLK)
#define pcie_clk		clkid(CLK_SEL, PCIE_CLK)
#define apb_clk			clkid(CLK_SEL, APB_CLK)
#define pcie_ref_clk		clkid(CLK_SEL, PCIE_REF_CLK)
#define cpu_clk			clkid(CLK_SEL, CPU_CLK)
#define periph_clk		apb_clk
#define pcie_aclk		pcie_clk
#define pcie_pclk		apb_clk
#define pcie_aux_clk		xin
#define pcie_ref_clk_p		pci_ref_clk
#define pcie_ref_clk_n		pci_ref_clk

#define CLK_RESET		((clk_cat_t)4)
#define NR_RESET_CLKS		NR_PLL_RSTS
#define srst_gpdpu		clkid(CLK_RESET, SRST_GPDPU)
#define srst_pcie0		clkid(CLK_RESET, SRST_PCIE0)
#define srst_pcie1		clkid(CLK_RESET, SRST_PCIE2)
#define srst_spi		clkid(CLK_RESET, SRST_SPI)
#define srst_i2c0		clkid(CLK_RESET, SRST_I2C0)
#define srst_i2c1		clkid(CLK_RESET, SRST_I2C1)
#define srst_i2c2		clkid(CLK_RESET, SRST_I2C2)
#define srst_uart		clkid(CLK_RESET, SRST_UART)
#define srst_plic		clkid(CLK_RESET, SRST_PLIC)
#define srst_gpio		clkid(CLK_RESET, SRST_GPIO)
#define srst_ram		clkid(CLK_RESET, SRST_RAM)
#define srst_rom		clkid(CLK_RESET, SRST_ROM)
#define srst_tmr		clkid(CLK_RESET, SRST_TMR)
#define srst_wdt		clkid(CLK_RESET, SRST_WDT)
#define srst_tcsr		clkid(CLK_RESET, SRST_TCSR)
#define srst_cluster_cfg	clkid(CLK_RESET, SRST_CLUSTER_CFG)
#define srst_imc		clkid(CLK_RESET, SRST_IMC)
#define srst_noc		clkid(CLK_RESET, SRST_NOC)
#define srst_flash		clkid(CLK_RESET, SRST_FLASH)
#define srst_sys		clkid(CLK_RESET, SRST_SYS)
#define wdt_rst_dis		clkid(CLK_RESET, WDT_RST_DIS)
#define srst_ddr0_0		clkid(CLK_RESET, SRST_DDR0_0)
#define srst_ddr0_1		clkid(CLK_RESET, SRST_DDR0_1)
#define srst_ddr1_0		clkid(CLK_RESET, SRST_DDR1_0)
#define srst_ddr1_1		clkid(CLK_RESET, SRST_DDR1_1)
#define srst_ddr0_por		clkid(CLK_RESET, SRST_DDR0_POR)
#define srst_ddr1_por		clkid(CLK_RESET, SRST_DDR0_POR)
#define srst_pcie0_por		clkid(CLK_RESET, SRST_PCIE0_POR)
#define srst_pcie1_por		clkid(CLK_RESET, SRST_PCIE1_POR)
#define por_arst		clkid(CLK_RESET, POR_ARST)
#define apc0_cpu0_func_arst	clkid(CLK_RESET, APC0_CPU0_FUNC_ARST)
#define apc0_cpu1_func_arst	clkid(CLK_RESET, APC0_CPU1_FUNC_ARST)
#define apc1_cpu0_func_arst	clkid(CLK_RESET, APC1_CPU0_FUNC_ARST)
#define apc1_cpu1_func_arst	clkid(CLK_RESET, APC1_CPU1_FUNC_ARST)
#define apc0_l2_func_arst	clkid(CLK_RESET, APC0_L2_FUNC_ARST)
#define apc1_l2_func_arst	clkid(CLK_RESET, APC1_L2_FUNC_ARST)
#define apc0_cpu0_dbg_arst	clkid(CLK_RESET, APC0_CPU0_DBG_ARST)
#define apc0_cpu1_dbg_arst	clkid(CLK_RESET, APC0_CPU1_DBG_ARST)
#define apc1_cpu0_dbg_arst	clkid(CLK_RESET, APC1_CPU0_DBG_ARST)
#define apc1_cpu1_dbg_arst	clkid(CLK_RESET, APC1_CPU1_DBG_ARST)
#define apc0_l2_dbg_arst	clkid(CLK_RESET, APC0_L2_DBG_ARST)
#define apc1_l2_dbg_arst	clkid(CLK_RESET, APC1_L2_DBG_ARST)

/* Enable clock tree core */
void clk_hw_ctrl_init(void);

#endif /* __CLK_DPU_H_INCLUDE__ */
