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

#ifdef CONFIG_DPU_PLL
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#define ARCH_HAVE_CLK_TYPES	1
typedef uint16_t clk_t;
typedef uint8_t clk_cat_t;
typedef uint8_t clk_clk_t;
#define clkid(cat, clk)		((clk_t)MAKEWORD(clk, cat))
#define invalid_clk		clkid(0xFF, 0xFF)
#define clk_clk(clkid)		LOBYTE(clkid)
#define clk_cat(clkid)		HIBYTE(clkid)

#define NR_FREQPLANS		5
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	(-1)

#define clk_freq_t		uint64_t

#ifdef CONFIG_DPU_RES
#ifdef CONFIG_DPU_RES_SLOW
#define PLL0_VCO_FREQ		ULL(4000000000)	/* 4GHz */
#else /* CONFIG_DPU_RES_SLOW */
#define PLL0_VCO_FREQ		ULL(3000000000)	/* 3GHz */
#endif /* CONFIG_DPU_RES_SLOW */
#else /* CONFIG_DPU_RES */
#define PLL0_VCO_FREQ		ULL(3200000000)	/* 3.2GHz */
#endif /* CONFIG_DPU_RES */
#define PLL1_VCO_FREQ		ULL(3600000000)	/* 3.6GHz */
#define PLL2_VCO_FREQ		ULL(3200000000)	/* 3.2GHz */
#ifdef CONFIG_DPU_RES
#ifdef CONFIG_DPU_RES_SLOW
#define PLL3_VCO_FREQ		ULL(4800000000)	/* 4.8GHz */
#else /* CONFIG_DPU_RES_SLOW */
#define PLL3_VCO_FREQ		ULL(3200000000)	/* 3.2GHz */
#endif /* CONFIG_DPU_RES_SLOW */
#else /* CONFIG_DPU_RES */
#define PLL3_VCO_FREQ		ULL(3200000000)	/* 3.2GHz */
#endif /* CONFIG_DPU_RES */
#define PLL4_VCO_FREQ		ULL(4000000000)	/* 4GHz */
#ifdef CONFIG_DPU_GEN2
#define PLL5_VCO_FREQ		ULL(4400000000)	/* 4.4GHz */
#else /* CONFIG_DPU_GEN2 */
#define PLL5_VCO_FREQ		ULL(2800000000)	/* 2.8GHz */
#endif /* CONFIG_DPU_GEN2 */

#include <dt-bindings/clock/sbi-clock-dpu.h>

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
#define PLL0_P			CPU_CLK
#define PLL1_P			PE_CLK
#define PLL2_P			DDR_CLK
#define PLL3_P			AXI_CLK
#define PLL4_P			VPU_BCLK
#define PLL2_R			DDR_BYPASS_PCLK
#define PLL3_R			APB_CLK

#ifdef CONFIG_DPU_GEN2
#define PLL5_P			VPU_CCLK
#define PLL4_R			TSENSOR_XO_CLK
#define PLL5_R			TSENSOR_METS_CLK
#else /* CONFIG_DPU_GEN2 */
#define PLL5_P			PCIE_REF_CLK
#define PLL4_R			VPU_CCLK
#endif /* CONFIG_DPU_GEN2 */

#define NR_PLL_CLKS		NR_SEL_CLKS
#define pll0_p			clkid(CLK_PLL, PLL0_P)
#define pll1_p			clkid(CLK_PLL, PLL1_P)
#define pll2_p			clkid(CLK_PLL, PLL2_P)
#define pll3_p			clkid(CLK_PLL, PLL3_P)
#define pll4_p			clkid(CLK_PLL, PLL4_P)
#define pll5_p			clkid(CLK_PLL, PLL5_P)
#define pll2_r			clkid(CLK_PLL, PLL2_R)
#define pll3_r			clkid(CLK_PLL, PLL3_R)
#define pll4_r			clkid(CLK_PLL, PLL4_R)
#ifdef CONFIG_DPU_GEN2
#define pll5_r			clkid(CLK_PLL, PLL5_R)
#endif /* CONFIG_DPU_GEN2 */

#define CLK_IS_PLL_RCLK(clk)	(!!((clk) >= NR_PLLS))
#define CLK_TO_PLL(clk, isrclk)	((isrclk) ? ((clk) - NR_PLLS + 2) : (clk))

#define CLK_SEL			((clk_cat_t)3)
#define CPU_CLK			((clk_clk_t)0) /* PLL0_P */
#define PE_CLK			((clk_clk_t)1) /* PLL1_P */
#define DDR_CLK			((clk_clk_t)2) /* PLL2_P */
#define AXI_CLK			((clk_clk_t)3) /* PLL3_P */
#define VPU_BCLK		((clk_clk_t)4) /* PLL4_P */
#define DDR_BYPASS_PCLK		((clk_clk_t)6) /* PLL2_R */
#define APB_CLK			((clk_clk_t)7) /* PLL3_R */

#ifdef CONFIG_DPU_GEN2
#define VPU_CCLK		((clk_clk_t)5) /* PLL5_P */
#define TSENSOR_XO_CLK		((clk_clk_t)8) /* PLL4_R */
#define TSENSOR_METS_CLK	((clk_clk_t)9) /* PLL5_R */
#define NR_SEL_CLKS		(TSENSOR_METS_CLK + 1)
#else /* CONFIG_DPU_GEN2 */
#define PCIE_REF_CLK		((clk_clk_t)5) /* PLL5_P */
#define VPU_CCLK		((clk_clk_t)8) /* PLL4_R */
#define NR_SEL_CLKS		(VPU_CCLK + 1)
#endif /* CONFIG_DPU_GEN2 */

