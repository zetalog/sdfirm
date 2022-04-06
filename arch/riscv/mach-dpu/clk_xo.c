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
 * @(#)clk.c: DPU specific clock tree implementation
 * $Id: clk.c,v 1.1 2020-03-06 16:07:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/panic.h>
#include <target/ddr.h>

#define CLK_SEL_F	_BV(0)
#define CLK_EN_F	_BV(1)
#define CLK_SRST_F	_BV(2)

struct reset_clk {
	clk_t clk_src;
	uint8_t flags;
	uint16_t axi_periphs;
};

#ifdef CONFIG_DPU_GEN2
#define CLK_RST_MAP	ULL(0x00001FFFFFFF)
#else /* CONFIG_DPU_GEN2 */
#define CLK_RST_MAP	ULL(0x000007FFFFFF)
#endif /* CONFIG_DPU_GEN2 */

struct reset_clk reset_clks[NR_RESET_CLKS] = {
	[SRST_GPDPU] = {
		.clk_src = pe_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PCIE0] = {
		.clk_src = axi_clk,
		.flags = CLK_SRST_F,
#ifdef CONFIG_DPU_TCSR
		.axi_periphs = _BV(IMC_PCIE_X16_MST) |
			       _BV(IMC_PCIE_X8_MST) |
			       _BV(IMC_PCIE_X4_0_MST) |
			       _BV(IMC_PCIE_X4_1_MST) |
			       _BV(IMC_PCIE_X16_SLV) |
			       _BV(IMC_PCIE_X8_SLV) |
			       _BV(IMC_PCIE_X4_0_SLV) |
			       _BV(IMC_PCIE_X4_1_SLV) |
			       _BV(IMC_PCIE_X16_DBI) |
			       _BV(IMC_PCIE_X8_DBI) |
			       _BV(IMC_PCIE_X4_0_DBI) |
			       _BV(IMC_PCIE_X4_1_DBI),
#endif
	},
	[SRST_PCIE1] = {
		.clk_src = axi_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_SPI] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_I2C0] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_I2C1] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_I2C2] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_UART] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PLIC] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_GPIO] = {
		.clk_src = apb_clk,
		.flags = CLK_EN_F,
	},
	[SRST_RAM] = {
		.clk_src = apb_clk,
		.flags = CLK_EN_F,
	},
	[SRST_ROM] = {
		.clk_src = apb_clk,
		.flags = CLK_EN_F,
	},
	[SRST_TMR] = {
		.clk_src = xin,
		.flags = CLK_EN_F,
	},
	[SRST_WDT] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_TCSR] = {
		.clk_src = apb_clk,
		.flags = CLK_EN_F,
	},
	[SRST_VPU] = {
		.clk_src = vpu_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_CPU] = {
		.clk_src = cpu_clk,
		.flags = CLK_EN_F,
	},
	[SRST_NOC] = {
		.clk_src = apb_clk,
		.flags = CLK_EN_F,
	},
	[SRST_FLASH] = {
		.clk_src = apb_clk,
		.flags = CLK_EN_F,
	},
	[SRST_DDR0_0] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
#ifdef CONFIG_DPU_TCSR
		.axi_periphs = _BV(IMC_DDR0) | _BV(IMC_DDR0_CTRL),
#endif
	},
	[SRST_DDR0_1] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
#ifdef CONFIG_DPU_TCSR
		.axi_periphs = _BV(IMC_DDR1) | _BV(IMC_DDR1_CTRL),
#endif
	},
	[SRST_DDR1_0] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_DDR1_1] = {
		.clk_src = ddr_clk,
		.flags = CLK_SRST_F,
	},
	/* DDR requires this to be done earlier than speed determination,
	 * thus it should be independent of any ddr clock.
	 */
	[SRST_DDR0_POR] = {
		.clk_src = invalid_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_DDR1_POR] = {
		.clk_src = invalid_clk,
		.flags = CLK_SRST_F,
	},
#ifdef CONFIG_DPU_GEN2
	[SRST_PCIE0_POR] = {
		.clk_src = apb_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_TSENSOR_XO] = {
		.clk_src = tsensor_xo_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_TSENSOR_METS] = {
		.clk_src = tsensor_mets_clk,
		.flags = CLK_SRST_F,
	},
