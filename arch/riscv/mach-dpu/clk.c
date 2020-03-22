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
 * @(#)clk.c: DPU specific clock tree implementation
 * $Id: clk.c,v 1.1 2020-03-06 16:07:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/panic.h>

#define CLK_SEL_F	_BV(0)
#define CLK_EN_F	_BV(1)
#define CLK_SRST_F	_BV(2)

struct reset_clk {
	clk_t clk_src;
	uint8_t flags;
};

#define CLK_SRST_MAP	ULL(0x000007FFFFFF)
#define CLK_ARST_MAP	ULL(0x1FFE00000000)
#ifdef CONFIG_DPU_PLL_ARST
#define CLK_RST_MAP	(CLK_ARST_MAP | CLK_SRST_MAP)
#else
#define CLK_RST_MAP	CLK_SRST_MAP
#endif

struct reset_clk reset_clks[NR_RESET_CLKS] = {
	[SRST_GPDPU] = {
		.clk_src = pe_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PCIE0] = {
		.clk_src = pcie_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PCIE1] = {
		.clk_src = pcie_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_SPI] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_I2C0] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_I2C1] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_I2C2] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_UART] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PLIC] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_GPIO] = {
		.clk_src = periph_clk,
		.flags = CLK_EN_F,
	},
	[SRST_RAM] = {
		.clk_src = periph_clk,
		.flags = CLK_EN_F,
	},
	[SRST_ROM] = {
		.clk_src = periph_clk,
		.flags = CLK_EN_F,
	},
	[SRST_TMR] = {
		.clk_src = xin,
		.flags = CLK_EN_F,
	},
	[SRST_WDT] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_TCSR] = {
		.clk_src = periph_clk,
		.flags = CLK_EN_F,
	},
	[SRST_CLUSTER_CFG] = {
		.clk_src = periph_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_IMC] = {
		.clk_src = periph_clk,
		.flags = CLK_EN_F,
	},
	[SRST_NOC] = {
		.clk_src = periph_clk,
		.flags = CLK_EN_F,
	},
	[SRST_FLASH] = {
		.clk_src = periph_clk,
		.flags = CLK_EN_F,
	},
	[SRST_DDR0_0] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_DDR0_1] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_DDR1_0] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_DDR1_1] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_DDR0_POR] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_DDR1_POR] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PCIE0_POR] = {
		.clk_src = pcie_ref_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PCIE1_POR] = {
		.clk_src = pcie_ref_clk,
		.flags = CLK_SRST_F,
	},
