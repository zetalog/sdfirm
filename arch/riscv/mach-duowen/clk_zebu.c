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
 * @(#)clk_socv2_zebu.c: DUOWEN SoCv2 ZEBU frequency plan implementation
 * $Id: clk_socv2_zebu.c,v 1.1 2020-10-27 10:26:00 zhenglv Exp $
 */

#include <target/clk.h>

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
	},
	[SYSFAB_CLK_SEL] = {
		.clk_sels = {
			sysfab_pll,
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
	[SOC_CLK_SEL] = {
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
	},
	[COHFAB_CLK_SEL] = {
		.clk_sels = {
			cohfab_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F)
	},
	[CL0_CLK_SEL] = {
		.clk_sels = {
			cl0_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F)
	},
	[CL1_CLK_SEL] = {
		.clk_sels = {
			cl1_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F)
	},
	[CL2_CLK_SEL] = {
		.clk_sels = {
			cl2_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F)
	},
	[CL3_CLK_SEL] = {
		.clk_sels = {
			cl3_pll,
			xo_clk,
		},
		CLK_DEF_FLAGS(CLK_COHFAB_CFG_F)
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
	crcntl_trace(true, get_clk_sel_name(clk));
	if (!(clk_read_flags(0, select_clks[clk]) & CLK_CLK_SEL_F)) {
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
	crcntl_trace(false, get_clk_sel_name(clk));
	if (clk_read_flags(0, select_clks[clk]) & CLK_CLK_SEL_F) {
		/* The PLL select clocks on ZeBu are used to gate the PLLs.
		 * Since there is no control over PLL clocks, the select
		 * clocks are programmed as the child nodes of the PLL
		 * clocks, and should always be enabled by the child nodes
		 * of the PLL clocks.
		 */
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

struct clk_driver clk_select = {
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
	crcntl_trace(true, get_clk_sel_name(clk));
	if (!(clk_read_flags(1, select_clks[clk]) & CLK_CLK_SEL_F)) {
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
	crcntl_trace(false, get_clk_sel_name(clk));
	if (clk_read_flags(1, select_clks[clk]) & CLK_CLK_SEL_F) {
		/* The PLL select clocks on ZeBu are used to gate the PLLs.
		 * Since there is no control over PLL clocks, the select
		 * clocks are programmed as the child nodes of the PLL
		 * clocks, and should always be enabled by the child nodes
		 * of the PLL clocks.
		 */
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

struct clk_driver clk_select2 = {
	.max_clocks = NR_SELECT_CLKS,
	.enable = enable_clk_sel2,
	.disable = disable_clk_sel2,
	.get_freq = get_clk_sel_freq2,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_clk_sel_name,
};
#endif /* CONFIG_DUOWEN_SBI_DUAL */

uint32_t input_clks[NR_INPUT_CLKS] = {
	[XO_CLK] = XO_CLK_FREQ,
	[SOC_PLL] = SOC_PLL_FREQ,
	[DDR_BUS_PLL] = DDR_BUS_PLL_FREQ,
	[DDR_PLL] = DDR_PLL_FREQ,
	[COHFAB_PLL] = CFAB_PLL_FREQ,
	[CL0_PLL] = CL_PLL_FREQ,
	[CL1_PLL] = CL_PLL_FREQ,
	[CL2_PLL] = CL_PLL_FREQ,
	[CL3_PLL] = CL_PLL_FREQ,
	[ETH_PLL] = ETH_PLL_FREQ,
	[SYSFAB_PLL] = SFAB_PLL_FREQ,
	[SGMII_PLL] = SGMII_PLL_FREQ,
};

#ifdef CONFIG_CLK_MNEMONICS
const char *input_clk_names[NR_INPUT_CLKS] = {
	[XO_CLK] = "xo_clk",
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

#ifdef CONFIG_DUOWEN_SBI_DUAL
struct clk_driver clk_input2 = {
	.max_clocks = NR_INPUT_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_input_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_input_clk_name,
};
#endif /* CONFIG_DUOWEN_SBI_DUAL */

#ifdef CONFIG_CONSOLE_COMMAND
void clk_pll_dump(void)
{
	int i;

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
	clk_register_driver(CLK_SELECT, &clk_select);
#ifdef CONFIG_DUOWEN_SBI_DUAL
	clk_register_driver(CLK_INPUT2, &clk_input2);
	clk_register_driver(CLK_SELECT2, &clk_select2);
#endif /* CONFIG_DUOWEN_SBI_DUAL */
}