#else /* CONFIG_DPU_GEN2 */
	[SRST_PCIE0_POR] = {
		.clk_src = pcie_ref_clk,
		.flags = CLK_SRST_F,
	},
	[SRST_PCIE1_POR] = {
		.clk_src = pcie_ref_clk,
		.flags = CLK_SRST_F,
	},
#endif /* CONFIG_DPU_GEN2 */
};

#ifdef CONFIG_CLK_MNEMONICS
const char *reset_clk_names[NR_RESET_CLKS] = {
	[SRST_GPDPU] = "srst_gpdpu",
	[SRST_PCIE0] = "srst_pcie0",
	[SRST_PCIE1] = "srst_pcie1",
	[SRST_SPI] = "srst_spi",
	[SRST_I2C0] = "srst_i2c0",
	[SRST_I2C1] = "srst_i2c1",
	[SRST_I2C2] = "srst_i2c2",
	[SRST_UART] = "srst_uart",
	[SRST_PLIC] = "srst_plic",
	[SRST_GPIO] = "srst_gpio",
	[SRST_RAM] = "srst_ram",
	[SRST_ROM] = "srst_rom",
	[SRST_TMR] = "srst_tmr",
	[SRST_WDT] = "srst_wdt",
	[SRST_TCSR] = "srst_tcsr",
	[SRST_VPU] = "srst_vpu",
	[SRST_CPU] = "srst_cpu",
	[SRST_NOC] = "srst_noc",
	[SRST_FLASH] = "srst_flash",
	[SRST_DDR0_0] = "srst_ddr0_0",
	[SRST_DDR0_1] = "srst_ddr0_1",
	[SRST_DDR1_0] = "srst_ddr1_0",
	[SRST_DDR1_1] = "srst_ddr1_1",
	[SRST_DDR0_POR] = "srst_ddr0_por",
	[SRST_DDR1_POR] = "srst_ddr1_por",
	[SRST_PCIE0_POR] = "srst_pcie0_por",
	[SRST_PCIE1_POR] = "srst_pcie1_por",
#ifdef CONFIG_DPU_GEN2
	[SRST_TSENSOR_XO] = "srst_tsensor_xo",
	[SRST_TSENSOR_METS] = "srst_tsensor_mets",
#endif /* CONFIG_DPU_GEN2 */
};

static const char *get_reset_clk_name(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS)
		return NULL;
	return reset_clk_names[clk];
}
#else
#define get_reset_clk_name		NULL
#endif

static int enable_reset_clk(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS || !(_BV_ULL(clk) & CLK_RST_MAP))
		return -EINVAL;
	if (!(reset_clks[clk].flags & CLK_SRST_F))
		return -EINVAL;

	if (reset_clks[clk].clk_src != invalid_clk)
		clk_enable(reset_clks[clk].clk_src);
	dpu_pll_soft_reset(clk);
	reset_clks[clk].flags |= CLK_EN_F;
	imc_axi_register_periphs(reset_clks[clk].axi_periphs);
	return 0;
}

static void disable_reset_clk(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS || !(_BV_ULL(clk) & CLK_RST_MAP))
		return;
	if (!(reset_clks[clk].flags & CLK_SRST_F))
		return;

	if (reset_clks[clk].clk_src != invalid_clk)
		clk_disable(reset_clks[clk].clk_src);
	dpu_pll_soft_reset(clk);
	reset_clks[clk].flags &= ~CLK_EN_F;
	imc_axi_unregister_periphs(reset_clks[clk].axi_periphs);
}

static clk_freq_t get_reset_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_RESET_CLKS || !(_BV_ULL(clk) & CLK_RST_MAP))
		return INVALID_FREQ;
	return clk_get_frequency(reset_clks[clk].clk_src);
}

const struct clk_driver clk_srst = {
	.max_clocks = NR_RESET_CLKS,
	.enable = enable_reset_clk,
	.disable = disable_reset_clk,
	.select = NULL,
	.get_freq = get_reset_clk_freq,
	.set_freq = NULL,
	.get_name = get_reset_clk_name,
};

