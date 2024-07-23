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
 * @(#)cru_rst.c: k1matrix reset unit implementations
 * $Id: cru_rst.c,v 1.1 2024-06-05 17:37:00 zhenglv Exp $
 */

#include <target/clk.h>

struct rstn_clk {
	caddr_t rst_reg;
	clk_t src;
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
	[PCIE9_PERST_N] = {
		.rst_reg = CRU_PCIE9_SW_RESET,
		.flags = CRU_RESET_2BIT,
	},
	[CLUSTER0_SRST_N] = {
		.rst_reg = CRU_CLUSTER0_SW_RESET,
		.flags = CRU_RESET,
	},
	[C1_CFG_SRST_N] = {
		.rst_reg = CRU_CLUSTER1_SW_RESET,
		.flags = CRU_RESET_4BIT,
	},
	[C0_CFG_SRST_N] = {
		.rst_reg = CRU_CLUSTER0_COREX_SW_RESET,
		.flags = CRU_RESET,
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
	[CLUSTER0_SRST_N] = "cluster0_srst_n",
	[C0_CFG_SRST_N] = "c0_cfg_srst_n",

	[C1_CFG_SRST_N] = "c1_cfg_srst_n",
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

static int enable_rstn(clk_clk_t clk)
{
	if (clk >= NR_RSTN_CLKS)
		return -EINVAL;
	if (rstn_clks[clk].flags & CRU_SRC_CLKSEL)
		clk_select_source(rstn_clks[clk].src, 1);
	else
	clk_enable(rstn_clks[clk].src);
	cru_trace(true, get_rstn_name(clk));
	__raw_setl(CRU_RSTN(0), rstn_clks[clk].rst_reg);
	if (rstn_clks[clk].flags & CRU_RESET_2BIT)
		__raw_setl(CRU_RSTN(1), rstn_clks[clk].rst_reg);
	if (rstn_clks[clk].flags & CRU_RESET_3BIT) {
		__raw_setl(CRU_RSTN(1), rstn_clks[clk].rst_reg);
		__raw_setl(CRU_RSTN(2), rstn_clks[clk].rst_reg);
	}
	if (rstn_clks[clk].flags & CRU_RESET_4BIT) {
		__raw_setl(CRU_RSTN(1), rstn_clks[clk].rst_reg);
		__raw_setl(CRU_RSTN(2), rstn_clks[clk].rst_reg);
		__raw_setl(CRU_RSTN(3), rstn_clks[clk].rst_reg);
	}
	return 0;
}


static void disable_rstn(clk_clk_t clk)
{
	if (clk >= NR_RSTN_CLKS)
		return;
	if (rstn_clks[clk].flags & CRU_SRC_CLKSEL)
		clk_select_source(rstn_clks[clk].src, 0);
	else
		clk_disable(rstn_clks[clk].src);
	__raw_clearl(CRU_RSTN(0), rstn_clks[clk].rst_reg);
	if (rstn_clks[clk].flags & CRU_RESET_2BIT)
		__raw_clearl(CRU_RSTN(1), rstn_clks[clk].rst_reg);
	if (rstn_clks[clk].flags & CRU_RESET_3BIT) {
		__raw_clearl(CRU_RSTN(1), rstn_clks[clk].rst_reg);
		__raw_clearl(CRU_RSTN(2), rstn_clks[clk].rst_reg);
	}
	if (rstn_clks[clk].flags & CRU_RESET_4BIT) {
		__raw_clearl(CRU_RSTN(1), rstn_clks[clk].rst_reg);
		__raw_clearl(CRU_RSTN(2), rstn_clks[clk].rst_reg);
		__raw_clearl(CRU_RSTN(3), rstn_clks[clk].rst_reg);
	}
}

const struct clk_driver clk_rstn = {
	.max_clocks = NR_RSTN_CLKS,
	.enable = enable_rstn,
	.disable = disable_rstn,
	.get_freq = NULL,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_rstn_name,
};

void clk_rst_dump(void)
{
}

void clk_rst_init(void)
{
	clk_register_driver(CLK_RST, &clk_rstn);
}
