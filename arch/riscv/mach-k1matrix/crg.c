
/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2024
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
 * @(#)crg.c: k1matrix clock/reset generator implementations
 * $Id: crg.c,v 1.1 2024-06-05 17:37:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/cmdline.h>

#define CRG_4PHASE	_BV(0)
#define CRG_FRAC	_BV(1)
#define CRG_JITTER	_BV(2)
#define CRG_FOUT0	_BV(3)
#define CRG_FOUT1	_BV(4)
#define CRG_ENABLED	_BV(8)

struct dyn_clk {
	clk_t pll0;
	clk_t pll1;
	clk_t clksel;
	uint8_t flags;
};

struct dyn_clk dyn_clks[] = {
	[CPU_CLK] = {
		cpu0_pll_foutpostdiv,
		cpu1_pll_foutpostdiv,
		cpu_clksel,
		0,
	},
	[DDR_CLK] = {
		ddr0_pll_foutpostdiv,
		ddr1_pll_foutpostdiv,
		ddr_sub_clksel,
		0,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *dyn_clk_names[NR_DYN_CLKS] = {
	[DDR_CLK] = "ddr_clk",
	[CPU_CLK] = "cpu_clk",
};

const char *get_dyn_name(clk_clk_t clk)
{
	if (clk >= NR_DYN_CLKS)
		return NULL;
	return dyn_clk_names[clk];
}
#else
#define get_dyn_name		NULL
#endif

static void __enable_dyn(clk_clk_t pll)
{
	if (!(dyn_clks[pll].flags & CRG_ENABLED)) {
		crg_trace(true, get_dyn_name());
		if (!(dyn_clks[pll].flags & CRG_FOUT1)) {
			clk_enable(dyn_clks[pll].pll0);
			clk_select_source(dyn_clks[pll].clksel,
					  dyn_clks[pll].pll0);
		} else {
			clk_enable(dyn_clks[pll].pll1);
			clk_select_source(dyn_clks[pll].clksel,
					  dyn_clks[pll].pll1);
		}
		dyn_clks[pll].flags |= CRG_ENABLED;
	}
}

static int enable_dyn(clk_clk_t pll)
{
	if (pll >= NR_DYN_CLKS)
		return -EINVAL;
	__enable_dyn(pll);
	return 0;
}

static void disable_dyn(clk_clk_t pll)
{
	if (pll >= NR_DYN_CLKS)
		return;
	clk_select_source(dyn_clks[pll].clksel, osc_clk);
	dyn_clks[pll].flags &= ~CRG_ENABLED;
}

static clk_freq_t get_dyn_freq(clk_clk_t pll)
{
	if (pll >= NR_DYN_CLKS)
		return INVALID_FREQ;
	if (!(dyn_clks[pll].flags & CRG_ENABLED))
		return clk_get_frequency(osc_clk);
	else {
		if (!(dyn_clks[pll].flags & CRG_FOUT1))
			return clk_get_frequency(dyn_clks[pll].pll0);
		else
			return clk_get_frequency(dyn_clks[pll].pll1);
	}
}

static int set_dyn_freq(clk_clk_t pll, clk_freq_t freq)
{
	if (pll >= NR_DYN_CLKS)
		return -EINVAL;

	if (!(dyn_clks[pll].flags & CRG_ENABLED)) {
		clk_set_frequency(dyn_clks[pll].pll0, freq);
		dyn_clks[pll].flags |= CRG_ENABLED;
	} else if (dyn_clks[pll].flags & CRG_FOUT1) {
		clk_set_frequency(dyn_clks[pll].pll0, freq);
		clk_select_source(dyn_clks[pll].clksel, 1);
		dyn_clks[pll].flags &= ~CRG_FOUT1;
		clk_disable(dyn_clks[pll].pll1);
	} else {
		clk_set_frequency(dyn_clks[pll].pll1, freq);
		clk_select_source(dyn_clks[pll].clksel, 2);
		dyn_clks[pll].flags |= CRG_FOUT1;
		clk_disable(dyn_clks[pll].pll0);
	}
	return 0;
}

const struct clk_driver clk_dyn = {
	.max_clocks = NR_DYN_CLKS,
	.enable = enable_dyn,
	.disable = disable_dyn,
	.get_freq = get_dyn_freq,
	.set_freq = set_dyn_freq,
	.select = NULL,
	.get_name = get_dyn_name,
};

struct div_clk {
	caddr_t reg;
	uint16_t max_div;
	uint8_t div;
};

struct div_clk div_clks[] = {
	[CPU_NIC_CLKDIV] = {
		.reg = CPU_NIC_CLK_CTL,
		.max_div = 4,
		.div = 2,
	},
	[CPU_HAP_CLKDIV] = {
		.reg = CPU_HAP_CLK_CTL,
		.max_div = 4,
		.div = 2,
	},
	[PCIE_TOP_AUX_CLKDIV] = {
		.reg = PCIE_TOP_AUXCLK_CTL,
		.max_div = 256,
		.div = 64,
	},
	[PCIE_TOP_CFG_CLKDIV] = {
		.reg = PCIE_TOP_CFGCLK_CTL,
		.max_div = 256,
		.div = 64,
	},
	[PCIE_BOT_CFG_CLKDIV] = {
		.reg = PCIE_BOT_CFGCLK_CTL,
		.max_div = 256,
		.div = 64,
	},
	[PCIE_BOT_AUX_CLKDIV] = {
		.reg = PCIE_BOT_AUXCLK_CTL,
		.max_div = 256,
		.div = 64,
	},

};

const struct clk_driver clk_div = {
	.max_clocks = NR_SEL_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_sel_freq,
	.set_freq = NULL,
	.select = select_sel_source,
	.get_name = get_sel_name,
};

struct sel_clk {
	caddr_t reg;
	clk_t *clksels;
	uint8_t nr_clksels;
	uint8_t sel;
};

clk_t ddr_sub_clksels[] = {
	osc_clk,
	ddr0_pll_foutpostdiv,
	ddr1_pll_foutpostdiv,
};

clk_t cpu_clksels[] = {
	osc_clk,
	cpu0_pll_foutpostdiv,
	cpu1_pll_foutpostdiv,
};

clk_t cpu_nic_clksels[] = {
	osc_clk,
	cpu0_pll_foutpostdiv,
	cpu1_pll_foutpostdiv,
};

clk_t pcie_peri_xclksels[] = {
	osc_clk,
	peri_pll_foutpostdiv,
};

clk_t pcie_com_xclksels[] = {
	osc_clk,
	com_pll_foutpostdiv,
};

struct sel_clk sel_clks[NR_SEL_CLKS] = {
	[DDR_SUB_CLKSEL] = {
		.reg = DDR_SUB_CLK_CTL,
		.clksels = ddr_sub_clksels,
		.nr_clksels = 3,
		.sel = 0,
	},
	[CPU_CLKSEL] = {
		.reg = CPU_CLK_CTL,
		.clksels = cpu_clksels,
		.nr_clksels = 3,
		.sel = 0,
	},
	[PCIE_TOP_CFG_CLKSEL] = {
		.clksels = pcie_com_xclksels,
		.reg = PCIE_TOP_CLK_CTL,
		.nr_clksels = 2,
		.sel = 0,
	},
	[PCIE_TOP_AUX_CLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = PCIE_TOP_AUXCLK_CTL,
		.nr_clksels = 2,
		.sel = 0,
	},
	[PCIE_TOP_XCLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = ,
		.nr_clksels = 2,
		.sel = 0,
	},
	[PCIE_BOT_CFG_CLKSEL] = {
		.clksels = pcie_com_xclksels,
		.reg = PCIE_BOT_CLK_CTL,
		.nr_clksels = 2,
		.sel = 0,
	},
	[PCIE_BOT_AUX_CLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = PCIE_BOT_AUXCLK_CTL,
		.nr_clksels = 2,
		.sel = 0,
	},
	[PCIE_BOT_XCLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = ,
		.nr_clksels = 2,
		.sel = 0,
	},
	[PCIE_SUB_CLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = PCIE_BOT_AUXCLK_CTL,
		.nr_clksels = 2,
		.sel = 0,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *sel_clk_names[NR_SEL_CLKS] = {
	[MESH_SUB_CLKSEL] = "mesh_sub_clksel",
	[DDR_SUB_CLKSEL] = "ddr_sub_clksel",
	[CPU_CLKSEL] = "cpu_clksel",
};

static const char *get_sel_name(clk_clk_t sel)
{
	if (sel >= NR_SEL_CLKS)
		return NULL;
	return sel_clk_names[sel];
}
#else
#define get_sel_name		NULL
#endif

static clk_freq_t get_sel_freq(clk_clk_t sel)
{
	if (sel >= NR_SEL_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(sel_clks[sel].clksels[sel_clks[sel].sel]);
}

static void select_sel_source(clk_clk_t sel, clk_t src)
{
	uint8_t clksel;

	if (sel >= NR_SEL_CLKS)
		return;
	for (clksel = 0; clksel < sel_clks[sel].nr_clksels; clksel++) {
		if (src == sel_clks[sel].clksels[clksel])
			break;
	}
	if (clksel == sel_clks[sel].nr_clksels)
		return;
	if (sel_clks[sel].sel != clksel) {
		clk_enable(src);
		__raw_writel_mask(CRG_CLKSEL(clksel),
				  CRG_CLKSEL(CRG_CLKSEL_MASK),
				  sel_clks[sel].reg);
	}
}

const struct clk_driver clk_sel = {
	.max_clocks = NR_SEL_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_sel_freq,
	.set_freq = NULL,
	.select = select_sel_source,
	.get_name = get_sel_name,
};


struct pll_clk {
	uint32_t Fref;
	uint32_t Fvco;
	uint32_t Fout;
	uint8_t flags;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[COM_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = COM_PLL_FREQ,
		.Fout = COM_PLL_FREQ,
		.flags = CRG_4PHASE,
	},
	[MESH_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = MESH_PLL_FREQ,
		.Fout = MESH_PLL_FREQ,
		.flags = 0,
	},
	[PERI_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = PERI_PLL_FREQ,
		.Fout = PERI_PLL_FREQ,
		.flags = CRG_4PHASE,
	},
	[DDR0_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = DDR0_PLL_FREQ,
		.Fout = DDR0_PLL_FREQ,
		.flags = CRG_FOUT0,
	},
	[DDR1_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = DDR1_PLL_FREQ,
		.Fout = DDR1_PLL_FREQ,
		.flags = CRG_FOUT1,
	},
	[CPU0_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = CPU0_PLL_FREQ,
		.Fout = CPU0_PLL_FREQ,
		.flags = CRG_FOUT0,
	},
	[CPU1_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = CPU1_PLL_FREQ,
		.Fout = CPU1_PLL_FREQ,
		.flags = CRG_FOUT1,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *pll_clk_names[NR_PLL_CLKS] = {
	[COM_PLL] = "com_pll_foutpostdiv",
	[MESH_PLL] = "mesh_pll_foutpostdiv",
	[PERI_PLL] = "peri_pll_foutpostdiv",
	[DDR0_PLL] = "ddr0_pll_foutpostdiv",
	[DDR1_PLL] = "ddr1_pll_foutpostdiv",
	[CPU0_PLL] = "cpu0_pll_foutpostdiv",
	[CPU1_PLL] = "cpu1_pll_foutpostdiv",
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

static void __enable_pll(clk_clk_t pll)
{
	if (!(pll_clks[pll].flags & CRG_ENABLED)) {
		crg_trace(true, get_pll_name());
		sc_pllts12ffclafrac2_enable(pll,
			!!(pll_clks[pll].flags & CRG_4PHASE),
			pll_clks[pll].Fref,
			pll_clks[pll].Fvco,
			pll_clks[pll].Fout);
		pll_clks[pll].flags |= CRG_ENABLED;
	}
}

static void __disable_pll(clk_clk_t pll)
{
	if (pll_clks[pll].flags & CRG_ENABLED) {
		crg_trace(false, get_pll_name(clk));
		pll_clks[pll].flags &= ~CRG_ENABLED;
		sc_pllts12ffclafrac2_disable(pll);
	}
}

static int enable_pll(clk_clk_t pll)
{
	if (pll >= NR_PLL_CLKS)
		return -EINVAL;
	__enable_pll(pll);
	return 0;
}

static void disable_pll(clk_clk_t pll)
{
	if (pll >= NR_PLL_CLKS)
		return;
	__disable_pll(pll);
}

static clk_freq_t get_pll_freq(clk_clk_t pll)
{
	if (pll >= NR_PLL_CLKS)
		return INVALID_FREQ;
	return pll_clks[pll].Fout;
}

static int set_pll_freq(clk_clk_t pll, clk_freq_t freq)
{
	if (pll >= NR_PLL_CLKS)
		return -EINVAL;

	if (pll_clks[pll].Fout != freq) {
		__disable_pll(pll);
		pll_clks[pll].Fout = freq;
		pll_clks[pll].Fvco = sc_pllts12ffclafrac2_recalc(
			pll, pll_clks[pll].Fref, pll_clks[pll].Fout);
	}
	__enable_pll(pll);
	return 0;
}

const struct clk_driver clk_pll = {
	.max_clocks = NR_PLL_CLKS,
	.enable = enable_pll,
	.disable = disable_pll,
	.get_freq = get_pll_freq,
	.set_freq = set_pll_freq,
	.select = NULL,
	.get_name = get_pll_name,
};

const uint32_t input_clks[NR_INPUT_CLKS] = {
	[OSC_CLK] = OSC_CLK_FREQ,
};

#ifdef CONFIG_CLK_MNEMONICS
const char *input_clk_names[NR_INPUT_CLKS] = {
	[OSC_CLK] = "osc_clk",
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

/*===========================================================================
 * CRG TRACE APIs
 *===========================================================================*/
#ifdef CONFIG_K1MATRIX_CRG_TRACE
void crg_trace(bool enabling, const char *name)
{
	con_dbg("crg: %c %s\n", enabling ? 'E' : 'D', name);
}
#endif

bool k1matrix_clk_initialized;

void k1matrix_clk_init(void)
{
	if (!k1matrix_clk_initialized) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_PLL, &clk_pll);
		clk_register_driver(CLK_DYN, &clk_dyn);
		k1matrix_clk_initialized = true;
	}
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}

#ifdef CONFIG_CONSOLE_COMMAND
static void clk_dyn_dump(void)
{
	int i;
	const char *name;

	for (i = 0; i < NR_DYN_CLKS; i++) {
		name = clk_get_mnemonic(clkid(CLK_DYN, i));
		if (name)
			printf("pll  %3d %20s %20s\n", i, name,
			       (dyn_clks[i].flags & CRG_FOUT1) ?
			       clk_get_mnemonic(dyn_clks[i].pll1) :
			       clk_get_mnemonic(dyn_clks[i].pll0));
	}
}

static void clk_pll_dump(void)
{
	int i;
	const char *name;

	for (i = 0; i < NR_PLL_CLKS; i++) {
		name = clk_get_mnemonic(clkid(CLK_PLL, i));
		if (name)
			printf("pll  %3d %20s %20s\n", i, name, "osc_clk");
	}
}

#if 0
static void clk_mux_dump(void)
{
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

static void clk_out_dump(void)
{
	int i;

	for (i = 0; i < NR_OUTPUT_CLKS; i++) {
		if (output_clk_names[i] &&
		    output_clks[i].flags & CLK_CLK_EN_F) {
			printf("clk  %3d %20s %20s\n",
			       i, output_clk_names[i],
			       clk_get_mnemonic(output_clks[i].clk_src));
			if (output_clks[i].clk_dep != invalid_clk)
				printf("%4s %3s %20s %20s\n", "", "", "",
				       clk_get_mnemonic(
					       output_clks[i].clk_dep));
		}
	}
}

static void clk_div_dump(void)
{
	int i;

	for (i = 0; i < NR_DIV_CLKS; i++) {
		if (div_clk_names[i]) {
			printf("div  %3d %20s %20s\n",
			       i, div_clk_names[i],
			       clk_get_mnemonic(div_clks[i].src));
		}
	}
}
#endif

static int do_crg_dump(int argc, char *argv[])
{
	printf("type id  %20s %20s\n", "name", "source");
	clk_pll_dump();
	clk_dyn_dump();
	return 0;
}
#else
static inline int do_crg_dump(int argc, char *argv[])
{
	return 0;
}
#endif

static int do_crg(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_crg_dump(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(crg, do_crg, "Clock/reset generator (CRG)",
	"crg dump\n"
	"    -display clock tree source multiplexing\n"
);