#define cpu_clk			clkid(CLK_SEL, CPU_CLK)
#define pe_clk			clkid(CLK_SEL, PE_CLK)
#define ddr_clk			clkid(CLK_SEL, DDR_CLK)
#define axi_clk			clkid(CLK_SEL, AXI_CLK)
#define vpu_bclk		clkid(CLK_SEL, VPU_BCLK)
#define ddr_bypass_pclk		clkid(CLK_SEL, DDR_BYPASS_PCLK)
#define apb_clk			clkid(CLK_SEL, APB_CLK)
#define vpu_cclk		clkid(CLK_SEL, VPU_CCLK)

#ifdef CONFIG_DPU_GEN2
#define tsensor_xo_clk		clkid(CLK_SEL, TSENSOR_XO_CLK)
#define tsensor_mets_clk	clkid(CLK_SEL, TSENSOR_METS_CLK)
#else /* CONFIG_DPU_GEN2 */
#define pcie_ref_clk		clkid(CLK_SEL, PCIE_REF_CLK)
#endif /* CONFIG_DPU_GEN2 */

/* XXX: Do not confuse:
 *      The ddr_clk named in CLKRST is actually the core_ddr_clk (the
 *      uMCTL2 clock) in DesignWare specification where the ddr_clk refers
 *      to the DDR data sampling clock whose frequency is 2 * Fdfi_clk in
 *      the DPU SoC design due to the synthesis restriction as Fdfi_clk
 *      equals to Fcore_ddr_clk, while it's much difficult in achieving
 *      1.6GHz core_ddr_clk for DDR4_3200 speed grade.
 */
#define dfi_clk			ddr_clk
#define core_ddr_clk		ddr_clk

/* DDR clock alias */
#define ddr0_clk		ddr_clk
#define ddr0_aclk		axi_clk
#define ddr0_pclk		apb_clk
#define ddr0_bypass_pclk	ddr_bypass_pclk
/* PCIe clock alias */
#define pcie0_aclk		axi_clk
#define pcie0_pclk		apb_clk
#define pcie0_aux_clk		xin
#define pcie0_ref_clk		pcie_ref_clk
#define pcie1_aclk		axi_clk
#define pcie1_pclk		apb_clk
#define pcie1_aux_clk		xin
#define pcie1_ref_clk		pcie_ref_clk

#define CLK_DEP			((clk_cat_t)4)
#define VPU_CLK			((clk_clk_t)0)
#define NR_DEP_CLKS		(VPU_CLK + 1)
#define vpu_clk			clkid(CLK_DEP, 0)

#define CLK_RESET		((clk_cat_t)5)
#define NR_RESET_CLKS		NR_PLL_RSTS
#define srst_gpdpu		clkid(CLK_RESET, SRST_GPDPU)
#define srst_pcie0		clkid(CLK_RESET, SRST_PCIE0)
#define srst_pcie1		clkid(CLK_RESET, SRST_PCIE1)
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
#define srst_vpu		clkid(CLK_RESET, SRST_VPU)
#define srst_cpu		clkid(CLK_RESET, SRST_CPU)
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
#ifdef CONFIG_DPU_GEN2
#define srst_tsensor_xo		clkid(CLK_RESET, SRST_TSENSOR_XO)
#define srst_tsensor_mets	clkid(CLK_RESET, SRST_TSENSOR_METS)
#endif /* CONFIG_DPU_GEN2 */
#define por_arst		clkid(CLK_RESET, POR_ARST)

/* DDR reset alias */
#define ddr0_pwr_ok_in		srst_ddr0_por
#define ddr0_prst_n		srst_ddr0_0
#define ddr0_rst_n		srst_ddr0_1
#define ddr0_arst_n		srst_ddr0_1
#define ddr1_pwr_ok_in		srst_ddr1_por
#define ddr1_prst_n		srst_ddr1_0
#define ddr1_rst_n		srst_ddr1_1
#define ddr1_arst_n		srst_ddr1_1
/* PCIe reset alias */
#define pcie0_arst_n		srst_pcie0
#define pcie0_por_n		srst_pcie0_por
#define pcie1_arst_n		srst_pcie1
#define pcie1_por_n		srst_pcie1_por

#ifdef CONFIG_DPU_RES
#define CLK_DIV			((clk_cat_t)6)
#ifdef CONFIG_DPU_BOOT_ROM
#define XIN_DIV2		((clk_clk_t)0)
#define NR_DIV_CLKS		(XIN_DIV2 + 1)
#define xin_div2		clkid(CLK_DIV, XIN_DIV2)
#else /* CONFIG_DPU_BOOT_ROM */
#define PLL0_P_DIV2		((clk_clk_t)0)
#define NR_DIV_CLKS		(PLL0_P_DIV2 + 1)
#define pll0_p_div2		clkid(CLK_DIV, PLL0_P_DIV2)
#endif /* CONFIG_DPU_BOOT_ROM */
#endif /* CONFIG_DPU_RES */

#include <asm/mach/pll.h>

/* Enable clock tree core */
void clk_hw_ctrl_init(void);

clk_freq_t freqplan_get_fvco(int pll, int plan);
clk_freq_t freqplan_get_fpclk(int pll, int plan);
clk_freq_t freqplan_get_frclk(int pll, int plan);

/* Helper for frequency plans */
void clk_apply_vco(clk_clk_t clk, clk_freq_t freq);
void clk_apply_pll(clk_clk_t clk, clk_freq_t freq);

#ifdef CONFIG_MMU
#define clk_hw_mmu_init()	dpu_mmu_map_pll()
#endif

#endif /* __CLK_DPU_H_INCLUDE__ */
