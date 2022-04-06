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
 * @(#)clk_socv2_asic.c: DUOWEN SoCv2 ASIC frequency plan implementation
 * $Id: clk_socv2_asic.c,v 1.1 2020-10-27 09:39:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/ddr.h>

struct select_clk {
	clk_t clk_sels[2];
	CLK_DEC_FLAGS
};

struct select_clk select_clks[NR_SELECT_CLKS] = {
	[SOC_CLK_DIV2_SEL] = {
		.clk_sels = {
			soc_clk_div2,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_DIV_SEL_F)
	},
	[SYSFAB_CLK_SEL] = {
		.clk_sels = {
			sysfab_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[DDR_BUS_CLK_SEL] = {
		.clk_sels = {
			ddr_bus_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[DDR_CLK_SEL] = {
		.clk_sels = {
			ddr_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[SOC_CLK_SEL] = {
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[COHFAB_CLK_SEL] = {
		.clk_sels = {
			cohfab_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F | CLK_PLL_SEL_F)
	},
	[CL0_CLK_SEL] = {
		.clk_sels = {
			cl0_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F | CLK_PLL_SEL_F)
	},
	[CL1_CLK_SEL] = {
		.clk_sels = {
			cl1_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F | CLK_PLL_SEL_F)
	},
	[CL2_CLK_SEL] = {
		.clk_sels = {
			cl2_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F | CLK_PLL_SEL_F)
	},
	[CL3_CLK_SEL] = {
		.clk_sels = {
			cl3_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F | CLK_PLL_SEL_F)
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *sel_clk_names[NR_SELECT_CLKS] = {
	[SOC_CLK_DIV2_SEL] = "soc_clk_div2_sel",
	[SYSFAB_CLK_SEL] = "sysfab_clk_sel",
	[DDR_BUS_CLK_SEL] = "ddr_bus_clk_sel",
	[DDR_CLK_SEL] = "ddr_clk_sel",
	[SOC_CLK_SEL] = "soc_clk_sel",
	[COHFAB_CLK_SEL] = "cohfab_clk_sel",
	[CL0_CLK_SEL] = "cl0_clk_sel",
	[CL1_CLK_SEL] = "cl1_clk_sel",
	[CL2_CLK_SEL] = "cl2_clk_sel",
	[CL3_CLK_SEL] = "cl3_clk_sel",
};

static const char *get_clk_sel_name(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return NULL;
	return sel_clk_names[clk];
}
#else
#define get_clk_sel_name	NULL
#endif

static int enable_clk_sel(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return -EINVAL;
	if (!(clk_read_flags(0, select_clks[clk]) & CLK_CLK_SEL_F)) {
		crcntl_trace(true, get_clk_sel_name(clk));
		if (!(clk_read_flags(0, select_clks[clk]) & CLK_MUX_SEL_F))
			clk_enable(select_clks[clk].clk_sels[0]);
		if (clk_read_flags(0, select_clks[clk]) & CLK_COHFAB_CFG_F)
			cohfab_clk_select(clk);
		else
			crcntl_clk_select(clk);
		if (!(clk_read_flags(0, select_clks[clk]) & CLK_CLK_EN_F))
			clk_set_flags(0, select_clks[clk], CLK_CLK_EN_F);
		clk_set_flags(0, select_clks[clk], CLK_CLK_SEL_F);
	}
	return 0;
}

static void disable_clk_sel(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return;
	if (clk_read_flags(0, select_clks[clk]) & CLK_CLK_SEL_F) {
		crcntl_trace(false, get_clk_sel_name(clk));
		clk_enable(select_clks[clk].clk_sels[1]);
		if (clk_read_flags(0, select_clks[clk]) & CLK_COHFAB_CFG_F)
			cohfab_clk_deselect(clk);
		else
			crcntl_clk_deselect(clk);
		if (!(clk_read_flags(0, select_clks[clk]) & CLK_CLK_EN_F))
			clk_set_flags(0, select_clks[clk], CLK_CLK_EN_F);
		clk_clear_flags(0, select_clks[clk], CLK_CLK_SEL_F);
	}
}

static clk_freq_t get_clk_sel_freq(clk_clk_t clk)
{
	bool selected;

	if (clk >= NR_SELECT_CLKS)
		return INVALID_FREQ;
	if (clk_read_flags(0, select_clks[clk]) & CLK_COHFAB_CFG_F)
		selected = cohfab_clk_selected(clk);
	else
		selected = crcntl_clk_selected(clk);
	if (selected)
		return clk_get_frequency(select_clks[clk].clk_sels[0]);
	else
		return clk_get_frequency(select_clks[clk].clk_sels[1]);
}

const struct clk_driver clk_select = {
	.max_clocks = NR_SELECT_CLKS,
	.enable = enable_clk_sel,
	.disable = disable_clk_sel,
	.get_freq = get_clk_sel_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_clk_sel_name,
};

#ifdef CONFIG_DUOWEN_SBI_DUAL
static int enable_clk_sel2(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return -EINVAL;
	if (!(clk_read_flags(1, select_clks[clk]) & CLK_CLK_SEL_F)) {
		crcntl_trace(true, get_clk_sel_name(clk));
		if (!(clk_read_flags(1, select_clks[clk]) & CLK_MUX_SEL_F))
			clk_enable(clkid2(select_clks[clk].clk_sels[0]));
		if (clk_read_flags(1, select_clks[clk]) & CLK_COHFAB_CFG_F)
			__cohfab_clk_select(clk, 1);
		else
			__crcntl_clk_select(clk, 1);
		if (!(clk_read_flags(1, select_clks[clk]) & CLK_CLK_EN_F))
			clk_set_flags(1, select_clks[clk], CLK_CLK_EN_F);
		clk_set_flags(1, select_clks[clk], CLK_CLK_SEL_F);
	}
	return 0;
}

static void disable_clk_sel2(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return;
	if (clk_read_flags(1, select_clks[clk]) & CLK_CLK_SEL_F) {
		crcntl_trace(false, get_clk_sel_name(clk));
		clk_enable(clkid2(select_clks[clk].clk_sels[1]));
		if (clk_read_flags(1, select_clks[clk]) & CLK_COHFAB_CFG_F)
			__cohfab_clk_deselect(clk, 1);
		else
			__crcntl_clk_deselect(clk, 1);
		if (!(clk_read_flags(1, select_clks[clk]) & CLK_CLK_EN_F))
			clk_set_flags(1, select_clks[clk], CLK_CLK_EN_F);
		clk_clear_flags(1, select_clks[clk], CLK_CLK_SEL_F);
	}
}

static clk_freq_t get_clk_sel_freq2(clk_clk_t clk)
{
	bool selected;

	if (clk >= NR_SELECT_CLKS)
		return INVALID_FREQ;
	if (clk_read_flags(1, select_clks[clk]) & CLK_COHFAB_CFG_F)
		selected = __cohfab_clk_selected(clk, 1);
	else
		selected = __crcntl_clk_selected(clk, 1);
	if (selected)
		return clk_get_frequency(clkid2(select_clks[clk].clk_sels[0]));
	else
		return clk_get_frequency(clkid2(select_clks[clk].clk_sels[1]));
}

const struct clk_driver clk_select2 = {
	.max_clocks = NR_SELECT_CLKS,
	.enable = enable_clk_sel2,
	.disable = disable_clk_sel2,
	.get_freq = get_clk_sel_freq2,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_clk_sel_name,
};
#endif /* CONFIG_DUOWEN_SBI_DUAL */

struct pll_clk {
	clk_t src;
	clk_t mux;
	uint32_t alt;
	clk_freq_t freq;
	bool enabled;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[SOC_PLL] = {
		.src = soc_vco,
		.mux = soc_clk_sel,
		.alt = _BV(SYSFAB_CLK_SEL) | _BV(SOC_CLK_DIV2_SEL), /* 0x03 */
		.freq = SOC_PLL_FREQ,
		.enabled = false,
	},
	[DDR_BUS_PLL] = {
		.src = ddr_bus_vco,
		.mux = ddr_bus_clk_sel,
		.alt = 0,
		.freq = DDR_BUS_PLL_FREQ,
		.enabled = false,
	},
	[DDR_PLL] = {
		.src = ddr_vco,
		.mux = ddr_clk_sel,
		.alt = 0,
		.freq = DDR_PLL_FREQ,
		.enabled = false,
	},
	[COHFAB_PLL] = {
		.src = cohfab_vco,
		.mux = cohfab_clk_sel,
		.alt = 0,
		.freq = CFAB_PLL_FREQ,
		.enabled = false,
	},
	[CL0_PLL] = {
		.src = cl0_vco,
		.mux = cl0_clk_sel,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL1_PLL] = {
		.src = cl1_vco,
		.mux = cl1_clk_sel,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL2_PLL] = {
		.src = cl2_vco,
		.mux = cl2_clk_sel,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL3_PLL] = {
		.src = cl3_vco,
		.mux = cl3_clk_sel,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[ETH_PLL] = {
		.src = eth_vco,
		.mux = invalid_clk,
		.alt = 0,
		.freq = ETH_PLL_FREQ,
		.enabled = false,
	},
	[SYSFAB_PLL] = {
		.src = soc_vco,
		.mux = sysfab_clk_sel,
		.alt = _BV(SOC_CLK_SEL) | _BV(SOC_CLK_DIV2_SEL), /* 0x21 */
		.freq = SFAB_PLL_FREQ,
		.enabled = false,
	},
	[SGMII_PLL] = {
		.src = eth_vco,
		.mux = invalid_clk,
		.alt = 0,
		.freq = SGMII_PLL_FREQ,
		.enabled = false,
	},
#ifdef CONFIG_DUOWEN_SBI_DUAL
	[SOC_PLL2] = {
		.src = soc_vco2,
		.mux = soc_clk_sel2,
		.alt = _BV(SYSFAB_CLK_SEL) | _BV(SOC_CLK_DIV2_SEL), /* 0x03 */
		.freq = SOC_PLL_FREQ,
		.enabled = false,
	},
	[DDR_BUS_PLL2] = {
		.src = ddr_bus_vco2,
		.mux = ddr_bus_clk_sel2,
		.alt = 0,
		.freq = DDR_BUS_PLL_FREQ,
		.enabled = false,
	},
	[DDR_PLL2] = {
		.src = ddr_vco2,
		.mux = ddr_clk_sel2,
		.alt = 0,
		.freq = DDR_PLL_FREQ,
		.enabled = false,
	},
	[COHFAB_PLL2] = {
		.src = cohfab_vco2,
		.mux = cohfab_clk_sel2,
		.alt = 0,
		.freq = CFAB_PLL_FREQ,
		.enabled = false,
	},
	[CL0_PLL2] = {
		.src = cl0_vco2,
		.mux = cl0_clk_sel2,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL1_PLL2] = {
		.src = cl1_vco2,
		.mux = cl1_clk_sel2,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL2_PLL2] = {
		.src = cl2_vco2,
		.mux = cl2_clk_sel2,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL3_PLL2] = {
		.src = cl3_vco2,
		.mux = cl3_clk_sel,
		.alt = 0,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[ETH_PLL2] = {
		.src = eth_vco2,
		.mux = invalid_clk,
		.alt = 0,
		.freq = ETH_PLL_FREQ,
		.enabled = false,
	},
	[SYSFAB_PLL2] = {
		.src = soc_vco2,
		.mux = sysfab_clk_sel2,
		.alt = _BV(SOC_CLK_SEL) | _BV(SOC_CLK_DIV2_SEL), /* 0x21 */
		.freq = SFAB_PLL_FREQ,
		.enabled = false,
	},
	[SGMII_PLL2] = {
		.src = eth_vco2,
		.mux = invalid_clk,
		.alt = 0,
		.freq = SGMII_PLL_FREQ,
		.enabled = false,
	},
#endif /* CONFIG_DUOWEN_SBI_DUAL */
};

#ifdef CONFIG_CLK_MNEMONICS
const char *pll_clk_names[__NR_PLL_CLKS] = {
	[SOC_PLL] = "soc_pll",
	[DDR_BUS_PLL] = "ddr_bus_pll",
	[DDR_PLL] = "ddr_pll",
	[COHFAB_PLL] = "cohfab_pll",
	[CL0_PLL] = "cl0_pll",
	[CL1_PLL] = "cl1_pll",
	[CL2_PLL] = "cl2_pll",
	[CL3_PLL] = "cl3_pll",
	[ETH_PLL] = "eth_pll",
	[SYSFAB_PLL] = "sysfab_pll",
	[SGMII_PLL] = "sgmii_pll",
};

const char *get_pll_name(clk_clk_t clk)
{
	if (clk >= __NR_PLL_CLKS)
		return NULL;
	return pll_clk_names[clk];
}
#else
#define get_pll_name		NULL
#endif

static void __enable_pll(clk_clk_t pll, clk_clk_t clk, bool force)
{
	bool r = !!(clk >= __DUOWEN_MAX_PLLS);
	clk_clk_t vco = r ? clk - __DUOWEN_MAX_PLLS : clk;
	uint32_t clk_sels = 0;
	uint32_t alt;
	clk_freq_t fvco_orig, fvco, fclk;

	if (!pll_clks[pll].enabled || force) {
		crcntl_trace(true, get_pll_name(clk));
		fclk = pll_clks[pll].freq;
		alt = pll_clks[pll].alt;
		/* XXX: Protect Dynamic PLL Change
		 *
		 * The PLL might be the source of the system clocks (CPU,
		 * BUS, etc.). Since the possible dynamic PLL change may
		 * disable VCO internally to lead to the unpredictable
		 * system hangs, switching to the xo_clk can help to
		 * ensure a safer P/R clkout enabling.
		 */
		clk_deselect_mux(pll_clks[pll].mux);
		/* XXX: Lowest Power Consumption P/R
		 *
		 * Stay sourcing xo_clk to utilize the low power
		 * consumption mode.
		 */
		if (pll_clks[pll].mux != invalid_clk &&
		    alt == 0 && fclk == XO_CLK_FREQ)
			goto exit_xo_clk;
		if (alt) {
			/* XXX: No Cohfab Clock Selection Masking
			 *
			 * To avoid complications, no cohfab clock
			 * selection are masked as alternative masks.
			 */
			clk_sels = crcntl_clk_sel_read();
			crcntl_clk_sel_write(clk_sels | alt);
		}
		fvco_orig = fvco = clk_get_frequency(pll_clks[pll].src);
		if (clk == DDR_PLL)
			fvco = ddr_clk_fvco(fclk, fvco_orig);
		if (fvco != fvco_orig) {
			clk_apply_vco(vco, clk, fvco);
			clk_disable(pll_clks[pll].src);
		}
		clk_enable(pll_clks[pll].src);
		duowen_div_enable(vco, fvco, fclk, r);
		if (alt)
			crcntl_clk_sel_write(clk_sels);
		clk_select_mux(pll_clks[pll].mux);
exit_xo_clk:
		pll_clks[pll].enabled = true;
	}
}

static void __disable_pll(clk_clk_t pll, clk_clk_t clk)
{
	bool r = !!(clk >= __DUOWEN_MAX_PLLS);
	clk_clk_t vco = r ? clk - __DUOWEN_MAX_PLLS : clk;
	uint32_t clk_sels = 0;
	uint32_t alt;

	if (pll_clks[pll].enabled) {
		crcntl_trace(false, get_pll_name(clk));
		alt = pll_clks[pll].alt;
		pll_clks[pll].enabled = false;
		/* XXX: Ensure System Clocking
		 *
		 * The PLL might be the source of the system clocks (CPU,
		 * BUS, etc.). Make sure it's still clocking after
		 * disabling the P/R clkout by switching to the xo_clk.
		 */
		clk_deselect_mux(pll_clks[pll].mux);
		if (alt) {
			/* XXX: No Cohfab Clock Selection Masking
			 *
			 * To avoid complications, no cohfab clock
			 * selection are masked as alternative masks.
			 */
			clk_sels = crcntl_clk_sel_read();
			crcntl_clk_sel_write(clk_sels | alt);
		}
		duowen_div_disable(vco, r);
		/* XXX: Lowest Power Consumption VCO
		 *
		 * And VCO is also disabled when all related P/R outputs
		 * are deselected (clocking with xo_clk).
		 */
		if ((clk_sels & alt) == alt)
			clk_disable(pll_clks[pll].src);
		else if (alt)
			crcntl_clk_sel_write(clk_sels);
	}
}

static int enable_pll(clk_clk_t clk)
{
	if (clk >= __NR_PLL_CLKS)
		return -EINVAL;
	__enable_pll(clk, clk, false);
	return 0;
}

static void disable_pll(clk_clk_t clk)
{
	if (clk >= __NR_PLL_CLKS)
		return;
	__disable_pll(clk, clk);
}

static clk_freq_t get_pll_freq(clk_clk_t clk)
{
	if (clk >= __NR_PLL_CLKS)
		return INVALID_FREQ;
	return pll_clks[clk].freq;
}

static int set_pll_freq(clk_clk_t clk, clk_freq_t freq)
{
	if (clk >= __NR_PLL_CLKS)
		return -EINVAL;

	if (pll_clks[clk].freq != freq) {
		clk_apply_pll(clk, clk, freq);
		__enable_pll(clk, clk, true);
	}
	return 0;
}

const struct clk_driver clk_pll = {
	.max_clocks = __NR_PLL_CLKS,
	.enable = enable_pll,
	.disable = disable_pll,
	.get_freq = get_pll_freq,
	.set_freq = set_pll_freq,
	.select = NULL,
	.get_name = get_pll_name,
};

#ifdef CONFIG_DUOWEN_SBI_DUAL
static void __enable_pll2(clk_clk_t pll, clk_clk_t clk, bool force)
{
	bool r = !!(clk >= __DUOWEN_MAX_PLLS);
	clk_clk_t vco = clk2vco(r ? clk - __DUOWEN_MAX_PLLS : clk, 1);
	uint32_t clk_sels = 0;
	uint32_t alt;
	clk_freq_t fvco_orig, fvco, fclk;

	if (!pll_clks[pll].enabled || force) {
		crcntl_trace(true, get_pll_name(clk));
		fclk = pll_clks[pll].freq;
		alt = pll_clks[pll].alt;
		/* XXX: Protect Dynamic PLL Change
		 *
		 * The PLL might be the source of the system clocks (CPU,
		 * BUS, etc.). Since the possible dynamic PLL change may
		 * disable VCO internally to lead to the unpredictable
		 * system hangs, switching to the xo_clk can help to
		 * ensure a safer P/R clkout enabling.
		 */
		clk_deselect_mux(clkid2(pll_clks[pll].mux));
		/* XXX: Lowest Power Consumption P/R
		 *
		 * Stay sourcing xo_clk to utilize the low power
		 * consumption mode.
		 */
		if (pll_clks[pll].mux != invalid_clk &&
		    alt == 0 && fclk == XO_CLK_FREQ)
			goto exit_xo_clk;
		if (alt) {
			/* XXX: No Cohfab Clock Selection Masking
			 *
			 * To avoid complications, no cohfab clock
			 * selection are masked as alternative masks.
			 */
			clk_sels = __crcntl_clk_sel_read(1);
			__crcntl_clk_sel_write(clk_sels | alt, 1);
		}
		fvco_orig = fvco = clk_get_frequency(clkid2(pll_clks[pll].src));
		if (clk == DDR_PLL)
			fvco = ddr_clk_fvco(fclk, fvco_orig);
		if (fvco != fvco_orig) {
			clk_apply_vco(vco, clk, fvco);
			clk_disable(clkid2(pll_clks[pll].src));
		}
		clk_enable(clkid2(pll_clks[pll].src));
		duowen_div_enable(vco, fvco, fclk, r);
		if (alt)
			__crcntl_clk_sel_write(clk_sels, 1);
		clk_select_mux(clkid2(pll_clks[pll].mux));
exit_xo_clk:
		pll_clks[pll].enabled = true;
	}
}

static void __disable_pll2(clk_clk_t pll, clk_clk_t clk)
{
	bool r = !!(clk >= __DUOWEN_MAX_PLLS);
	clk_clk_t vco = clk2vco(r ? clk - __DUOWEN_MAX_PLLS : clk, 1);
	uint32_t clk_sels = 0;
	uint32_t alt;

	if (pll_clks[pll].enabled) {
		crcntl_trace(false, get_pll_name(clk));
		alt = pll_clks[pll].alt;
		pll_clks[pll].enabled = false;
		/* XXX: Ensure System Clocking
		 *
		 * The PLL might be the source of the system clocks (CPU,
		 * BUS, etc.). Make sure it's still clocking after
		 * disabling the P/R clkout by switching to the xo_clk.
		 */
		clk_deselect_mux(clkid2(pll_clks[pll].mux));
		if (alt) {
			/* XXX: No Cohfab Clock Selection Masking
			 *
			 * To avoid complications, no cohfab clock
			 * selection are masked as alternative masks.
			 */
			clk_sels = __crcntl_clk_sel_read(1);
			__crcntl_clk_sel_write(clk_sels | alt, 1);
		}
		duowen_div_disable(vco, r);
		/* XXX: Lowest Power Consumption VCO
		 *
		 * And VCO is also disabled when all related P/R outputs
		 * are deselected (clocking with xo_clk).
		 */
		if ((clk_sels & alt) == alt)
			clk_disable(clkid2(pll_clks[pll].src));
		else if (alt)
			__crcntl_clk_sel_write(clk_sels, 1);
	}
}

static int enable_pll2(clk_clk_t clk)
{
	clk_clk_t pll = clk2pll(clk, 1);

	if (pll >= NR_PLL_CLKS)
		return -EINVAL;
	__enable_pll2(pll, clk, false);
	return 0;
}

static void disable_pll2(clk_clk_t clk)
{
	clk_clk_t pll = clk2pll(clk, 1);

	if (pll >= NR_PLL_CLKS)
		return;
	__disable_pll2(pll, clk);
}

static clk_freq_t get_pll_freq2(clk_clk_t clk)
{
	clk_clk_t pll = clk2pll(clk, 1);

	if (pll >= NR_PLL_CLKS)
		return INVALID_FREQ;
	return pll_clks[pll].freq;
}

static int set_pll_freq2(clk_clk_t clk, clk_freq_t freq)
{
	clk_clk_t pll = clk2pll(clk, 1);

	if (pll >= NR_PLL_CLKS)
		return -EINVAL;

	if (pll_clks[pll].freq != freq) {
		clk_apply_pll(pll, clk, freq);
		__enable_pll(pll, clk, true);
	}
	return 0;
}

const struct clk_driver clk_pll2 = {
	.max_clocks = __NR_PLL_CLKS,
	.enable = enable_pll2,
	.disable = disable_pll2,
	.get_freq = get_pll_freq2,
	.set_freq = set_pll_freq2,
	.select = NULL,
	.get_name = get_pll_name,
};
#endif /* CONFIG_DUOWEN_SBI_DUAL */

struct vco_clk {
	clk_freq_t freq;
	clk_freq_t freq_p;
	clk_freq_t freq_r;
	bool enabled;
};

struct vco_clk vco_clks[NR_VCO_CLKS] = {
	[SOC_VCO] = {
		.freq = SOC_VCO_FREQ,
		.freq_p = SOC_PLL_FREQ,
		.freq_r = SFAB_PLL_FREQ,
		.enabled = false,
	},
	[DDR_BUS_VCO] = {
		.freq = DDR_BUS_VCO_FREQ,
		.freq_p = DDR_BUS_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[DDR_VCO] = {
		.freq = DDR_VCO_FREQ,
		.freq_p = DDR_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[COHFAB_VCO] = {
		.freq = COHFAB_VCO_FREQ,
		.freq_p = CFAB_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL0_VCO] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL1_VCO] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL2_VCO] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL3_VCO] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[ETH_VCO] = {
		.freq = ETH_VCO_FREQ,
		.freq_p = ETH_PLL_FREQ,
		.freq_r = SGMII_PLL_FREQ,
		.enabled = false,
	},
#ifdef CONFIG_DUOWEN_SBI_DUAL
	[SOC_VCO2] = {
		.freq = SOC_VCO_FREQ,
		.freq_p = SOC_PLL_FREQ,
		.freq_r = SFAB_PLL_FREQ,
		.enabled = false,
	},
	[DDR_BUS_VCO2] = {
		.freq = DDR_BUS_VCO_FREQ,
		.freq_p = DDR_BUS_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[DDR_VCO2] = {
		.freq = DDR_VCO_FREQ,
		.freq_p = DDR_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[COHFAB_VCO2] = {
		.freq = COHFAB_VCO_FREQ,
		.freq_p = CFAB_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL0_VCO2] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL1_VCO2] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL2_VCO2] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[CL3_VCO2] = {
		.freq = CL_VCO_FREQ,
		.freq_p = CL_PLL_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[ETH_VCO2] = {
		.freq = ETH_VCO_FREQ,
		.freq_p = ETH_PLL_FREQ,
		.freq_r = SGMII_PLL_FREQ,
		.enabled = false,
	},
#endif /* CONFIG_DUOWEN_SBI_DUAL */
};

#ifdef CONFIG_CLK_MNEMONICS
const char *vco_clk_names[__NR_VCO_CLKS] = {
	[SOC_VCO] = "soc_vco",
	[DDR_BUS_VCO] = "ddr_bus_vco",
	[DDR_VCO] = "ddr_vco",
	[COHFAB_VCO] = "cohfab_vco",
	[CL0_VCO] = "cl0_vco",
	[CL1_VCO] = "cl1_vco",
	[CL2_VCO] = "cl2_vco",
	[CL3_VCO] = "cl3_vco",
	[ETH_VCO] = "eth_vco",
};

const char *get_vco_name(clk_clk_t clk)
{
	if (clk >= __NR_VCO_CLKS)
		return NULL;
	return vco_clk_names[clk];
}
#else
#define get_vco_name		NULL
#endif

static void __enable_vco(clk_clk_t vco, clk_clk_t clk)
{
	if (!vco_clks[vco].enabled) {
		crcntl_trace(true, get_vco_name());
		duowen_pll_enable2(vco, vco_clks[vco].freq,
				   vco_clks[vco].freq_p,
				   vco_clks[vco].freq_r);
		vco_clks[vco].enabled = true;
	}
}

static void __disable_vco(clk_clk_t vco, clk_clk_t clk)
{
	if (vco_clks[vco].enabled) {
		crcntl_trace(false, get_vco_name(clk));
		vco_clks[vco].enabled = false;
		duowen_pll_disable(vco);
	}
}

static int enable_vco(clk_clk_t clk)
{
	if (clk >= __NR_VCO_CLKS)
		return -EINVAL;
	__enable_vco(clk, clk);
	return 0;
}

static void disable_vco(clk_clk_t clk)
{
	if (clk >= __NR_VCO_CLKS)
		return;
	__disable_vco(clk, clk);
}

static clk_freq_t get_vco_freq(clk_clk_t clk)
{
	if (clk >= __NR_VCO_CLKS)
		return INVALID_FREQ;
	return vco_clks[clk].freq;
}

static int set_vco_freq(clk_clk_t clk, clk_freq_t freq)
{
	if (clk >= __NR_VCO_CLKS)
		return -EINVAL;

	if (vco_clks[clk].freq != freq) {
		__disable_vco(clk, clk);
		clk_apply_vco(clk, clk, freq);
	}
	__enable_vco(clk, clk);
	return 0;
}

const struct clk_driver clk_vco = {
	.max_clocks = __NR_VCO_CLKS,
	.enable = enable_vco,
	.disable = disable_vco,
	.get_freq = get_vco_freq,
	.set_freq = set_vco_freq,
	.select = NULL,
	.get_name = get_vco_name,
};

#ifdef CONFIG_DUOWEN_SBI_DUAL
static int enable_vco2(clk_clk_t clk)
{
	clk_clk_t vco = clk2vco(clk, 1);

	if (vco >= NR_VCO_CLKS)
		return -EINVAL;
	__enable_vco(vco, clk);
	return 0;
}

static void disable_vco2(clk_clk_t clk)
{
	clk_clk_t vco = clk2vco(clk, 1);

	if (vco >= NR_VCO_CLKS)
		return;
	__disable_vco(vco, clk);
}

static clk_freq_t get_vco_freq2(clk_clk_t clk)
{
	clk_clk_t vco = clk2vco(clk, 1);

	if (vco >= NR_VCO_CLKS)
		return INVALID_FREQ;
	return vco_clks[vco].freq;
}

static int set_vco_freq2(clk_clk_t clk, clk_freq_t freq)
{
	clk_clk_t vco = clk2vco(clk, 1);

	if (vco >= NR_VCO_CLKS)
		return -EINVAL;

	if (vco_clks[vco].freq != freq) {
		__disable_vco(vco, clk);
		clk_apply_vco(vco, clk, freq);
	}
	__enable_vco(vco, clk);
	return 0;
}

const struct clk_driver clk_vco2 = {
	.max_clocks = __NR_VCO_CLKS,
	.enable = enable_vco2,
	.disable = disable_vco2,
	.get_freq = get_vco_freq2,
	.set_freq = set_vco_freq2,
	.select = NULL,
	.get_name = get_vco_name,
};
#endif /* CONFIG_DUOWEN_SBI_DUAL */

const uint32_t input_clks[NR_INPUT_CLKS] = {
	[XO_CLK] = XO_CLK_FREQ,
};

#ifdef CONFIG_CLK_MNEMONICS
const char *input_clk_names[NR_INPUT_CLKS] = {
	[XO_CLK] = "xo_clk",
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

const struct clk_driver clk_input = {
	.max_clocks = NR_INPUT_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_input_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_input_clk_name,
};

#ifdef CONFIG_DUOWEN_SBI_DUAL
const struct clk_driver clk_input2 = {
	.max_clocks = NR_INPUT_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_input_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_input_clk_name,
};
#endif /* CONFIG_DUOWEN_SBI_DUAL */

void clk_apply_vco(clk_clk_t vco, clk_clk_t clk, clk_freq_t freq)
{
	if (vco >= NR_VCO_CLKS)
		return;
	vco_clks[vco].freq = freq;
}

void clk_apply_pll(clk_clk_t pll, clk_clk_t clk, clk_freq_t freq)
{
	bool r = !!(clk >= __DUOWEN_MAX_PLLS);
	clk_clk_t vco = clk2vco(r ? clk - __DUOWEN_MAX_PLLS : clk,
			        pll == clk ? 0 : 1);

	if (pll >= NR_PLL_CLKS || vco >= NR_VCO_CLKS)
		return;
	pll_clks[pll].freq = freq;

	/* XXX: Avoid Dynamic PLL Change
	 *
	 * This API should be used right after clk_apply_vco(). The usage
	 * should only affect static PLL changes. So changes P/R clkouts
	 * presets accordingly to avoid dynamic PLL changes in the follow
	 * up clk_enable() invocations.
	 */
	if (r)
		vco_clks[vco].freq_r = freq;
	else
		vco_clks[vco].freq_p = freq;
}

#ifdef CONFIG_CONSOLE_COMMAND
void clk_pll_dump(void)
{
	int i;
	const char *name;

	for (i = 0; i < NR_PLL_CLKS; i++) {
		name = clk_get_mnemonic(clkid(CLK_PLL, i));
		if (name)
			printf("pll  %3d %20s %20s\n", i, name, "xo_clk");
	}
	for (i = 0; i < NR_SELECT_CLKS; i++) {
		if (sel_clk_names[i]) {
			printf("clk  %3d %20s %20s\n",
			       i, sel_clk_names[i],
			       clk_get_mnemonic(select_clks[i].clk_sels[0]));
			if (select_clks[i].clk_sels[1] != invalid_clk)
				printf("%4s %3s %20s %20s\n", "", "", "",
				       clk_get_mnemonic(
					       select_clks[i].clk_sels[1]));
		}
	}
}
#endif

void clk_pll_init(void)
{
	clk_register_driver(CLK_INPUT, &clk_input);
	clk_register_driver(CLK_VCO, &clk_vco);
	clk_register_driver(CLK_PLL, &clk_pll);
	clk_register_driver(CLK_SELECT, &clk_select);
#ifdef CONFIG_DUOWEN_SBI_DUAL
	clk_register_driver(CLK_INPUT2, &clk_input2);
	clk_register_driver(CLK_VCO2, &clk_vco2);
	clk_register_driver(CLK_PLL2, &clk_pll2);
	clk_register_driver(CLK_SELECT2, &clk_select2);
#endif /* CONFIG_DUOWEN_SBI_DUAL */
}