#ifdef CONFIG_DPU_PLL_ARST
	[APC0_CPU0_FUNC_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC0_CPU1_FUNC_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC1_CPU0_FUNC_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC1_CPU1_FUNC_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC0_L2_FUNC_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC1_L2_FUNC_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC0_CPU0_DBG_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC0_CPU1_DBG_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC1_CPU0_DBG_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC1_CPU1_DBG_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC0_L2_DBG_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
	[APC1_L2_DBG_ARST] = {
		.clk_src = cpu_clk,
		.flags = CLK_SRST_F,
	},
#endif
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *reset_clk_names[NR_RESET_CLKS] = {
	[SRST_GPDPU] = "srst_gpdpu",
	[SRST_PCIE0] = "srst_pcie0",
	[SRST_PCIE1] = "srst_pcie1",
	[SRST_SPI] = "srst_spi",
	[SRST_I2C0] = "srst_i2c0",
	[SRST_I2C1] = "srst_i2c1",
	[SRST_I2C2] = "srst_i2c2",
	[SRST_UART] = "srst_uart",
	[SRST_PLIC] = "srst_plic",
	[SRST_GPIO] = "srst_gpio",
	[SRST_RAM] = "srst_ram",
	[SRST_ROM] = "srst_rom",
	[SRST_TMR] = "srst_tmr",
	[SRST_WDT] = "srst_wdt",
	[SRST_TCSR] = "srst_tcsr",
	[SRST_CLUSTER_CFG] = "srst_cluster_cfg",
	[SRST_IMC] = "srst_imc",
	[SRST_NOC] = "srst_noc",
	[SRST_FLASH] = "srst_flash",
	[SRST_DDR0_0] = "srst_ddr0_0",
	[SRST_DDR0_1] = "srst_ddr0_1",
	[SRST_DDR1_0] = "srst_ddr1_0",
	[SRST_DDR1_1] = "srst_ddr1_1",
	[SRST_DDR0_POR] = "srst_ddr0_por",
	[SRST_DDR1_POR] = "srst_ddr1_por",
	[SRST_PCIE0_POR] = "srst_pcie0_por",
	[SRST_PCIE1_POR] = "srst_pcie1_por",
#ifdef CONFIG_DPU_PLL_ARST
	[APC0_CPU0_FUNC_ARST] = "apc0_cpu0_func_arst",
	[APC0_CPU1_FUNC_ARST] = "apc0_cpu1_func_arst",
	[APC1_CPU0_FUNC_ARST] = "apc1_cpu0_func_arst",
	[APC1_CPU1_FUNC_ARST] = "apc1_cpu1_func_arst",
	[APC0_L2_FUNC_ARST] = "apc0_l2_func_arst",
	[APC1_L2_FUNC_ARST] = "apc1_l2_func_arst",
	[APC0_CPU0_DBG_ARST] = "apc0_cpu0_dbg_arst",
	[APC0_CPU1_DBG_ARST] = "apc0_cpu1_dbg_arst",
	[APC1_CPU0_DBG_ARST] = "apc1_cpu0_dbg_arst",
	[APC1_CPU1_DBG_ARST] = "apc1_cpu1_dbg_arst",
	[APC0_L2_DBG_ARST] = "apc0_l2_dbg_arst",
	[APC1_L2_DBG_ARST] = "apc1_l2_dbg_arst",
#endif
};

static const char *get_reset_clk_name(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS)
		return NULL;
	return reset_clk_names[clk];
}
#else
#define get_reset_clk_name		NULL
#endif

static int enable_reset_clk(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS || !(_BV_ULL(clk) & CLK_RST_MAP))
		return -EINVAL;
	if (!(reset_clks[clk].flags & CLK_SRST_F))
		return -EINVAL;

	if (reset_clks[clk].clk_src != invalid_clk)
		clk_enable(reset_clks[clk].clk_src);
	dpu_pll_soft_reset(clk);
	reset_clks[clk].flags |= CLK_EN_F;
	return 0;
}

static void disable_reset_clk(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS || !(_BV_ULL(clk) & CLK_RST_MAP))
		return;
	if (!(reset_clks[clk].flags & CLK_SRST_F))
		return;

	if (reset_clks[clk].clk_src != invalid_clk)
		clk_disable(reset_clks[clk].clk_src);
	dpu_pll_soft_reset(clk);
	reset_clks[clk].flags &= ~CLK_EN_F;
}

static clk_freq_t get_reset_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS || !(_BV_ULL(clk) & CLK_RST_MAP))
		return INVALID_FREQ;
	return clk_get_frequency(reset_clks[clk].clk_src);
}

struct clk_driver clk_srst = {
	.max_clocks = NR_RESET_CLKS,
	.enable = enable_reset_clk,
	.disable = disable_reset_clk,
	.select = NULL,
	.get_freq = get_reset_clk_freq,
	.set_freq = NULL,
	.get_name = get_reset_clk_name,
};

struct sel_clk {
	clk_t clk_sels[2];
	bool enabled;
};

