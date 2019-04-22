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
 * @(#)clk.c: QDF2400 specific clock tree framework driver
 * $Id: clk.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <errno.h>
#include <target/clk.h>
#include <target/arch.h>

#ifdef CONFIG_QDF2400_CLK_DEF_PERIPH_BLSP_FREQ
#define FREQ_PERIPH_BLSP0_SPI0	20000000
#define FREQ_PERIPH_BLSP0_SPI1	25806000
#define FREQ_PERIPH_BLSP_UART	1840000
#else
#define FREQ_PERIPH_BLSP0_SPI0	FREQ_BLSP_SPI
#define FREQ_PERIPH_BLSP0_SPI1	FREQ_BLSP_SPI
#define FREQ_PERIPH_BLSP_UART	FREQ_BLSP_UART
#endif

uint32_t padring_clks[] = {
	[PLL0_REF_CLK] = FREQ_PLL0_REF_CLK,
	[PLL1_REF_CLK] = FREQ_PLL0_REF_CLK,
	[PLL2_REF_CLK] = FREQ_PLL0_REF_CLK,
	[PLL_TEST_SE] = FREQ_PLL_TEST_SE,
	[WEST_XO] = FREQ_WEST_XO,
	[EAST_XO] = FREQ_EAST_XO,
	[SLEEP_CLK] = FREQ_SLEEP_CLK,
	[TIC_CLK] = FREQ_TIC_CLK,
	[GMII0_CLK_125M] = FREQ_GMII0_CLK_125M,
	[GMII1_CLK_125M] = FREQ_GMII1_CLK_125M,
	[SATA_LN_ASIC_CLK] = FREQ_SATA_PHY_CLK,
	[SATA_LN_RX_CLK] = FREQ_SATA_PHY_CLK,
	[PCIE_X16_MSTR_Q23_CLK] = FREQ_PCIE_X16_MSTR_Q23_CLK,
	[PCIE_X16_SLV_Q23_CLK] = FREQ_PCIE_X16_SLV_Q23_CLK,
	[PCIE_X8_MSTR_Q23_CLK] = FREQ_PCIE_X8_MSTR_Q23_CLK,
	[PCIE_X8_SLV_Q23_CLK] = FREQ_PCIE_X8_SLV_Q23_CLK,
};

static uint32_t get_padring_freq(clk_clk_t clk)
{
	if (clk >= NR_PADRING_CLKS)
		return INVALID_FREQ;
	return padring_clks[clk];
}

struct clk_driver clk_padring = {
	.max_clocks = NR_PADRING_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_padring_freq,
	.set_freq = NULL,
};

struct fabia_pll fabia_plls[] = {
	[WEST_PLL0] = {
		.base = GCCMW_WEST_PLL_BASE(GCCMW_WEST_PLL0),
		.input_clk = west_xo,
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_WEST_PLL0_1_0,
			[FREQPLAN_1_1] = FREQ_WEST_PLL0,
			[FREQPLAN_2_0] = FREQ_WEST_PLL0,
		},
	},
	[WEST_PLL1] = {
		.base = GCCMW_WEST_PLL_BASE(GCCMW_WEST_PLL1),
		.input_clk = west_xo,
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_WEST_PLL1,
			[FREQPLAN_1_1] = FREQ_WEST_PLL1,
			[FREQPLAN_2_0] = FREQ_WEST_PLL1,
		},
	},
	[EAST_PLL0] = {
		.base = GCCE_EAST_PLL_BASE(GCCE_EAST_PLL0),
		.input_clk = east_xo,
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_EAST_PLL0,
			[FREQPLAN_1_1] = FREQ_EAST_PLL0,
			[FREQPLAN_2_0] = FREQ_EAST_PLL0,
		},
	},
	[EAST_PLL1] = {
		.base = GCCE_EAST_PLL_BASE(GCCE_EAST_PLL1),
		.input_clk = east_xo,
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_EAST_PLL1_1,
			[FREQPLAN_1_1] = FREQ_EAST_PLL1_1,
			[FREQPLAN_2_0] = FREQ_EAST_PLL1,
		},
	},
};

static uint32_t get_fabia_pll_freq(clk_clk_t pll)
{
	int plan;

	if (pll >= NR_FABIA_PLL_CLKS)
		return INVALID_FREQ;
	if (INVALID_FREQ != fabia_plls[pll].output_hz)
		return fabia_plls[pll].output_hz;
	plan = freqplan_get_plan();
	if (plan == INVALID_FREQPLAN)
		return INVALID_FREQ;
	return fabia_plls[pll].def_output_hz[plan];
}

static int enable_fabia_pll(clk_clk_t pll)
{
	uint32_t in_freq;
	uint32_t out_freq;
	int ret;

	out_freq = get_fabia_pll_freq(pll);
	if (out_freq == INVALID_FREQ)
		return -EINVAL;
	fabia_plls[pll].refcnt++;
	if (fabia_plls[pll].refcnt > 1)
		return 0;
	ret = clk_enable(fabia_plls[pll].input_clk);
	if (ret)
		return ret;
	in_freq = clk_get_frequency(fabia_plls[pll].input_clk);
	if (in_freq == INVALID_FREQ)
		return -EINVAL;
	if (!__fabia_enable_pll(fabia_plls[pll].base, in_freq, out_freq))
		return -EFAULT;
	fabia_plls[pll].output_hz = out_freq;
	return 0;
}

struct clk_driver clk_fabia_pll = {
	.max_clocks = NR_FABIA_PLL_CLKS,
	.enable = enable_fabia_pll,
	.disable = NULL,
	.get_freq = get_fabia_pll_freq,
	.set_freq = NULL,
};

static uint32_t get_fabia_pll_out_freq(clk_clk_t clk)
{
	uint32_t freq;

	freq = clk_get_frequency(clkid(CLK_FABIA_PLL, clk & 0x03));
	switch (clk >> 2) {
	case 0: /* EVEN */
		return freq / 2;
	case 1: /* ODD */
		return freq / 3;
	case 2: /* TEST */
		return freq;
	case 3: /* MAIN_DIV2 */
		return freq / 2;
	case 4: /* EVEN_DIV2 */
		return freq / 4;
	case 5: /* MAIN_DIV10 */
		return freq / 10;
	case 6:
		if (clk == PARB_PRE_DIV)
			return FREQ_PARB_PRE_DIV;
		if (clk == GCCE_XO_DIV4_CLK_SRC)
			return clk_get_frequency(gcce_xo_clk) / 4;
	default:
		return INVALID_FREQ;
	}
}

uint32_t parb_pre_divs[NR_FREQPLANS] = {
	[FREQPLAN_1_0] = 65536, /* Div 2 */
	[FREQPLAN_1_1] = 589824, /* Div 10 */
	[FREQPLAN_2_0] = 589824, /* Div 10 */
};
bool parb_pre_div_enabled = false;

static int enable_fabia_pll_out(clk_clk_t clk)
{
	int ret;

	switch (clk >> 2) {
	case 0: /* EVEN/MAIN_DIV2 */
	case 1: /* ODD */
	case 2: /* TEST */
	case 3: /* MAIN_DIV2 */
	case 4: /* EVEN_DIV2 */
	case 5: /* MAIN_DIV10 */
		return clk_enable(clkid(CLK_FABIA_PLL, clk & 0x03));
	case 6: /* MISC */
		ret = clk_enable(west_pll0_out_main);
		if (ret)
			return ret;
		if (clk == PARB_PRE_DIV && !parb_pre_div_enabled) {
			int plan = freqplan_get_plan();
			if (plan == INVALID_FREQPLAN)
				return -EINVAL;
			__raw_writel(parb_pre_divs[plan],
				     GCCMW_PARB_PRE_DIV_CDIVR);
			parb_pre_div_enabled = true;
		}
		if (clk == GCCE_XO_DIV4_CLK_SRC)
			return clk_enable(gcce_xo_clk);
		return 0;
	default:
		return -EINVAL;
	}
}

struct clk_driver clk_fabia_pllout = {
	.max_clocks = NR_FABIA_PLLOUT_CLKS,
	.enable = enable_fabia_pll_out,
	.disable = NULL,
	.get_freq = get_fabia_pll_out_freq,
	.set_freq = NULL,
};

clk_t imc_clk_srcs[] = {
	sys_xo,
	sys_pll0_out_main,
	sys_pll0_out_main_div2,
	invalid_clk,
	sys_pll1_out_main,
	sys_pll1_out_main_div2,
};

clk_t west_xo_clk_srcs[] = {
	west_xo,
};

clk_t east_xo_clk_srcs[] = {
	east_xo,
};

clk_t west_pll_clk_srcs[] = {
	west_xo,
	west_pll0_out_main,
	west_pll0_out_main_div2,
	invalid_clk,
	west_pll1_out_main,
	west_pll1_out_main_div2,
};

clk_t east_pll_clk_srcs[] = {
	east_xo,
	east_pll0_out_even,
	invalid_clk,
	east_pll0_out_odd,
	invalid_clk,
	east_pll1_out_even,
	east_pll1_out_odd,
};

clk_t west_pll0_clk_srcs[] = {
	west_xo,
	west_pll0_out_main,
	west_pll0_out_main_div2,
};

clk_t west_pll1_clk_srcs[] = {
	west_xo,
	invalid_clk,
	invalid_clk,
	invalid_clk,
	west_pll1_out_main,
	west_pll1_out_main_div2,
};

clk_t west_pll_main_clk_srcs[] = {
	west_xo,
	west_pll0_out_main,
	invalid_clk,
	invalid_clk,
	west_pll1_out_main,
};

clk_t fan_tach_clk_srcs[] = {
	west_xo,
	invalid_clk,
	west_pll0_out_main_div10,
};

clk_t parb_clk_srcs[] = {
	west_xo,
	invalid_clk,
	invalid_clk,
	parb_pre_div,
};

clk_t gmii0_clk_srcs[] = {
	west_xo,
	gmii0_clk_125m,
};

clk_t gmii1_clk_srcs[] = {
	west_xo,
	gmii1_clk_125m,
};

