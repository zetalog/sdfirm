/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)clk.h: K1Matrix specific clock tree definitions
 * $Id: clk.h,v 1.1 2023-09-06 18:54:00 zhenglv Exp $
 */

#ifndef __CLK_K1MATRIX_H_INCLUDE__
#define __CLK_K1MATRIX_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/cru.h>

#ifdef CONFIG_K1MATRIX_CRU
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#include <dt-bindings/clock/sbi-clock-k1matrix.h>

#define NR_FREQPLANS		5
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	(-1)

#define osc_clk			clkid(CLK_INPUT, OSC_CLK)

#define com_pll_foutpostdiv	clkid(CLK_PLL, COM_PLL)
#define mesh_pll_foutpostdiv	clkid(CLK_PLL, MESH_PLL)
#define peri_pll_foutpostdiv	clkid(CLK_PLL, PERI_PLL)
#define ddr0_pll_foutpostdiv	clkid(CLK_PLL, DDR0_PLL)
#define ddr1_pll_foutpostdiv	clkid(CLK_PLL, DDR1_PLL)
#define cpu0_pll_foutpostdiv	clkid(CLK_PLL, CPU0_PLL)
#define cpu1_pll_foutpostdiv	clkid(CLK_PLL, CPU1_PLL)
#define com_pll_fout1ph0	clkid(CLK_PLL, COM_FOUT1PH0)
#define peri_pll_fout1ph0	clkid(CLK_PLL, PERI_FOUT1PH0)

#define cpu_nic_clksel		clkid(CLK_SEL, CPU_NIC_CLKSEL)
#define pcie_top_xclksel	clkid(CLK_SEL, PCIE_TOP_XCLKSEL)
#define pcie_top_aux_clksel	clkid(CLK_SEL, PCIE_TOP_AUX_CLKSEL)
#define pcie_top_cfg_clksel	clkid(CLK_SEL, PCIE_TOP_CFG_CLKSEL)
#define pcie_bot_cfg_clksel	clkid(CLK_SEL, PCIE_BOT_CFG_CLKSEL)
#define pcie_bot_aux_clksel	clkid(CLK_SEL, PCIE_BOT_AUX_CLKSEL)
#define pcie_bot_xclksel	clkid(CLK_SEL, PCIE_BOT_XCLKSEL)
#define peri_sub_clksel		clkid(CLK_SEL, PERI_SUB_CLKSEL)
#define peri_gmac_txclk_sel	clkid(CLK_SEL, PERI_GMAC_TXCLK_SEL)

#define cpu_nic_clkdiv		clkid(CLK_DIV, CPU_NIC_CLKDIV)
#define cpu_hap_clkdiv		clkid(CLK_DIV, CPU_HAP_CLKDIV)
#define pcie_top_cfg_clkdiv	clkid(CLK_DIV, PCIE_TOP_CFG_CLKDIV)
#define pcie_top_aux_clkdiv	clkid(CLK_DIV, PCIE_TOP_AUX_CLKDIV)
#define pcie_bot_cfg_clkdiv	clkid(CLK_DIV, PCIE_BOT_CFG_CLKDIV)
#define pcie_bot_aux_clkdiv	clkid(CLK_DIV, PCIE_BOT_AUX_CLKDIV)
#define peri_gpio0_db_clkdiv	clkid(CLK_DIV, PERI_GPIO0_DB_CLKDIV)
#define peri_gpio1_db_clkdiv	clkid(CLK_DIV, PERI_GPIO1_DB_CLKDIV)
#define peri_gpio2_db_clkdiv	clkid(CLK_DIV, PERI_GPIO2_DB_CLKDIV)
#define peri_gpio3_db_clkdiv	clkid(CLK_DIV, PERI_GPIO3_DB_CLKDIV)
#define peri_gpio4_db_clkdiv	clkid(CLK_DIV, PERI_GPIO4_DB_CLKDIV)
#define peri_gmac_axi_clkdiv	clkid(CLK_DIV, PERI_GMAC_AXI_CLKDIV)

#define rmu_qspi_clken		clkid(CLK_EN, RMU_QSPI_CLKEN)
#define rmu_lpc_clken		clkid(CLK_EN, RMU_LPC_CLKEN)
#define rmu_espi_clken		clkid(CLK_EN, RMU_ESPI_CLKEN)
#define rmu_uart0_clken		clkid(CLK_EN, RMU_UART0_CLKEN)
#define rmu_uart1_clken		clkid(CLK_EN, RMU_UART1_CLKEN)
#define rmu_mailbox_s_clken	clkid(CLK_EN, RMU_MAILBOX_S_CLKEN)
#define rmu_mailbox_ns_clken	clkid(CLK_EN, RMU_MAILBOX_NS_CLKEN)
#define peri_smbus0_clken	clkid(CLK_EN, PERI_SMBUS0_CLKEN)
#define peri_smbus1_clken	clkid(CLK_EN, PERI_SMBUS1_CLKEN)
#define peri_i2c0_clken		clkid(CLK_EN, PERI_I2C0_CLKEN)
#define peri_i2c1_clken		clkid(CLK_EN, PERI_I2C1_CLKEN)
#define peri_i2c2_clken		clkid(CLK_EN, PERI_I2C2_CLKEN)
#define peri_i2c3_clken		clkid(CLK_EN, PERI_I2C3_CLKEN)
#define peri_i2c4_clken		clkid(CLK_EN, PERI_I2C4_CLKEN)
#define peri_i2c5_clken		clkid(CLK_EN, PERI_I2C5_CLKEN)
#define peri_uart0_clken	clkid(CLK_EN, PERI_UART0_CLKEN)
#define peri_uart1_clken	clkid(CLK_EN, PERI_UART1_CLKEN)
#define peri_uart2_clken	clkid(CLK_EN, PERI_UART2_CLKEN)
#define peri_uart3_clken	clkid(CLK_EN, PERI_UART3_CLKEN)
#define peri_dmac_clken		clkid(CLK_EN, PERI_DMAC_CLKEN)

