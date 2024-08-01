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
 * @(#)cru_clk_fake.c: k1matrix fake clock units implementations
 * $Id: cru_clk_fake.c,v 1.1 2024-06-05 17:37:00 zhenglv Exp $
 */

#include <target/clk.h>

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

struct div_clk {
	caddr_t reg;
	caddr_t rst_reg;
	uint16_t max_div;
	uint8_t div;
	cru_flags_t flags;
	clk_t src;

};

#ifdef CONFIG_K1MATRIX_64C
struct div_clk div_clks[] = {


}
#endif

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
	[RMU_AXI_CLK_DIV] = {
		.reg = CRU_RMU_AXI_CLK_DIV,
		.max_div = 1,
		.div = 1,
		.flags = CRU_SRC_CLKSEL,
		.src = rmu_clksel,
	},
	[RMU_APB_CLK_DIV] = {
		.reg = CRU_RMU_APB_CLK_DIV,
		.max_div = 1,
		.div = 1,
		.flags = 0,
		.src = rmu_axi_clk_div,
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
		.src = rmu_lpc_clk_div,
	},
	[RMU_LPC_CLK_DIV] = {
                .reg = CRU_RMU_LPC_CLK_DIV,
                .max_div = 1,
                .div = 1,
                .src = rmu_clk,
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
		.src = rmu_apb_clk_div,
	},
	[RMU_UART1_CLKEN] = {
		.reg = CRU_RMU_UART1_CLK_EN,
		.rst_reg = CRU_RMU_UART1_SW_RSTN,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN | CRU_RESET,
		.src = rmu_apb_clk_div,
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
	[SYS_APB_CFG_CLKEN] = {
		.reg = CRU_SYS_APB_CLK_CTL,
		.max_div = 1,
		.div = 1,
		.flags = CRU_CLKEN,
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
		.src = peri_sub_mclk,
	},
	[C0_CLK_CFG_EN] = {
		.reg = CRU_CLUSTER0_CLK_CTL,
		.flags = CRU_CLKEN,
		.src = osc_clk,
	},
};


const char *div_clk_names[NR_DIV_CLKS] = {
	[CPU_NIC_CLKDIV] = "cpu_nic_clkdiv",
	[CPU_HAP_CLKDIV] = "cpu_hap_clkdiv",
	[PCIE_TOP_CFG_CLKDIV] = "pcie_top_cfg_clkdiv",
	[PCIE_TOP_AUX_CLKDIV] = "pcie_top_aux_clkdiv",
	[PCIE_BOT_CFG_CLKDIV] = "pcie_bot_cfg_clkdiv",
	[PCIE_BOT_AUX_CLKDIV] = "pcie_bot_aux_clkdiv",
	[RMU_AXI_CLK_DIV] = "rmu_axi_clk_div",
	[RMU_APB_CLK_DIV] = "rmu_apb_clk_div",
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
	[SYS_APB_CFG_CLKEN] = "sys_apb_cfg_clken",
	[C0_CLK_CFG_EN] = "c0_clk_cfg_en",
	[PERI_GPIO0_DB_CLKDIV] = "peri_gpio0_db_clkdiv",//debounce clk
	[PERI_GPIO1_DB_CLKDIV] = "peri_gpio1_db_clkdiv",
	[PERI_GPIO2_DB_CLKDIV] = "peri_gpio2_db_clkdiv",
	[PERI_GPIO3_DB_CLKDIV] = "peri_gpio3_db_clkdiv",
	[PERI_GPIO4_DB_CLKDIV] = "peri_gpio4_db_clkdiv",
	[PERI_DMAC_CLKEN] = "peri_dmac_clken",
	[PERI_GMAC_AXI_CLKDIV] = "peri_gmac_axi_clkdiv",
	[PERI_GMAC_AHB_CLKDIV] = "peri_gmac_ahb_clkdiv",
	[RMU_LPC_CLK_DIV] = "rmu_lpc_clk_div",
};

#ifdef CONFIG_CLK_MNEMONICS
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

static int enable_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return -EINVAL;
	if (div_clks[clk].flags & CRU_SRC_CLKSEL)
		clk_select_source(div_clks[clk].src, 1);
	else
		clk_enable(div_clks[clk].src);
	if (div_clks[clk].flags & CRU_CLKEN) {
		__raw_setl(CRU_CLKENABLE(0), div_clks[clk].reg);
		if (div_clks[clk].flags & CRU_RESET) {
			__raw_setl(CRU_RSTN(0), div_clks[clk].rst_reg);
			if (div_clks[clk].flags & CRU_RESET_3BIT) {
				__raw_setl(CRU_RSTN(1), div_clks[clk].rst_reg);
				__raw_setl(CRU_RSTN(2), div_clks[clk].rst_reg);
			}
		}
		if (div_clks[clk].flags & CRU_CLKEN_2BIT)
			__raw_setl(CRU_CLKENABLE(1), div_clks[clk].reg);
	}
	return 0;
}


