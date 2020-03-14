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
 * @(#)clk.c: FU540 (unleashed) specific clock tree implementation
 * $Id: clk.c,v 1.1 2019-10-17 15:46:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/paging.h>

#ifdef CONFIG_CONSOLE_COMMAND
const char *output_clk_names[NR_OUTPUT_CLKS] = {
	[CORECLK] = "coreclk",
	[TLCLK] = "tlclk",
};

static const char *get_output_clk_name(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return NULL;
	return output_clk_names[clk];
}
#else
#define get_output_clk_name	NULL
#endif

static int enable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;
	if (clk == CORECLK) {
		clk_enable(corepll);
		core_select_corepll();
	}
	if (clk == TLCLK)
		clk_enable(coreclk);
	return 0;
}

static void disable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return;
	if (clk == CORECLK)
		core_select_hfclk();
	if (clk == TLCLK)
		clk_disable(coreclk);
}

static uint32_t get_output_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return INVALID_FREQ;

	if (clk == CORECLK) {
		if (core_selected_corepll())
			return clk_get_frequency(corepll);
		else
			return HFCLK_FREQ;
	}
	if (clk == TLCLK)
		return clk_get_frequency(coreclk) / 2;
	return INVALID_FREQ;
}

static int set_output_clk_freq(clk_clk_t clk, uint32_t freq)
{
	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;
	if (clk == CORECLK) {
		if (freq != FREQ_33MHZ) {
			clk_set_frequency(corepll, freq);
			core_select_corepll();
		} else
			core_select_hfclk();
	}
	if (clk == TLCLK)
		clk_set_frequency(coreclk, freq / 2);
	return 0;
}

struct clk_driver clk_output = {
	.max_clocks = NR_OUTPUT_CLKS,
	.enable = enable_output_clk,
	.disable = disable_output_clk,
	.get_freq = get_output_clk_freq,
	.set_freq = set_output_clk_freq,
	.get_name = get_output_clk_name,
};

struct pll_clk {
	uint32_t freq;
	bool enabled;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[COREPLL] = {
		.freq = CORECLK_FREQ,
		.enabled = false,
	},
	[DDRPLL] = {
		.freq = DDRCTRLCLK_FREQ,
		.enabled = false,
	},
	[GEMGXLPLL] = {
		.freq = GEMGXLCLK_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *pll_clk_names[NR_PLL_CLKS] = {
	[COREPLL] = "corepll",
	[DDRPLL] = "ddrpll",
	[GEMGXLPLL] = "gemgxlpll",
};

static const char *get_pll_name(clk_clk_t clk)
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
		pll_config_freq(clk, pll_clks[clk].freq);
		pll_clks[clk].enabled = true;
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
	pll_output_disable(clk);
}

static uint32_t get_pll_freq(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return INVALID_FREQ;
	return pll_clks[clk].freq;
}

static int set_pll_freq(clk_clk_t clk, uint32_t freq)
{
	if (clk >= NR_PLL_CLKS)
		return -EINVAL;

	if (pll_clks[clk].freq != freq) {
		pll_clks[clk].freq = freq;
		pll_clks[clk].enabled = false;
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
	.get_name = get_pll_name,
};

uint32_t input_clks[NR_INPUT_CLKS] = {
	[HFCLK] = HFCLK_FREQ,
	[RTCCLK] = RTCCLK_FREQ,
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *input_clk_names[NR_INPUT_CLKS] = {
	[HFCLK] = "hfclk",
	[RTCCLK] = "rtcclk",
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

static uint32_t get_input_clk_freq(clk_clk_t clk)
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
	.get_name = get_input_clk_name,
};

static bool clk_hw_init = false;

void board_init_clock(void)
{
	if (!clk_hw_init) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_PLL, &clk_pll);
		clk_register_driver(CLK_OUTPUT, &clk_output);
		clk_enable(tlclk);
	}
	clk_hw_init = true;
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}
