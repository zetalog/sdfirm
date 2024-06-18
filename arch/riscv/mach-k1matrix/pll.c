
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
 * @(#)pll.c: k1matrix clock/reset generator PLL implementations
 * $Id: pll.c,v 1.1 2024-06-05 17:37:00 zhenglv Exp $
 */

#include <target/clk.h>

struct pll_clk {
	uint32_t Fref;
	uint32_t Fvco;
	uint32_t Fout;
	cru_flags_t flags;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[COM_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = COM_PLL_FREQ,
		.Fout = COM_PLL_FREQ,
		.flags = CRU_4PHASE,
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
		.flags = CRU_4PHASE,
	},
	[DDR0_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = DDR0_PLL_FREQ,
		.Fout = DDR0_PLL_FREQ,
		.flags = CRU_FOUT0,
	},
	[DDR1_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = DDR1_PLL_FREQ,
		.Fout = DDR1_PLL_FREQ,
		.flags = CRU_FOUT1,
	},
	[CPU0_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = CPU0_PLL_FREQ,
		.Fout = CPU0_PLL_FREQ,
		.flags = CRU_FOUT0,
	},
	[CPU1_PLL] = {
		.Fref = OSC_CLK_FREQ,
		.Fvco = CPU1_PLL_FREQ,
		.Fout = CPU1_PLL_FREQ,
		.flags = CRU_FOUT1,
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

void clk_pll_dump(void)
{
	int i;
	const char *name;

	for (i = 0; i < NR_PLL_CLKS; i++) {
		name = clk_get_mnemonic(clkid(CLK_PLL, i));
		if (name)
			printf("pll  %3d %20s %20s\n", i, name, "osc_clk");
	}
}

void clk_pll_init(void)
{
	clk_register_driver(CLK_PLL, &clk_pll);
}