static void disable_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return;
	if (div_clks[clk].flags & CRU_SRC_CLKSEL)
		clk_select_source(div_clks[clk].src, 0);
	else
		clk_disable(div_clks[clk].src);
	if (div_clks[clk].flags & CRU_CLKEN) {
		__raw_clearl(CRU_CLKENABLE(0), div_clks[clk].reg);
		if (div_clks[clk].flags & CRU_RESET) {
			__raw_clearl(CRU_RSTN(0), div_clks[clk].rst_reg);
			if (div_clks[clk].flags & CRU_RESET_3BIT) {
				__raw_clearl(CRU_RSTN(1), div_clks[clk].rst_reg);
				__raw_clearl(CRU_RSTN(2), div_clks[clk].rst_reg);
			}
		}
		if (div_clks[clk].flags & CRU_CLKEN_2BIT)
			__raw_clearl(CRU_CLKENABLE(1), div_clks[clk].reg);
	}
}

static int set_div_freq(clk_clk_t clk, clk_freq_t freq)
{
	clk_freq_t src_freq;
	if (clk >= NR_DIV_CLKS)
		return -EINVAL;
	src_freq = clk_get_frequency(div_clks[clk].src);
	if (src_freq == INVALID_FREQ)
		return -EINVAL;
	div_clks[clk].div = src_freq / freq;
	if (div_clks[clk].div > div_clks[clk].max_div)
		return -EINVAL;
	__raw_writel_mask(CRU_CLKDIV0(div_clks[clk].div), CRU_CLKDIV0(CRU_CLKDIV0_MASK), div_clks[clk].reg);
	return 0;
}

