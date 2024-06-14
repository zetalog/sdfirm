
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

struct mesh_clk {
	caddr_t reg;
	clk_t *clksels;
	uint8_t nr_clksels;
	uint8_t sel;
	cru_flags_t flags;
};

clk_t mesh_clksels[] = {
	osc_clk,
	mesh_pll_foutpostdiv,
	com_pll_foutpostdiv,
};

struct mesh_clk mesh_clks[] = {
	[MESH_CLK] = {
		.clksels = mesh_clksels,
		.reg = CRU_MESH_SUB_CLK_CTL,
		.nr_clksels = 3,
		.sel = 1,	
	}
};

#ifdef CONFIG_CLK_MNEMONICS
const char *mesh_clk_names[NR_MESH_CLKS] = {
	[MESH_CLK] = "mesh_clk",
};

const char *get_mesh_name(clk_clk_t clk)
{
	if (clk >= NR_MESH_CLKS)
		return NULL;
	return mesh_clk_names[clk];
}

#else
#define get_mesh_name		NULL
#endif
static clk_freq_t get_mesh_freq(clk_clk_t sel)
{
	if (sel >= NR_MESH_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(mesh_clks[sel].clksels[mesh_clks[sel].sel]);
}

const struct clk_driver clk_mesh = {
	.max_clocks = NR_MESH_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_mesh_freq,
	.set_freq = NULL,
	.select = NULL, 
	.get_name = get_mesh_name,
};

struct dyn_clk {
	clk_t pll0;
	clk_t pll1;
	clk_t clksel;
	cru_flags_t flags;
};

struct dyn_clk dyn_clks[] = {
	[CPU_CLK] = {
		cpu0_pll_foutpostdiv,
		cpu1_pll_foutpostdiv,
		osc_clk,
		1,
	},
	[DDR_CLK] = {
		ddr0_pll_foutpostdiv,
		ddr1_pll_foutpostdiv,
		osc_clk,
		1,
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

static clk_freq_t get_dyn_freq(clk_clk_t pll)
{
	if (pll >= NR_DYN_CLKS)
		return INVALID_FREQ;
	if (!(dyn_clks[pll].flags & CRU_ENABLED))
		return clk_get_frequency(osc_clk);
	else {
		if (!(dyn_clks[pll].flags & CRU_FOUT1))
			return clk_get_frequency(dyn_clks[pll].pll0);
		else
			return clk_get_frequency(dyn_clks[pll].pll1);
	}
}

const struct clk_driver clk_dyn = {
	.max_clocks = NR_DYN_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_dyn_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_dyn_name,
};

struct div_clk {
	caddr_t reg;
	caddr_t rst_reg;
	uint16_t max_div;
	uint8_t div;
	cru_flags_t flags;
	clk_t src;

};

struct div_clk div_clks[] = {
	[CPU_NIC_CLKDIV] = {
		.reg = CRU_CPU_NIC_CLK_CTL,
		.max_div = 1,
		.div = 1,
		.flags = 0,
		.src = cpu_nic_clksel,
	},
	[CPU_HAP_CLKDIV] = {
		.reg = CRU_CPU_HAP_CLK_CTL,
		.max_div = 1,
		.div = 1,
		.flags = 0,
		.src = cpu_nic_clkdiv,
	},
	[PCIE_TOP_CFG_CLKDIV] = {
		.reg = CRU_PCIE_TOP_CFGCLK_CTL,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN,
		.src = pcie_top_cfg_clksel,
	},
	[PCIE_TOP_AUX_CLKDIV] = {
		.reg = CRU_PCIE_TOP_AUXCLK_CTL,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN,
		.src = pcie_top_aux_clksel,
	},
	[PCIE_BOT_CFG_CLKDIV] = {
		.reg = CRU_PCIE_BOT_CFGCLK_CTL,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN,
		.src = pcie_bot_cfg_clksel,
	},
	[PCIE_BOT_AUX_CLKDIV] = {
		.reg = CRU_PCIE_BOT_AUXCLK_CTL,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN,
		.src = pcie_bot_aux_clksel,
	},
	[RMU_QSPI_CLKEN] = {
		.reg = CRU_RMU_QSPI_CLK_EN,
		.rst_reg = CRU_RMU_QSPI_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[RMU_LPC_CLKEN] = {
		.reg = CRU_RMU_LPC_CLK_EN,
		.rst_reg = CRU_RMU_LPC_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_CLKEN_2BIT | CRU_RESET | CRU_RESET_3BIT,
		.src = osc_clk,
	},
	[RMU_ESPI_CLKEN] = {
		.reg = CRU_RMU_eSPI_CLK_EN,
		.rst_reg = CRU_RMU_eSPI_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[RMU_UART0_CLKEN] = {
		.reg = CRU_RMU_UART0_CLK_EN,
		.rst_reg = CRU_RMU_UART0_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[RMU_UART1_CLKEN] = {
		.reg = CRU_RMU_UART1_CLK_EN,
		.rst_reg = CRU_RMU_UART1_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[RMU_MAILBOX_S_CLKEN] = {
		.reg = CRU_RMU_Mailbox_S_CLK_EN,
		.rst_reg = CRU_RMU_Mailbox_S_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[RMU_MAILBOX_NS_CLKEN] = {
		.reg = CRU_RMU_Mailbox_NS_CLK_EN,
		.rst_reg = CRU_RMU_Mailbox_NS_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_SMBUS0_CLKEN] = {
		.reg = CRU_PERI_SMBUS0_CLK_CTL,
		.rst_reg = CRU_PERI_SMBUS0_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_SMBUS1_CLKEN] = {
		.reg = CRU_PERI_SMBUS1_CLK_CTL,
		.rst_reg = CRU_PERI_SMBUS1_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_I2C0_CLKEN] = {
		.reg = CRU_PERI_I2C0_CLK_CTL,
		.rst_reg = CRU_PERI_I2C0_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_I2C1_CLKEN] = {
		.reg = CRU_PERI_I2C1_CLK_CTL,
		.rst_reg = CRU_PERI_I2C1_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN |CRU_RESET,
		.src = osc_clk,
	},
	[PERI_I2C2_CLKEN] = {
		.reg = CRU_PERI_I2C2_CLK_CTL,
		.rst_reg = CRU_PERI_I2C2_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_I2C3_CLKEN] = {
		.reg = CRU_PERI_I2C3_CLK_CTL,
		.rst_reg = CRU_PERI_I2C3_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN |CRU_RESET,
		.src = osc_clk,
	},
	[PERI_I2C4_CLKEN] = {
		.reg = CRU_PERI_I2C4_CLK_CTL,
		.rst_reg = CRU_PERI_I2C4_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_I2C5_CLKEN] = {
		.reg = CRU_PERI_I2C5_CLK_CTL,
		.rst_reg = CRU_PERI_I2C5_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_UART0_CLKEN] = {
		.reg = CRU_PERI_UART0_CLK_CTL,
		.rst_reg = CRU_PERI_UART0_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_UART1_CLKEN] = {
		.reg = CRU_PERI_UART1_CLK_CTL,
		.rst_reg = CRU_PERI_UART1_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_UART2_CLKEN] = {
		.reg = CRU_PERI_UART2_CLK_CTL,
		.rst_reg = CRU_PERI_UART2_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_UART3_CLKEN] = {
		.reg = CRU_PERI_UART3_CLK_CTL,
		.rst_reg = CRU_PERI_UART3_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_GPIO0_DB_CLKDIV] = {
		.reg = CRU_PERI_GPIO0_CLK_CTL,
		.rst_reg = CRU_PERI_GPIO0_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_GPIO1_DB_CLKDIV] = {
		.reg = CRU_PERI_GPIO1_CLK_CTL,
		.rst_reg = CRU_PERI_GPIO1_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_GPIO2_DB_CLKDIV] = {
		.reg = CRU_PERI_GPIO2_CLK_CTL,
		.rst_reg = CRU_PERI_GPIO2_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_GPIO3_DB_CLKDIV] = {
		.reg = CRU_PERI_GPIO3_CLK_CTL,
		.rst_reg = CRU_PERI_GPIO3_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_GPIO4_DB_CLKDIV] = {
		.reg = CRU_PERI_GPIO4_CLK_CTL,
		.rst_reg = CRU_PERI_GPIO4_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_DMAC_CLKEN] = {
		.reg = CRU_PERI_DMAC_CLK_CTL,
		.rst_reg = CRU_PERI_DMAC_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = osc_clk,
	},
	[PERI_GMAC_AXI_CLKDIV] = {
		.reg = CRU_PERI_GMAC_CLK_CTL,
		.rst_reg = CRU_PERI_GMAC_SW_RESET,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = peri_gmac_txclk_sel,//?
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *div_clk_names[NR_DIV_CLKS] = {
	[CPU_NIC_CLKDIV] = "cpu_nic_clkdiv",
	[CPU_HAP_CLKDIV] = "cpu_hap_clkdiv",
	[PCIE_TOP_CFG_CLKDIV] = "pcie_top_cfg_clkdiv",
	[PCIE_TOP_AUX_CLKDIV] = "pcie_top_aux_clkdiv",
	[PCIE_BOT_CFG_CLKDIV] = "pcie_bot_cfg_clkdiv",
	[PCIE_BOT_AUX_CLKDIV] = "pcie_bot_aux_clkdiv",
	[RMU_QSPI_CLKEN] = "rmu_qspi_clken",
	[RMU_LPC_CLKEN] = "rmu_lpc_clken",//2bits lcken, 3bits rstn 
	[RMU_ESPI_CLKEN] = "rmu_espi_clken",
	[RMU_UART0_CLKEN] = "rmu_uart0_clken",
	[RMU_UART1_CLKEN] = "rmu_uart1_clken",
	[RMU_MAILBOX_S_CLKEN] = "rmu_mailbox_s_clken",
	[RMU_MAILBOX_NS_CLKEN] = "rmu_mailbox_ns_clken",
	[PERI_SMBUS0_CLKEN] = "peri_smbus0_clken",
	[PERI_SMBUS1_CLKEN] = "peri_smbus1_clken",
	[PERI_I2C0_CLKEN] = "peri_i2c0_clken",
	[PERI_I2C1_CLKEN] = "peri_i2c1_clken",
	[PERI_I2C2_CLKEN] = "peri_i2c2_clken",
	[PERI_I2C3_CLKEN] = "peri_i2c3_clken",
	[PERI_I2C4_CLKEN] = "peri_i2c4_clken",
	[PERI_I2C5_CLKEN] = "peri_i2c5_clken",
	[PERI_UART0_CLKEN] = "peri_uart0_clken",
	[PERI_UART1_CLKEN] = "peri_uart1_clken",
	[PERI_UART2_CLKEN] = "peri_uart2_clken",
	[PERI_UART3_CLKEN] = "peri_uart3_clken",
	[PERI_GPIO0_DB_CLKDIV] = "peri_gpio0_db_clkdiv",//debounce clk
	[PERI_GPIO1_DB_CLKDIV] = "peri_gpio1_db_clkdiv",
	[PERI_GPIO2_DB_CLKDIV] = "peri_gpio2_db_clkdiv",
	[PERI_GPIO3_DB_CLKDIV] = "peri_gpio3_db_clkdiv",
	[PERI_GPIO4_DB_CLKDIV] = "peri_gpio4_db_clkdiv",
	[PERI_DMAC_CLKEN] = "peri_dmac_clken",
	[PERI_GMAC_AXI_CLKDIV] = "peri_gmac_axi_clkdiv",//select source null
};

const char *get_div_name(clk_clk_t clk) 
{
	if (clk >= NR_DIV_CLKS)
		return NULL;
	return div_clk_names[clk];
}
#else
#define get_div_name		NULL
#endif

static clk_freq_t get_div_freq(clk_clk_t clk) 
{
	clk_freq_t freq;
	if (clk >= NR_DIV_CLKS)
		return INVALID_FREQ;
	freq = clk_get_frequency(div_clks[clk].src);
	if (freq == INVALID_FREQ)
		return INVALID_FREQ;
	return freq / div_clks[clk].div;
}

const struct clk_driver clk_div = {
    	.max_clocks = NR_DIV_CLKS,
    	.enable = NULL,
    	.disable = NULL,
    	.get_freq = get_div_freq,
    	.set_freq = NULL,
    	.select = NULL,
    	.get_name = get_div_name,
};

struct sel_clk {
	caddr_t reg;
	clk_t *clksels;
	uint8_t nr_clksels;
	uint8_t sel;
	cru_flags_t flags;
};

clk_t cpu_nic_clksels[] = {
	osc_clk,
	com_pll_fout1ph0,
};

clk_t pcie_peri_xclksels[] = {
	osc_clk,
	peri_pll_foutpostdiv,
};

clk_t pcie_com_xclksels[] = {
	osc_clk,
	com_pll_foutpostdiv,
};

clk_t peri_sub_clksels[] = {
	osc_clk,
	peri_pll_fout1ph0,
};

struct sel_clk sel_clks[] = {
	[CPU_NIC_CLKSEL] = {
		.reg = CRU_CPU_NIC_CLK_CTL,
		.clksels = cpu_nic_clksels,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[PCIE_TOP_XCLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = CRU_PCIE_TOP_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[PCIE_TOP_AUX_CLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = CRU_PCIE_TOP_AUXCLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[PCIE_TOP_CFG_CLKSEL] = {
		.clksels = pcie_com_xclksels,
		.reg = CRU_PCIE_TOP_CFGCLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[PCIE_BOT_CFG_CLKSEL] = {
		.clksels = pcie_com_xclksels,
		.reg = CRU_PCIE_BOT_CFGCLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[PCIE_BOT_AUX_CLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = CRU_PCIE_BOT_AUXCLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[PCIE_BOT_XCLKSEL] = {
		.clksels = pcie_peri_xclksels,
		.reg = CRU_PCIE_BOT_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[PERI_SUB_CLKSEL] = {
		.clksels = peri_sub_clksels,
		.reg = CRU_PERI_SUB_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[PERI_GMAC_TXCLK_SEL] = {
		//.clksels = ,
		.reg = CRU_PERI_GMAC_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *sel_clk_names[NR_SEL_CLKS] = {
	[CPU_NIC_CLKSEL] = "cpu_nic_clksel",
	[PCIE_TOP_CFG_CLKSEL] = "pcie_top_cfg_clksel",
	[PCIE_TOP_AUX_CLKSEL] = "pcie_top_aux_clksel",
	[PCIE_TOP_XCLKSEL] = "pcie_top_xclksel",
	[PCIE_BOT_CFG_CLKSEL] = "pcie_bot_cfg_clksel",
	[PCIE_BOT_AUX_CLKSEL] = "pcie_bot_aux_clksel",
	[PCIE_BOT_XCLKSEL] = "pcie_bot_xclksel",
	[PERI_SUB_CLKSEL] = "peri_sub_clksel",
	[PERI_GMAC_TXCLK_SEL] = "peri_gmac_txclk_sel",//?
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

const struct clk_driver clk_sel = {
	.max_clocks = NR_SEL_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_sel_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_sel_name,
};

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
		clk_register_driver(CLK_DYN, &clk_dyn);
		clk_register_driver(CLK_DIV, &clk_div);
		clk_register_driver(CLK_SEL, &clk_sel);
		clk_register_driver(CLK_RSTN, &clk_rstn);
		clk_register_driver(CLK_MESH, &clk_mesh);
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
#endif
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

static void clk_sel_dump(void)
{
	int i;
	
	for (i = 0; i < NR_SEL_CLKS; i++) {
		if (div_sel_names[i]) {
			printf("sel  %3d %20s %20s\n",
			       i, sel_clk_names[i],
			       clk_get_mnemonic(sel_clks[i].sel));
	}
}
static int do_cru_dump(int argc, char *argv[])
{
	printf("type id  %20s %20s\n", "name", "source");
	clk_dyn_dump();
	clk_div_dump();
	clk_sel_dump();
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