struct clk_rcg clk_rcgs[] = {
	/* IMC_CC */
	[IMC_PROC_CLK_SRC] = {
		.base = IMC_CC_IMC_PROC_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(imc_clk_srcs),
		.input_clks = ARRAY_ADDR(imc_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 400000000,
			[FREQPLAN_1_1] = 500000000,
			[FREQPLAN_2_0] = 500000000,
		},
	},
	[IMC_CLK_SRC] = {
		.base = IMC_CC_IMC_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(imc_clk_srcs),
		.input_clks = ARRAY_ADDR(imc_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 285714285,
			[FREQPLAN_2_0] = 333333333,
		},
	},
	/* GCCMW */
	[GCCMW_XO_CLK_SRC] = {
		.base = GCCMW_XO_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_xo_clk_srcs),
		.input_clks = ARRAY_ADDR(west_xo_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 20000000,
			[FREQPLAN_1_1] = 20000000,
			[FREQPLAN_2_0] = 20000000,
		},
	},
	[CONFIG_FAB_CLK_SRC] = {
		.base = GCCMW_CFAB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[SYSTEM_FAB_CLK_SRC] = {
		.base = GCCMW_SFAB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[WCSFPB_CLK_SRC] = {
		.base = GCCMW_W_CSFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[N1_CSFPB_CLK_SRC] = {
		.base = GCCMW_N1_CSFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[S1_CSFPB_CLK_SRC] = {
		.base = GCCMW_S1_CSFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[EMAC0_125M_CLK_SRC] = {
		.base = GCCMW_EMAC_125M_CMD_RCGR(GCCMW_EMAC0),
		.nr_input_clks = ARRAY_SIZE(gmii0_clk_srcs),
		.input_clks = ARRAY_ADDR(gmii0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 125000000,
			[FREQPLAN_1_1] = 125000000,
			[FREQPLAN_2_0] = 125000000,
		},
	},
	[EMAC0_AHB_CLK_SRC] = {
		.base = GCCMW_EMAC_AHB_CMD_RCGR(GCCMW_EMAC0),
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[EMAC0_SYS_25M_CLK_SRC] = {
		.base = GCCMW_EMAC_SYS_25M_CMD_RCGR(GCCMW_EMAC0),
		.nr_input_clks = ARRAY_SIZE(gmii0_clk_srcs),
		.input_clks = ARRAY_ADDR(gmii0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 25000000,
			[FREQPLAN_1_1] = 25000000,
			[FREQPLAN_2_0] = 25000000,
		},
	},
	[EMAC1_125M_CLK_SRC] = {
		.base = GCCMW_EMAC_125M_CMD_RCGR(GCCMW_EMAC1),
		.nr_input_clks = ARRAY_SIZE(gmii1_clk_srcs),
		.input_clks = ARRAY_ADDR(gmii1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 125000000,
			[FREQPLAN_1_1] = 125000000,
			[FREQPLAN_2_0] = 125000000,
		},
	},
	[EMAC1_AHB_CLK_SRC] = {
		.base = GCCMW_EMAC_AHB_CMD_RCGR(GCCMW_EMAC1),
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[EMAC1_SYS_25M_CLK_SRC] = {
		.base = GCCMW_EMAC_SYS_25M_CMD_RCGR(GCCMW_EMAC1),
		.nr_input_clks = ARRAY_SIZE(gmii1_clk_srcs),
		.input_clks = ARRAY_ADDR(gmii1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 25000000,
			[FREQPLAN_1_1] = 25000000,
			[FREQPLAN_2_0] = 25000000,
		},
	},
	[HMSS_W_BUS_CLK_SRC] = {
		.base = GCCMW_HMSS_BUS_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[CE0_CLK_SRC] = {
		.base = GCCMW_CE_CMD_RCGR(GCCMW_CE0),
		.nr_input_clks = ARRAY_SIZE(west_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[CE1_CLK_SRC] = {
		.base = GCCMW_CE_CMD_RCGR(GCCMW_CE1),
		.nr_input_clks = ARRAY_SIZE(west_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[CPRC_DIG_CLK_SRC] = {
		.base = GCCMW_CPRC_DIG_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[CPRC_MEM_CLK_SRC] = {
		.base = GCCMW_CPRC_MEM_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 66666666,
			[FREQPLAN_1_1] = 66666666,
			[FREQPLAN_2_0] = 72727272,
		},
	},
	[CPRC_APCS_CLK_SRC] = {
		.base = GCCMW_CPRC_APCS_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[CPRC_NVDDA_CLK_SRC] = {
		.base = GCCMW_CPRC_VDDA_CMD_RCGR(GCCMW_CPRC_NVDDA),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[CPRC_SVDDA_CLK_SRC] = {
		.base = GCCMW_CPRC_VDDA_CMD_RCGR(GCCMW_CPRC_SVDDA),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[FAN_TACH_CLK_SRC] = {
		.base = GCCMW_FAN_TACH_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(fan_tach_clk_srcs),
		.input_clks = ARRAY_ADDR(fan_tach_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 20000000, /* check! */
			[FREQPLAN_1_1] = 25000000, /* check! */
			[FREQPLAN_2_0] = 25000000,
		},
	},
	[GP0_CLK_SRC] = {
		.base = GCCMW_GP_CMD_RCGR(GCCMW_GP0),
		.flags = CLK_MND_MASK(GCCMW_GP_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll_main_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll_main_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[GP1_CLK_SRC] = {
		.base = GCCMW_GP_CMD_RCGR(GCCMW_GP1),
		.flags = CLK_MND_MASK(GCCMW_GP_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll_main_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll_main_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[GP2_CLK_SRC] = {
		.base = GCCMW_GP_CMD_RCGR(GCCMW_GP2),
		.flags = CLK_MND_MASK(GCCMW_GP_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll_main_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll_main_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[HDMA_AHB_CLK_SRC] = {
		.base = GCCMW_HDMA_AHB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000, /* check! */
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[HDMA_SFPB_CLK_SRC] = {
		.base = GCCMW_HDMA_SFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[HDMA_SYS_CLK_SRC] = {
		.base = GCCMW_HDMA_SYS_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 500000000,
			[FREQPLAN_2_0] = 500000000,
		},
	},
	[HDMA_VBU_CLK_SRC] = {
		.base = GCCMW_HDMA_VBU_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 800000000,
			[FREQPLAN_1_1] = 800000000,
			[FREQPLAN_2_0] = 1000000000,
		},
	},
	[W_QDSS_AT_CLK_SRC] = {
		.base = GCCMW_QDSS_ATB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 250000000,
			[FREQPLAN_2_0] = 250000000,
		},
	},
	[SYS_FAB_VBU_CLK_SRC] = {
		.base = GCCMW_SFAB_VBU_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 500000000,
			[FREQPLAN_2_0] = 500000000,
		},
	},
	[SATA_VBU_CLK_SRC] = {
		.base = GCCMW_SATA_VBU_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 500000000,
			[FREQPLAN_2_0] = 500000000,
		},
	},
	[PARB0_SER_CLK_SRC] = {
		.base = GCCMW_PARB_SER_CMD_RCGR(GCCMW_PARB0),
		.nr_input_clks = ARRAY_SIZE(parb_clk_srcs),
		.input_clks = ARRAY_ADDR(parb_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 25000000,
			[FREQPLAN_1_1] = 25000000,
			[FREQPLAN_2_0] = 25000000,
		},
	},
	[PARB1_SER_CLK_SRC] = {
		.base = GCCMW_PARB_SER_CMD_RCGR(GCCMW_PARB1),
		.nr_input_clks = ARRAY_SIZE(parb_clk_srcs),
		.input_clks = ARRAY_ADDR(parb_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 25000000,
			[FREQPLAN_1_1] = 25000000,
			[FREQPLAN_2_0] = 25000000,
		},
	},
	[PARB2_SER_CLK_SRC] = {
		.base = GCCMW_PARB2_SER_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(parb_clk_srcs),
		.input_clks = ARRAY_ADDR(parb_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 25000000,
			[FREQPLAN_1_1] = 25000000,
			[FREQPLAN_2_0] = 25000000,
		},
	},
	[QDSS_TRACECLKIN_CLK_SRC] = {
		.base = GCCMW_QDSS_TRACE_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 333333333,
			[FREQPLAN_2_0] = 333333333,
		},
	},
	[ACC_CLK_SRC] = {
		.base = GCCMW_ACC_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[SEC_CTRL_CLK_SRC] = {
		.base = GCCMW_SEC_CTRL_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 20000000,
			[FREQPLAN_1_1] = 20000000,
			[FREQPLAN_2_0] = 20000000,
		},
	},
	[SATA_AHB_CLK_SRC] = {
		.base = GCCMW_SATA0_AHB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[SATA_Q22_CLK_SRC] = {
		.base = GCCMW_SATA_Q22_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 200000000,
			[FREQPLAN_1_1] = 200000000,
			[FREQPLAN_2_0] = 200000000,
		},
	},
	[SATA_PMALIVE_CLK_SRC] = {
		.base = GCCMW_SATA_PMALIVE_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[SATA_RXOOB_CLK_SRC] = {
		.base = GCCMW_SATA_RXOOB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[SATA_CSFPB_CLK_SRC] = {
		.base = GCCMW_SATA_SS_CSFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	/* GCCE */
	[SECSFPB_CLK_SRC] = {
		.base = GCCE_SECSFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[S2_CSFPB_CLK_SRC] = {
		.base = GCCE_S2_CSFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[N2_CSFPB_CLK_SRC] = {
		.base = GCCE_N2_CSFPB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[HMSS_E_BUS_CLK_SRC] = {
		.base = GCCE_HMSS_BUS_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[E_QDSS_AT_CLK_SRC] = {
		.base = GCCE_QDSS_ATB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 250000000,
			[FREQPLAN_1_1] = 250000000,
			[FREQPLAN_2_0] = 250000000,
		},
	},
	[PCIE_SS_0_VBU_CLK_SRC] = {
		.base = GCCE_PCIE_SS_VBU_CMD_RCGR(GCCE_PCIE_SS_0),
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 980000000,
			[FREQPLAN_1_1] = 980000000,
			[FREQPLAN_2_0] = 1000000000,
		},
	},
	[PCIE_SS_1_VBU_CLK_SRC] = {
		.base = GCCE_PCIE_SS_VBU_CMD_RCGR(GCCE_PCIE_SS_1),
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 980000000,
			[FREQPLAN_1_1] = 980000000,
			[FREQPLAN_2_0] = 1000000000,
		},
	},
	[GCCE_XO_CLK_SRC] = {
		.base = GCCE_XO_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(east_xo_clk_srcs),
		.input_clks = ARRAY_ADDR(east_xo_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 20000000,
			[FREQPLAN_1_1] = 20000000,
			[FREQPLAN_2_0] = 20000000,
		},
	},
	/* PCC */
	[PERIPH_FAB_CLK_SRC] = {
		.base = PCC_PFAB_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[BLSP0_QUP0_SPI_APPS_CLK_SRC] = {
		.base = PCC_SPI_CMD_RCGR(PCC_BLSP0_QUP0),
		.flags = CLK_MND_MASK(PCC_SPI_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP0_SPI0,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP0_SPI0,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP0_SPI0,
		},
	},
	[BLSP0_QUP1_SPI_APPS_CLK_SRC] = {
		.base = PCC_SPI_CMD_RCGR(PCC_BLSP0_QUP1),
		.flags = CLK_MND_MASK(PCC_SPI_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP0_SPI1,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP0_SPI1,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP0_SPI1,
		},
	},
	[BLSP0_UART0_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP0_UART0),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[BLSP0_UART1_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP0_UART1),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[BLSP0_UART2_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP0_UART2),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[BLSP0_UART3_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP0_UART3),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[BLSP1_QUP0_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP1_QUP0),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP1_QUP1_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP1_QUP1),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP1_QUP2_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP1_QUP2),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP1_QUP3_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP1_QUP3),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP2_QUP0_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP2_QUP0),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP2_QUP1_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP2_QUP1),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP2_QUP2_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP2_QUP2),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP2_QUP3_I2C_APPS_CLK_SRC] = {
		.base = PCC_I2C_CMD_RCGR(PCC_BLSP2_QUP3),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_BLSP_I2C,
			[FREQPLAN_1_1] = FREQ_BLSP_I2C,
			[FREQPLAN_2_0] = FREQ_BLSP_I2C,
		},
	},
	[BLSP3_QUP0_SPI_APPS_CLK_SRC] = {
		.base = PCC_SPI_CMD_RCGR(PCC_BLSP3_QUP0),
		.flags = CLK_MND_MASK(PCC_SPI_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_1] = FREQ_BLSP_SPI,
			[FREQPLAN_2_0] = FREQ_BLSP_SPI,
		},
	},
	[BLSP3_QUP1_SPI_APPS_CLK_SRC] = {
		.base = PCC_SPI_CMD_RCGR(PCC_BLSP3_QUP1),
		.flags = CLK_MND_MASK(PCC_SPI_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_1] = FREQ_BLSP_SPI,
			[FREQPLAN_2_0] = FREQ_BLSP_SPI,
		},
	},
	[BLSP3_UART0_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP3_UART0),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[BLSP3_UART1_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP3_UART1),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[BLSP3_UART2_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP3_UART2),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[BLSP3_UART3_APPS_CLK_SRC] = {
		.base = PCC_UART_CMD_RCGR(PCC_BLSP3_UART3),
		.flags = CLK_MND_MASK(PCC_UART_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll1_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll1_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_1_1] = FREQ_PERIPH_BLSP_UART,
			[FREQPLAN_2_0] = FREQ_PERIPH_BLSP_UART,
		},
	},
	[PDM2_CLK_SRC] = {
		.base = PCC_PDM2_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 57143000,
			[FREQPLAN_1_1] = 62500000,
			[FREQPLAN_2_0] = 62500000,
		},
	},
	[PERIPH_SPI_CLK_SRC] = {
		.base = PCC_PERIPH_SPI_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[SDCC0_APPS_CLK_SRC] = {
		.base = PCC_SDCC_APPS_CMD_RCGR(PCC_SDCC0),
		.flags = CLK_MND_MASK(PCC_SDCC_APPS_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_1] = FREQ_SDCC,
			[FREQPLAN_2_0] = FREQ_SDCC,
		},
	},
	[SDCC1_APPS_CLK_SRC] = {
		.base = PCC_SDCC_APPS_CMD_RCGR(PCC_SDCC1),
		.flags = CLK_MND_MASK(PCC_SDCC_APPS_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_1] = FREQ_SDCC,
			[FREQPLAN_2_0] = FREQ_SDCC,
		},
	},
	[SDCC2_APPS_CLK_SRC] = {
		.base = PCC_SDCC_APPS_CMD_RCGR(PCC_SDCC2),
		.flags = CLK_MND_MASK(PCC_SDCC_APPS_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_1] = FREQ_SDCC,
			[FREQPLAN_2_0] = FREQ_SDCC,
		},
	},
	[SDCC3_APPS_CLK_SRC] = {
		.base = PCC_SDCC_APPS_CMD_RCGR(PCC_SDCC3),
		.flags = CLK_MND_MASK(PCC_SDCC_APPS_MND_MASK),
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_1] = FREQ_SDCC,
			[FREQPLAN_2_0] = FREQ_SDCC,
		},
	},
	[USB2_MSTR_CLK_SRC] = {
		.base = PCC_USB2_MSTR_CMD_RCGR,
		.nr_input_clks = ARRAY_SIZE(west_pll0_clk_srcs),
		.input_clks = ARRAY_ADDR(west_pll0_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 66666666,
			[FREQPLAN_1_1] = 62500000,
			[FREQPLAN_2_0] = 62500000,
		},
	},
	[PCIE0_SFPB_CLK_SRC] = {
		.base = PCIE_CC_PCIE_SFPB_CMD_RCGR(0),
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[PCIE0_AUX_CLK_SRC] = {
		.base = PCIE_CC_PCIE_AUX_CMD_RCGR(0),
		.flags = CLK_MND_MASK(PCIE_CC_PCIE_AUX_MND_MASK),
		/* TODO:
		 * pcie_aux_clk_srcs should include pcie_cc_im_sleep_clk.
		 */
		.nr_input_clks = ARRAY_SIZE(east_xo_clk_srcs),
		.input_clks = ARRAY_ADDR(east_xo_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 1000000,
			[FREQPLAN_1_1] = 1000000,
			[FREQPLAN_2_0] = 1000000,
		},
	},
	[PCIE1_SFPB_CLK_SRC] = {
		.base = PCIE_CC_PCIE_SFPB_CMD_RCGR(1),
		.nr_input_clks = ARRAY_SIZE(east_pll_clk_srcs),
		.input_clks = ARRAY_ADDR(east_pll_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 100000000,
			[FREQPLAN_1_1] = 100000000,
			[FREQPLAN_2_0] = 100000000,
		},
	},
	[PCIE1_AUX_CLK_SRC] = {
		.base = PCIE_CC_PCIE_AUX_CMD_RCGR(1),
		.flags = CLK_MND_MASK(PCIE_CC_PCIE_AUX_MND_MASK),
		/* TODO:
		 * pcie_aux_clk_srcs should include pcie_cc_im_sleep_clk.
		 */
		.nr_input_clks = ARRAY_SIZE(east_xo_clk_srcs),
		.input_clks = ARRAY_ADDR(east_xo_clk_srcs),
		.def_output_hz = {
			[FREQPLAN_1_0] = 1000000,
			[FREQPLAN_1_1] = 1000000,
			[FREQPLAN_2_0] = 1000000,
		},
	},
};

uint32_t get_root_clock_freq(clk_clk_t clk)
{
	int plan;

	if (clk >= NR_ROOT_CLKS)
		return INVALID_FREQ;
	if (INVALID_FREQ != clk_rcgs[clk].output_hz)
		return clk_rcgs[clk].output_hz;
	plan = freqplan_get_plan();
	if (plan == INVALID_FREQPLAN)
		return INVALID_FREQ;
	return clk_rcgs[clk].def_output_hz[plan];
}

void select_root_clock_source(clk_clk_t clk, clk_t src)
{
	uint8_t i;

	if (clk >= NR_ROOT_CLKS)
		return;
	for (i = 0; i < clk_rcgs[clk].nr_input_clks; i++) {
		if (clk_rcgs[clk].input_clks[i] == src) {
			clk_rcgs[clk].src_sel = i;
			break;
		}
	}
}

int enable_root_clock(clk_clk_t clk)
{
	struct clk_rcg *cfg;
	uint32_t in_freq;
	uint32_t out_freq;
	clk_t src;
	int ret;

	if (clk >= NR_ROOT_CLKS)
		return -EINVAL;

	out_freq = get_root_clock_freq(clk);
	if (out_freq == INVALID_FREQ)
		return -EINVAL;
	cfg = &clk_rcgs[clk];
	cfg->refcnt++;
	if (cfg->refcnt > 1)
		return 0;
	src = cfg->input_clks[cfg->src_sel];
	ret = clk_enable(src);
	if (ret)
		return ret;
	in_freq = clk_get_frequency(src);
	if (in_freq == INVALID_FREQ)
		return -EINVAL;
	if (cfg->flags) {
		__clk_generate_mnd(cfg->base,
				   cfg->src_sel,
				   in_freq,
				   out_freq,
				   RCG_MODE_DUAL_EDGE,
				   _GET_FV(CLK_MND_MASK, cfg->flags));
	} else {
		__clk_generate_hid(cfg->base,
				   cfg->src_sel,
				   in_freq, out_freq,
				   RCG_MODE_BYPASS);
	}
	__clk_update_root(cfg->base);
	return 0;
}

int set_root_clock_freq(clk_clk_t clk, uint32_t freq)
{
	return 0;
}

struct clk_driver clk_root_clock = {
	.max_clocks = NR_ROOT_CLKS,
	.enable = enable_root_clock,
	.disable = NULL,
	.get_freq = get_root_clock_freq,
	.set_freq = set_root_clock_freq,
	.select = select_root_clock_source,
};

struct clk_cbc clk_cbcs[] = {
	/* IMC_CC PSCBC */
	[IMC_CC_IMC_PROC_CLK] = {
		.base = IMC_CC_IMC_PROC_CBCR,
		.input_clk = imc_proc_clk_src,
	},
	[IMC_CC_QSMMUV3_CLIENT_CLK] = {
		.base = IMC_CC_IMC_QSMMUV3_CLIENT_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_BOOT_ROM_AHB_CLK] = {
		.base = IMC_CC_BOOT_ROM_AHB_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_RAM0_AXI_CLK] = {
		.base = IMC_CC_IMC_RAM0_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_RAM1_AXI_CLK] = {
		.base = IMC_CC_IMC_RAM1_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_RAM2_AXI_CLK] = {
		.base = IMC_CC_IMC_RAM2_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_RAM3_AXI_CLK] = {
		.base = IMC_CC_IMC_RAM3_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_RAM4_AXI_CLK] = {
		.base = IMC_CC_IMC_RAM4_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_RAM5_AXI_CLK] = {
		.base = IMC_CC_IMC_RAM5_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	/* IMC_CC CBC */
	[IMC_CC_IMC_PROC_AXI_CLK] = {
		.base = IMC_CC_IMC_PROC_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IOMMU_AXI_CLK] = {
		.base = IMC_CC_IMC_IOMMU_AXI_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_S0_AHB_CLK] = {
		.base = IMC_CC_IMC_S0_AHB_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_S1_AHB_CLK] = {
		.base = IMC_CC_IMC_S1_AHB_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_BOOT_ROM_FCLK] = {
		.base = IMC_CC_IMCFAB_BOOT_ROM_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_RAM0_FCLK] = {
		.base = IMC_CC_IMCFAB_RAM0_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_RAM1_FCLK] = {
		.base = IMC_CC_IMCFAB_RAM1_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_RAM2_FCLK] = {
		.base = IMC_CC_IMCFAB_RAM2_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_RAM3_FCLK] = {
		.base = IMC_CC_IMCFAB_RAM3_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_RAM4_FCLK] = {
		.base = IMC_CC_IMCFAB_RAM4_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_RAM5_FCLK] = {
		.base = IMC_CC_IMCFAB_RAM5_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_CORE_CLK] = {
		.base = IMC_CC_IMCFAB_CORE_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_S0_FCLK] = {
		.base = IMC_CC_IMCFAB_S0_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_S1_FCLK] = {
		.base = IMC_CC_IMCFAB_S1_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMCFAB_SFAB_FCLK] = {
		.base = IMC_CC_IMCFAB_SFAB_FCLK_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_SFVBU_IMC_DVM_CLK] = {
		.base = IMC_CC_SFVBU_IMC_DVM_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_AXI_BRDG_PROC_CLK] = {
		.base = IMC_CC_IMC_AXI_BRDG_PROC_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	[IMC_CC_IMC_DBG_BRDG_PROC_CLK] = {
		.base = IMC_CC_IMC_DBG_BRDG_PROC_CBCR,
		.input_clk = imc_postdiv_clk_src,
	},
	/* 5.2.4.2.1 Family ACC */
	[GCCMW_SEC_CTRL_ACC_CLK] = {
		.base = GCCMW_SEC_CTRL_ACC_CBCR,
		.input_clk = acc_clk_src,
	},
	/* 5.2.4.2.2 Family CE0 */
	[GCCMW_CE0_CLK] = {
		.base = GCCMW_CE_CBCR(GCCMW_CE0),
		.input_clk = ce0_clk_src,
	},
	/* 5.2.4.2.3 Family CE1 */
	[GCCMW_CE1_CLK] = {
		.base = GCCMW_CE_CBCR(GCCMW_CE1),
		.input_clk = ce1_clk_src,
	},
	/* 5.2.4.2.4 Family CONF_FAB */
	[GCCMW_CFAB_N1_CSFPB_FCLK] = {
		.base = GCCMW_N1_CSFPB_CFAB_FCLK_CBCR,
		.input_clk = config_fab_clk_src,
	},
	[GCCMW_FAN_TACH_AHB_CLK] = {
		.base = GCCMW_FAN_TACH_AHB_CBCR,
		.input_clk = config_fab_clk_src, /* gccmw_topcfg2_ahb_clk */
	},
	[GCCMW_HDMA_SFPB_AHB_CLK] = {
		.base = GCCMW_HDMA_SFPB_AHB_CBCR,
		.input_clk = config_fab_clk_src,
	},
	[GCCMW_NCSFPB_XPU_CFG_AHB_CLK] = {
		.base = GCCMW_N_CSFPB_XPU_CFG_AHB_CBCR,
		.input_clk = config_fab_clk_src, /* gccmw_topcfg2_ahb_clk */
	},
	/* 5.2.4.2.5 Family CPRC_APCS */
	[GCCMW_HMSS_CPRC_APCS_CLK] = {
		.base = GCCMW_CPRC_APCS_HMSS_CBCR,
		.input_clk = cprc_apcs_clk_src,
	},
	/* 5.2.4.2.6 Family CPRC_DIG */
	[GCCMW_CPRC_DIG_CLK] = {
		.base = GCCMW_CPRC_DIG_CBCR,
		.input_clk = cprc_dig_clk_src,
	},
	/* 5.2.4.2.7 Family CPRC_MEM */
	[GCCMW_CPRC_MEM_CLK] = {
		.base = GCCMW_CPRC_MEM_CBCR,
		.input_clk = cprc_mem_clk_src,
	},
	/* 5.2.4.2.8 Family CPRC_NVDDA */
	[GCCMW_CPRC_NVDDA_CLK] = {
		.base = GCCMW_CPRC_VDDA_CBCR(GCCMW_CPRC_NVDDA),
		.input_clk = cprc_nvdda_clk_src,
	},
	/* 5.2.4.2.9 Family CPRC_SVDDA */
	[GCCMW_CPRC_SVDDA_CLK] = {
		.base = GCCMW_CPRC_VDDA_CBCR(GCCMW_CPRC_SVDDA),
		.input_clk = cprc_svdda_clk_src,
	},
	/* 5.2.4.2.10 Family EMAC0_125M */
	[GCCMW_EMAC0_125M_CLK] = {
		.base = GCCMW_EMAC_125M_CBCR(GCCMW_EMAC0),
		.input_clk = gmii0_clk_125m, /* emac0_125m_clk_src */
	},
	/* 5.2.4.2.11 Family EMAC0_AHB */
	[GCCMW_EMAC0_AHB_CLK] = {
		.base = GCCMW_EMAC_AHB_CBCR(GCCMW_EMAC0),
		.input_clk = emac0_ahb_clk_src,
	},
	[GCCMW_EMAC0_IOMMU_CFG_AHB_CLK] = {
		.base = GCCMW_EMAC_IOMMU_CFG_AHB_CBCR(GCCMW_EMAC0),
		.input_clk = emac0_ahb_clk_src,
	},
	/* 5.2.4.2.12 Family EMAC0_SYS_25M */
	[GCCMW_EMAC0_SYS_25M_CLK] = {
		.base = GCCMW_EMAC_SYS_25M_CBCR(GCCMW_EMAC0),
		.input_clk = emac0_sys_25m_clk_src,
	},
	/* 5.2.4.2.13 Family EMAC0_TX */
	[GCCMW_GMII0_TX_CLK] = {
		.base = GCCMW_EMAC_GMII_TX_CBCR(GCCMW_EMAC0),
		.input_clk = gmii0_clk_125m, /* emac0_125m_clk_src */
	},
	/* EMAC0 missing */
	[GCCMW_EMAC0_RAM_SYS_CLK] = {
		.base = GCCMW_EMAC_RAM_SYS_CBCR(GCCMW_EMAC0),
		.input_clk = emac0_sys_25m_clk_src,
	},
	[GCCMW_EMAC0_RX_CLK] = {
		.base = GCCMW_EMAC_RX_CBCR(GCCMW_EMAC0),
		.input_clk = gmii0_clk_125m, /* emac0_125m_clk_src */
	},
	/* 5.2.4.2.14 Family EMAC1_125M */
	[GCCMW_EMAC1_125M_CLK] = {
		.base = GCCMW_EMAC_125M_CBCR(GCCMW_EMAC1),
		.input_clk = gmii1_clk_125m, /* emac1_125m_clk_src */
	},
	/* 5.2.4.2.15 Family EMAC1_AHB */
	[GCCMW_EMAC1_AHB_CLK] = {
		.base = GCCMW_EMAC_AHB_CBCR(GCCMW_EMAC1),
		.input_clk = emac1_ahb_clk_src,
	},
	[GCCMW_EMAC1_IOMMU_CFG_AHB_CLK] = {
		.base = GCCMW_EMAC_IOMMU_CFG_AHB_CBCR(GCCMW_EMAC1),
		.input_clk = emac1_ahb_clk_src,
	},
	/* 5.2.4.2.16 Family EMAC1_SYS_25M */
	[GCCMW_EMAC1_SYS_25M_CLK] = {
		.base = GCCMW_EMAC_SYS_25M_CBCR(GCCMW_EMAC1),
		.input_clk = emac1_sys_25m_clk_src,
	},
	/* 5.2.4.2.17 Family EMAC1_TX */
	[GCCMW_GMII1_TX_CLK] = {
		.base = GCCMW_EMAC_GMII_TX_CBCR(GCCMW_EMAC1),
		.input_clk = gmii1_clk_125m, /* emac1_125m_clk_src */
	},
	/* EMAC1 missing */
	[GCCMW_EMAC1_RAM_SYS_CLK] = {
		.base = GCCMW_EMAC_RAM_SYS_CBCR(GCCMW_EMAC1),
		.input_clk = emac1_sys_25m_clk_src,
	},
	[GCCMW_EMAC1_RX_CLK] = {
		.base = GCCMW_EMAC_RX_CBCR(GCCMW_EMAC1),
		.input_clk = gmii1_clk_125m, /* emac1_125m_clk_src */
	},
	/* 5.2.4.2.18 Family FAN_TACH */
	[GCCMW_FAN_TACH_PWM_CLK] = {
		.base = GCCMW_FAN_TACH_PWM_CBCR,
		.input_clk = fan_tach_clk_src,
	},
	/* 5.2.4.2.19 Family GP0 */
	[GCCMW_GP0_CLK] = {
		.base = GCCMW_GP_CBCR(GCCMW_GP0),
		.input_clk = gp0_clk_src,
	},
	/* 5.2.4.2.20 Family GP1 */
	[GCCMW_GP1_CLK] = {
		.base = GCCMW_GP_CBCR(GCCMW_GP1),
		.input_clk = gp0_clk_src,
	},
	/* 5.2.4.2.21 Family GP2 */
	[GCCMW_GP2_CLK] = {
		.base = GCCMW_GP_CBCR(GCCMW_GP2),
		.input_clk = gp0_clk_src,
	},
	/* 5.2.4.2.22 Family HDMA_AHB */
	[GCCMW_HDMA0_AHB_CLK] = {
		.base = GCCMW_HDMA_AHB_CBCR(GCCMW_HDMA0),
		.input_clk = hdma_ahb_clk_src,
	},
	[GCCMW_HDMA1_AHB_CLK] = {
		.base = GCCMW_HDMA_AHB_CBCR(GCCMW_HDMA1),
		.input_clk = hdma_ahb_clk_src,
	},
	[GCCMW_HDMA2_AHB_CLK] = {
		.base = GCCMW_HDMA_AHB_CBCR(GCCMW_HDMA2),
		.input_clk = hdma_ahb_clk_src,
	},
	[GCCMW_HDMA3_AHB_CLK] = {
		.base = GCCMW_HDMA_AHB_CBCR(GCCMW_HDMA3),
		.input_clk = hdma_ahb_clk_src,
	},
	/* 5.2.4.2.23 Family HDMA_SFPB */
	[GCCMW_HDMA_SFPB_ALWAYS_ON_CLK] = {
		.base = GCCMW_HDMA_SFPB_ALWAYS_ON_CBCR,
		.input_clk = hdma_sfpb_clk_src,
	},
	[GCCMW_HDMA_SFPB_CLK] = {
		.base = GCCMW_HDMA_SFPB_CBCR,
		.input_clk = hdma_sfpb_clk_src,
	},
	[GCCMW_HDMA_VBU_CFG_AHB_CLK] = {
		.base = GCCMW_HDMA_VBU_CFG_AHB_CBCR,
		.input_clk = hdma_sfpb_clk_src,
	},
	/* 5.2.4.2.24 Family HDMA_SYS */
	[GCCMW_HDMA0_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_HDMA_QSMMUV3_CLIENT_CBCR(GCCMW_HDMA0),
		.input_clk = hdma_sys_clk_src,
	},
	[GCCMW_HDMA0_SYS_CLK] = {
		.base = GCCMW_HDMA_SYS_CBCR(GCCMW_HDMA0),
		.input_clk = hdma_sys_clk_src,
	},
	[GCCMW_HDMA1_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_HDMA_QSMMUV3_CLIENT_CBCR(GCCMW_HDMA1),
		.input_clk = hdma_sys_clk_src,
	},
	[GCCMW_HDMA1_SYS_CLK] = {
		.base = GCCMW_HDMA_SYS_CBCR(GCCMW_HDMA1),
		.input_clk = hdma_sys_clk_src,
	},
	[GCCMW_HDMA2_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_HDMA_QSMMUV3_CLIENT_CBCR(GCCMW_HDMA2),
		.input_clk = hdma_sys_clk_src,
	},
	[GCCMW_HDMA2_SYS_CLK] = {
		.base = GCCMW_HDMA_SYS_CBCR(GCCMW_HDMA2),
		.input_clk = hdma_sys_clk_src,
	},
	[GCCMW_HDMA3_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_HDMA_QSMMUV3_CLIENT_CBCR(GCCMW_HDMA3),
		.input_clk = hdma_sys_clk_src,
	},
	[GCCMW_HDMA3_SYS_CLK] = {
		.base = GCCMW_HDMA_SYS_CBCR(GCCMW_HDMA3),
		.input_clk = hdma_sys_clk_src,
	},
	/* 5.2.4.2.25 Family HDMA_VBU */
	[GCCMW_HDMA_VBU_CORE_CLK] = {
		.base = GCCMW_HDMA_VBU_CORE_CBCR,
		.input_clk = hdma_vbu_clk_src,
	},
	/* 5.2.4.2.26 Family HMSS_BUS */
	[GCCMW_HMSS_AXI_CLK] = {
		.base = GCCMW_HMSS_AXI_CBCR,
		.input_clk = hmss_w_bus_clk_src,
	},
	[GCCMW_HMSS_W_AHB_CLK] = {
		.base = GCCMW_HMSS_W_AHB_CBCR,
		.input_clk = hmss_w_bus_clk_src,
	},
	[GCCMW_HMSS_SW_AHB_CLK] = {
		.base = GCCMW_HMSS_SW_AHB_CBCR,
		.input_clk = hmss_w_bus_clk_src,
	},
	[GCCMW_HMSS_NW_AHB_CLK] = {
		.base = GCCMW_HMSS_NW_AHB_CBCR,
		.input_clk = hmss_w_bus_clk_src,
	},
	/* 5.2.4.2.27 Family N1_CSFPB */
	[GCCMW_MDDR2_AHB_CLK] = {
		.base = GCCMW_MDDR2_AHB_CBCR,
		.input_clk = n1_csfpb_clk_src,
	},
	[GCCMW_MDDR3_AHB_CLK] = {
		.base = GCCMW_MDDR3_AHB_CBCR,
		.input_clk = n1_csfpb_clk_src,
	},
	[GCCMW_TSENS2_AHB_CLK] = {
		.base = GCCMW_TSENS_AHB_CBCR(GCCMW_TSENS2),
		.input_clk = n1_csfpb_clk_src,
	},
	[GCCMW_TSENS3_AHB_CLK] = {
		.base = GCCMW_TSENS_AHB_CBCR(GCCMW_TSENS3),
		.input_clk = n1_csfpb_clk_src,
	},
	[GCCMW_TSENS4_AHB_CLK] = {
		.base = GCCMW_TSENS_AHB_CBCR(GCCMW_TSENS4),
		.input_clk = n1_csfpb_clk_src,
	},
	/* 5.2.4.2.28 Family PARB0_SER */
	[GCCMW_PARB0_SER_CLK] = {
		.base = GCCMW_PARB_SER_CBCR(GCCMW_PARB0),
		.input_clk = parb0_ser_clk_src,
	},
	/* 5.2.4.2.29 Family PARB1_SER */
	[GCCMW_PARB1_SER_CLK] = {
		.base = GCCMW_PARB_SER_CBCR(GCCMW_PARB1),
		.input_clk = parb1_ser_clk_src,
	},
	/* 5.2.4.2.30 Family PARB2_SER */
	[GCCMW_PARB2_SER_CLK] = {
		.base = GCCMW_PARB2_SER_CBCR,
		.input_clk = parb2_ser_clk_src,
	},
	/* 5.2.4.2.31 Family QDSS_ATB */
	[GCCMW_AT_W1_CLK] = {
		.base = GCCMW_AT_W1_CBCR,
		.input_clk = qdss_at_clk_src,
	},
	[GCCMW_AT_W2_CLK] = {
		.base = GCCMW_AT_W2_CBCR,
		.input_clk = qdss_at_clk_src,
	},
	[GCCMW_AT_W3_CLK] = {
		.base = GCCMW_AT_W3_CBCR,
		.input_clk = qdss_at_clk_src,
	},
	[GCCMW_AT_W5_CLK] = {
		.base = GCCMW_AT_W5_CBCR,
		.input_clk = qdss_at_clk_src,
	},
	[GCCMW_AT_W7_CLK] = {
		.base = GCCMW_AT_W7_CBCR,
		.input_clk = qdss_at_clk_src,
	},
	[GCCMW_AT_W9_CLK] = {
		.base = GCCMW_AT_W9_CBCR,
		.input_clk = qdss_at_clk_src,
	},
	[GCCMW_HMSS_AT_W_CLK] = {
		.base = GCCMW_HMSS_AT_W_CBCR,
		.input_clk = qdss_at_clk_src,
	},
	/* 5.2.4.2.32 Family QDSS_TRACE */
	[GCCMW_QDSS_TRACECLKIN_CLK] = {
		.base = GCCMW_QDSS_TRACECLKIN_CBCR,
		.input_clk = qdss_traceclkin_clk_src,
	},
	/* 5.2.4.2.33 Family S1_CSFPB */
	[GCCMW_MDDR0_AHB_CLK] = {
		.base = GCCMW_MDDR0_AHB_CBCR,
		.input_clk = s1_csfpb_clk_src,
	},
	[GCCMW_MDDR1_AHB_CLK] = {
		.base = GCCMW_MDDR1_AHB_CBCR,
		.input_clk = s1_csfpb_clk_src,
	},
	[GCCMW_TSENS9_AHB_CLK] = {
		.base = GCCMW_TSENS_AHB_CBCR(GCCMW_TSENS9),
		.input_clk = s1_csfpb_clk_src,
	},
	[GCCMW_TSENS10_AHB_CLK] = {
		.base = GCCMW_TSENS_AHB_CBCR(GCCMW_TSENS10),
		.input_clk = s1_csfpb_clk_src,
	},
	/* 5.2.4.2.34 Family SATA_AHB */
	[GCCMW_SATA_LN0_AHB_CLK] = {
		.base = GCCMW_SATA0_LN_AHB_CBCR,
		.input_clk = sata_ahb_clk_src,
	},
	[GCCMW_SATA_LN1_AHB_CLK] = {
		.base = GCCMW_SATA_LN_AHB_CBCR(GCCMW_SATA1),
		.input_clk = sata_ahb_clk_src,
	},
	[GCCMW_SATA_LN2_AHB_CLK] = {
		.base = GCCMW_SATA_LN_AHB_CBCR(GCCMW_SATA2),
		.input_clk = sata_ahb_clk_src,
	},
	[GCCMW_SATA_LN3_AHB_CLK] = {
		.base = GCCMW_SATA_LN_AHB_CBCR(GCCMW_SATA3),
		.input_clk = sata_ahb_clk_src,
	},
	[GCCMW_SATA_LN4_AHB_CLK] = {
		.base = GCCMW_SATA4_LN_AHB_CBCR,
		.input_clk = sata_ahb_clk_src,
	},
	[GCCMW_SATA_LN5_AHB_CLK] = {
		.base = GCCMW_SATA_LN_AHB_CBCR(GCCMW_SATA5),
		.input_clk = sata_ahb_clk_src,
	},
	[GCCMW_SATA_LN6_AHB_CLK] = {
		.base = GCCMW_SATA_LN_AHB_CBCR(GCCMW_SATA6),
		.input_clk = sata_ahb_clk_src,
	},
	[GCCMW_SATA_LN7_AHB_CLK] = {
		.base = GCCMW_SATA_LN_AHB_CBCR(GCCMW_SATA7),
		.input_clk = sata_ahb_clk_src,
	},
	/* 5.2.4.2.35 Family SATA_LN0_ASIC */
	[GCCMW_SATA_LN0_ASIC_CLK] = {
		.base = GCCMW_SATA0_LN_ASIC_CBCR,
		.input_clk = sata_ln0_asic_clk_src,
	},
	/* 5.2.4.2.36 Family SATA_LN0_RX */
	[GCCMW_SATA_LN0_RX_CLK] = {
		.base = GCCMW_SATA0_LN_RX_CBCR,
		.input_clk = sata_ln0_rx_clk_src,
	},
	/* 5.2.4.2.37 Family SATA_LN1_ASIC */
	[GCCMW_SATA_LN1_ASIC_CLK] = {
		.base = GCCMW_SATA_LN_ASIC_CBCR(GCCMW_SATA1),
		.input_clk = sata_ln1_asic_clk_src,
	},
	/* 5.2.4.2.38 Family SATA_LN1_RX */
	[GCCMW_SATA_LN1_RX_CLK] = {
		.base = GCCMW_SATA_LN_RX_CBCR(GCCMW_SATA1),
		.input_clk = sata_ln1_rx_clk_src,
	},
	/* 5.2.4.2.39 Family SATA_LN2_ASIC */
	[GCCMW_SATA_LN2_ASIC_CLK] = {
		.base = GCCMW_SATA_LN_ASIC_CBCR(GCCMW_SATA2),
		.input_clk = sata_ln2_asic_clk_src,
	},
	/* 5.2.4.2.40 Family SATA_LN2_RX */
	[GCCMW_SATA_LN2_RX_CLK] = {
		.base = GCCMW_SATA_LN_RX_CBCR(GCCMW_SATA2),
		.input_clk = sata_ln2_rx_clk_src,
	},
	/* 5.2.4.2.41 Family SATA_LN3_ASIC */
	[GCCMW_SATA_LN3_ASIC_CLK] = {
		.base = GCCMW_SATA_LN_ASIC_CBCR(GCCMW_SATA3),
		.input_clk = sata_ln3_asic_clk_src,
	},
	/* 5.2.4.2.42 Family SATA_LN3_RX */
	[GCCMW_SATA_LN3_RX_CLK] = {
		.base = GCCMW_SATA_LN_RX_CBCR(GCCMW_SATA3),
		.input_clk = sata_ln3_rx_clk_src,
	},
	/* 5.2.4.2.43 Family SATA_LN4_ASIC */
	[GCCMW_SATA_LN4_ASIC_CLK] = {
		.base = GCCMW_SATA4_LN_ASIC_CBCR,
		.input_clk = sata_ln4_asic_clk_src,
	},
	/* 5.2.4.2.44 Family SATA_LN4_RX */
	[GCCMW_SATA_LN4_RX_CLK] = {
		.base = GCCMW_SATA4_LN_RX_CBCR,
		.input_clk = sata_ln4_rx_clk_src,
	},
	/* 5.2.4.2.45 Family SATA_LN5_ASIC */
	[GCCMW_SATA_LN5_ASIC_CLK] = {
		.base = GCCMW_SATA_LN_ASIC_CBCR(GCCMW_SATA5),
		.input_clk = sata_ln5_asic_clk_src,
	},
	/* 5.2.4.2.46 Family SATA_LN5_RX */
	[GCCMW_SATA_LN5_RX_CLK] = {
		.base = GCCMW_SATA_LN_RX_CBCR(GCCMW_SATA5),
		.input_clk = sata_ln5_rx_clk_src,
	},
	/* 5.2.4.2.47 Family SATA_LN6_ASIC */
	[GCCMW_SATA_LN6_ASIC_CLK] = {
		.base = GCCMW_SATA_LN_ASIC_CBCR(GCCMW_SATA6),
		.input_clk = sata_ln6_asic_clk_src,
	},
	/* 5.2.4.2.48 Family SATA_LN6_RX */
	[GCCMW_SATA_LN6_RX_CLK] = {
		.base = GCCMW_SATA_LN_RX_CBCR(GCCMW_SATA6),
		.input_clk = sata_ln6_rx_clk_src,
	},
	/* 5.2.4.2.49 Family SATA_LN7_ASIC */
	[GCCMW_SATA_LN7_ASIC_CLK] = {
		.base = GCCMW_SATA_LN_ASIC_CBCR(GCCMW_SATA7),
		.input_clk = sata_ln7_asic_clk_src,
	},
	/* 5.2.4.2.50 Family SATA_LN7_RX */
	[GCCMW_SATA_LN7_RX_CLK] = {
		.base = GCCMW_SATA_LN_RX_CBCR(GCCMW_SATA7),
		.input_clk = sata_ln7_rx_clk_src,
	},
	/* 5.2.4.2.51 Family SATA_PMALIVE */
	[GCCMW_SATA_LN0_PMALIVE_CLK] = {
		.base = GCCMW_SATA0_LN_PMALIVE_CBCR,
		.input_clk = sata_pmalive_clk_src,
	},
	[GCCMW_SATA_LN1_PMALIVE_CLK] = {
		.base = GCCMW_SATA_LN_PMALIVE_CBCR(GCCMW_SATA1),
		.input_clk = sata_pmalive_clk_src,
	},
	[GCCMW_SATA_LN2_PMALIVE_CLK] = {
		.base = GCCMW_SATA_LN_PMALIVE_CBCR(GCCMW_SATA2),
		.input_clk = sata_pmalive_clk_src,
	},
	[GCCMW_SATA_LN3_PMALIVE_CLK] = {
		.base = GCCMW_SATA_LN_PMALIVE_CBCR(GCCMW_SATA3),
		.input_clk = sata_pmalive_clk_src,
	},
	[GCCMW_SATA_LN4_PMALIVE_CLK] = {
		.base = GCCMW_SATA4_LN_PMALIVE_CBCR,
		.input_clk = sata_pmalive_clk_src,
	},
	[GCCMW_SATA_LN5_PMALIVE_CLK] = {
		.base = GCCMW_SATA_LN_PMALIVE_CBCR(GCCMW_SATA5),
		.input_clk = sata_pmalive_clk_src,
	},
	[GCCMW_SATA_LN6_PMALIVE_CLK] = {
		.base = GCCMW_SATA_LN_PMALIVE_CBCR(GCCMW_SATA6),
		.input_clk = sata_pmalive_clk_src,
	},
	[GCCMW_SATA_LN7_PMALIVE_CLK] = {
		.base = GCCMW_SATA_LN_PMALIVE_CBCR(GCCMW_SATA7),
		.input_clk = sata_pmalive_clk_src,
	},
	/* 5.2.4.2.52 Family SATA_Q22 */
	[GCCMW_SATA_LN0_AXI_CLK] = {
		.base = GCCMW_SATA0_LN_AXI_CBCR,
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN1_AXI_CLK] = {
		.base = GCCMW_SATA_LN_AXI_CBCR(GCCMW_SATA1),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN2_AXI_CLK] = {
		.base = GCCMW_SATA_LN_AXI_CBCR(GCCMW_SATA2),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN3_AXI_CLK] = {
		.base = GCCMW_SATA_LN_AXI_CBCR(GCCMW_SATA3),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN4_AXI_CLK] = {
		.base = GCCMW_SATA4_LN_AXI_CBCR,
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN5_AXI_CLK] = {
		.base = GCCMW_SATA_LN_AXI_CBCR(GCCMW_SATA5),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN6_AXI_CLK] = {
		.base = GCCMW_SATA_LN_AXI_CBCR(GCCMW_SATA6),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN7_AXI_CLK] = {
		.base = GCCMW_SATA_LN_AXI_CBCR(GCCMW_SATA7),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN0_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA0_LN_QSMMUV3_CLIENT_CBCR,
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN1_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA_LN_QSMMUV3_CLIENT_CBCR(GCCMW_SATA1),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN2_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA_LN_QSMMUV3_CLIENT_CBCR(GCCMW_SATA2),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN3_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA_LN_QSMMUV3_CLIENT_CBCR(GCCMW_SATA3),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN4_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA4_LN_QSMMUV3_CLIENT_CBCR,
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN5_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA_LN_QSMMUV3_CLIENT_CBCR(GCCMW_SATA5),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN6_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA_LN_QSMMUV3_CLIENT_CBCR(GCCMW_SATA6),
		.input_clk = sata_q22_clk_src,
	},
	[GCCMW_SATA_LN7_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_SATA_LN_QSMMUV3_CLIENT_CBCR(GCCMW_SATA7),
		.input_clk = sata_q22_clk_src,
	},
	/* 5.2.4.2.53 Family SATA_RXOOB */
	[GCCMW_SATA_LN0_RXOOB_CLK] = {
		.base = GCCMW_SATA0_LN_RXOOB_CBCR,
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_LN1_RXOOB_CLK] = {
		.base = GCCMW_SATA_LN_RXOOB_CBCR(GCCMW_SATA1),
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_LN2_RXOOB_CLK] = {
		.base = GCCMW_SATA_LN_RXOOB_CBCR(GCCMW_SATA2),
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_LN3_RXOOB_CLK] = {
		.base = GCCMW_SATA_LN_RXOOB_CBCR(GCCMW_SATA3),
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_LN4_RXOOB_CLK] = {
		.base = GCCMW_SATA4_LN_RXOOB_CBCR,
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_LN5_RXOOB_CLK] = {
		.base = GCCMW_SATA_LN_RXOOB_CBCR(GCCMW_SATA5),
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_LN6_RXOOB_CLK] = {
		.base = GCCMW_SATA_LN_RXOOB_CBCR(GCCMW_SATA6),
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_LN7_RXOOB_CLK] = {
		.base = GCCMW_SATA_LN_RXOOB_CBCR(GCCMW_SATA7),
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_PHY0_RXOOB_CLK] = {
		.base = GCCMW_SATA_PHY0_RXOOB_CBCR,
		.input_clk = sata_rxoob_clk_src,
	},
	[GCCMW_SATA_PHY1_RXOOB_CLK] = {
		.base = GCCMW_SATA_PHY1_RXOOB_CBCR,
		.input_clk = sata_rxoob_clk_src,
	},
	/* 5.2.4.2.54 Family SATA_SS_CSFPB */
	[GCCMW_SATA_VBU_AHB_CFG_CLK] = {
		.base = GCCMW_SATA_VBU_AHB_CFG_CBCR,
		.input_clk = sata_csfpb_clk_src,
	},
	/* 5.2.4.2.55 Family SATA_VBU */
	[GCCMW_SATA_VBU_CORE_CLK] = {
		.base = GCCMW_SATA_VBU_CORE_CBCR,
		.input_clk = sata_vbu_clk_src,
	},
	/* 5.2.4.2.56 Family SEC_CTRL */
	[GCCMW_SEC_CTRL_CLK] = {
		.base = GCCMW_SEC_CTRL_CBCR,
		.input_clk = sec_ctrl_clk_src,
	},
	/* 5.2.4.2.57 Family SLEEP_CLK */
	[GCCMW_FAN_TACH_SLEEP_CLK] = {
		.base = GCCMW_FAN_TACH_SLEEP_CBCR,
		.input_clk = sec_ctrl_clk_src,
	},
	/* 5.2.4.2.58 Family SYS_FAB */
	[GCCMW_CE1_AHB_CLK] = {
		.base = GCCMW_CE_AHB_CBCR(GCCMW_CE1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_CE1_AXI_CLK] = {
		.base = GCCMW_CE_AXI_CBCR(GCCMW_CE1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_CE1_IOMMU_CFG_AHB_CLK] = {
		.base = GCCMW_CE_IOMMU_CFG_AHB_CBCR(GCCMW_CE1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_CE1_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_CE_QSMMUV3_CLIENT_CBCR(GCCMW_CE1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_EMAC0_AXI_CLK] = {
		.base = GCCMW_EMAC_AXI_CBCR(GCCMW_EMAC0),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_EMAC0_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_EMAC_QSMMUV3_CLIENT_CBCR(GCCMW_EMAC0),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_EMAC1_AXI_CLK] = {
		.base = GCCMW_EMAC_AXI_CBCR(GCCMW_EMAC1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_EMAC1_QSMMUV3_CLIENT_CLK] = {
		.base = GCCMW_EMAC_QSMMUV3_CLIENT_CBCR(GCCMW_EMAC1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_QDSS_FCLK] = {
		.base = GCCMW_QDSS_FCLK_CBCR,
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_QDSS_STM_CLK] = {
		.base = GCCMW_QDSS_STM_CBCR,
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFAB_CE1_FCLK] = {
		.base = GCCMW_SFAB_CE1_FCLK_CBCR,
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFAB_EMAC0_FCLK] = {
		.base = GCCMW_SFAB_EMAC0_FCLK_CBCR,
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFAB_EMAC1_FCLK] = {
		.base = GCCMW_SFAB_EMAC1_FCLK_CBCR,
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFAB_VBU_FCLK] = {
		.base = GCCMW_SFAB_VBU_FCLK_CBCR,
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFVBU_CE1_DVM_CLK] = {
		.base = GCCMW_CE_SFVBU_DVM_CBCR(GCCMW_CE1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFVBU_EMAC0_DVM_CLK] = {
		.base = GCCMW_EMAC_SFVBU_DVM_CBCR(GCCMW_EMAC0),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFVBU_EMAC1_DVM_CLK] = {
		.base = GCCMW_EMAC_SFVBU_DVM_CBCR(GCCMW_EMAC1),
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFVBU_Q22M_CLK] = {
		.base = GCCMW_SFAB_VBU_Q22M_CBCR,
		.input_clk = system_fab_clk_src,
	},
	[GCCMW_SFVBU_Q22S_CLK] = {
		.base = GCCMW_SFAB_VBU_Q22S_CBCR,
		.input_clk = system_fab_clk_src,
	},
	/* 5.2.4.2.59 Family SYS_FAB_VBU */
	[GCCMW_SFAB_VBU_CORE_CLK] = {
		.base = GCCMW_SFAB_VBU_CORE_CBCR,
		.input_clk = sys_fab_vbu_clk_src,
	},
	/* 5.2.4.2.60 Family W_CONF_SFPB */
	[GCCMW_EMAC_AHB2PHY_CFG_AHB_CLK] = {
		.base = GCCMW_EMAC_AHB2PHY_CFG_AHB_CBCR,
		.input_clk = wcsfpb_clk_src,
	},
	[GCCMW_QDSS_CFG_AHB_CLK] = {
		.base = GCCMW_QDSS_CFG_AHB_CBCR,
		.input_clk = wcsfpb_clk_src,
	},
	[GCCMW_QDSS_DAP_AHB_CLK] = {
		.base = GCCMW_QDSS_DAP_AHB_CBCR,
		.input_clk = wcsfpb_clk_src,
	},
	[GCCMW_QDSS_RBCPR_XPU_AHB_CLK] = {
		.base = GCCMW_QDSS_RBCPR_XPU_AHB_CBCR,
		.input_clk = wcsfpb_clk_src,
	},
	[GCCMW_SFAB_VBU_CFG_CLK] = {
		.base = GCCMW_SFAB_VBU_CFG_CBCR,
		.input_clk = wcsfpb_clk_src,
	},
	[GCCMW_TSENS1_AHB_CLK] = {
		.base = GCCMW_TSENS_AHB_CBCR(GCCMW_TSENS1),
		.input_clk = wcsfpb_clk_src,
	},
	/* TIC missing */
	[GCCMW_TIC_CLK] = {
		.base = GCCMW_TIC_CBCR,
		.input_clk = tic_clk,
	},
	/* 5.1.4.2.1 Family HMSS_BUS */
	[GCCE_HMSS_NE_AHB_CLK] = {
		.base = GCCE_HMSS_NE_AHB_CBCR,
		.input_clk = hmss_e_bus_clk_src,
	},
	[GCCE_HMSS_SE_AHB_CLK] = {
		.base = GCCE_HMSS_SE_AHB_CBCR,
		.input_clk = hmss_e_bus_clk_src,
	},
	/* 5.1.4.2.2 Family N2_CSFPB */
	[GCCE_MDDR4_AHB_CLK] = {
		.base = GCCE_MDDR4_AHB_CBCR,
		.input_clk = n2_csfpb_clk_src,
	},
	[GCCE_TSENS5_AHB_CLK] = {
		.base = GCCE_TSENS_AHB_CBCR(GCCE_TSENS5),
		.input_clk = n2_csfpb_clk_src,
	},
	/* 5.1.4.2.3 Family PCIE_SS_0_VBU */
	[GCCE_PCIE_SS_0_VBU_CORE_CLK] = {
		.base = GCCE_PCIE_SS_VBU_CORE_CBCR(GCCE_PCIE_SS_0),
		.input_clk = pcie_ss_0_vbu_clk_src,
	},
	/* 5.1.4.2.4 Family PCIE_SS_1_VBU */
	[GCCE_PCIE_SS_1_VBU_CORE_CLK] = {
		.base = GCCE_PCIE_SS_VBU_CORE_CBCR(GCCE_PCIE_SS_1),
		.input_clk = pcie_ss_1_vbu_clk_src,
	},
	/* 5.1.4.2.5 Family QDSS_ATB */
	[GCCE_AT_E2_CLK] = {
		.base = GCCE_AT_E2_CBCR,
		.input_clk = e_qdss_at_clk_src,
	},
	[GCCE_AT_E3_CLK] = {
		.base = GCCE_AT_E3_CBCR,
		.input_clk = e_qdss_at_clk_src,
	},
	[GCCE_AT_E5_CLK] = {
		.base = GCCE_AT_E5_CBCR,
		.input_clk = e_qdss_at_clk_src,
	},
	[GCCE_AT_E7_CLK] = {
		.base = GCCE_AT_E7_CBCR,
		.input_clk = e_qdss_at_clk_src,
	},
	[GCCE_AT_E9_CLK] = {
		.base = GCCE_AT_E9_CBCR,
		.input_clk = e_qdss_at_clk_src,
	},
	[GCCE_HMSS_AT_E_CLK] = {
		.base = GCCE_HMSS_AT_E_CBCR,
		.input_clk = e_qdss_at_clk_src,
	},
	/* 5.1.4.2.6 Family S2_CSFPB */
	[GCCE_MDDR5_AHB_CLK] = {
		.base = GCCE_MDDR5_AHB_CBCR,
		.input_clk = s2_csfpb_clk_src,
	},
	[GCCE_TSENS8_AHB_CLK] = {
		.base = GCCE_TSENS_AHB_CBCR(GCCE_TSENS8),
		.input_clk = s2_csfpb_clk_src,
	},
	/* 5.1.4.2.7 Family SE_CONF_SFPB */
	[GCCE_PCIE_SS_0_SECSFPB_AHB_CLK] = {
		.base = GCCE_PCIE_SS_SECSFPB_AHB_CBCR(GCCE_PCIE_SS_0),
		.input_clk = secsfpb_clk_src,
	},
	[GCCE_PCIE_SS_1_SECSFPB_AHB_CLK] = {
		.base = GCCE_PCIE_SS_SECSFPB_AHB_CBCR(GCCE_PCIE_SS_1),
		.input_clk = secsfpb_clk_src,
	},
	[GCCE_PSCLKGEN_CFG_AHB_CLK] = {
		.base = GCCE_PSCLKGEN_CFG_AHB_CBCR,
		.input_clk = secsfpb_clk_src,
	},
	[GCCE_TSENS6_AHB_CLK] = {
		.base = GCCE_TSENS_AHB_CBCR(GCCE_TSENS6),
		.input_clk = secsfpb_clk_src,
	},
	[GCCE_TSENS7_AHB_CLK] = {
		.base = GCCE_TSENS_AHB_CBCR(GCCE_TSENS7),
		.input_clk = secsfpb_clk_src,
	},
	/* 5.1.4.2.8 Family XO */
	[GCCE_XO_CLK] = {
		.base = GCCE_XO_CBCR,
		.input_clk = gcce_xo_clk_src,
	},
	[GCCE_TSENS5_EXT_CLK] = {
		.base = GCCE_TSENS_EXT_CBCR(GCCE_TSENS5),
		.input_clk = gcce_xo_clk_src,
	},
	[GCCE_TSENS6_EXT_CLK] = {
		.base = GCCE_TSENS_EXT_CBCR(GCCE_TSENS6),
		.input_clk = gcce_xo_clk_src,
	},
	[GCCE_TSENS7_EXT_CLK] = {
		.base = GCCE_TSENS_EXT_CBCR(GCCE_TSENS7),
		.input_clk = gcce_xo_clk_src,
	},
	[GCCE_TSENS8_EXT_CLK] = {
		.base = GCCE_TSENS_EXT_CBCR(GCCE_TSENS8),
		.input_clk = gcce_xo_clk_src,
	},
	[GCCE_XO_DIV4_CLK] = {
		.base = GCCE_XO_DIV4_CBCR,
		.input_clk = gcce_xo_div4_clk_src,
	},
	/* 5.5.4.2.5 Family PCIE_SFPB */
	[PCIE_CC_0_PCIE_PHY_AA_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(0, PCIE_CC_PCIE_PHY_AA),
		.input_clk = pcie0_sfpb_clk_src,
	},
	[PCIE_CC_0_PCIE_PHY_AB_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(0, PCIE_CC_PCIE_PHY_AB),
		.input_clk = pcie0_sfpb_clk_src,
	},
	[PCIE_CC_0_PCIE_PHY_B_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(0, PCIE_CC_PCIE_PHY_B),
		.input_clk = pcie0_sfpb_clk_src,
	},
	[PCIE_CC_0_PCIE_PHY_C_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(0, PCIE_CC_PCIE_PHY_C),
		.input_clk = pcie0_sfpb_clk_src,
	},
	/* 5.5.4.2.6 Family PCIE_X16_MSTR_Q23 */
	[PCIE_CC_0_PCIE_X16_MMU_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X16_MMU_MSTR_Q23_CBCR(0),
		.input_clk = pcie_x16_mstr_q23_clk_src,
	},
	[PCIE_CC_0_PCIE_X16_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X16_MSTR_Q23_CBCR(0),
		.input_clk = pcie_x16_mstr_q23_clk_src,
	},
	/* 5.5.4.2.7 Family PCIE_X16_SLV_Q23 */
	[PCIE_CC_0_PCIE_X16_SLV_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X16_SLV_Q23_CBCR(0),
		.input_clk = pcie_x16_slv_q23_clk_src,
	},
	/* 5.5.4.2.8 Family PCIE_X8_MSTR_Q23 */
	[PCIE_CC_0_PCIE_X8_0_MMU_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MMU_MSTR_Q23_CBCR(0, PCIE_CC_X8_0),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	[PCIE_CC_0_PCIE_X8_0_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MSTR_Q23_CBCR(0, PCIE_CC_X8_0),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	[PCIE_CC_0_PCIE_X8_1_MMU_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MMU_MSTR_Q23_CBCR(0, PCIE_CC_X8_1),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	[PCIE_CC_0_PCIE_X8_1_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MSTR_Q23_CBCR(0, PCIE_CC_X8_1),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	/* 5.5.4.2.9 Family PCIE_X8_SLV_Q23 */
	[PCIE_CC_0_PCIE_X8_0_SLV_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_SLV_Q23_CBCR(0, PCIE_CC_X8_0),
		.input_clk = pcie_x8_slv_q23_clk_src,
	},
	[PCIE_CC_0_PCIE_X8_1_SLV_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_SLV_Q23_CBCR(0, PCIE_CC_X8_1),
		.input_clk = pcie_x8_slv_q23_clk_src,
	},
	/* 5.5.4.2.5 Family PCIE_SFPB */
	[PCIE_CC_1_PCIE_PHY_AA_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(1, PCIE_CC_PCIE_PHY_AA),
		.input_clk = pcie1_sfpb_clk_src,
	},
	[PCIE_CC_1_PCIE_PHY_AB_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(1, PCIE_CC_PCIE_PHY_AB),
		.input_clk = pcie1_sfpb_clk_src,
	},
	[PCIE_CC_1_PCIE_PHY_B_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(1, PCIE_CC_PCIE_PHY_B),
		.input_clk = pcie1_sfpb_clk_src,
	},
	[PCIE_CC_1_PCIE_PHY_C_CFG_AHB_CLK] = {
		.base = PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(1, PCIE_CC_PCIE_PHY_C),
		.input_clk = pcie1_sfpb_clk_src,
	},
	/* 5.5.4.2.6 Family PCIE_X16_MSTR_Q23 */
	[PCIE_CC_1_PCIE_X16_MMU_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X16_MMU_MSTR_Q23_CBCR(1),
		.input_clk = pcie_x16_mstr_q23_clk_src,
	},
	[PCIE_CC_1_PCIE_X16_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X16_MSTR_Q23_CBCR(1),
		.input_clk = pcie_x16_mstr_q23_clk_src,
	},
	/* 5.5.4.2.7 Family PCIE_X16_SLV_Q23 */
	[PCIE_CC_1_PCIE_X16_SLV_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X16_SLV_Q23_CBCR(1),
		.input_clk = pcie_x16_slv_q23_clk_src,
	},
	/* 5.5.4.2.8 Family PCIE_X8_MSTR_Q23 */
	[PCIE_CC_1_PCIE_X8_0_MMU_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MMU_MSTR_Q23_CBCR(1, PCIE_CC_X8_0),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	[PCIE_CC_1_PCIE_X8_0_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MSTR_Q23_CBCR(1, PCIE_CC_X8_0),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	[PCIE_CC_1_PCIE_X8_1_MMU_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MMU_MSTR_Q23_CBCR(1, PCIE_CC_X8_1),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	[PCIE_CC_1_PCIE_X8_1_MSTR_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_MSTR_Q23_CBCR(1, PCIE_CC_X8_1),
		.input_clk = pcie_x8_mstr_q23_clk_src,
	},
	/* 5.5.4.2.9 Family PCIE_X8_SLV_Q23 */
	[PCIE_CC_1_PCIE_X8_0_SLV_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_SLV_Q23_CBCR(1, PCIE_CC_X8_0),
		.input_clk = pcie_x8_slv_q23_clk_src,
	},
	[PCIE_CC_1_PCIE_X8_1_SLV_Q23_CLK] = {
		.base = PCIE_CC_PCIE_X8_SLV_Q23_CBCR(1, PCIE_CC_X8_1),
		.input_clk = pcie_x8_slv_q23_clk_src,
	},
};

uint32_t get_branch_clock_freq(clk_clk_t clk)
{
	if (clk >= NR_BRANCH_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(clk_cbcs[clk].input_clk);
}

int enable_branch_clock(clk_clk_t clk)
{
	struct clk_cbc *cfg;

	if (clk >= NR_BRANCH_CLKS)
		return -EINVAL;

	cfg = &clk_cbcs[clk];
	cfg->refcnt++;
	if (cfg->refcnt > 1)
		return 0;
	__clk_enable_branch(cfg->base);
	return clk_enable(cfg->input_clk);
}

int set_branch_clock_freq(clk_clk_t clk, uint32_t freq)
{
	return 0;
}

struct clk_driver clk_branch_clock = {
	.max_clocks = NR_BRANCH_CLKS,
	.enable = enable_branch_clock,
	.disable = NULL,
	.get_freq = get_branch_clock_freq,
	.set_freq = set_branch_clock_freq,
};

extern caddr_t Qdf2400V10CBCRs[];
extern clk_t clk_1_0[];
extern clk_mux_t clk_mux_1_0[];

extern caddr_t Qdf2400V11CBCRs[];
extern clk_t clk_1_1[];
extern clk_mux_t clk_mux_1_1[];

extern caddr_t Qdf2400V20CBCRs[];
extern clk_t clk_2_0[];
extern clk_mux_t clk_mux_2_0[];

static freqplan_t freqplan_qdf2400[NR_FREQPLANS] = {
	{
		.major = 1,
		.minor = 0,
		.cbcr  = &Qdf2400V10CBCRs[0],
		.srcs  = &clk_mux_1_0[0],
		.clks  = &clk_1_0[0],
	},
	{
		.major = 1,
		.minor = 1,
		.cbcr  = &Qdf2400V10CBCRs[0], /* Use 1.0 CBC */
		.srcs  = &clk_mux_1_1[0],
		.clks  = &clk_1_1[0],
	},
	{
		.major = 2,
		.minor = 0,
		.cbcr  = &Qdf2400V20CBCRs[0],
		.srcs  = &clk_mux_2_0[0],
		.clks  = &clk_2_0[0],
	},
};

int freqplan_id = INVALID_FREQPLAN;

freqplan_t *freqplan_get(void)
{
	int id = freqplan_get_plan();

	if (id == INVALID_FREQPLAN)
		return NULL;
	return &freqplan_qdf2400[id];
}

int freqplan_get_plan(void)
{
	int i;
	freqplan_t *freq_plan = NULL;
	uint8_t major = 2;
	uint8_t minor = 0, minor_info;

	if (freqplan_id != INVALID_FREQPLAN)
		return freqplan_id;

	soc_get_version(major, minor_info);
	if (major == 1 && minor_info >= 1)
		minor = 1;

	for (i = 0; i < NR_FREQPLANS; i++) {
		freq_plan = &freqplan_qdf2400[i];
		if (major == freq_plan->major &&
		    minor == freq_plan->minor) {
			freqplan_id = i;
			return i;
		}
	}
	return INVALID_FREQPLAN;
}

#ifdef CONFIG_QDF2400_CLK_FREQPLAN
static bool freqplan_config_plls(void)
{
	bool ret = true;
	clk_clk_t pll;

	for (pll = 0; pll < NR_FABIA_PLL_CLKS; pll++) {
		if (clk_enable(clkid(CLK_FABIA_PLL, pll)) != 0)
			ret = false;
	}
	return ret;
}

static bool freqplan_config_srcs(clk_mux_t *src)
{
	bool ret = true;

	while (src && src->output_clk != invalid_clk) {
		clk_select_source(src->output_clk, src->input_clk);
		if (clk_enable(src->output_clk) != 0)
			ret = false;
		src++;
	}
	return ret;
}

static bool freqplan_enable_clks(clk_t *clk)
{
	bool ret = true;

	while (clk && *clk != invalid_clk) {
		if (clk_enable(*clk) != 0)
			ret = false;
		clk++;
	}
	return ret;
}

static void freqplan_enable_branches(caddr_t *cbc)
{
	while (cbc && *cbc != 0) {
		__clk_enable_branch(*cbc);
		cbc++;
	}
}

static int clk_freqplan_init(void)
{
	freqplan_t *freqplan = freqplan_get();

	if (!freqplan)
		return -ENODEV;

	/* Configure clocks in the following order:
	 *   1) Configure PLLs
	 *   2) Enable clock branches to drive RCG outputs
	 *   3) Configure misc
	 *   4) Configure RCGs
	 */
	if (!freqplan_config_plls())
		return -EINVAL;
	if (!freqplan_config_srcs(freqplan->srcs))
		return -EINVAL;
	if (!freqplan_enable_clks(freqplan->clks))
		return -EINVAL;
	freqplan_enable_branches(freqplan->cbcr);
	return 0;
}
#else
static void freqplan_select_source(clk_mux_t *src)
{
	while (src && src->output_clk != invalid_clk) {
		clk_select_source(src->output_clk, src->input_clk);
		src++;
	}
}

static inline int clk_freqplan_init(void)
{
	freqplan_t *freqplan = freqplan_get();

	if (!freqplan)
		return -ENODEV;
	freqplan_select_source(freqplan->srcs);
	return 0;
}
#endif

int clk_hw_ctrl_init(void)
{
	clk_register_driver(CLK_PADRING, &clk_padring);
	clk_register_driver(CLK_FABIA_PLL, &clk_fabia_pll);
	clk_register_driver(CLK_FABIA_PLLOUT, &clk_fabia_pllout);
	clk_register_driver(CLK_ROOT_CLOCK, &clk_root_clock);
	clk_register_driver(CLK_BRANCH_CLOCK, &clk_branch_clock);
	return clk_freqplan_init();
}