struct sel_clk sel_clks[NR_SEL_CLKS] = {
	[IMC_CLK] = {
		.clk_sels = {
			pll0_p,
			xin,
		},
	},
	[PE_CLK] = {
		.clk_sels = {
			pll1_p,
			xin,
		},
	},
	[DDR_CLK] = {
		.clk_sels = {
			pll2_p,
			xin,
		},
	},
	[PCIE_CLK] = {
		.clk_sels = {
			pll3_p,
			xin,
		},
	},
	[CPU_CLK] = {
		.clk_sels = {
			pll4_p,
			xin,
		},
	},
	[PCIE_REF_CLK] = {
		.clk_sels = {
			pll5_p,
			pcie_phy_clk,
		},
	},
	[APB_CLK] = {
		.clk_sels = {
			pll3_r,
			xin,
		},
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *sel_clk_names[NR_SEL_CLKS] = {
	[IMC_CLK] = "imc_clk (pll0_p_gmux)",
	[PE_CLK] = "pe_clk (pll1_p_gmux)",
	[DDR_CLK] = "ddr_clk (pll2_p_gmux)",
	[PCIE_CLK] = "pcie_clk (pll3_p_gmux)",
	[CPU_CLK] = "cpu_clk (pll4_p_gmux)",
	[PCIE_REF_CLK] = "pcie_ref_clk (pll5_p_gmux)",
	[APB_CLK] = "apb_clk (pll3_r_gmux)",
};

static const char *get_clk_sel_name(clk_clk_t clk)
{
	if (clk >= NR_SEL_CLKS)
		return NULL;
	return sel_clk_names[clk];
}
#else
#define get_clk_sel_name	NULL
#endif

static int enable_clk_sel(clk_clk_t clk)
{
	bool r = (clk >= NR_PLLS);
	uint8_t pll = r ? 3 : clk;

	if (clk >= NR_SEL_CLKS)
		return -EINVAL;

	if (sel_clks[clk].enabled) {
		if (!dpu_gmux_selected(pll, r)) {
			clk_enable(sel_clks[clk].clk_sels[0]);
			dpu_gmux_select(pll, r);
			clk_disable(sel_clks[clk].clk_sels[1]);
		}
		return 0;
	}
	clk_enable(sel_clks[clk].clk_sels[0]);
	sel_clks[clk].enabled = true;
	dpu_gmux_select(pll, r);
	return 0;
}

static void disable_clk_sel(clk_clk_t clk)
{
	bool r = (clk >= NR_PLLS);
	uint8_t pll = r ? 3 : clk;

	if (clk >= NR_SEL_CLKS)
		return;

	if (sel_clks[clk].enabled) {
		if (dpu_gmux_selected(pll, r)) {
			clk_enable(sel_clks[clk].clk_sels[1]);
			dpu_gmux_deselect(pll, r);
			clk_disable(sel_clks[clk].clk_sels[0]);
		}
		return;
	}
	clk_enable(sel_clks[clk].clk_sels[1]);
	sel_clks[clk].enabled = true;
	dpu_gmux_deselect(pll, r);
}

static clk_freq_t get_clk_sel_freq(clk_clk_t clk)
{
	bool r = (clk >= NR_PLLS);
	uint8_t pll = r ? 3 : clk;

	if (clk >= NR_SEL_CLKS)
		return INVALID_FREQ;
	if (dpu_gmux_selected(pll, r))
		return clk_get_frequency(sel_clks[clk].clk_sels[0]);
	else
		return clk_get_frequency(sel_clks[clk].clk_sels[1]);
}

static int set_clk_sel_freq(clk_clk_t clk, clk_freq_t freq)
{
	int i;
	int ret;

	/* GMUX of apb_clk should be auto-balanced */
	if (clk > NR_PLLS)
		return -EINVAL;
	BUG_ON(clk == APB_CLK);

	/* Only allows pre-defined frequency plans */
	for (i = 0; i < NR_FREQPLANS; i++) {
		if (freq == freqplan_get_fpclk_nodef(clk, i))
			goto freq_valid;
	}
	return -EINVAL;

freq_valid:
	/* Auto-balance apb_clk */
	if (clk == PCIE_CLK)
		clk_disable(apb_clk);
	disable_clk_sel(clk);
	ret = clk_set_frequency(sel_clks[clk].clk_sels[0], freq);
	if (ret)
		return ret;
	enable_clk_sel(clk);
	/* Auto-balance apb_clk */
	if (clk == PCIE_CLK)
		clk_enable(apb_clk);
	return 0;
}

struct clk_driver clk_gmux = {
	.max_clocks = NR_SEL_CLKS,
	.enable = enable_clk_sel,
	.disable = disable_clk_sel,
	.get_freq = get_clk_sel_freq,
	.set_freq = set_clk_sel_freq,
	.select = NULL,
	.get_name = get_clk_sel_name,
};

struct pll_clk {
	clk_t src;
	clk_freq_t freq;
	bool enabled;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[PLL0_P] = {
		.src = pll0_vco,
		.freq = PLL0_P_FREQ,
		.enabled = false,
	},
	[PLL1_P] = {
		.src = pll1_vco,
		.freq = PLL1_P_FREQ,
		.enabled = false,
	},
	[PLL2_P] = {
		.src = pll2_vco,
		.freq = PLL2_P_FREQ,
		.enabled = false,
	},
	[PLL3_P] = {
		.src = pll3_vco,
		.freq = PLL3_P_FREQ,
		.enabled = false,
	},
	[PLL4_P] = {
		.src = pll4_vco,
		.freq = PLL4_P_FREQ,
		.enabled = false,
	},
	[PLL5_P] = {
		.src = pll5_vco,
		.freq = PLL5_P_FREQ,
		.enabled = false,
	},
	[PLL3_R] = {
		.src = pll3_vco,
		.freq = PLL3_R_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *pll_clk_names[NR_PLL_CLKS] = {
	[PLL0_P] = "pll0_p",
	[PLL1_P] = "pll1_p",
	[PLL2_P] = "pll2_p",
	[PLL3_P] = "pll3_p",
	[PLL4_P] = "pll4_p",
	[PLL5_P] = "pll5_p",
	[PLL3_R] = "pll3_r",
};

const char *get_pll_name(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return NULL;
	return pll_clk_names[clk];
}
#else
#define get_pll_name		NULL
#endif

static void __enable_pll(clk_clk_t clk)
{
	bool r = (clk >= NR_PLLS);
	uint8_t pll = r ? 3 : clk;

	if (!pll_clks[clk].enabled) {
		clk_enable(pll_clks[clk].src);
		dpu_div_enable(pll,
			       clk_get_frequency(pll_clks[clk].src),
			       pll_clks[clk].freq, r);
		pll_clks[clk].enabled = true;
	}
}

static void __disable_pll(clk_clk_t clk)
{
	bool r = (clk >= NR_PLLS);
	uint8_t pll = r ? 3 : clk;

	if (pll_clks[clk].enabled) {
		pll_clks[clk].enabled = false;
		dpu_div_disable(pll, r);
		clk_disable(pll_clks[clk].src);
	}
}

static int enable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return -EINVAL;
	__enable_pll(clk);
	return 0;
}

static void disable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return;
	__disable_pll(clk);
}

static clk_freq_t get_pll_freq(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return INVALID_FREQ;
	return pll_clks[clk].freq;
}

static int set_pll_freq(clk_clk_t clk, clk_freq_t freq)
{
	int i;
	int ret;

	/* PLL rclkout of apb_clk should be auto-balanced */
	if (clk > NR_PLLS)
		return -EINVAL;
	BUG_ON(clk == PLL3_R);

	/* Only allows pre-defined frequency plans */
	for (i = 0; i < NR_FREQPLANS; i++) {
		if (freq == freqplan_get_fpclk_nodef(clk, i))
			goto freq_valid;
	}
	return -EINVAL;

freq_valid:
	/* Auto-balance apb_clk */
	if (clk == PLL3_P) {
		pll_clks[PLL3_R].freq = freqplan_get_frclk(clk, i);
		__disable_pll(PLL3_R);
	}
	pll_clks[clk].freq = freq;
	__disable_pll(clk);
	ret = clk_set_frequency(pll_clks[clk].src,
				freqplan_get_fvco(clk, i));
	if (ret) {
		con_printf("PLL(%d): set frequency(%lld) failure.\n",
			   clk, freq);
		return ret;
	}
	__enable_pll(clk);
	/* Auto-balance apb_clk */
	if (clk == PLL3_P)
		__enable_pll(PLL3_R);
	return 0;
}

struct clk_driver clk_pll = {
	.max_clocks = NR_PLL_CLKS,
	.enable = enable_pll,
	.disable = disable_pll,
	.get_freq = get_pll_freq,
	.set_freq = set_pll_freq,
	.select = NULL,
	.get_name = get_pll_name,
};

struct vco_clk {
	clk_freq_t freq;
	bool enabled;
};

struct vco_clk vco_clks[NR_VCO_CLKS] = {
	[PLL0_VCO] = {
		.freq = PLL0_VCO_FREQ,
		.enabled = false,
	},
	[PLL1_VCO] = {
		.freq = PLL1_VCO_FREQ,
		.enabled = false,
	},
	[PLL2_VCO] = {
		.freq = PLL2_VCO_FREQ,
		.enabled = false,
	},
	[PLL3_VCO] = {
		.freq = PLL3_VCO_FREQ,
		.enabled = false,
	},
	[PLL4_VCO] = {
		.freq = PLL4_VCO_FREQ,
		.enabled = false,
	},
	[PLL5_VCO] = {
		.freq = PLL5_VCO_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *vco_clk_names[NR_VCO_CLKS] = {
	[PLL0_VCO] = "pll0_vco",
	[PLL1_VCO] = "pll1_vco",
	[PLL2_VCO] = "pll2_vco",
	[PLL3_VCO] = "pll3_vco",
	[PLL4_VCO] = "pll4_vco",
	[PLL5_VCO] = "pll5_vco",
};

const char *get_vco_name(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return NULL;
	return vco_clk_names[clk];
}
#else
#define get_vco_name		NULL
#endif

static void __enable_vco(clk_clk_t clk)
{
	if (!vco_clks[clk].enabled) {
		dpu_pll_enable(clk, vco_clks[clk].freq);
		vco_clks[clk].enabled = true;
	}
}

static void __disable_vco(clk_clk_t clk)
{
	if (vco_clks[clk].enabled) {
		vco_clks[clk].enabled = false;
		dpu_pll_disable(clk);
	}
}

static int enable_vco(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return -EINVAL;
	__enable_vco(clk);
	return 0;
}

static void disable_vco(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return;
	__disable_vco(clk);
}

static clk_freq_t get_vco_freq(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return INVALID_FREQ;
	return vco_clks[clk].freq;
}

static int set_vco_freq(clk_clk_t clk, clk_freq_t freq)
{
	if (clk >= NR_VCO_CLKS)
		return -EINVAL;

	if (vco_clks[clk].freq != freq) {
		__disable_vco(clk);
		vco_clks[clk].freq = freq;
	}
	__enable_vco(clk);
	return 0;
}

struct clk_driver clk_vco = {
	.max_clocks = NR_VCO_CLKS,
	.enable = enable_vco,
	.disable = disable_vco,
	.get_freq = get_vco_freq,
	.set_freq = set_vco_freq,
	.select = NULL,
	.get_name = get_vco_name,
};

uint32_t input_clks[NR_INPUT_CLKS] = {
	[XIN] = XIN_FREQ,
	[PCIE_PHY_CLK] = PCIE_PHY_CLK_FREQ,
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *input_clk_names[NR_INPUT_CLKS] = {
	[XIN] = "xin",
	[PCIE_PHY_CLK] = "pcie_phy_clk",
};

static const char *get_input_clk_name(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return NULL;
	return input_clk_names[clk];
}
#else
#define get_input_clk_name	NULL
#endif

static clk_freq_t get_input_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return INVALID_FREQ;
	return input_clks[clk];
}

struct clk_driver clk_input = {
	.max_clocks = NR_INPUT_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_input_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_input_clk_name,
};

/*===========================================================================
 * Clock tree APIs
 *===========================================================================*/
static bool clk_hw_init = false;

void board_init_clock(void)
{
	if (!clk_hw_init) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_VCO, &clk_vco);
		clk_register_driver(CLK_PLL, &clk_pll);
		clk_register_driver(CLK_SEL, &clk_gmux);
		clk_register_driver(CLK_RESET, &clk_srst);
		/* Update the status of the default enabled clocks */
		clk_enable(imc_clk);
		clk_enable(apb_clk);
	}
	clk_hw_init = true;
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}