#define cpu_sub_rstn		clkid(CLK_RST, CPU_SUB_RSTN)
#define pcie_top_rstn		clkid(CLK_RST, PCIE_TOP_RSTN)
#define pcie_bot_rstn		clkid(CLK_RST, PCIE_BOT_RSTN)
#define peri_sub_rstn		clkid(CLK_RST, PERI_SUB_RSTN)
#define mesh_sub_rstn		clkid(CLK_RST, MESH_SUB_RSTN)
#define ddr_sub_rstn		clkid(CLK_RST, DDR_SUB_RSTN)
#define ras_srst_n		clkid(CLK_RST, RAS_SRST_N)
#define cpu_sub_srst_n		clkid(CLK_RST, CPU_SUB_SRST_N)
#define rmu_sram_sw_rstn	clkid(CLK_RST, RMU_SRAM_SW_RSTN)
#define pcie0_perst_n		clkid(CLK_RST, PCIE0_PERST_N)
#define pcie1_perst_n		clkid(CLK_RST, PCIE1_PERST_N)
#define pcie2_perst_n		clkid(CLK_RST, PCIE2_PERST_N)
#define pcie3_perst_n		clkid(CLK_RST, PCIE3_PERST_N)
#define pcie4_perst_n		clkid(CLK_RST, PCIE4_PERST_N)
#define pcie5_perst_n		clkid(CLK_RST, PCIE5_PERST_N)
#define pcie6_perst_n		clkid(CLK_RST, PCIE6_PERST_N)
#define pcie7_perst_n		clkid(CLK_RST, PCIE7_PERST_N)
#define pcie8_perst_n		clkid(CLK_RST, PCIE8_PERST_N)
#define pcie9_perst_n		clkid(CLK_RST, PCIE9_PERST_N)

#define cpu_nic_clk		cpu_nic_clkdiv
#define cpu_hap_clk		cpu_hap_clkdiv
#define pcie_top_cfg_clk	pcie_top_cfg_clkdiv
#define pcie_top_aux_clk	pcie_top_aux_clkdiv
#define pcie_top_xclk		pcie_top_xclksel
#define pcie_bot_cfg_clk	pcie_bot_cfg_clkdiv
#define pcie_bot_aux_clk	pcie_bot_aux_clkdiv
#define pcie_bot_xclk		pcie_bot_xclksel
#define peri_sub_clk		peri_sub_clksel
#define peri_gmac_txclk		peri_gmac_txclk_sel
#define rmu_clk			com_pll_fout1ph0	
#define rmu_qspi_clk		rmu_qspi_clken
#define rmu_lpc_clk		rmu_lpc_clken
#define rmu_lpc_lclk		rmu_lpc_clken
#define rmu_espi_clk		rmu_espi_clken
#define rmu_uart0_clk		rmu_uart0_clken
#define rmu_uart1_clk		rmu_uart1_clken
#define rmu_mailbox_s_clk	rmu_mailbox_s_clken
#define rmu_mailbox_ns_clk	rmu_mailbox_ns_clken
#define peri_smbus0_clk		peri_smbus0_clken
#define peri_smbus1_clk		peri_smbus1_clken
#define peri_i2c0_clk		peri_i2c0_clken
#define peri_i2c1_clk		peri_i2c1_clken
#define peri_i2c2_clk		peri_i2c2_clken
#define peri_i2c3_clk		peri_i2c3_clken
#define peri_i2c4_clk		peri_i2c4_clken
#define peri_i2c5_clk		peri_i2c5_clken
#define peri_uart0_clk		peri_uart0_clken
#define peri_uart1_clk		peri_uart1_clken
#define peri_uart2_clk		peri_uart2_clken
#define peri_uart3_clk		peri_uart3_clken
#define peri_gpio0_db_clk	peri_gpio0_db_clkdiv
#define peri_gpio1_db_clk	peri_gpio1_db_clkdiv
#define peri_gpio2_db_clk	peri_gpio2_db_clkdiv
#define peri_gpio3_db_clk	peri_gpio3_db_clkdiv
#define peri_gpio4_db_clk	peri_gpio4_db_clkdiv
#define peri_dmac_clk		peri_dmac_clken
#define peri_gmac_axi_clk	peri_gmac_axi_clkdiv

#define ddr_clk			clkid(CLK_DYN, DDR_CLK)
#define cpu_clk			clkid(CLK_DYN, CPU_CLK)

#define clk_freq_t		uint64_t
#define invalid_clk		clkid(0xFF, 0xFF)

void clk_hw_ctrl_init(void);

#endif /* __CLK_K1MATRIX_H_INCLUDE__ */
