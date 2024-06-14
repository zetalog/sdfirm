
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

struct rstn_clk {
	caddr_t rst_reg;
	cru_flags_t flags;
};

struct rstn_clk rstn_clks[] = {
	[CPU_SUB_RSTN] = {
		.rst_reg = CRU_CPU_SUB_SW_RESET,
		.flags = CRU_RESET,	
	},
	[PCIE_TOP_RSTN] = {
		.rst_reg = CRU_PCIE_TOP_SW_RESET,
		.flags = CRU_RESET,	
	},
	[PCIE_BOT_RSTN] = {
		.rst_reg = CRU_PCIE_BOT_SW_RESET,
		.flags = CRU_RESET,	
	},
	[PERI_SUB_RSTN] = {
		.rst_reg = CRU_PERI_SUB_SW_RESET,
		.flags = CRU_RESET,	
	},
	[MESH_SUB_RSTN] = {
		.rst_reg = CRU_MESH_SUB_SW_RESET,
		.flags = CRU_RESET,	
	},
	[DDR_SUB_RSTN] = {
		.rst_reg = CRU_DDR_SUB_SW_RESET,
		.flags = CRU_RESET,	
	},
	[RAS_SRST_N] = {
		.rst_reg = CRU_CPU_RAS_SW_RESET,
		.flags = CRU_RESET,	
	},
	[CPU_SUB_SRST_N] = {
		.rst_reg = CRU_CPU_SW_RESET,
		.flags = CRU_RESET,	
	},
	[RMU_SRAM_SW_RSTN] = {
		.rst_reg = CRU_RMU_SRAM_SW_RSTN,
		.flags = CRU_RESET,	
	},
	[PCIE0_PERST_N] = {
		.rst_reg = CRU_PCIE0_SW_RESET,
		.flags = CRU_RESET_2BIT,	
	},
	[PCIE1_PERST_N] = {
		.rst_reg = CRU_PCIE1_SW_RESET,
		.flags = CRU_RESET_2BIT,	
	},
	[PCIE2_PERST_N] = {
		.rst_reg = CRU_PCIE2_SW_RESET,
		.flags = CRU_RESET_3BIT,	
	},
	[PCIE3_PERST_N] = {
		.rst_reg = CRU_PCIE3_SW_RESET,
		.flags = CRU_RESET_3BIT,	
	},
	[PCIE4_PERST_N] = {
		.rst_reg = CRU_PCIE4_SW_RESET,
		.flags = CRU_RESET_3BIT,	
	},
	[PCIE5_PERST_N] = {
		.rst_reg = CRU_PCIE5_SW_RESET,
		.flags = CRU_RESET_2BIT,	
	},
	[PCIE6_PERST_N] = {
		.rst_reg = CRU_PCIE6_SW_RESET,
		.flags = CRU_RESET_2BIT,	
	},
	[PCIE7_PERST_N] = {
		.rst_reg = CRU_PCIE7_SW_RESET,
		.flags = CRU_RESET_2BIT,	
	},
	[PCIE8_PERST_N] = {
		.rst_reg = CRU_PCIE8_SW_RESET,
		.flags = CRU_RESET_2BIT,	
	},
	[PCIE8_PERST_N] = {
		.rst_reg = CRU_PCIE9_SW_RESET,
		.flags = CRU_RESET_2BIT,	
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *rstn_clk_names[NR_RSTN_CLKS] = {
	[CPU_SUB_RSTN] = "cpu_sub_rstn",
	[PCIE_TOP_RSTN] = "pcie_top_rstn",
	[PCIE_BOT_RSTN] = "pcie_bot_rstn",
	[PERI_SUB_RSTN] = "peri_sub_rstn",
	[MESH_SUB_RSTN] = "mesh_sub_rstn",
	[DDR_SUB_RSTN] = "ddr_sub_rstn",
	//CLUSTER0 ~ CLUSTER7
	[RAS_SRST_N] = "ras_srst_n",
	[CPU_SUB_SRST_N] = "cpu_sub_srst_n",
	[RMU_SRAM_SW_RSTN] = "rmu_sram_sw_rstn",
	//PCIE0_PERST_N ~ PCIE9_PERST_N 
	[PCIE0_PERST_N] = "pcie0_perst_n",
	[PCIE1_PERST_N] = "pcie1_perst_n",
	[PCIE2_PERST_N] = "pcie2_perst_n",
	[PCIE3_PERST_N] = "pcie3_perst_n",
	[PCIE4_PERST_N] = "pcie4_perst_n",
	[PCIE5_PERST_N] = "pcie5_perst_n",
	[PCIE6_PERST_N] = "pcie6_perst_n",
	[PCIE7_PERST_N] = "pcie7_perst_n",
	[PCIE8_PERST_N] = "pcie8_perst_n",
	[PCIE9_PERST_N] = "pcie9_perst_n",

};

const char *get_rstn_name(clk_clk_t clk)
{
	if (clk >= NR_RSTN_CLKS)
		return NULL;
	return rstn_clk_names[clk];
}
#else
#define get_rstn_name		NULL
#endif

const struct clk_driver clk_rstn = {
	.max_clocks = NR_RSTN_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = NULL,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_rstn_name,
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
		clk_register_driver(CLK_RSTN, &clk_rstn);
		k1matrix_clk_initialized = true;
	}
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}

#ifdef CONFIG_CONSOLE_COMMAND
static void clk_rstn_dump(void)
{
	int i;
	const char *name;

	for( i = 0; i < NR_RSTN_CLKS; i++) {
		name = clk_get_mnemonic(clkid(CLK_RSTN, i));
		if(name)
			printf("rstn	%3d %20s\n", i, name);
	}
}

#if 0
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
	clk_rstn_dump();
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