const struct clk_driver clk_div = {
    	.max_clocks = NR_DIV_CLKS,
	.enable = enable_div,
	.disable = disable_div,
    	.get_freq = get_div_freq,
	.set_freq = set_div_freq,
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

clk_t fout1ph0_clksels[] = {
	osc_clk,
	com_pll_fout1ph0,
};

clk_t pcie_peri_xclksels[] = {
	osc_clk,
	peri_pll_foutpostdiv,
};

clk_t com_clksels[] = {
	osc_clk,
	com_pll_foutpostdiv,
};

clk_t peri_sub_clksels[] = {
	osc_clk,
	peri_pll_fout1ph0,
};

clk_t peri_gmac_tx_clksels[] = {
	phy2gmac_rx_clk,
	mac_clk1,
};

struct sel_clk sel_clks[] = {
	[RMU_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_RMU_CLK_SEL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[CPU_NIC_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_CPU_NIC_CLK_CTL,
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
		.clksels = com_clksels,
		.reg = CRU_PCIE_TOP_CFGCLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[PCIE_BOT_CFG_CLKSEL] = {
		.clksels = com_clksels,
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
		.clksels = peri_gmac_tx_clksels,
		.reg = CRU_PERI_GMAC_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
#ifdef CONFIG_K1MATRIX_64C
	[LTOP_CFG_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_LTOP_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[LTOP_PCIE_CFG_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_LTOP_PCIE_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = CRU_CLKEN,
	},
	[LTOP_PCIE_AUX_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_LTOP_PCIE_AUX_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = 0,
	}
	[LTOP_PCIE_TFR_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_LTOP_PCIE_TFR_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = 0,
	}
	[LTOP_PHY_FW_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_LTOP_PHY_FW_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = CRU_CLKEN,
	}
	[RTOP_CFG_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_RTOP_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[RTOP_PCIE_CFG_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_RTOP_PCIE_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = CRU_CLKEN,
	},
	[RTOP_PCIE_AUX_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_RTOP_PCIE_AUX_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = 0,
	},
	[RTOP_PCIE_TFR_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_RTOP_PCIE_TFR_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = 0,
	},
	[BOT_PHY_FW_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_BOT_PHY_FW_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = CRU_CLKEN,
	},
	[BOT_CFG_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_BOT_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[BOT_PCIE_CFG_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_BOT_PCIE_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = CRU_CLKEN,
	},
	[BOT_PCIE_AUX_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_BOT_PCIE_AUX_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = 0,
	},
	[BOT_PCIE_TFR_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_BOT_PCIE_TFR_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = 0,
	},
	[BOT_PHY_FW_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_BOT_PHY_FW_CLK_CTL,
		.nr_clksels = 2,
		.sel =1,
		.flags = CRU_CLKEN,
	},
	[CPU_NIC_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_CPU_NIC_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[TRMU_IOPMP_CFG_CLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_TRMU_IOPMP_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[DDR_CFG_CLKSEL] = {
		.clksels = com_clksels,
		.reg = CRU_DDR_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = 0,
	},
	[TPERI_SUB_MCLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_TPERI_SUB_MCLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[TPERI_SUB_SCLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_TPERI_SUB_SCLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
	[HNI_PERI_CFG_SCLKSEL] = {
		.clksels = fout1ph0_clksels,
		.reg = CRU_HNI_PERI_CFG_CLK_CTL,
		.nr_clksels = 2,
		.sel = 1,
		.flags = CRU_CLKEN,
	},
#endif
};


const char *sel_clk_names[NR_SEL_CLKS] = {
	[RMU_CLKSEL] = "rmu_clksel",
	[CPU_NIC_CLKSEL] = "cpu_nic_clksel",
	[PCIE_TOP_CFG_CLKSEL] = "pcie_top_cfg_clksel",
	[PCIE_TOP_AUX_CLKSEL] = "pcie_top_aux_clksel",
	[PCIE_TOP_XCLKSEL] = "pcie_top_xclksel",
	[PCIE_BOT_CFG_CLKSEL] = "pcie_bot_cfg_clksel",
	[PCIE_BOT_AUX_CLKSEL] = "pcie_bot_aux_clksel",
	[PCIE_BOT_XCLKSEL] = "pcie_bot_xclksel",
	[PERI_SUB_CLKSEL] = "peri_sub_clksel",
	[PERI_GMAC_TXCLK_SEL] = "peri_gmac_txclk_sel",
	#ifdef CONFIG_K1MATRIX_64C
	[LTOP_CFG_CLKSEL] = "ltop_cfg_clksel",
	[LTOP_PCIE_CFG_CLKSEL] = "ltop_pcie_cfg_clksel",
	[LTOP_PCIE_AUX_CLKSEL] = "ltop_pcie_aux_clksel",
	[LTOP_PCIE_TFR_CLKSEL] = "ltop_pcie_tfr_clksel",
	[LTOP_PHY_FW_CLKSEL] = "ltop_phy_fw_clksel",
	[RTOP_CFG_CLKSEL] = "rtop_cfg_clksel",
	[RTOP_PCIE_CFG_CLKSEL] = "rtop_pcie_cfg_clksel",
	[RTOP_PCIE_AUX_CLKSEL] = "rtop_pcie_aux_clksel",
	[RTOP_PCIE_TFR_CLKSEL] = "rtop_pcie_tfr_clksel",
	[RTOP_PHY_FW_CLKSEL] = "rtop_phy_fw_clksel",
	[BOT_CFG_CLKSEL] = "bot_cfg_clksel",
	[BOT_PCIE_CFG_CLKSEL] = "bot_pcie_cfg_clksel",
	[BOT_PCIE_AUX_CLKSEL] = "bot_pcie_aux_clksel",
	[BOT_PCIE_TFR_CLKSEL] = "bot_pcie_tfr_clksel",
	[BOT_PHY_FW_CLKSEL] = "bot_phy_fw_clksel",
	[CPU_NIC_CLKSEL] = "cpu_nic_clksel",
	[TRMU_IOPMP_CFG_CLKSEL] = "trmu_iopmp_cfg_clksel",
	[DDR_CFG_CLKSEL] = "ddr_cfg_clksel",
	[TPERI_SUB_MCLKSEL] = "tperi_sub_mclksel",
	[TPERI_SUB_SCLKSEL] = "tperi_sub_sclksel",
	[HNI_PERI_CFG_SCLKSEL] = "hni_peri_cfg_sclksel",
	#endif
};

#ifdef CONFIG_CLK_MNEMONICS
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
		__raw_writel_mask(CRU_CLKSEL(clksel),
				  CRU_CLKSEL(CRU_CLKSEL_OFFSET),
				  sel_clks[sel].reg);
		if (sel_clks[sel].flags & CRU_CLKEN)
			__raw_setl(CRU_CLKENABLE(0), sel_clks[sel].reg);
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

#ifdef CONFIG_CONSOLE_COMMAND
void clk_div_dump(void)
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

void clk_sel_dump(void)
{
	int i;

	for (i = 0; i < NR_SEL_CLKS; i++) {
		if (sel_clk_names[i]) {
			printf("sel  %3d %20s %20s\n",
			       i, sel_clk_names[i],
			       clk_get_mnemonic(sel_clks[i].sel));
		}
	}
}
#endif

void clk_div_init(void)
{
	clk_register_driver(CLK_DIV, &clk_div);
}

void clk_sel_init(void)
{
	clk_register_driver(CLK_SEL, &clk_sel);
}