struct dep_clk {
	clk_t *clk_deps;
};

clk_t vpu_clks[] = {
	vpu_bclk,
	vpu_cclk,
	invalid_clk,
};

struct dep_clk dep_clks[NR_DEP_CLKS] = {
	[VPU_CLK] = {
		.clk_deps = vpu_clks,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *dep_clk_names[NR_DEP_CLKS] = {
	[VPU_CLK] = "vpu_clk",
};

static const char *get_dep_clk_name(clk_clk_t clk)
{
	if (clk >= NR_DEP_CLKS)
		return NULL;
	return dep_clk_names[clk];
}
#else
#define get_dep_clk_name	NULL
#endif

static int enable_dep_clk(clk_clk_t clk)
{
	clk_t *deps;
	int i;

	if (clk >= NR_DEP_CLKS)
		return -EINVAL;
	if (dep_clks[clk].clk_deps == NULL)
		return -EINVAL;

	deps = dep_clks[clk].clk_deps;
	for (i = 0; deps[i] != invalid_clk; i++)
		clk_enable(deps[i]);
	return 0;
}

static void disable_dep_clk(clk_clk_t clk)
{
	clk_t *deps;
	int i;

	if (clk >= NR_DEP_CLKS)
		return;
	if (dep_clks[clk].clk_deps == NULL)
		return;

	deps = dep_clks[clk].clk_deps;
	for (i = 0; deps[i] != invalid_clk; i++)
		clk_disable(deps[i]);
}

static clk_freq_t get_dep_clk_freq(clk_clk_t clk)
{
	clk_t *deps;

	if (clk >= NR_DEP_CLKS)
		return INVALID_FREQ;
	deps = dep_clks[clk].clk_deps;
	return clk_get_frequency(deps[0]);
}

const struct clk_driver clk_dep = {
	.max_clocks = NR_DEP_CLKS,
	.enable = enable_dep_clk,
	.disable = disable_dep_clk,
	.get_freq = get_dep_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_dep_clk_name,
};

struct sel_clk {
	clk_t clk_src;
	bool allow_gating;
	bool enabled;
};

struct sel_clk sel_clks[NR_SEL_CLKS] = {
#ifdef CONFIG_DPU_RES
	[CPU_CLK] = {
#ifdef CONFIG_DPU_IMC
		.clk_src = xin_div2,
#endif /* CONFIG_DPU_IMC */
#ifdef CONFIG_DPU_APC
		.clk_src = xin,
#endif /* CONFIG_DPU_APC */
		.allow_gating = false, /* CPU is required to boot */
	},
#else /* CONFIG_DPU_RES */
	[CPU_CLK] = {
		.clk_src = xin,
		.allow_gating = false, /* CPU is required to boot */
	},
#endif /* CONFIG_DPU_RES */
	[PE_CLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
	[DDR_CLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
	[AXI_CLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
	[VPU_BCLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
	[DDR_BYPASS_PCLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
	[APB_CLK] = {
		.clk_src = xin,
		.allow_gating = false, /* ROM/RAM/TMR are required to boot */
	},
	[VPU_CCLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
#ifdef CONFIG_DPU_GEN2
	[TSENSOR_XO_CLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
	[TSENSOR_METS_CLK] = {
		.clk_src = xin,
		.allow_gating = true,
	},
#else /* CONFIG_DPU_GEN2 */
	[PCIE_REF_CLK] = {
		.clk_src = pcie_phy_clk,
		.allow_gating = true,
	},
#endif /* CONFIG_DPU_GEN2 */
};

#ifdef CONFIG_CLK_MNEMONICS
const char *sel_clk_names[NR_SEL_CLKS] = {
	[CPU_CLK] = "cpu_clk",
	[PE_CLK] = "pe_clk",
	[DDR_CLK] = "ddr_clk",
	[AXI_CLK] = "axi_clk",
	[VPU_BCLK] = "vpu_bclk",
	[DDR_BYPASS_PCLK] = "ddr_bypass_pclk",
	[APB_CLK] = "apb_clk",
	[VPU_CCLK] = "vpu_cclk",
#ifdef CONFIG_DPU_GEN2
	[TSENSOR_XO_CLK] = "tsensor_xo_clk",
	[TSENSOR_METS_CLK] = "tsensor_mets_clk",
#else /* CONFIG_DPU_GEN2 */
	[PCIE_REF_CLK] = "pcie_ref_clk",
#endif /* CONFIG_DPU_GEN2 */
};

static const char *get_clk_sel_name(clk_clk_t clk)
{
	if (clk >= NR_SEL_CLKS)
		return NULL;
	return sel_clk_names[clk];
}
#else
#define get_clk_sel_name	NULL
#endif

static int enable_clk_sel(clk_clk_t clk)
{
	bool r = CLK_IS_PLL_RCLK(clk);
	uint8_t pll = CLK_TO_PLL(clk, r);

	if (clk >= NR_SEL_CLKS)
		return -EINVAL;

	dpu_gmux_deselect(pll, r);
	if (sel_clks[clk].allow_gating && !dpu_gmux_enabled(pll, r))
		dpu_gmux_enable(pll, r);
	return 0;
}

static clk_freq_t get_clk_sel_freq(clk_clk_t clk)
{
	if (clk >= NR_SEL_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(sel_clks[clk].clk_src);
}

const struct clk_driver clk_gmux = {
	.max_clocks = NR_SEL_CLKS,
	.enable = enable_clk_sel,
	.disable = NULL,
	.get_freq = get_clk_sel_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_clk_sel_name,
};

uint32_t input_clks[NR_INPUT_CLKS] = {
	[XIN] = XIN_FREQ,
	[PCIE_PHY_CLK] = PCIE_PHY_CLK_FREQ,
};

#ifdef CONFIG_CLK_MNEMONICS
const char *input_clk_names[NR_INPUT_CLKS] = {
	[XIN] = "xin",
	[PCIE_PHY_CLK] = "pcie_phy_clk",
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

#ifdef CONFIG_DPU_RES
struct div_clk {
	clk_t src;
	uint8_t div;
};

struct div_clk div_clks[NR_DIV_CLKS] = {
	[XIN_DIV2] = {
		.src = xin,
		.div = 2,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *div_clk_names[NR_DIV_CLKS] = {
	[XIN_DIV2] = "xin_div2",
};

static const char *get_pll_div_name(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return NULL;
	return div_clk_names[clk];
}
#else
#define get_pll_div_name	NULL
#endif

static int enable_pll_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return -EINVAL;
	return clk_enable(div_clks[clk].src);
}

static void disable_pll_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return;
	clk_disable(div_clks[clk].src);
}

static clk_freq_t get_pll_div_freq(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(div_clks[clk].src) / div_clks[clk].div;
}

static int set_pll_div_freq(clk_clk_t clk, clk_freq_t freq)
{
	if (clk >= NR_DIV_CLKS)
		return -EINVAL;
	return clk_set_frequency(div_clks[clk].src,
				 freq * div_clks[clk].div);
}

const struct clk_driver clk_div = {
	.max_clocks = NR_DIV_CLKS,
	.enable = enable_pll_div,
	.disable = disable_pll_div,
	.get_freq = get_pll_div_freq,
	.set_freq = set_pll_div_freq,
	.select = NULL,
	.get_name = get_pll_div_name,
};
#endif /* CONFIG_DPU_RES */

/*===========================================================================
 * Clock tree APIs
 *===========================================================================*/
static bool clk_hw_init = false;

void board_init_clock(void)
{
	if (!clk_hw_init) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_SEL, &clk_gmux);
		clk_register_driver(CLK_DEP, &clk_dep);
		clk_register_driver(CLK_RESET, &clk_srst);
#ifdef CONFIG_DPU_RES
		clk_register_driver(CLK_DIV, &clk_div);
#endif /* CONFIG_DPU_RES */
		/* Update the status of the default enabled clocks */
		clk_enable(cpu_clk);
		clk_enable(apb_clk);
	}
	clk_hw_init = true;
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}
