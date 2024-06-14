
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
 * @(#)cru.c: k1matrix clock/reset generator implementations
 * $Id: cru.c,v 1.1 2024-06-05 17:37:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/cmdline.h>

#define CRU_4PHASE	_BV(0)
#define CRU_FRAC	_BV(1)
#define CRU_JITTER	_BV(2)
#define CRU_FOUT0	_BV(3)
#define CRU_FOUT1	_BV(4)
#define CRU_CLKEN	_BV(5)
#define CRU_SRC_CLKSEL	_BV(6)
#define CRU_CLKEN_2BIT	_BV(7)
#define CRU_ENABLED	_BV(8)
#define CRU_RESET	_BV(9)
#define CRU_RESET_2BIT	_BV(10)
#define CRU_RESET_3BIT	_BV(11)

typedef uint16_t cru_flags_t;

struct pll_clk {
	uint32_t Fout;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[COM_PLL] = {
		.Fout = COM_PLL_FREQ,
	},
	[MESH_PLL] = {
		.Fout = MESH_PLL_FREQ,
	},
	[PERI_PLL] = {
		.Fout = PERI_PLL_FREQ,
	},
	[DDR0_PLL] = {
		.Fout = DDR0_PLL_FREQ,
	},
	[DDR1_PLL] = {
		.Fout = DDR1_PLL_FREQ,
	},
	[CPU0_PLL] = {
		.Fout = CPU0_PLL_FREQ,
	},
	[CPU1_PLL] = {
		.Fout = CPU1_PLL_FREQ,
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
	[COM_FOUT1PH0] = "com_pll_fout1ph0",
	[PERI_FOUT1PH0] = "peri_pll_fout1ph0"
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

static clk_freq_t get_pll_freq(clk_clk_t pll)
{
	if (pll >= NR_PLL_CLKS)
		return INVALID_FREQ;
	return pll_clks[pll].Fout;
}

const struct clk_driver clk_pll = {
	.max_clocks = NR_PLL_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_pll_freq,
	.set_freq = NULL,
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
 * CRU TRACE APIs
 *===========================================================================*/
#ifdef CONFIG_K1MATRIX_CRU_TRACE
void cru_trace(bool enabling, const char *name)
{
	con_dbg("cru: %c %s\n", enabling ? 'E' : 'D', name);
}
#endif

bool k1matrix_clk_initialized;

void k1matrix_clk_init(void)
{
	if (!k1matrix_clk_initialized) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_PLL, &clk_pll);
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
			       (dyn_clks[i].flags & CRU_FOUT1) ?
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

static int do_cru_dump(int argc, char *argv[])
{
	printf("type id  %20s %20s\n", "name", "source");
	clk_pll_dump();
	return 0;
}
#else
static inline int do_cru_dump(int argc, char *argv[])
{
	return 0;
}
#endif

static int do_cru(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_cru_dump(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(cru, do_cru, "Clock/reset generator (CRU)",
	"cru dump\n"
	"    -display clock tree source multiplexing\n"
);
