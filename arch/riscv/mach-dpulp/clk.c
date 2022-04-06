/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)clk.c: DPU-LP CRU clock framework implementation
 * $Id: clk.c,v 1.1 2021-11-16 11:09:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/ddr.h>

struct rst_clk {
	clk_t clk_dep;
	clk_t clk_src;
	uint16_t clken;
	/* uint16_t swallow_bypass; */
	uint16_t reset;
	CLK_DEC_FLAGS_RO
};

struct rst_clk rst_clks[] = {
	/* 3.1.1 CPU/RAM Clocks */
	[CPU_FUNC_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = cpu_clksel,
		.reset = CRU_cpu_func_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[CPU_DBG_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = cpu_clksel,
		.reset = CRU_cpu_dbg_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	/* 3.1.2 GPDPU clocks */
	[GPDPU_CLKEN] = {
		.clk_dep = gpdpu_bus_clksel,
		.clk_src = gpdpu_core_clksel,
		.clken = CRU_gpdpu_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[GPDPU_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = gpdpu_clk,
		.reset = CRU_gpdpu_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	/* 3.1.3 DDR clocks */
	[DDR0_CLKEN] = {
		.clk_dep = invalid_clk,
		.clk_src = ddr_clksel,
		.clken = CRU_ddr0_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[DDR1_CLKEN] = {
		.clk_dep = invalid_clk,
		.clk_src = ddr_clksel,
		.clken = CRU_ddr1_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[DDR_BYPASSPCLKEN] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_200_clksel,
		.clken = CRU_ddr_bypassPclken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[DDR0_PWROKIN] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.reset = CRU_ddr0_pwrokin,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR0_APB_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_ddr0_apb_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR0_AXI_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_800_clksel,
		.reset = CRU_ddr0_axi_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR0_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.reset = CRU_ddr0_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR1_PWROKIN] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.reset = CRU_ddr1_pwrokin,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR1_APB_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_ddr1_apb_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR1_AXI_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_800_clksel,
		.reset = CRU_ddr1_axi_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR1_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.reset = CRU_ddr1_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	/* 3.1.4 VPU Clocks */
	[VPU0_CLKEN] = {
		.clk_dep = vpu_bclksel,
		.clk_src = vpu_cclksel,
		.clken = CRU_vpu0_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[VPU1_CLKEN] = {
		.clk_dep = vpu_bclksel,
		.clk_src = vpu_cclksel,
		.clken = CRU_vpu1_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[VPU_RESET] = {
		.clk_dep = vpu1_clk,
		.clk_src = vpu0_clk,
		.reset = CRU_vpu_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	/* 3.1.5 PCIe CLocks */
	[PCIE_CLKEN] = {
		.clk_dep = pcie_aux_clk,
		.clk_src = soc_800_clksel,
		.clken = CRU_pcie_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[PCIE_PWR_UP_RESET] = {
		.clk_dep = soc_100_clksel,
		.clk_src = pcie_clk,
		.reset = CRU_pcie_pwr_up_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	/* 3.1.6 RAB Clocks */
	[RAB0_CLKEN] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_400_clksel,
		.clken = CRU_rab0_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[RAB1_CLKEN] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_400_clksel,
		.clken = CRU_rab1_clken,
		CLK_DEF_FLAGS_RO(CLK_C)
	},
	[RAB0_RESET] = {
		.clk_dep = rab0_rio_reset,
		.clk_src = rab0_clk,
		.reset = CRU_rab0_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[RAB0_RIO_RESET] = {
		.clk_dep = rab0_rio_logic_reset,
		.clk_src = rab0_phy_clksel,
		.reset = CRU_rab0_rio_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[RAB0_RIO_LOGIC_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.reset = CRU_rab0_rio_logic_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[RAB1_RESET] = {
		.clk_dep = rab1_rio_reset,
		.clk_src = rab1_clk,
		.reset = CRU_rab1_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[RAB1_RIO_RESET] = {
		.clk_dep = rab1_rio_logic_reset,
		.clk_src = rab1_phy_clksel,
		.reset = CRU_rab1_rio_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[RAB1_RIO_LOGIC_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.reset = CRU_rab1_rio_logic_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	/* 3.1.7 Ethernet Clocks */
	[ETH0_RESET] = {
		.clk_dep = eth0_phy_pll,
		.clk_src = soc_200_clksel,
		.reset = CRU_eth0_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[ETH1_RESET] = {
		.clk_dep = eth1_phy_pll,
		.clk_src = soc_200_clksel,
		.reset = CRU_eth1_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	/* 3.1.8 Low Speed IO Clocks */
	[GPIO_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_gpio_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[SSI_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_ssi_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[FLASH_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_flash_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[I2C0_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_i2c0_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[I2C1_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_i2c1_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[I2C2_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_i2c2_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[I2C3_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_i2c3_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[UART0_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_uart0_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[UART1_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_uart1_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[UART2_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_uart2_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[UART3_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_uart3_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[SD_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_sd_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[PLIC_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_plic_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[TMR_RESET] = {
		.clk_dep = soc_100_clksel,
		.clk_src = xo_clk,
		.reset = CRU_tmr_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[WDT_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_wdt_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[AON_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_aon_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[GPDPU_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_gpdpu_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR0_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_ddr0_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[DDR1_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_ddr1_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[RAB0_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_rab0_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[RAB1_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_rab1_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[ETH0_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_eth0_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
	[ETH1_TSENSOR_RESET] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_100_clksel,
		.reset = CRU_eth1_tsensor_reset,
		CLK_DEF_FLAGS_RO(CLK_R)
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *rst_clk_names[] = {
	[GPDPU_CLKEN] = "gpdpu_clk",
	[DDR0_CLKEN] = "ddr0_clk",
	[DDR1_CLKEN] = "ddr1_clk",
	[DDR_BYPASSPCLKEN] = "ddr_bypassPclk",
	[VPU0_CLKEN] = "vpu0_clk",
	[VPU1_CLKEN] = "vpu1_clk",
	[PCIE_CLKEN] = "pcie_clk",
	[RAB0_CLKEN] = "rab0_clk",
	[RAB1_CLKEN] = "rab1_clk",
	[CPU_FUNC_RESET] = "cpu_func_reset",
	[CPU_DBG_RESET] = "cpu_dbg_reset",
	[GPDPU_RESET] = "gpdpu_reset",
	[DDR0_PWROKIN] = "ddr0_pwrokin",
	[DDR0_APB_RESET] = "ddr0_apb_reset",
	[DDR0_AXI_RESET] = "ddr0_axi_reset",
	[DDR0_RESET] = "ddr0_reset",
	[DDR1_PWROKIN] = "ddr1_pwrokin",
	[DDR1_APB_RESET] = "ddr1_apb_reset",
	[DDR1_AXI_RESET] = "ddr1_axi_reset",
	[DDR1_RESET] = "ddr1_reset",
	[VPU_RESET] = "vpu_reset",
	[PCIE_PWR_UP_RESET] = "pcie_pwr_up_reset",
	[RAB0_RESET] = "rab0_reset",
	[RAB0_RIO_RESET] = "rab0_rio_reset",
	[RAB0_RIO_LOGIC_RESET] = "rab0_rio_logic_reset",
	[RAB1_RESET] = "rab1_reset",
	[RAB1_RIO_RESET] = "rab1_rio_reset",
	[RAB1_RIO_LOGIC_RESET] = "rab1_rio_logic_reset",
	[ETH0_RESET] = "eth0_reset",
	[ETH1_RESET] = "eth1_reset",
	[GPIO_RESET] = "gpio_reset",
	[SSI_RESET] = "ssi_reset",
	[FLASH_RESET] = "flash_reset",
	[I2C0_RESET] = "i2c0_reset",
	[I2C1_RESET] = "i2c1_reset",
	[I2C2_RESET] = "i2c2_reset",
	[I2C3_RESET] = "i2c3_reset",
	[UART0_RESET] = "uart0_reset",
	[UART1_RESET] = "uart1_reset",
	[UART2_RESET] = "uart2_reset",
	[UART3_RESET] = "uart3_reset",
	[SD_RESET] = "sd_reset",
	[PLIC_RESET] = "plic_reset",
	[TMR_RESET] = "tmr_reset",
	[WDT_RESET] = "wdt_reset",
	[AON_TSENSOR_RESET] = "aon_tsensor_reset",
	[GPDPU_TSENSOR_RESET] = "gpdpu_tsensor_reset",
	[DDR0_TSENSOR_RESET] = "ddr0_tsensor_reset",
	[DDR1_TSENSOR_RESET] = "ddr1_tsensor_reset",
	[RAB0_TSENSOR_RESET] = "rab0_tsensor_reset",
	[RAB1_TSENSOR_RESET] = "rab1_tsensor_reset",
	[ETH0_TSENSOR_RESET] = "eth0_tsensor_reset",
	[ETH1_TSENSOR_RESET] = "eth1_tsensor_reset",
};

static const char *get_rst_clk_name(clk_clk_t clk)
{
	if (clk >= NR_RST_CLKS)
		return NULL;
	return rst_clk_names[clk];
}
#else
#define get_rst_clk_name		NULL
#endif

static int enable_rst_clk(clk_clk_t clk)
{
	if (clk >= NR_RST_CLKS)
		return -EINVAL;

	cru_trace(true, get_rst_clk_name(clk));
	if (rst_clks[clk].clk_dep != invalid_clk)
		clk_enable(rst_clks[clk].clk_dep);
	if (rst_clks[clk].clk_src != invalid_clk)
		clk_enable(rst_clks[clk].clk_src);
	if (rst_clks[clk].flags & CLK_CLKEN_F)
		cru_clk_enable(clk);
	if (rst_clks[clk].flags & CLK_RESET_F)
		cru_rst_deassert(clk);
	return 0;
}

static void disable_rst_clk(clk_clk_t clk)
{
	if (clk >= NR_RST_CLKS)
		return;

	cru_trace(false, get_rst_clk_name(clk));
	if (rst_clks[clk].clk_dep != invalid_clk)
		clk_disable(rst_clks[clk].clk_dep);
	if (rst_clks[clk].clk_src != invalid_clk)
		clk_disable(rst_clks[clk].clk_src);
	if (rst_clks[clk].flags & CLK_RESET_F)
		cru_rst_assert(clk);
	if (rst_clks[clk].flags & CLK_CLKEN_F)
		cru_clk_disable(clk);
}

static clk_freq_t get_rst_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_RST_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(rst_clks[clk].clk_src);
}

static int set_rst_clk_freq(clk_clk_t clk, clk_freq_t freq)
{
	if (clk >= NR_RST_CLKS)
		return -EINVAL;
	return clk_set_frequency(rst_clks[clk].clk_src, freq);
}

const struct clk_driver clk_rst = {
	.max_clocks = NR_RST_CLKS,
	.enable = enable_rst_clk,
	.disable = disable_rst_clk,
	.select = NULL,
	.get_freq = get_rst_clk_freq,
	.set_freq = set_rst_clk_freq,
	.get_name = get_rst_clk_name,
};

struct div_clk {
	clk_t src;
	uint8_t div;
};

struct div_clk div_clks[NR_DIV_CLKS] = {
	[SOC_PLL_DIV2] = {
		.src = axi_pll,
		.div = 2,
	},
	[SOC_PLL_DIV4] = {
		.src = axi_pll,
		.div = 4,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *div_clk_names[NR_DIV_CLKS] = {
	[SOC_PLL_DIV2] = "soc_pll_div2",
	[SOC_PLL_DIV4] = "soc_pll_div4",
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
	int ret;

	if (clk >= NR_DIV_CLKS)
		return -EINVAL;
	cru_trace(true, get_pll_div_name(clk));
	ret = clk_enable(div_clks[clk].src);
	if (ret)
		return ret;
	return 0;
}

static void disable_pll_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return;
	cru_trace(false, get_pll_div_name(clk));
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

struct sel_clk {
	clk_t clk_sels[2];
	clk_t clk_dep;
	uint16_t clk_cfg;
	CLK_DEC_FLAGS
};

struct sel_clk sel_clks[NR_SEL_CLKS] = {
	[CPU_CLKSEL] = {
		.clk_sels = {
			cpu_pll,
			xo_clk,
		},
		.clk_cfg = CRU_cpu_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[GPDPU_BUS_CLKSEL] = {
		.clk_sels = {
			gpdpu_bus_pll,
			xo_clk,
		},
		.clk_cfg = CRU_gpdpu_bus_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[GPDPU_CORE_CLKSEL] = {
		.clk_sels = {
			gpdpu_core_pll,
			xo_clk,
		},
		.clk_dep = gpdpu_bus_clksel,
		.clk_cfg = CRU_gpdpu_core_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[DDR_CLKSEL] = {
		.clk_sels = {
			ddr_pll,
			xo_clk,
		},
		.clk_cfg = CRU_ddr_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[SOC_800_CLKSEL] = {
		.clk_sels = {
			axi_pll,
			xo_clk,
		},
		.clk_cfg = CRU_soc_800_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[SOC_400_CLKSEL] = {
		.clk_sels = {
			soc_pll_div2,
			xo_clk,
		},
		.clk_cfg = CRU_soc_400_clksel,
		CLK_DEF_FLAGS(CLK_DIV_SEL_F)
	},
	[SOC_200_CLKSEL] = {
		.clk_sels = {
			soc_pll_div4,
			xo_clk,
		},
		.clk_cfg = CRU_soc_200_clksel,
		CLK_DEF_FLAGS(CLK_DIV_SEL_F)
	},
	[SOC_100_CLKSEL] = {
		.clk_sels = {
			apb_pll,
			xo_clk,
		},
		.clk_cfg = CRU_soc_100_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[VPU_CCLKSEL] = {
		.clk_sels = {
			vpu_c_pll,
			xo_clk,
		},
		.clk_cfg = CRU_vpu_cclksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[VPU_BCLKSEL] = {
		.clk_sels = {
			vpu_b_pll,
			xo_clk,
		},
		.clk_cfg = CRU_vpu_bclksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[RAB0_PHY_CLKSEL] = {
		.clk_sels = {
			rab0_phy_pll,
			xo_clk,
		},
		.clk_cfg = CRU_rab0_phy_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
	[RAB1_PHY_CLKSEL] = {
		.clk_sels = {
			rab1_phy_pll,
			xo_clk,
		},
		.clk_cfg = CRU_rab1_phy_clksel,
		CLK_DEF_FLAGS(CLK_PLL_SEL_F)
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *sel_clk_names[NR_SEL_CLKS] = {
	[CPU_CLKSEL] = "cpu_clksel",
	[GPDPU_BUS_CLKSEL] = "gpdpu_bus_clksel",
	[GPDPU_CORE_CLKSEL] = "gpdpu_core_clksel",
	[DDR_CLKSEL] = "ddr_clksel",
	[SOC_800_CLKSEL] = "soc_800_clksel",
	[SOC_400_CLKSEL] = "soc_400_clksel",
	[SOC_200_CLKSEL] = "soc_200_clksel",
	[SOC_100_CLKSEL] = "soc_100_clksel",
	[VPU_CCLKSEL] = "vpu_cclksel",
	[VPU_BCLKSEL] = "vpu_bclksel",
	[RAB0_PHY_CLKSEL] = "rab0_phy_clksel",
	[RAB1_PHY_CLKSEL] = "rab1_phy_clksel",
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
	if (clk >= NR_SEL_CLKS)
		return -EINVAL;
	if (!(clk_read_flags(0, sel_clks[clk]) & CLK_CLKSEL_F)) {
		cru_trace(true, get_clk_sel_name(clk));
		if (!(clk_read_flags(0, sel_clks[clk]) & CLK_MUX_SEL_F))
			clk_enable(sel_clks[clk].clk_sels[0]);
		cru_clk_select(clk);
		if (!(clk_read_flags(0, sel_clks[clk]) & CLK_CLKEN_F))
			clk_set_flags(0, sel_clks[clk], CLK_CLKEN_F);
		clk_set_flags(0, sel_clks[clk], CLK_CLKSEL_F);
	}
	return 0;
}

static void disable_clk_sel(clk_clk_t clk)
{
	if (clk >= NR_SEL_CLKS)
		return;
	if (clk_read_flags(0, sel_clks[clk]) & CLK_CLKSEL_F) {
		cru_trace(false, get_clk_sel_name(clk));
		clk_enable(sel_clks[clk].clk_sels[1]);
		cru_clk_deselect(clk);
		if (!(clk_read_flags(0, sel_clks[clk]) & CLK_CLKEN_F))
			clk_set_flags(0, sel_clks[clk], CLK_CLKEN_F);
		clk_clear_flags(0, sel_clks[clk], CLK_CLKSEL_F);
	}
}

static clk_freq_t get_clk_sel_freq(clk_clk_t clk)
{
	bool selected;

	if (clk >= NR_SEL_CLKS)
		return INVALID_FREQ;
	selected = cru_clk_selected(clk);
	if (selected)
		return clk_get_frequency(sel_clks[clk].clk_sels[0]);
	else
		return clk_get_frequency(sel_clks[clk].clk_sels[1]);
}

const struct clk_driver clk_sel = {
	.max_clocks = NR_SEL_CLKS,
	.enable = enable_clk_sel,
	.disable = disable_clk_sel,
	.get_freq = get_clk_sel_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_clk_sel_name,
};

struct pll_clk {
	clk_t src;
	clk_t mux;
	caddr_t reg;
	uint32_t alt;
	clk_freq_t freq;
	bool enabled;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[CPU_PLL] = {
		.src = cpu_vco,
		.mux = cpu_clksel,
		.alt = 0,
		.freq = CPU_CLK_FREQ,
		.enabled = false,
	},
	[GPDPU_BUS_PLL] = {
		.src = gpdpu_bus_vco,
		.mux = gpdpu_bus_clksel,
		.alt = 0,
		.freq = GPDPU_BUS_CLK_FREQ,
		.enabled = false,
	},
	[GPDPU_CORE_PLL] = {
		.src = gpdpu_core_vco,
		.mux = gpdpu_core_clksel,
		.alt = 0,
		.freq = GPDPU_CORE_CLK_FREQ,
		.enabled = false,
	},
	[DDR_PLL] = {
		.src = ddr_vco,
		.mux = ddr_clksel,
		.alt = 0,
		.freq = DDR_CLK_FREQ,
		.enabled = false,
	},
	[AXI_PLL] = {
		.src = soc_vco,
		.mux = soc_800_clksel,
		.reg = CRU_ADR(CRU_soc_800_clksel),
		.alt = CRU_MSK(CRU_soc_400_clksel) |
		       CRU_MSK(CRU_soc_200_clksel) |
		       CRU_MSK(CRU_soc_100_clksel),
		.alt = 0,
		.freq = AXI_CLK_FREQ,
		.enabled = false,
	},
	[VPU_B_PLL] = {
		.src = vpu_b_vco,
		.mux = vpu_bclksel,
		.alt = 0,
		.freq = VPU_BCLK_FREQ,
		.enabled = false,
	},
	[VPU_C_PLL] = {
		.src = vpu_c_vco,
		.mux = vpu_cclksel,
		.alt = 0,
		.freq = VPU_CCLK_FREQ,
		.enabled = false,
	},
	[RAB0_PHY_PLL] = {
		.src = rab0_phy_vco,
		.mux = rab0_phy_clksel,
		.alt = 0,
		.freq = RAB0_PHY_CLK_FREQ,
		.enabled = false,
	},
	[RAB1_PHY_PLL] = {
		.src = rab1_phy_vco,
		.mux = rab1_phy_clksel,
		.alt = 0,
		.freq = RAB1_PHY_CLK_FREQ,
		.enabled = false,
	},
	[ETH0_PHY_PLL] = {
		.src = eth0_phy_vco,
		.mux = invalid_clk,
		.alt = 0,
		.freq = ETH0_PHY_CLK_FREQ,
		.enabled = false,
	},
	[ETH1_PHY_PLL] = {
		.src = eth1_phy_vco,
		.mux = invalid_clk,
		.alt = 0,
		.freq = ETH1_PHY_CLK_FREQ,
		.enabled = false,
	},
	[APB_PLL] = {
		.src = soc_vco,
		.mux = soc_100_clksel,
		.reg = CRU_ADR(CRU_soc_100_clksel),
		.alt = CRU_MSK(CRU_soc_800_clksel) |
		       CRU_MSK(CRU_soc_400_clksel) |
		       CRU_MSK(CRU_soc_200_clksel),
		.freq = APB_CLK_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *pll_clk_names[NR_PLL_CLKS] = {
	[CPU_PLL] = "cpu_pll",
	[GPDPU_BUS_PLL] = "gpdpu_bus_pll",
	[GPDPU_CORE_PLL] = "gpdpu_core_pll",
	[DDR_PLL] = "ddr_pll",
	[AXI_PLL] = "axi_pll",
	[VPU_B_PLL] = "vpu_b_pll",
	[VPU_C_PLL] = "vpu_c_pll",
	[RAB0_PHY_PLL] = "rab0_phy_pll",
	[RAB1_PHY_PLL] = "rab1_phy_pll",
	[ETH0_PHY_PLL] = "eth0_phy_pll",
	[ETH1_PHY_PLL] = "eth1_phy_pll",
	[APB_PLL] = "apb_pll",
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

static void __enable_pll(clk_clk_t pll, clk_clk_t clk, bool force)
{
	bool r = !!(clk >= DPULP_MAX_PLLS);
	clk_clk_t vco = r ? clk - DPULP_MAX_PLLS : clk;
	uint32_t clk_sels = 0;
	caddr_t reg;
	uint32_t alt;
	clk_freq_t fvco_orig, fvco, fclk;

	if (!pll_clks[pll].enabled || force) {
		cru_trace(true, get_pll_name(clk));
		fclk = pll_clks[pll].freq;
		reg = pll_clks[pll].reg;
		alt = pll_clks[pll].alt;
		/* XXX: Protect Dynamic PLL Change
		 *
		 * The PLL might be the source of the system clocks (CPU,
		 * BUS, etc.). Since the possible dynamic PLL change may
		 * disable VCO internally to lead to the unpredictable
		 * system hangs, switching to the xo_clk can help to
		 * ensure a safer P/R clkout enabling.
		 */
		clk_deselect_mux(pll_clks[pll].mux);
		/* XXX: Lowest Power Consumption P/R
		 *
		 * Stay sourcing xo_clk to utilize the low power
		 * consumption mode.
		 */
		if (pll_clks[pll].mux != invalid_clk &&
		    alt == 0 && fclk == XO_CLK_FREQ)
			goto exit_xo_clk;
		if (alt) {
			clk_sels = __raw_readl(reg);
			__raw_writel(clk_sels | alt, reg);
		}
		fvco_orig = fvco = clk_get_frequency(pll_clks[pll].src);
		if (clk == DDR_PLL)
			fvco = ddr_clk_fvco(fclk, fvco_orig);
		if (fvco != fvco_orig) {
			clk_apply_vco(vco, clk, fvco);
			clk_disable(pll_clks[pll].src);
		}
		clk_enable(pll_clks[pll].src);
		dpulp_div_enable(vco, fvco, fclk, r);
		if (alt)
			__raw_writel(clk_sels, reg);
		clk_select_mux(pll_clks[pll].mux);
exit_xo_clk:
		pll_clks[pll].enabled = true;
	}
}

static void __disable_pll(clk_clk_t pll, clk_clk_t clk)
{
	bool r = !!(clk >= DPULP_MAX_PLLS);
	clk_clk_t vco = r ? clk - DPULP_MAX_PLLS : clk;
	uint32_t clk_sels = 0;
	caddr_t reg;
	uint32_t alt;

	if (pll_clks[pll].enabled) {
		cru_trace(false, get_pll_name(clk));
		reg = pll_clks[pll].reg;
		alt = pll_clks[pll].alt;
		pll_clks[pll].enabled = false;
		/* XXX: Ensure System Clocking
		 *
		 * The PLL might be the source of the system clocks (CPU,
		 * BUS, etc.). Make sure it's still clocking after
		 * disabling the P/R clkout by switching to the xo_clk.
		 */
		clk_deselect_mux(pll_clks[pll].mux);
		if (alt) {
			clk_sels = __raw_readl(reg);
			__raw_writel(clk_sels | alt, reg);
		}
		dpulp_div_disable(vco, r);
		/* XXX: Lowest Power Consumption VCO
		 *
		 * And VCO is also disabled when all related P/R outputs
		 * are deselected (clocking with xo_clk).
		 */
		if ((clk_sels & alt) == alt)
			clk_disable(pll_clks[pll].src);
		else if (alt)
			__raw_writel(clk_sels, reg);
	}
}

static int enable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return -EINVAL;
	__enable_pll(clk, clk, false);
	return 0;
}

static void disable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return;
	__disable_pll(clk, clk);
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
		clk_apply_pll(clk, clk, freq);
		__enable_pll(clk, clk, true);
	}
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

struct vco_clk {
	clk_freq_t freq;
	clk_freq_t freq_p;
	clk_freq_t freq_r;
	bool enabled;
};

struct vco_clk vco_clks[NR_VCO_CLKS] = {
	[CPU_VCO] = {
		.freq = CPU_VCO_FREQ,
		.freq_p = CPU_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[GPDPU_BUS_VCO] = {
		.freq = GPDPU_BUS_VCO_FREQ,
		.freq_p = GPDPU_BUS_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[GPDPU_CORE_VCO] = {
		.freq = GPDPU_CORE_VCO_FREQ,
		.freq_p = GPDPU_CORE_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[DDR_VCO] = {
		.freq = DDR_VCO_FREQ,
		.freq_p = DDR_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[SOC_VCO] = {
		.freq = SOC_VCO_FREQ,
		.freq_p = AXI_CLK_FREQ,
		.freq_r = APB_CLK_FREQ,
		.enabled = false,
	},
	[VPU_B_VCO] = {
		.freq = VPU_B_VCO_FREQ,
		.freq_p = VPU_BCLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[VPU_C_VCO] = {
		.freq = VPU_C_VCO_FREQ,
		.freq_p = VPU_CCLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[RAB0_PHY_VCO] = {
		.freq = RAB_PHY_VCO_FREQ,
		.freq_p = RAB0_PHY_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[RAB1_PHY_VCO] = {
		.freq = RAB_PHY_VCO_FREQ,
		.freq_p = RAB1_PHY_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[ETH0_PHY_VCO] = {
		.freq = ETH_PHY_VCO_FREQ,
		.freq_p = ETH0_PHY_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
	[ETH1_PHY_VCO] = {
		.freq = ETH_PHY_VCO_FREQ,
		.freq_p = ETH1_PHY_CLK_FREQ,
		.freq_r = INVALID_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *vco_clk_names[NR_VCO_CLKS] = {
	[CPU_VCO] = "cpu_vco",
	[GPDPU_BUS_VCO] = "gpdpu_bus_vco",
	[GPDPU_CORE_VCO] = "gpdpu_core_vco",
	[DDR_VCO] = "ddr_vco",
	[SOC_VCO] = "soc_vco",
	[VPU_B_VCO] = "vpu_b_vco",
	[VPU_C_VCO] = "vpu_c_vco",
	[RAB0_PHY_VCO] = "rab0_phy_vco",
	[RAB1_PHY_VCO] = "rab1_phy_vco",
	[ETH0_PHY_VCO] = "eth0_phy_vco",
	[ETH1_PHY_VCO] = "eth1_phy_vco",
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

static void __enable_vco(clk_clk_t vco, clk_clk_t clk)
{
	if (!vco_clks[vco].enabled) {
		cru_trace(true, get_vco_name());
		dpulp_pll_enable2(vco, vco_clks[vco].freq,
				  vco_clks[vco].freq_p,
				  vco_clks[vco].freq_r);
		vco_clks[vco].enabled = true;
	}
}

static void __disable_vco(clk_clk_t vco, clk_clk_t clk)
{
	if (vco_clks[vco].enabled) {
		cru_trace(false, get_vco_name(clk));
		vco_clks[vco].enabled = false;
		dpulp_pll_disable(vco);
	}
}

static int enable_vco(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return -EINVAL;
	__enable_vco(clk, clk);
	return 0;
}

static void disable_vco(clk_clk_t clk)
{
	if (clk >= NR_VCO_CLKS)
		return;
	__disable_vco(clk, clk);
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
		__disable_vco(clk, clk);
		clk_apply_vco(clk, clk, freq);
	}
	__enable_vco(clk, clk);
	return 0;
}

const struct clk_driver clk_vco = {
	.max_clocks = NR_VCO_CLKS,
	.enable = enable_vco,
	.disable = disable_vco,
	.get_freq = get_vco_freq,
	.set_freq = set_vco_freq,
	.select = NULL,
	.get_name = get_vco_name,
};

uint32_t ref_clks[NR_REF_CLKS] = {
	[XO_CLK] = XO_CLK_FREQ,
};

#ifdef CONFIG_CLK_MNEMONICS
const char *ref_clk_names[NR_REF_CLKS] = {
	[XO_CLK] = "xo_clk",
};

static const char *get_ref_clk_name(clk_clk_t clk)
{
	if (clk >= NR_REF_CLKS)
		return NULL;
	return ref_clk_names[clk];
}
#else
#define get_ref_clk_name	NULL
#endif

static clk_freq_t get_ref_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_REF_CLKS)
		return INVALID_FREQ;
	return ref_clks[clk];
}

const struct clk_driver clk_ref = {
	.max_clocks = NR_REF_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_ref_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_ref_clk_name,
};

void clk_apply_vco(clk_clk_t vco, clk_clk_t clk, clk_freq_t freq)
{
	if (vco >= NR_VCO_CLKS)
		return;
	vco_clks[vco].freq = freq;
}

void clk_apply_pll(clk_clk_t pll, clk_clk_t clk, clk_freq_t freq)
{
	bool r = !!(clk >= DPULP_MAX_PLLS);
	clk_clk_t vco = r ? clk - DPULP_MAX_PLLS : clk;

	if (pll >= NR_PLL_CLKS || vco >= NR_VCO_CLKS)
		return;
	pll_clks[pll].freq = freq;

	/* XXX: Avoid Dynamic PLL Change
	 *
	 * This API should be used right after clk_apply_vco(). The usage
	 * should only affect static PLL changes. So changes P/R clkouts
	 * presets accordingly to avoid dynamic PLL changes in the follow
	 * up clk_enable() invocations.
	 */
	if (r)
		vco_clks[vco].freq_r = freq;
	else
		vco_clks[vco].freq_p = freq;
}

#ifdef CONFIG_CONSOLE_COMMAND
void clk_pll_dump(void)
{
	int i;
	const char *name;

	for (i = 0; i < NR_PLL_CLKS; i++) {
		name = clk_get_mnemonic(clkid(CLK_PLL, i));
		if (name)
			printf("pll  %3d %20s %20s\n", i, name, "xo_clk");
	}
	for (i = 0; i < NR_SEL_CLKS; i++) {
		if (sel_clk_names[i]) {
			printf("clk  %3d %20s %20s\n",
			       i, sel_clk_names[i],
			       clk_get_mnemonic(sel_clks[i].clk_sels[0]));
			if (sel_clks[i].clk_sels[1] != invalid_clk)
				printf("%4s %3s %20s %20s\n", "", "", "",
				       clk_get_mnemonic(
					       sel_clks[i].clk_sels[1]));
		}
	}
}
#endif

void clk_pll_init(void)
{
	clk_register_driver(CLK_REF, &clk_ref);
	clk_register_driver(CLK_VCO, &clk_vco);
	clk_register_driver(CLK_PLL, &clk_pll);
	clk_register_driver(CLK_SEL, &clk_sel);
	clk_register_driver(CLK_DIV, &clk_div);
	clk_register_driver(CLK_RST, &clk_rst);
}

void board_init_clock(void)
{
}
