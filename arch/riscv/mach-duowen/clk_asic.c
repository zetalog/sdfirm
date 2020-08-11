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
 * @(#)clk_asic.c: DUOWEN ASIC frequency plan implementation
 * $Id: clk_asic.c,v 1.1 2020-06-30 14:31:00 zhenglv Exp $
 */

#include <target/clk.h>

struct select_clk {
	clk_t clk_sels[2];
	uint8_t flags;
};

struct select_clk select_clks[] = {
	[SOC_CLK_SEL] = {
		.clk_sels = {
			soc_pll_div4,
			xo_clk,
		},
	},
	[DDR_BUS_CLK_SEL] = {
		.clk_sels = {
			ddr_bus_pll,
			xo_clk,
		},
	},
	[DDR_CLK_SEL] = {
		.clk_sels = {
			ddr_pll,
			xo_clk,
		},
	},
	[DDR_CLK_DIV4_SEL] = {
		.clk_sels = {
			ddr_clk_sel,
			ddr_clk_sel_div4,
		},
		.flags = CLK_HOMOLOG_SRC_F,
	},
#if 0
	[SD_RX_TX_SEL] = {
		.clk_sels = {
			sd_hw_clk,
			sd_sw_clk,
		},
	},
#endif
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *sel_clk_names[NR_DIV_CLKS] = {
	[SOC_CLK_SEL] = "soc_clk_sel",
	[DDR_BUS_CLK_SEL] = "ddr_bus_clk_sel",
	[DDR_CLK_SEL] = "ddr_clk_sel",
	[DDR_CLK_DIV4_SEL] = "ddr_clk_div4_sel",
	[SD_RX_TX_CLK_SEL] = "sd_rx_tx_clk_sel",
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
	crcntl_trace(true, get_clk_sel_name(clk));
	if (!(select_clks[clk].flags & CLK_CLK_SEL_F)) {
		clk_enable(select_clks[clk].clk_sels[0]);
		crcntl_clk_select(clk);
		if (select_clks[clk].flags & CLK_CLK_EN_F) {
			if (!(select_clks[clk].flags & CLK_HOMOLOG_SRC_F))
				clk_disable(select_clks[clk].clk_sels[1]);
		} else
			select_clks[clk].flags |= CLK_CLK_EN_F;
		select_clks[clk].flags |= CLK_CLK_SEL_F;
	}
	return 0;
}

static void disable_clk_sel(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return;
	crcntl_trace(false, get_clk_sel_name(clk));
	if (select_clks[clk].flags & CLK_CLK_SEL_F) {
		clk_enable(select_clks[clk].clk_sels[1]);
		crcntl_clk_deselect(clk);
		if (select_clks[clk].flags & CLK_CLK_EN_F) {
			if (!(select_clks[clk].flags & CLK_HOMOLOG_SRC_F))
				clk_disable(select_clks[clk].clk_sels[0]);
		} else
			select_clks[clk].flags |= CLK_CLK_EN_F;
		select_clks[clk].flags &= ~CLK_CLK_SEL_F;
	}
}

static clk_freq_t get_clk_sel_freq(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return INVALID_FREQ;
	if (crcntl_clk_selected(clk))
		return clk_get_frequency(select_clks[clk].clk_sels[0]);
	else
		return clk_get_frequency(select_clks[clk].clk_sels[1]);
}

struct clk_driver clk_select = {
	.max_clocks = NR_SELECT_CLKS,
	.enable = enable_clk_sel,
	.disable = disable_clk_sel,
	.get_freq = get_clk_sel_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_clk_sel_name,
};

struct pll_clk {
	clk_t src;
	clk_freq_t freq;
	bool enabled;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[SOC_PLL] = {
		.src = soc_vco,
		.freq = SOC_PLL_FREQ,
		.enabled = false,
	},
	[DDR_BUS_PLL] = {
		.src = ddr_bus_vco,
		.freq = DDR_BUS_PLL_FREQ,
		.enabled = false,
	},
	[DDR_PLL] = {
		.src = ddr_vco,
		.freq = DDR_PLL_FREQ,
		.enabled = false,
	},
	[PCIE_PLL] = {
		.src = pcie_vco,
		.freq = PCIE_PLL_FREQ,
		.enabled = false,
	},
	[COHFAB_PLL] = {
		.src = cohfab_vco,
		.freq = CFAB_PLL_FREQ,
		.enabled = false,
	},
	[CL0_PLL] = {
		.src = cl0_vco,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL1_PLL] = {
		.src = cl1_vco,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL2_PLL] = {
		.src = cl2_vco,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
	[CL3_PLL] = {
		.src = cl3_vco,
		.freq = CL_PLL_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *pll_clk_names[NR_PLL_CLKS] = {
	[SOC_PLL] = "soc_pll",
	[DDR_BUS_PLL] = "ddr_bus_pll",
	[DDR_PLL] = "ddr_pll",
	[PCIE_PLL] = "pcie_pll",
	[COHFAB_PLL] = "cohfab_pll",
	[CL0_PLL] = "cl0_pll",
	[CL1_PLL] = "cl1_pll",
	[CL2_PLL] = "cl2_pll",
	[CL3_PLL] = "cl3_pll",
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
	if (!pll_clks[clk].enabled) {
		clk_enable(pll_clks[clk].src);
		crcntl_div_enable(clk,
				  clk_get_frequency(pll_clks[clk].src),
				  pll_clks[clk].freq, false);
		pll_clks[clk].enabled = true;
	}
}

static void __disable_pll(clk_clk_t clk)
{
	if (pll_clks[clk].enabled) {
		pll_clks[clk].enabled = false;
		crcntl_div_disable(clk, false);
		clk_disable(pll_clks[clk].src);
	}
}

static int enable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return -EINVAL;
	crcntl_trace(true, get_pll_name(clk));
	__enable_pll(clk);
	return 0;
}

static void disable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return;
	crcntl_trace(false, get_pll_name(clk));
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
	if (clk >= NR_PLL_CLKS)
		return -EINVAL;

	if (pll_clks[clk].freq != freq) {
		__disable_pll(clk);
		pll_clks[clk].freq = freq;
	}
	__enable_pll(clk);
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
	[SOC_VCO] = {
		.freq = SOC_VCO_FREQ,
		.enabled = false,
	},
	[DDR_BUS_VCO] = {
		.freq = DDR_BUS_VCO_FREQ,
		.enabled = false,
	},
	[DDR_VCO] = {
		.freq = DDR_VCO_FREQ,
		.enabled = false,
	},
	[PCIE_VCO] = {
		.freq = PCIE_VCO_FREQ,
		.enabled = false,
	},
	[COHFAB_VCO] = {
		.freq = COHFAB_VCO_FREQ,
		.enabled = false,
	},
	[CL0_VCO] = {
		.freq = CL_VCO_FREQ,
		.enabled = false,
	},
	[CL1_VCO] = {
		.freq = CL_VCO_FREQ,
		.enabled = false,
	},
	[CL2_VCO] = {
		.freq = CL_VCO_FREQ,
		.enabled = false,
	},
	[CL3_VCO] = {
		.freq = CL_VCO_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *vco_clk_names[NR_VCO_CLKS] = {
	[SOC_VCO] = "soc_vco",
	[DDR_BUS_VCO] = "ddr_bus_vco",
	[DDR_VCO] = "ddr_vco",
	[PCIE_VCO] = "pcie_vco",
	[COHFAB_VCO] = "cohfab_vco",
	[CL0_VCO] = "cl0_vco",
	[CL1_VCO] = "cl1_vco",
	[CL2_VCO] = "cl2_vco",
	[CL3_VCO] = "cl3_vco",
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
		crcntl_pll_enable(clk, vco_clks[clk].freq);
		vco_clks[clk].enabled = true;
	}
}

static void __disable_vco(clk_clk_t clk)
{
	if (vco_clks[clk].enabled) {
		vco_clks[clk].enabled = false;
		crcntl_pll_disable(clk);
	}
}

static int enable_vco(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return -EINVAL;
	crcntl_trace(true, get_vco_name(clk));
	__enable_vco(clk);
	return 0;
}

static void disable_vco(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return;
	crcntl_trace(false, get_vco_name(clk));
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
	[XO_CLK] = XO_CLK_FREQ,
};

#ifdef CONFIG_CONSOLE_COMMAND
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

struct clk_driver clk_input = {
	.max_clocks = NR_INPUT_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_input_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_input_clk_name,
};

void clk_apply_vco(clk_clk_t clk, clk_freq_t freq)
{
	if (clk >= NR_VCO_CLKS)
		return;
	vco_clks[clk].freq = freq;
}

void clk_apply_pll(clk_clk_t clk, clk_freq_t freq)
{
	if (clk >= NR_PLL_CLKS)
		return;
	pll_clks[clk].freq = freq;
}

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

void clk_pll_init(void)
{
	clk_register_driver(CLK_INPUT, &clk_input);
	clk_register_driver(CLK_VCO, &clk_vco);
	clk_register_driver(CLK_PLL, &clk_pll);
	clk_register_driver(CLK_SELECT, &clk_select);
}
