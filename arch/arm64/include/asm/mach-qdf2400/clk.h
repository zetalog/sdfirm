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
 * clk.h: QDF2400 specific clock definitions
 * $Id: clk.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __CLK_QDF2400_H_INLCUDE__
#define __CLK_QDF2400_H_INLCUDE__

#include <target/compiler.h>
#include <target/generic.h>

#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif

#define NR_FREQPLANS		3
#define FREQPLAN_1_0		0
#define FREQPLAN_1_1		1
#define FREQPLAN_2_0		2
#define INVALID_FREQPLAN	NR_FREQPLANS

/* Padring reference clocks */
/* clk_cat_t */
#define CLK_PADRING		((clk_cat_t)0)
/* clk_clk_t */
#define PLL0_REF_CLK		((clk_clk_t)0)
#define PLL1_REF_CLK		((clk_clk_t)1)
#define PLL2_REF_CLK		((clk_clk_t)2)
#define PLL_TEST_SE		((clk_clk_t)3)
#define WEST_XO			((clk_clk_t)4)
#define EAST_XO			((clk_clk_t)5)
#define SLEEP_CLK		((clk_clk_t)6)
#define TIC_CLK			((clk_clk_t)7)
#define TCB_TCK			((clk_clk_t)8)
#define GMII0_CLK_125M		((clk_clk_t)9)
#define GMII1_CLK_125M		((clk_clk_t)10)
#define SATA_LN_ASIC_CLK	((clk_clk_t)11)
#define SATA_LN_RX_CLK		((clk_clk_t)12)
#define PCIE_X16_MSTR_Q23_CLK	((clk_clk_t)13)
#define PCIE_X16_SLV_Q23_CLK	((clk_clk_t)14)
#define PCIE_X8_MSTR_Q23_CLK	((clk_clk_t)15)
#define PCIE_X8_SLV_Q23_CLK	((clk_clk_t)16)
#define FREQ_PLL0_REF_CLK	20000000
#define FREQ_PLL1_REF_CLK	20000000
#define FREQ_PLL2_REF_CLK	20000000
#define FREQ_WEST_XO		20000000
#define FREQ_EAST_XO		20000000
#define FREQ_SLEEP_CLK		32000
#define FREQ_GMII0_CLK_125M	125000000
#define FREQ_GMII1_CLK_125M	125000000
#define FREQ_TIC_CLK		20000000
#ifdef CONFIG_CENTRIQ_FREQ_PLL_TEST_SE
#define FREQ_PLL_TEST_SE	CONFIG_CENTRIQ_FREQ_PLL_TEST_SE
#else
#define FREQ_PLL_TEST_SE	20000000
#endif
#if defined(CONFIG_CENTRIQ_FREQ_SATA_PHY_CLK_75M)
#define FREQ_SATA_PHY_CLK	75000000
#elif defined(CONFIG_CENTRIQ_FREQ_SATA_PHY_CLK_150M)
#define FREQ_SATA_PHY_CLK	150000000
#elif defined(CONFIG_CENTRIQ_FREQ_SATA_PHY_CLK_300M)
#define FREQ_SATA_PHY_CLK	300000000
#else
#define FREQ_SATA_PHY_CLK	75000000
#endif
#define FREQ_PCIE_X16_MSTR_Q23_CLK	666666666
#define FREQ_PCIE_X16_SLV_Q23_CLK	666666666
#define FREQ_PCIE_X8_MSTR_Q23_CLK	500000000
#define FREQ_PCIE_X8_SLV_Q23_CLK	500000000
#define pll0_ref_clk		clkid(CLK_PADRING, PLL0_REF_CLK)
#define pll1_ref_clk		clkid(CLK_PADRING, PLL1_REF_CLK)
#define pll2_ref_clk		clkid(CLK_PADRING, PLL2_REF_CLK)
#define pll_test_se		clkid(CLK_PADRING, PLL_TEST_SE)
#define west_xo			clkid(CLK_PADRING, WEST_XO)
#define east_xo			clkid(CLK_PADRING, EAST_XO)
#define sleep_clk		clkid(CLK_PADRING, SLEEP_CLK)
#define tic_clk			clkid(CLK_PADRING, TIC_CLK)
#define tcb_tck			clkid(CLK_PADRING, TCB_TCK)
#define gmii0_clk_125m		clkid(CLK_PADRING, GMII0_CLK_125M)
#define gmii1_clk_125m		clkid(CLK_PADRING, GMII1_CLK_125M)
#define sata_ln_asic_clk	clkid(CLK_PADRING, SATA_LN_ASIC_CLK)
#define sata_ln_rx_clk		clkid(CLK_PADRING, SATA_LN_RX_CLK)
#define pcie_x16_mstr_q23_clk	clkid(CLK_PADRING, PCIE_X16_MSTR_Q23_CLK)
#define pcie_x16_slv_q23_clk	clkid(CLK_PADRING, PCIE_X16_SLV_Q23_CLK)
#define pcie_x8_mstr_q23_clk	clkid(CLK_PADRING, PCIE_X8_MSTR_Q23_CLK)
#define pcie_x8_slv_q23_clk	clkid(CLK_PADRING, PCIE_X8_SLV_Q23_CLK)
#define NR_PADRING_CLKS		(PCIE_X8_SLV_Q23_CLK + 1)
#define core_bi_pll_test_se	pll_test_se
#define core_pi_sleep_clk	sleep_clk
#define pll_test_w		tcb_tck
#define pll_test_e		tcb_tck
#define gccmw_xo_logic_aon_clk	west_xo /* mux of west_xo / tcb_tck */
#define sys_xo			gccmw_xo_logic_aon_clk
#define sata_ln0_asic_clk	sata_ln_asic_clk
#define sata_ln1_asic_clk	sata_ln_asic_clk
#define sata_ln2_asic_clk	sata_ln_asic_clk
#define sata_ln3_asic_clk	sata_ln_asic_clk
#define sata_ln4_asic_clk	sata_ln_asic_clk
#define sata_ln5_asic_clk	sata_ln_asic_clk
#define sata_ln6_asic_clk	sata_ln_asic_clk
#define sata_ln7_asic_clk	sata_ln_asic_clk
#define sata_ln0_rx_clk		sata_ln_rx_clk
#define sata_ln1_rx_clk		sata_ln_rx_clk
#define sata_ln2_rx_clk		sata_ln_rx_clk
#define sata_ln3_rx_clk		sata_ln_rx_clk
#define sata_ln4_rx_clk		sata_ln_rx_clk
#define sata_ln5_rx_clk		sata_ln_rx_clk
#define sata_ln6_rx_clk		sata_ln_rx_clk
#define sata_ln7_rx_clk		sata_ln_rx_clk
#define pcie_x16_mstr_q23_clk_src	pcie_x16_mstr_q23_clk
#define pcie_x16_slv_q23_clk_src	pcie_x16_slv_q23_clk
#define pcie_x8_mstr_q23_clk_src	pcie_x8_mstr_q23_clk
#define pcie_x8_slv_q23_clk_src		pcie_x8_slv_q23_clk

/* PLLs */
#define CLK_FABIA_PLL		((clk_cat_t)1)
#define WEST_PLL0		((clk_clk_t)0)
#define WEST_PLL1		((clk_clk_t)1)
#define EAST_PLL0		((clk_clk_t)2)
#define EAST_PLL1		((clk_clk_t)3)
#define FREQ_WEST_PLL0		1000000000
#define FREQ_WEST_PLL1		800000000
#define FREQ_EAST_PLL0		2000000000
#define FREQ_EAST_PLL1		2000000000
#define west_pll0		clkid(CLK_FABIA_PLL, WEST_PLL0)
#define west_pll1		clkid(CLK_FABIA_PLL, WEST_PLL1)
#define east_pll0		clkid(CLK_FABIA_PLL, EAST_PLL0)
#define east_pll1		clkid(CLK_FABIA_PLL, EAST_PLL1)
#define NR_FABIA_PLL_CLKS	(EAST_PLL1 + 1)
#define FREQ_WEST_PLL0_1_0	200000000
#define FREQ_EAST_PLL1_1	1960000000
#define WEST_PLL0_OUT_MAIN	WEST_PLL0
#define WEST_PLL1_OUT_MAIN	WEST_PLL1
#define EAST_PLL0_OUT_MAIN	EAST_PLL0
#define EAST_PLL1_OUT_MAIN	EAST_PLL1
#define west_pll0_out_main	west_pll0
#define west_pll1_out_main	west_pll1
#define east_pll0_out_main	east_pll0
#define east_pll1_out_main	east_pll1

/* PLLOUT */
#define CLK_FABIA_PLLOUT		((clk_cat_t)2)
#define WEST_PLL0_OUT_EVEN		0
#define WEST_PLL1_OUT_EVEN		1
#define EAST_PLL0_OUT_EVEN		2
#define EAST_PLL1_OUT_EVEN		3
#define WEST_PLL0_OUT_ODD		4
#define WEST_PLL1_OUT_ODD		5
#define EAST_PLL0_OUT_ODD		6
#define EAST_PLL1_OUT_ODD		7
#define WEST_PLL0_OUT_TEST		8
#define WEST_PLL1_OUT_TEST		9
#define EAST_PLL0_OUT_TEST		10
#define EAST_PLL1_OUT_TEST		11
#define WEST_PLL0_OUT_MAIN_DIV2		12
#define WEST_PLL1_OUT_MAIN_DIV2		13
#define EAST_PLL0_OUT_MAIN_DIV2		14
#define EAST_PLL1_OUT_MAIN_DIV2		15
#define WEST_PLL0_OUT_EVEN_DIV2		16
#define WEST_PLL1_OUT_EVEN_DIV2		17
#define EAST_PLL0_OUT_EVEN_DIV2		18
#define EAST_PLL1_OUT_EVEN_DIV2		19
#define WEST_PLL0_OUT_MAIN_DIV10	20
#define PARB_PRE_DIV			24
#define IMC_POSTDIV_CLK_SRC		25
#define GCCE_XO_DIV4_CLK_SRC		26
#define FREQ_PARB_PRE_DIV	100000000
#define west_pll0_out_even	clkid(CLK_FABIA_PLLOUT, WEST_PLL0_OUT_EVEN)
#define west_pll1_out_even	clkid(CLK_FABIA_PLLOUT, WEST_PLL1_OUT_EVEN)
#define east_pll0_out_even	clkid(CLK_FABIA_PLLOUT, EAST_PLL0_OUT_EVEN)
#define east_pll1_out_even	clkid(CLK_FABIA_PLLOUT, EAST_PLL1_OUT_EVEN)
#define west_pll0_out_odd	clkid(CLK_FABIA_PLLOUT, WEST_PLL0_OUT_ODD)
#define west_pll1_out_odd	clkid(CLK_FABIA_PLLOUT, WEST_PLL1_OUT_ODD)
#define east_pll0_out_odd	clkid(CLK_FABIA_PLLOUT, EAST_PLL0_OUT_ODD)
#define east_pll1_out_odd	clkid(CLK_FABIA_PLLOUT, EAST_PLL1_OUT_ODD)
#define west_pll0_out_test	clkid(CLK_FABIA_PLLOUT, WEST_PLL0_OUT_TEST)
#define west_pll1_out_test	clkid(CLK_FABIA_PLLOUT, WEST_PLL1_OUT_TEST)
#define east_pll0_out_test	clkid(CLK_FABIA_PLLOUT, EAST_PLL0_OUT_TEST)
#define east_pll1_out_test	clkid(CLK_FABIA_PLLOUT, EAST_PLL1_OUT_TEST)
#define west_pll0_out_main_div2		\
	clkid(CLK_FABIA_PLLOUT, WEST_PLL0_OUT_MAIN_DIV2)
#define west_pll1_out_main_div2		\
	clkid(CLK_FABIA_PLLOUT, WEST_PLL1_OUT_MAIN_DIV2)
#define east_pll0_out_main_div2		\
	clkid(CLK_FABIA_PLLOUT, EAST_PLL0_OUT_MAIN_DIV2)
#define east_pll1_out_main_div2		\
	clkid(CLK_FABIA_PLLOUT, EAST_PLL1_OUT_MAIN_DIV2)
#define west_pll0_out_even_div2		\
	clkid(CLK_FABIA_PLLOUT, WEST_PLL0_OUT_EVEN_DIV2)
#define west_pll1_out_even_div2		\
	clkid(CLK_FABIA_PLLOUT, WEST_PLL1_OUT_EVEN_DIV2)
#define east_pll0_out_even_div2		\
	clkid(CLK_FABIA_PLLOUT, EAST_PLL0_OUT_EVEN_DIV2)
#define east_pll1_out_even_div2		\
	clkid(CLK_FABIA_PLLOUT, EAST_PLL1_OUT_EVEN_DIV2)
#define west_pll0_out_main_div10	\
	clkid(CLK_FABIA_PLLOUT, WEST_PLL0_OUT_MAIN_DIV10)
#define parb_pre_div		clkid(CLK_FABIA_PLLOUT, PARB_PRE_DIV)
#define imc_postdiv_clk_src	clkid(CLK_FABIA_PLLOUT, IMC_POSTDIV_CLK_SRC)
#define gcce_xo_div4_clk_src	clkid(CLK_FABIA_PLLOUT, GCCE_XO_DIV4_CLK_SRC)
#define NR_FABIA_PLLOUT_CLKS	(GCCE_XO_DIV4_CLK_SRC + 1)
#define sys_pll0_out_main	west_pll0_out_main
#define sys_pll0_out_main_div2	west_pll0_out_main_div2
#define sys_pll0_out_odd	west_pll0_out_odd
#define sys_pll0_out_even	west_pll0_out_even
#define sys_pll1_out_main	west_pll1_out_main
#define sys_pll1_out_main_div2	west_pll1_out_main_div2
#define sys_pll1_out_odd	west_pll1_out_odd
#define sys_pll1_out_even	west_pll1_out_even

/* RCG */
#define CLK_ROOT_CLOCK		((clk_cat_t)3)
#define IMC_PROC_CLK_SRC	0
#define IMC_CLK_SRC		1
#define GCCMW_XO_CLK_SRC	2
#define CONFIG_FAB_CLK_SRC	3
#define SYSTEM_FAB_CLK_SRC	4
#define WCSFPB_CLK_SRC		5
#define N1_CSFPB_CLK_SRC	6
#define S1_CSFPB_CLK_SRC	7
#define EMAC0_125M_CLK_SRC	8
#define EMAC0_AHB_CLK_SRC	9
#define EMAC0_SYS_25M_CLK_SRC	10
#define EMAC1_125M_CLK_SRC	11
#define EMAC1_AHB_CLK_SRC	12
#define EMAC1_SYS_25M_CLK_SRC	13
#define HMSS_W_BUS_CLK_SRC	14
#define CE0_CLK_SRC		15
#define CE1_CLK_SRC		16
#define CPRC_DIG_CLK_SRC	17
#define CPRC_MEM_CLK_SRC	18
#define CPRC_APCS_CLK_SRC	19
#define CPRC_NVDDA_CLK_SRC	20
#define CPRC_SVDDA_CLK_SRC	21
#define FAN_TACH_CLK_SRC	22
#define GP0_CLK_SRC		23
#define GP1_CLK_SRC		24
#define GP2_CLK_SRC		25
#define HDMA_AHB_CLK_SRC	26
#define HDMA_SFPB_CLK_SRC	27
#define HDMA_SYS_CLK_SRC	28
#define HDMA_VBU_CLK_SRC	29
#define W_QDSS_AT_CLK_SRC	30
#define SYS_FAB_VBU_CLK_SRC	31
#define SATA_VBU_CLK_SRC	32
#define PARB0_SER_CLK_SRC	33
#define PARB1_SER_CLK_SRC	34
#define PARB2_SER_CLK_SRC	35
#define QDSS_TRACECLKIN_CLK_SRC	36
#define ACC_CLK_SRC		37
#define SEC_CTRL_CLK_SRC	38
#define SATA_AHB_CLK_SRC	39
#define SATA_Q22_CLK_SRC	40
#define SATA_PMALIVE_CLK_SRC	41
#define SATA_RXOOB_CLK_SRC	42
#define SATA_CSFPB_CLK_SRC	43
#define SECSFPB_CLK_SRC		44
#define S2_CSFPB_CLK_SRC	45
#define N2_CSFPB_CLK_SRC	46
#define HMSS_E_BUS_CLK_SRC	47
#define E_QDSS_AT_CLK_SRC	48
#define PCIE_SS_0_VBU_CLK_SRC	49
#define PCIE_SS_1_VBU_CLK_SRC	50
#define GCCE_XO_CLK_SRC		51
#define PERIPH_FAB_CLK_SRC	52
#define BLSP0_QUP0_SPI_APPS_CLK_SRC	53
#define BLSP0_QUP1_SPI_APPS_CLK_SRC	54
#define BLSP0_UART0_APPS_CLK_SRC	55
#define BLSP0_UART1_APPS_CLK_SRC	56
#define BLSP0_UART2_APPS_CLK_SRC	57
#define BLSP0_UART3_APPS_CLK_SRC	58
#define BLSP1_QUP0_I2C_APPS_CLK_SRC	59
#define BLSP1_QUP1_I2C_APPS_CLK_SRC	60
#define BLSP1_QUP2_I2C_APPS_CLK_SRC	61
#define BLSP1_QUP3_I2C_APPS_CLK_SRC	62
#define BLSP2_QUP0_I2C_APPS_CLK_SRC	63
#define BLSP2_QUP1_I2C_APPS_CLK_SRC	64
#define BLSP2_QUP2_I2C_APPS_CLK_SRC	65
#define BLSP2_QUP3_I2C_APPS_CLK_SRC	66
#define BLSP3_QUP0_SPI_APPS_CLK_SRC	67
#define BLSP3_QUP1_SPI_APPS_CLK_SRC	68
#define BLSP3_UART0_APPS_CLK_SRC	69
#define BLSP3_UART1_APPS_CLK_SRC	70
#define BLSP3_UART2_APPS_CLK_SRC	71
#define BLSP3_UART3_APPS_CLK_SRC	72
#define PDM2_CLK_SRC		73
#define PERIPH_SPI_CLK_SRC	74
#define SDCC0_APPS_CLK_SRC	75
#define SDCC1_APPS_CLK_SRC	76
#define SDCC2_APPS_CLK_SRC	77
#define SDCC3_APPS_CLK_SRC	78
#define USB2_MSTR_CLK_SRC	79
#define PCIE0_SFPB_CLK_SRC	80
#define PCIE0_AUX_CLK_SRC	81
#define PCIE1_SFPB_CLK_SRC	82
#define PCIE1_AUX_CLK_SRC	83
#define FREQ_BLSP_SPI		50000000
#define FREQ_BLSP_I2C		50000000
#define FREQ_BLSP_UART		64000000
#define FREQ_SDCC		100000000
#define imc_proc_clk_src	clkid(CLK_ROOT_CLOCK, IMC_PROC_CLK_SRC)
#define imc_clk_src		clkid(CLK_ROOT_CLOCK, IMC_CLK_SRC)
#define gccmw_xo_clk_src	clkid(CLK_ROOT_CLOCK, GCCMW_XO_CLK_SRC)
#define config_fab_clk_src	clkid(CLK_ROOT_CLOCK, CONFIG_FAB_CLK_SRC)
#define system_fab_clk_src	clkid(CLK_ROOT_CLOCK, SYSTEM_FAB_CLK_SRC)
#define wcsfpb_clk_src		clkid(CLK_ROOT_CLOCK, WCSFPB_CLK_SRC)
#define n1_csfpb_clk_src	clkid(CLK_ROOT_CLOCK, N1_CSFPB_CLK_SRC)
#define s1_csfpb_clk_src	clkid(CLK_ROOT_CLOCK, S1_CSFPB_CLK_SRC)
#define emac0_125m_clk_src	clkid(CLK_ROOT_CLOCK, EMAC0_125M_CLK_SRC)
#define emac0_ahb_clk_src	clkid(CLK_ROOT_CLOCK, EMAC0_AHB_CLK_SRC)
#define emac0_sys_25m_clk_src	clkid(CLK_ROOT_CLOCK, EMAC0_SYS_25M_CLK_SRC)
#define emac1_125m_clk_src	clkid(CLK_ROOT_CLOCK, EMAC1_125M_CLK_SRC)
#define emac1_ahb_clk_src	clkid(CLK_ROOT_CLOCK, EMAC1_AHB_CLK_SRC)
#define emac1_sys_25m_clk_src	clkid(CLK_ROOT_CLOCK, EMAC1_SYS_25M_CLK_SRC)
#define hmss_w_bus_clk_src	clkid(CLK_ROOT_CLOCK, HMSS_W_BUS_CLK_SRC)
#define ce0_clk_src		clkid(CLK_ROOT_CLOCK, CE0_CLK_SRC)
#define ce1_clk_src		clkid(CLK_ROOT_CLOCK, CE1_CLK_SRC)
#define cprc_dig_clk_src	clkid(CLK_ROOT_CLOCK, CPRC_DIG_CLK_SRC)
#define cprc_mem_clk_src	clkid(CLK_ROOT_CLOCK, CPRC_MEM_CLK_SRC)
#define cprc_apcs_clk_src	clkid(CLK_ROOT_CLOCK, CPRC_APCS_CLK_SRC)
#define cprc_nvdda_clk_src	clkid(CLK_ROOT_CLOCK, CPRC_NVDDA_CLK_SRC)
#define cprc_svdda_clk_src	clkid(CLK_ROOT_CLOCK, CPRC_SVDDA_CLK_SRC)
#define fan_tach_clk_src	clkid(CLK_ROOT_CLOCK, FAN_TACH_CLK_SRC)
#define gp0_clk_src		clkid(CLK_ROOT_CLOCK, GP0_CLK_SRC)
#define gp1_clk_src		clkid(CLK_ROOT_CLOCK, GP1_CLK_SRC)
#define gp2_clk_src		clkid(CLK_ROOT_CLOCK, GP2_CLK_SRC)
#define hdma_ahb_clk_src	clkid(CLK_ROOT_CLOCK, HDMA_AHB_CLK_SRC)
#define hdma_sfpb_clk_src	clkid(CLK_ROOT_CLOCK, HDMA_SFPB_CLK_SRC)
#define hdma_sys_clk_src	clkid(CLK_ROOT_CLOCK, HDMA_SYS_CLK_SRC)
#define hdma_vbu_clk_src	clkid(CLK_ROOT_CLOCK, HDMA_VBU_CLK_SRC)
#define w_qdss_at_clk_src	clkid(CLK_ROOT_CLOCK, W_QDSS_AT_CLK_SRC)
#define sys_fab_vbu_clk_src	clkid(CLK_ROOT_CLOCK, SYS_FAB_VBU_CLK_SRC)
#define sata_vbu_clk_src	clkid(CLK_ROOT_CLOCK, SATA_VBU_CLK_SRC)
#define parb0_ser_clk_src	clkid(CLK_ROOT_CLOCK, PARB0_SER_CLK_SRC)
#define parb1_ser_clk_src	clkid(CLK_ROOT_CLOCK, PARB1_SER_CLK_SRC)
#define parb2_ser_clk_src	clkid(CLK_ROOT_CLOCK, PARB2_SER_CLK_SRC)
#define qdss_traceclkin_clk_src	clkid(CLK_ROOT_CLOCK, QDSS_TRACECLKIN_CLK_SRC)
#define acc_clk_src		clkid(CLK_ROOT_CLOCK, ACC_CLK_SRC)
#define sec_ctrl_clk_src	clkid(CLK_ROOT_CLOCK, SEC_CTRL_CLK_SRC)
#define sata_ahb_clk_src	clkid(CLK_ROOT_CLOCK, SATA_AHB_CLK_SRC)
#define sata_q22_clk_src	clkid(CLK_ROOT_CLOCK, SATA_Q22_CLK_SRC)
#define sata_pmalive_clk_src	clkid(CLK_ROOT_CLOCK, SATA_PMALIVE_CLK_SRC)
#define sata_rxoob_clk_src	clkid(CLK_ROOT_CLOCK, SATA_RXOOB_CLK_SRC)
#define sata_csfpb_clk_src	clkid(CLK_ROOT_CLOCK, SATA_CSFPB_CLK_SRC)
#define secsfpb_clk_src		clkid(CLK_ROOT_CLOCK, SECSFPB_CLK_SRC)
#define s2_csfpb_clk_src	clkid(CLK_ROOT_CLOCK, S2_CSFPB_CLK_SRC)
#define n2_csfpb_clk_src	clkid(CLK_ROOT_CLOCK, N2_CSFPB_CLK_SRC)
#define hmss_e_bus_clk_src	clkid(CLK_ROOT_CLOCK, HMSS_E_BUS_CLK_SRC)
#define e_qdss_at_clk_src	clkid(CLK_ROOT_CLOCK, E_QDSS_AT_CLK_SRC)
#define pcie_ss_0_vbu_clk_src	clkid(CLK_ROOT_CLOCK, PCIE_SS_0_VBU_CLK_SRC)
#define pcie_ss_1_vbu_clk_src	clkid(CLK_ROOT_CLOCK, PCIE_SS_1_VBU_CLK_SRC)
#define gcce_xo_clk_src		clkid(CLK_ROOT_CLOCK, GCCE_XO_CLK_SRC)
#define periph_fab_clk_src	clkid(CLK_ROOT_CLOCK, PERIPH_FAB_CLK_SRC)
#define blsp0_qup0_spi_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP0_QUP0_SPI_APPS_CLK_SRC)
#define blsp0_qup1_spi_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP0_QUP1_SPI_APPS_CLK_SRC)
#define blsp0_uart0_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP0_UART0_APPS_CLK_SRC)
#define blsp0_uart1_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP0_UART1_APPS_CLK_SRC)
#define blsp0_uart2_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP0_UART2_APPS_CLK_SRC)
#define blsp0_uart3_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP0_UART3_APPS_CLK_SRC)
#define blsp1_qup0_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP1_QUP0_I2C_APPS_CLK_SRC)
#define blsp1_qup1_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP1_QUP1_I2C_APPS_CLK_SRC)
#define blsp1_qup2_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP1_QUP2_I2C_APPS_CLK_SRC)
#define blsp1_qup3_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP1_QUP3_I2C_APPS_CLK_SRC)
#define blsp2_qup0_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP2_QUP0_I2C_APPS_CLK_SRC)
#define blsp2_qup1_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP2_QUP1_I2C_APPS_CLK_SRC)
#define blsp2_qup2_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP2_QUP2_I2C_APPS_CLK_SRC)
#define blsp2_qup3_i2c_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP2_QUP3_I2C_APPS_CLK_SRC)
#define blsp3_qup0_spi_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP3_QUP0_SPI_APPS_CLK_SRC)
#define blsp3_qup1_spi_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP3_QUP1_SPI_APPS_CLK_SRC)
#define blsp3_uart0_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP3_UART0_APPS_CLK_SRC)
#define blsp3_uart1_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP3_UART1_APPS_CLK_SRC)
#define blsp3_uart2_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP3_UART2_APPS_CLK_SRC)
#define blsp3_uart3_apps_clk_src	\
	clkid(CLK_ROOT_CLOCK, BLSP3_UART3_APPS_CLK_SRC)
#define pdm2_clk_src		clkid(CLK_ROOT_CLOCK, PDM2_CLK_SRC)
#define periph_spi_clk_src	clkid(CLK_ROOT_CLOCK, PERIPH_SPI_CLK_SRC)
#define sdcc0_apps_clk_src	clkid(CLK_ROOT_CLOCK, SDCC0_APPS_CLK_SRC)
#define sdcc1_apps_clk_src	clkid(CLK_ROOT_CLOCK, SDCC1_APPS_CLK_SRC)
#define sdcc2_apps_clk_src	clkid(CLK_ROOT_CLOCK, SDCC2_APPS_CLK_SRC)
#define sdcc3_apps_clk_src	clkid(CLK_ROOT_CLOCK, SDCC3_APPS_CLK_SRC)
#define usb2_mstr_clk_src	clkid(CLK_ROOT_CLOCK, USB2_MSTR_CLK_SRC)
#define pcie0_sfpb_clk_src	clkid(CLK_ROOT_CLOCK, PCIE0_SFPB_CLK_SRC)
#define pcie0_aux_clk_src	clkid(CLK_ROOT_CLOCK, PCIE0_AUX_CLK_SRC)
#define pcie1_sfpb_clk_src	clkid(CLK_ROOT_CLOCK, PCIE1_SFPB_CLK_SRC)
#define pcie1_aux_clk_src	clkid(CLK_ROOT_CLOCK, PCIE1_AUX_CLK_SRC)
#define qdss_at_clk_src		w_qdss_at_clk_src
/* TODO: SATA phy clock sources */
#define sata_ln0_asic_clk_src	sata_ln0_asic_clk
#define sata_ln1_asic_clk_src	sata_ln1_asic_clk
#define sata_ln2_asic_clk_src	sata_ln2_asic_clk
#define sata_ln3_asic_clk_src	sata_ln3_asic_clk
#define sata_ln4_asic_clk_src	sata_ln4_asic_clk
#define sata_ln5_asic_clk_src	sata_ln5_asic_clk
#define sata_ln6_asic_clk_src	sata_ln6_asic_clk
#define sata_ln7_asic_clk_src	sata_ln7_asic_clk
#define sata_ln0_rx_clk_src	sata_ln0_rx_clk
#define sata_ln1_rx_clk_src	sata_ln1_rx_clk
#define sata_ln2_rx_clk_src	sata_ln2_rx_clk
#define sata_ln3_rx_clk_src	sata_ln3_rx_clk
#define sata_ln4_rx_clk_src	sata_ln4_rx_clk
#define sata_ln5_rx_clk_src	sata_ln5_rx_clk
#define sata_ln6_rx_clk_src	sata_ln6_rx_clk
#define sata_ln7_rx_clk_src	sata_ln7_rx_clk
#define NR_ROOT_CLKS		(PCIE1_AUX_CLK_SRC + 1)

/* CBC */
#define CLK_BRANCH_CLOCK		((clk_cat_t)4)
#define IMC_CC_IMC_AXI_BRDG_PROC_CLK	((clk_clk_t)0)
#define IMC_CC_IMC_DBG_BRDG_PROC_CLK	((clk_clk_t)1)
#define IMC_CC_IMC_PROC_CLK		((clk_clk_t)2)
#define IMC_CC_IMC_PROC_AXI_CLK		((clk_clk_t)3)
#define IMC_CC_IMC_S0_AHB_CLK		((clk_clk_t)4)
#define IMC_CC_IMC_S1_AHB_CLK		((clk_clk_t)5)
#define IMC_CC_IMCFAB_SFAB_FCLK		((clk_clk_t)6)
#define IMC_CC_IMCFAB_RAM0_FCLK		((clk_clk_t)7)
#define IMC_CC_IMCFAB_RAM1_FCLK		((clk_clk_t)8)
#define IMC_CC_IMCFAB_RAM2_FCLK		((clk_clk_t)9)
#define IMC_CC_IMCFAB_RAM3_FCLK		((clk_clk_t)10)
#define IMC_CC_IMCFAB_RAM4_FCLK		((clk_clk_t)11)
#define IMC_CC_IMCFAB_RAM5_FCLK		((clk_clk_t)12)
#define IMC_CC_IMC_RAM0_AXI_CLK		((clk_clk_t)13)
#define IMC_CC_IMC_RAM1_AXI_CLK		((clk_clk_t)14)
#define IMC_CC_IMC_RAM2_AXI_CLK		((clk_clk_t)15)
#define IMC_CC_IMC_RAM3_AXI_CLK		((clk_clk_t)16)
#define IMC_CC_IMC_RAM4_AXI_CLK		((clk_clk_t)17)
#define IMC_CC_IMC_RAM5_AXI_CLK		((clk_clk_t)18)
#define IMC_CC_IMCFAB_CORE_CLK		((clk_clk_t)19)
#define IMC_CC_IMCFAB_S0_FCLK		((clk_clk_t)20)
#define IMC_CC_IMCFAB_S1_FCLK		((clk_clk_t)21)
#define IMC_CC_SFVBU_IMC_DVM_CLK	((clk_clk_t)22)
#define IMC_CC_IOMMU_AXI_CLK		((clk_clk_t)23)
#define IMC_CC_QSMMUV3_CLIENT_CLK	((clk_clk_t)24)
#define IMC_CC_IMCFAB_BOOT_ROM_FCLK	((clk_clk_t)25)
#define IMC_CC_BOOT_ROM_AHB_CLK		((clk_clk_t)26)
/* Security families */
#define GCCMW_SEC_CTRL_ACC_CLK		((clk_clk_t)27)
#define GCCMW_CE0_CLK			((clk_clk_t)28)
#define GCCMW_CE1_CLK			((clk_clk_t)29)
/* CONF_FAB families */
#define GCCMW_CFAB_N1_CSFPB_FCLK	((clk_clk_t)30)
#define GCCMW_FAN_TACH_AHB_CLK		((clk_clk_t)31)
#define GCCMW_HDMA_SFPB_AHB_CLK		((clk_clk_t)32)
#define GCCMW_NCSFPB_XPU_CFG_AHB_CLK	((clk_clk_t)33)
/* TODO:
 * GCCMW_AHB2MSM_CFAB_CFG_AHB_CLK
 * GCCMW_AHB_CLK
 * GCCMW_AHBMSM_SFAB_CFG_AHB_CLK
 * GCCMW_CFAB_CORE_CLK
 * GCCMW_CFAB_HDMA_SFPB_FCLK
 * GCCMW_CFAB_PARB_FCLK
 * GCCMW_CFAB_SATA_PHY_CFG_FCLK
 * GCCMW_CFAB_SFAB_FCLK
 * GCCMW_CFAB_SFAB_M_AXI_CLK
 * GCCMW_CFAB_TOPCFG1_FCLK
 * GCCMW_CFAB_TOPCFG2_FCLK
 * GCCMW_CFAB_WCSFPB_FCLK
 * GCCMW_CPRC_DIG_AHB_CLK
 * GCCMW_CPRC_MEM_AHB_CLK
 * GCCMW_CTCSR_AHB_CLK
 * GCCMW_MPM_AHB_CLK
 * GCCMW_N1_CSFPB_AHB_CLK
 * GCCMW_PARB0_CFG_AHB_CLK
 * GCCMW_PARB1_CFG_AHB_CLK
 * GCCMW_PARB2_CFG_AHB_CLK
 * GCCMW_PARB_AHB_CLK
 * GCCMW_QDSS_DAP_AXI_CLK
 * GCCMW_SATA_PHY_0_CFG_AHB_CLK
 * GCCMW_SATA_PHY_1_CFG_AHB_CLK
 * GCCMW_SATA_PHY_CFG_AHB_CLK
 * GCCMW_TLMM_AHB_CLK
 * GCCMW_TLMM_CLK
 * GCCMW_TOPCFG1_AHB_CLK
 * GCCMW_TOPCFG1_XPU_CFG_AHB_CLK
 * GCCMW_TOPCFG2_AHB_CLK
 * GCCMW_WCSFPB_AHB_CLK
 * GCCMW_WCSFPB_XPU_CFG_AHB_CLK
 */
/* CPRC families */
#define GCCMW_HMSS_CPRC_APCS_CLK	((clk_clk_t)34)
#define GCCMW_CPRC_DIG_CLK		((clk_clk_t)35)
#define GCCMW_CPRC_MEM_CLK		((clk_clk_t)36)
#define GCCMW_CPRC_NVDDA_CLK		((clk_clk_t)37)
#define GCCMW_CPRC_SVDDA_CLK		((clk_clk_t)38)
/* EMAC0 families */
#define GCCMW_EMAC0_125M_CLK		((clk_clk_t)39)
#define GCCMW_EMAC0_AHB_CLK		((clk_clk_t)40)
#define GCCMW_EMAC0_IOMMU_CFG_AHB_CLK	((clk_clk_t)41)
#define GCCMW_EMAC0_SYS_25M_CLK		((clk_clk_t)42)
#define GCCMW_GMII0_TX_CLK		((clk_clk_t)43)
#define GCCMW_EMAC0_RAM_SYS_CLK		((clk_clk_t)44)
#define GCCMW_EMAC0_RX_CLK		((clk_clk_t)45)
/* EMAC1 families */
#define GCCMW_EMAC1_125M_CLK		((clk_clk_t)46)
#define GCCMW_EMAC1_AHB_CLK		((clk_clk_t)47)
#define GCCMW_EMAC1_IOMMU_CFG_AHB_CLK	((clk_clk_t)48)
#define GCCMW_EMAC1_SYS_25M_CLK		((clk_clk_t)49)
#define GCCMW_GMII1_TX_CLK		((clk_clk_t)50)
#define GCCMW_EMAC1_RAM_SYS_CLK		((clk_clk_t)51)
#define GCCMW_EMAC1_RX_CLK		((clk_clk_t)52)
/* FAN_TACH families */
#define GCCMW_FAN_TACH_PWM_CLK		((clk_clk_t)53)
/* GP families */
#define GCCMW_GP0_CLK			((clk_clk_t)54)
#define GCCMW_GP1_CLK			((clk_clk_t)55)
#define GCCMW_GP2_CLK			((clk_clk_t)56)
/* HDMA families */
#define GCCMW_HDMA0_AHB_CLK		((clk_clk_t)57)
#define GCCMW_HDMA1_AHB_CLK		((clk_clk_t)58)
#define GCCMW_HDMA2_AHB_CLK		((clk_clk_t)59)
#define GCCMW_HDMA3_AHB_CLK		((clk_clk_t)60)
#define GCCMW_HDMA_SFPB_ALWAYS_ON_CLK	((clk_clk_t)61)
#define GCCMW_HDMA_SFPB_CLK		((clk_clk_t)62)
#define GCCMW_HDMA_VBU_CFG_AHB_CLK	((clk_clk_t)63)
#define GCCMW_HDMA0_QSMMUV3_CLIENT_CLK	((clk_clk_t)64)
#define GCCMW_HDMA0_SYS_CLK		((clk_clk_t)65)
#define GCCMW_HDMA1_QSMMUV3_CLIENT_CLK	((clk_clk_t)66)
#define GCCMW_HDMA1_SYS_CLK		((clk_clk_t)67)
#define GCCMW_HDMA2_QSMMUV3_CLIENT_CLK	((clk_clk_t)68)
#define GCCMW_HDMA2_SYS_CLK		((clk_clk_t)69)
#define GCCMW_HDMA3_QSMMUV3_CLIENT_CLK	((clk_clk_t)70)
#define GCCMW_HDMA3_SYS_CLK		((clk_clk_t)71)
#define GCCMW_HDMA_VBU_CORE_CLK		((clk_clk_t)72)
/* HMSS_BUS families */
#define GCCMW_HMSS_AXI_CLK		((clk_clk_t)73)
#define GCCMW_HMSS_W_AHB_CLK		((clk_clk_t)74)
#define GCCMW_HMSS_SW_AHB_CLK		((clk_clk_t)75)
#define GCCMW_HMSS_NW_AHB_CLK		((clk_clk_t)76)
/* N1_CSFPB families */
#define GCCMW_MDDR2_AHB_CLK		((clk_clk_t)77)
#define GCCMW_MDDR3_AHB_CLK		((clk_clk_t)78)
#define GCCMW_TSENS2_AHB_CLK		((clk_clk_t)79)
#define GCCMW_TSENS3_AHB_CLK		((clk_clk_t)80)
#define GCCMW_TSENS4_AHB_CLK		((clk_clk_t)81)
/* TODO:
 * GCCMW_N_GCCMS_AO_PGM_CLK
 * GCCMW_CPRC_NVDDA_AHB_CLK
 * GCCMW_N1_CSFPB_ALWAYS_ON_CLK
 * GCCMW_N1_CSFPB_CLK
 * GCCMW_N2_CSFPB_AHB_CLK
 * GCCMW_N_GCCMS_AHB_CLK
 * GCCMW_SATA_SS_SFPB_CLK
 */
/* PARB families */
#define GCCMW_PARB0_SER_CLK		((clk_clk_t)82)
#define GCCMW_PARB1_SER_CLK		((clk_clk_t)83)
#define GCCMW_PARB2_SER_CLK		((clk_clk_t)84)
/* QDSS families */
#define GCCMW_AT_W1_CLK			((clk_clk_t)85)
#define GCCMW_AT_W2_CLK			((clk_clk_t)86)
#define GCCMW_AT_W3_CLK			((clk_clk_t)87)
#define GCCMW_AT_W5_CLK			((clk_clk_t)88)
#define GCCMW_AT_W7_CLK			((clk_clk_t)89)
#define GCCMW_AT_W9_CLK			((clk_clk_t)90)
#define GCCMW_HMSS_AT_W_CLK		((clk_clk_t)91)
#define GCCMW_QDSS_TRACECLKIN_CLK	((clk_clk_t)92)
/* S1_CSFPB families */
#define GCCMW_MDDR0_AHB_CLK		((clk_clk_t)93)
#define GCCMW_MDDR1_AHB_CLK		((clk_clk_t)94)
#define GCCMW_TSENS9_AHB_CLK		((clk_clk_t)95)
#define GCCMW_TSENS10_AHB_CLK		((clk_clk_t)96)
/* TODO:
 * GCCMW_S_GCCMS_AO_PGM_CLK
 * GCCMW_CPRC_SVDDA_AHB_CLK
 * GCCMW_S1_CSFPB_ALWAYS_ON_CLK
 * GCCMW_S1_CSFPB_CLK
 * GCCMW_S2_CSFPB_AHB_CLK
 * GCCMW_S_GCCMS_AHB_CLK
 */
/* SATA families */
#define GCCMW_SATA_LN0_AHB_CLK		((clk_clk_t)97)
#define GCCMW_SATA_LN1_AHB_CLK		((clk_clk_t)98)
#define GCCMW_SATA_LN2_AHB_CLK		((clk_clk_t)99)
#define GCCMW_SATA_LN3_AHB_CLK		((clk_clk_t)100)
#define GCCMW_SATA_LN4_AHB_CLK		((clk_clk_t)101)
#define GCCMW_SATA_LN5_AHB_CLK		((clk_clk_t)102)
#define GCCMW_SATA_LN6_AHB_CLK		((clk_clk_t)103)
#define GCCMW_SATA_LN7_AHB_CLK		((clk_clk_t)104)
#define GCCMW_SATA_LN0_ASIC_CLK		((clk_clk_t)105)
#define GCCMW_SATA_LN0_RX_CLK		((clk_clk_t)106)
#define GCCMW_SATA_LN1_ASIC_CLK		((clk_clk_t)107)
#define GCCMW_SATA_LN1_RX_CLK		((clk_clk_t)108)
#define GCCMW_SATA_LN2_ASIC_CLK		((clk_clk_t)109)
#define GCCMW_SATA_LN2_RX_CLK		((clk_clk_t)110)
#define GCCMW_SATA_LN3_ASIC_CLK		((clk_clk_t)111)
#define GCCMW_SATA_LN3_RX_CLK		((clk_clk_t)112)
#define GCCMW_SATA_LN4_ASIC_CLK		((clk_clk_t)113)
#define GCCMW_SATA_LN4_RX_CLK		((clk_clk_t)114)
#define GCCMW_SATA_LN5_ASIC_CLK		((clk_clk_t)115)
#define GCCMW_SATA_LN5_RX_CLK		((clk_clk_t)116)
#define GCCMW_SATA_LN6_ASIC_CLK		((clk_clk_t)117)
#define GCCMW_SATA_LN6_RX_CLK		((clk_clk_t)118)
#define GCCMW_SATA_LN7_ASIC_CLK		((clk_clk_t)119)
#define GCCMW_SATA_LN7_RX_CLK		((clk_clk_t)120)
#define GCCMW_SATA_LN0_PMALIVE_CLK	((clk_clk_t)121)
#define GCCMW_SATA_LN1_PMALIVE_CLK	((clk_clk_t)122)
#define GCCMW_SATA_LN2_PMALIVE_CLK	((clk_clk_t)123)
#define GCCMW_SATA_LN3_PMALIVE_CLK	((clk_clk_t)124)
#define GCCMW_SATA_LN4_PMALIVE_CLK	((clk_clk_t)125)
#define GCCMW_SATA_LN5_PMALIVE_CLK	((clk_clk_t)126)
#define GCCMW_SATA_LN6_PMALIVE_CLK	((clk_clk_t)127)
#define GCCMW_SATA_LN7_PMALIVE_CLK	((clk_clk_t)128)
#define GCCMW_SATA_LN0_AXI_CLK		((clk_clk_t)129)
#define GCCMW_SATA_LN1_AXI_CLK		((clk_clk_t)130)
#define GCCMW_SATA_LN2_AXI_CLK		((clk_clk_t)131)
#define GCCMW_SATA_LN3_AXI_CLK		((clk_clk_t)132)
#define GCCMW_SATA_LN4_AXI_CLK		((clk_clk_t)133)
#define GCCMW_SATA_LN5_AXI_CLK		((clk_clk_t)134)
#define GCCMW_SATA_LN6_AXI_CLK		((clk_clk_t)135)
#define GCCMW_SATA_LN7_AXI_CLK		((clk_clk_t)136)
#define GCCMW_SATA_LN0_QSMMUV3_CLIENT_CLK	((clk_clk_t)137)
#define GCCMW_SATA_LN1_QSMMUV3_CLIENT_CLK	((clk_clk_t)138)
#define GCCMW_SATA_LN2_QSMMUV3_CLIENT_CLK	((clk_clk_t)139)
#define GCCMW_SATA_LN3_QSMMUV3_CLIENT_CLK	((clk_clk_t)140)
#define GCCMW_SATA_LN4_QSMMUV3_CLIENT_CLK	((clk_clk_t)141)
#define GCCMW_SATA_LN5_QSMMUV3_CLIENT_CLK	((clk_clk_t)142)
#define GCCMW_SATA_LN6_QSMMUV3_CLIENT_CLK	((clk_clk_t)143)
#define GCCMW_SATA_LN7_QSMMUV3_CLIENT_CLK	((clk_clk_t)144)
#define GCCMW_SATA_LN0_RXOOB_CLK	((clk_clk_t)145)
#define GCCMW_SATA_LN1_RXOOB_CLK	((clk_clk_t)146)
#define GCCMW_SATA_LN2_RXOOB_CLK	((clk_clk_t)147)
#define GCCMW_SATA_LN3_RXOOB_CLK	((clk_clk_t)148)
#define GCCMW_SATA_LN4_RXOOB_CLK	((clk_clk_t)149)
#define GCCMW_SATA_LN5_RXOOB_CLK	((clk_clk_t)150)
#define GCCMW_SATA_LN6_RXOOB_CLK	((clk_clk_t)151)
#define GCCMW_SATA_LN7_RXOOB_CLK	((clk_clk_t)152)
#define GCCMW_SATA_PHY0_RXOOB_CLK	((clk_clk_t)153)
#define GCCMW_SATA_PHY1_RXOOB_CLK	((clk_clk_t)154)
#define GCCMW_SATA_VBU_AHB_CFG_CLK	((clk_clk_t)155)
#define GCCMW_SATA_VBU_CORE_CLK		((clk_clk_t)156)
/* Security families */
#define GCCMW_SEC_CTRL_CLK		((clk_clk_t)157)
/* TODO:
 * GCCMW_SEC_CTRL_SENSE_CLK
 */
/* Sleep families */
#define GCCMW_FAN_TACH_SLEEP_CLK	((clk_clk_t)158)
/* TODO:
 * GCCMW_IM_SLEEP_CLK
 * GCCMW_IMEM_SLEEP_CLK
 * GCCMW_TSENS10_SLEEP_CLK
 * GCCMW_TSENS1_SLEEP_CLK
 * GCCMW_TSENS2_SLEEP_CLK
 * GCCMW_TSENS3_SLEEP_CLK
 * GCCMW_TSENS4_SLEEP_CLK
 * GCCMW_TSENS9_SLEEP_CLK
 */
/* SYS_FAB fammilies */
#define GCCMW_CE1_AHB_CLK		((clk_clk_t)159)
#define GCCMW_CE1_AXI_CLK		((clk_clk_t)160)
#define GCCMW_CE1_IOMMU_CFG_AHB_CLK	((clk_clk_t)161)
#define GCCMW_CE1_QSMMUV3_CLIENT_CLK	((clk_clk_t)162)
#define GCCMW_EMAC0_AXI_CLK		((clk_clk_t)163)
#define GCCMW_EMAC0_QSMMUV3_CLIENT_CLK	((clk_clk_t)164)
#define GCCMW_EMAC1_AXI_CLK		((clk_clk_t)165)
#define GCCMW_EMAC1_QSMMUV3_CLIENT_CLK	((clk_clk_t)166)
#define GCCMW_QDSS_FCLK			((clk_clk_t)167)
#define GCCMW_QDSS_STM_CLK		((clk_clk_t)168)
#define GCCMW_SFAB_CE1_FCLK		((clk_clk_t)169)
#define GCCMW_SFAB_EMAC0_FCLK		((clk_clk_t)170)
#define GCCMW_SFAB_EMAC1_FCLK		((clk_clk_t)171)
#define GCCMW_SFAB_VBU_FCLK		((clk_clk_t)172)
#define GCCMW_SFVBU_CE1_DVM_CLK		((clk_clk_t)173)
#define GCCMW_SFVBU_EMAC0_DVM_CLK	((clk_clk_t)174)
#define GCCMW_SFVBU_EMAC1_DVM_CLK	((clk_clk_t)175)
#define GCCMW_SFVBU_Q22M_CLK		((clk_clk_t)176)
#define GCCMW_SFVBU_Q22S_CLK		((clk_clk_t)177)
/* TODO:
 * GCCMW_IMEM_AXI_CLK
 * GCCMW_MSG_RAM_AXI_CLK
 * GCCMW_QDSS_ETR_AXI_CLK
 * GCCMW_SFAB_CE0_FCLK
 * GCCMW_SFAB_CFAB_AXI_CLK
 * GCCMW_SFAB_CFAB_FCLK
 * GCCMW_SFAB_CORE_CLK
 * GCCMW_SFAB_HMSS_FCLK
 * GCCMW_SFAB_IMCFAB_AXI_CLK
 * GCCMW_SFAB_IMCFAB_FCLK
 * GCCMW_SFAB_IMEM_FCLK
 * GCCMW_SFAB_MSG_RAM_FCLK
 * GCCMW_SFAB_PFAB_AXI_CLK
 * GCCMW_SFAB_PFAB_FCLK
 * GCCMW_SFAB_QDSS_STM_FCLK
 * GCCMW_SFVBU_CE0_DVM_CLK
 * GCCMW_SFVBU_PERIPH_DVM_CLK
 * GCCMW_TIC_AHB2AHB_TIC_CLK
 */
#define GCCMW_SFAB_VBU_CORE_CLK		((clk_clk_t)178)
/* W_CSFPB families */
#define GCCMW_EMAC_AHB2PHY_CFG_AHB_CLK	((clk_clk_t)179)
#define GCCMW_QDSS_CFG_AHB_CLK		((clk_clk_t)180)
#define GCCMW_QDSS_DAP_AHB_CLK		((clk_clk_t)181)
#define GCCMW_QDSS_RBCPR_XPU_AHB_CLK	((clk_clk_t)182)
#define GCCMW_SFAB_VBU_CFG_CLK		((clk_clk_t)183)
#define GCCMW_TSENS1_AHB_CLK		((clk_clk_t)184)
/* TODO:
 * GCCMW_IMC_IOMMU_CFG_AHB_CLK
 * GCCMW_IMEM_CFG_AHB_CLK
 * GCCMW_MSG_RAM_CFG_AHB_CLK
 * GCCMW_S1_CSFPB_AHB_CLK
 * GCCMW_SEC_CTRL_AHB_CLK
 * GCCMW_SFAB_CFG_AHB_CLK
 * GCCMW_WCSFPB_ALWAYS_ON_CLK
 * GCCMW_WCSFPB_CLK
 */
#define GCCMW_TIC_CLK			((clk_clk_t)185)
/* HMSS_BUS families */
#define GCCE_HMSS_NE_AHB_CLK		((clk_clk_t)186)
#define GCCE_HMSS_SE_AHB_CLK		((clk_clk_t)187)
/* N2_CSFPB families */
#define GCCE_MDDR4_AHB_CLK		((clk_clk_t)188)
/* TODO:
 * GCCE_N2_CSFPB_ALWAYS_ON_CLK
 * GCCE_N2_CSFPB_CLK
 */
#define GCCE_TSENS5_AHB_CLK		((clk_clk_t)189)
/* PCIE families */
#define GCCE_PCIE_SS_0_VBU_CORE_CLK	((clk_clk_t)190)
#define GCCE_PCIE_SS_1_VBU_CORE_CLK	((clk_clk_t)191)
/* QDSS families */
#define GCCE_AT_E1_CLK			((clk_clk_t)192)
#define GCCE_AT_E2_CLK			((clk_clk_t)193)
#define GCCE_AT_E3_CLK			((clk_clk_t)194)
#define GCCE_AT_E5_CLK			((clk_clk_t)195)
#define GCCE_AT_E7_CLK			((clk_clk_t)196)
#define GCCE_AT_E9_CLK			((clk_clk_t)197)
#define GCCE_HMSS_AT_E_CLK		((clk_clk_t)198)
/* S2_CSFPB families */
#define GCCE_MDDR5_AHB_CLK		((clk_clk_t)199)
#define GCCE_TSENS8_AHB_CLK		((clk_clk_t)200)
/* TODO:
 * GCCE_S2_CSFPB_ALWAYS_ON_CLK
 * GCCE_S2_CSFPB_CLK
 * GCCE_SECCSFPB_AHB_CLK
 */
/* SE_CSFPB families */
#define GCCE_PCIE_SS_0_SECSFPB_AHB_CLK	((clk_clk_t)201)
#define GCCE_PCIE_SS_1_SECSFPB_AHB_CLK	((clk_clk_t)202)
#define GCCE_PSCLKGEN_CFG_AHB_CLK	((clk_clk_t)203)
#define GCCE_TSENS6_AHB_CLK		((clk_clk_t)204)
#define GCCE_TSENS7_AHB_CLK		((clk_clk_t)205)
/* TODO:
 * GCCE_AHB_CLK
 * GCCE_SECSFPB_ALWAYS_ON_CLK
 * GCCE_SECSFPB_CLK
 */
/* XO families */
#define GCCE_XO_CLK			((clk_clk_t)206)
#define GCCE_TSENS5_EXT_CLK		((clk_clk_t)207)
#define GCCE_TSENS6_EXT_CLK		((clk_clk_t)208)
#define GCCE_TSENS7_EXT_CLK		((clk_clk_t)209)
#define GCCE_TSENS8_EXT_CLK		((clk_clk_t)210)
#define GCCE_XO_DIV4_CLK		((clk_clk_t)211)
/* PCIE_AUX families */
/* TODO:
 * PCIE_CC_0_PCIE_X16_AUX_CLK
 * PCIE_CC_0_PCIE_X8_0_AUX_CLK
 * PCIE_CC_0_PCIE_X8_1_AUX_CLK
 * PCIE_CC_0_PHY_A_AUX_CLK
 * PCIE_CC_0_PHY_B_AUX_CLK
 * PCIE_CC_0_PHY_C_AUX_CLK
 */
/* PCIE_PCLK families */
/* TODO:
 * PCIE_CC_0_PCIE_A_PCLK
 * PCIE_CC_0_PCIE_X16_PCLK
 * PCIE_CC_0_PCIE_X8_0_PCLK
 * PCIE_CC_0_PCIE_B_PCLK
 * PCIE_CC_0_PCIE_X8_1_PCLK
 * PCIE_CC_0_PCIE_C_PCLK
 */
/* PCIE_SFPB families */
#define PCIE_CC_0_PCIE_PHY_AA_CFG_AHB_CLK	((clk_clk_t)212)
#define PCIE_CC_0_PCIE_PHY_AB_CFG_AHB_CLK	((clk_clk_t)213)
#define PCIE_CC_0_PCIE_PHY_B_CFG_AHB_CLK	((clk_clk_t)214)
#define PCIE_CC_0_PCIE_PHY_C_CFG_AHB_CLK	((clk_clk_t)215)
/* TODO:
 * PCIE_CC_0_AHB_CLK
 * PCIE_CC_0_PCIE_SFPB_ALWAYS_ON_CLK
 * PCIE_CC_0_PCIE_SFPB_CLK
 * PCIE_CC_0_PCIE_X16_AHB_CLK
 * PCIE_CC_0_PCIE_X16_MMU_AHB_CLK
 * PCIE_CC_0_PCIE_X8_0_AHB_CLK
 * PCIE_CC_0_PCIE_X8_0_MMU_AHB_CLK
 * PCIE_CC_0_PCIE_X8_1_AHB_CLK
 * PCIE_CC_0_PCIE_X8_1_MMU_AHB_CLK
 * PCIE_CC_0_VBU_AHB_CFG_CLK
 */
/* PCIE_X16 families */
#define PCIE_CC_0_PCIE_X16_MMU_MSTR_Q23_CLK	((clk_clk_t)216)
#define PCIE_CC_0_PCIE_X16_MSTR_Q23_CLK	((clk_clk_t)217)
#define PCIE_CC_0_PCIE_X16_SLV_Q23_CLK	((clk_clk_t)218)
/* PCIE_X8 families */
#define PCIE_CC_0_PCIE_X8_0_MMU_MSTR_Q23_CLK	((clk_clk_t)219)
#define PCIE_CC_0_PCIE_X8_0_MSTR_Q23_CLK	((clk_clk_t)220)
#define PCIE_CC_0_PCIE_X8_1_MMU_MSTR_Q23_CLK	((clk_clk_t)221)
#define PCIE_CC_0_PCIE_X8_1_MSTR_Q23_CLK	((clk_clk_t)222)
#define PCIE_CC_0_PCIE_X8_0_SLV_Q23_CLK	((clk_clk_t)223)
#define PCIE_CC_0_PCIE_X8_1_SLV_Q23_CLK	((clk_clk_t)224)
/* PCIE_AUX families */
/* TODO:
 * PCIE_CC_1_PCIE_X16_AUX_CLK
 * PCIE_CC_1_PCIE_X8_0_AUX_CLK
 * PCIE_CC_1_PCIE_X8_1_AUX_CLK
 * PCIE_CC_1_PHY_A_AUX_CLK
 * PCIE_CC_1_PHY_B_AUX_CLK
 * PCIE_CC_1_PHY_C_AUX_CLK
 */
/* PCIE_PCLK families */
/* TODO:
 * PCIE_CC_1_PCIE_A_PCLK
 * PCIE_CC_1_PCIE_X16_PCLK
 * PCIE_CC_1_PCIE_X8_0_PCLK
 * PCIE_CC_1_PCIE_B_PCLK
 * PCIE_CC_1_PCIE_X8_1_PCLK
 * PCIE_CC_1_PCIE_C_PCLK
 */
/* PCIE_SFPB families */
#define PCIE_CC_1_PCIE_PHY_AA_CFG_AHB_CLK	((clk_clk_t)225)
#define PCIE_CC_1_PCIE_PHY_AB_CFG_AHB_CLK	((clk_clk_t)226)
#define PCIE_CC_1_PCIE_PHY_B_CFG_AHB_CLK	((clk_clk_t)227)
#define PCIE_CC_1_PCIE_PHY_C_CFG_AHB_CLK	((clk_clk_t)228)
/* TODO:
 * PCIE_CC_1_AHB_CLK
 * PCIE_CC_1_PCIE_SFPB_ALWAYS_ON_CLK
 * PCIE_CC_1_PCIE_SFPB_CLK
 * PCIE_CC_1_PCIE_X16_AHB_CLK
 * PCIE_CC_1_PCIE_X16_MMU_AHB_CLK
 * PCIE_CC_1_PCIE_X8_0_AHB_CLK
 * PCIE_CC_1_PCIE_X8_0_MMU_AHB_CLK
 * PCIE_CC_1_PCIE_X8_1_AHB_CLK
 * PCIE_CC_1_PCIE_X8_1_MMU_AHB_CLK
 * PCIE_CC_1_VBU_AHB_CFG_CLK
 */
/* PCIE_X16 families */
#define PCIE_CC_1_PCIE_X16_MMU_MSTR_Q23_CLK	((clk_clk_t)229)
#define PCIE_CC_1_PCIE_X16_MSTR_Q23_CLK	((clk_clk_t)230)
#define PCIE_CC_1_PCIE_X16_SLV_Q23_CLK	((clk_clk_t)231)
/* PCIE_X8 families */
#define PCIE_CC_1_PCIE_X8_0_MMU_MSTR_Q23_CLK	((clk_clk_t)232)
#define PCIE_CC_1_PCIE_X8_0_MSTR_Q23_CLK	((clk_clk_t)233)
#define PCIE_CC_1_PCIE_X8_1_MMU_MSTR_Q23_CLK	((clk_clk_t)234)
#define PCIE_CC_1_PCIE_X8_1_MSTR_Q23_CLK	((clk_clk_t)235)
#define PCIE_CC_1_PCIE_X8_0_SLV_Q23_CLK	((clk_clk_t)236)
#define PCIE_CC_1_PCIE_X8_1_SLV_Q23_CLK	((clk_clk_t)237)
#define imc_cc_imc_axi_brdg_proc_clk	\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_AXI_BRDG_PROC_CLK)
#define imc_cc_imc_dbg_brdg_proc_clk	\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_DBG_BRDG_PROC_CLK)
#define imc_cc_imc_proc_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_PROC_CLK)
#define imc_cc_imc_proc_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_PROC_AXI_CLK)
#define imc_cc_imc_s0_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_S0_AHB_CLK)
#define imc_cc_imc_s1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_S1_AHB_CLK)
#define imc_cc_imcfab_sfab_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_SFAB_FCLK)
#define imc_cc_imcfab_ram0_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_RAM0_FCLK)
#define imc_cc_imcfab_ram1_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_RAM1_FCLK)
#define imc_cc_imcfab_ram2_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_RAM2_FCLK)
#define imc_cc_imcfab_ram3_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_RAM3_FCLK)
#define imc_cc_imcfab_ram4_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_RAM4_FCLK)
#define imc_cc_imcfab_ram5_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_RAM5_FCLK)
#define imc_cc_imc_ram0_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_RAM0_AXI_CLK)
#define imc_cc_imc_ram1_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_RAM1_AXI_CLK)
#define imc_cc_imc_ram2_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_RAM2_AXI_CLK)
#define imc_cc_imc_ram3_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_RAM3_AXI_CLK)
#define imc_cc_imc_ram4_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_RAM4_AXI_CLK)
#define imc_cc_imc_ram5_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMC_RAM5_AXI_CLK)
#define imc_cc_imcfab_core_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_CORE_CLK)
#define imc_cc_imcfab_s0_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_S0_FCLK)
#define imc_cc_imcfab_s1_fclk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_S1_FCLK)
#define imc_cc_sfvbu_imc_dvm_clk	\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_SFVBU_IMC_DVM_CLK)
#define imc_cc_iommu_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IOMMU_AXI_CLK)
#define imc_cc_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_QSMMUV3_CLIENT_CLK)
#define imc_cc_imcfab_boot_rom_fclk	\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_IMCFAB_BOOT_ROM_FCLK)
#define imc_cc_boot_rom_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, IMC_CC_BOOT_ROM_AHB_CLK)
#define gccmw_sec_ctrl_acc_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SEC_CTRL_ACC_CLK)
#define gccmw_ce0_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE0_CLK)
#define gccmw_ce1_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE1_CLK)
#define gccmw_ahb2MSM_cfab_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AHB2MSM_CFAB_CFG_AHB_CLK)
#define gccmw_ahb_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AHB_CLK)
#define gccmw_ahbMSM_sfab_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AHBMSM_SFAB_CFG_AHB_CLK)
#define gccmw_cfab_core_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_CORE_CLK)
#define gccmw_cfab_hdma_sfpb_fclk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_HDMA_SFPB_FCLK)
#define gccmw_cfab_n1_csfpb_fclk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_N1_CSFPB_FCLK)
#define gccmw_cfab_parb_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_PARB_FCLK)
#define gccmw_cfab_sata_phy_cfg_fclk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_SATA_PHY_CFG_FCLK)
#define gccmw_cfab_sfab_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_SFAB_FCLK)
#define gccmw_cfab_sfab_m_axi_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_SFAB_M_AXI_CLK)
#define gccmw_cfab_topcfg1_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_TOPCFG1_FCLK)
#define gccmw_cfab_topcfg2_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_TOPCFG2_FCLK)
#define gccmw_cfab_wcsfpb_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CFAB_WCSFPB_FCLK)
#define gccmw_cprc_dig_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_DIG_AHB_CLK)
#define gccmw_cprc_mem_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_MEM_AHB_CLK)
#define gccmw_ctCSR_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CTCSR_AHB_CLK)
#define gccmw_fan_tach_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_FAN_TACH_AHB_CLK)
#define gccmw_hdma_sfpb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA_SFPB_AHB_CLK)
#define gccmw_cfab_hdma_sfpb_ahb_clk	gccmw_hdma_sfpb_ahb_clk
#define gccmw_mpm_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_MPM_AHB_CLK)
#define gccmw_n1_csfpb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_N1_CSFPB_AHB_CLK)
#define gccmw_cfab_n1_csfpb_ahb_clk	gccmw_n1_csfpb_ahb_clk
#define gccmw_ncsfpb_xpu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_NCSFPB_XPU_CFG_AHB_CLK)
#define gccmw_parb0_cfg_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_PARB0_CFG_AHB_CLK)
#define gccmw_parb1_cfg_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_PARB1_CFG_AHB_CLK)
#define gccmw_parb2_cfg_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_PARB2_CFG_AHB_CLK)
#define gccmw_parb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_PARB_AHB_CLK)
#define gccmw_cfab_parb_ahb_clk		gccmw_parb_ahb_clk
#define gccmw_qdss_dap_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_DAP_AXI_CLK)
#define gccmw_cfab_qdss_dap_axi_clk	gccmw_qdss_dap_axi_clk
#define gccmw_sata_phy_0_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_PHY_0_CFG_AHB_CLK)
#define gccmw_sata_phy_1_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_PHY_1_CFG_AHB_CLK)
#define gccmw_sata_phy_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_PHY_CFG_AHB_CLK)
#define gccmw_cfab_sata_phy_cfg_ahb_clk	gccmw_sata_phy_cfg_ahb_clk
#define gccmw_tlmm_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TLMM_AHB_CLK)
#define gccmw_tlmm_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TLMM_CLK)
#define gccmw_topcfg1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TOPCFG1_AHB_CLK)
#define gccmw_cfab_topcfg1_ahb_clk	gccmw_topcfg1_ahb_clk
#define gccmw_topcfg1_xpu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TOPCFG1_XPU_CFG_AHB_CLK)
#define gccmw_topcfg2_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TOPCFG2_AHB_CLK)
#define gccmw_cfab_topcfg2_ahb_clk	gccmw_topcfg2_ahb_clk
#define gccmw_wcsfpb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_WCSFPB_AHB_CLK)
#define gccmw_cfab_wcsfpb_ahb_clk	gccmw_wcsfpb_ahb_clk
#define gccmw_wcsfpb_xpu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_WCSFPB_XPU_CFG_AHB_CLK)
#define gccmw_hmss_cprc_apcs_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HMSS_CPRC_APCS_CLK)
#define gccmw_cprc_dig_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_DIG_CLK)
#define gccmw_cprc_mem_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_MEM_CLK)
#define gccmw_cprc_nvdda_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_NVDDA_CLK)
#define gccmw_cprc_svdda_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_SVDDA_CLK)
#define gccmw_emac0_125m_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_125M_CLK)
#define gccmw_emac0_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_AHB_CLK)
#define gccmw_sfab_ema0_ahb_clk		gccmw_emac0_ahb_clk
#define gccmw_emac0_iommu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_IOMMU_CFG_AHB_CLK)
#define gccmw_emac0_sys_25m_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_SYS_25M_CLK)
#define gccmw_gmii0_tx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_GMII0_TX_CLK)
#define gccmw_emac0_ram_sys_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_RAM_SYS_CLK)
#define gccmw_emac0_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_RX_CLK)
#define gccmw_emac1_125m_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_125M_CLK)
#define gccmw_emac1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_AHB_CLK)
#define gccmw_sfab_ema1_ahb_clk		gccmw_emac1_ahb_clk
#define gccmw_emac1_iommu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_IOMMU_CFG_AHB_CLK)
#define gccmw_emac1_sys_25m_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_SYS_25M_CLK)
#define gccmw_gmii1_tx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_GMII1_TX_CLK)
#define gccmw_emac1_ram_sys_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_RAM_SYS_CLK)
#define gccmw_emac1_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_RX_CLK)
#define gccmw_fan_tach_pwm_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_FAN_TACH_PWM_CLK)
#define gccmw_gp0_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_GP0_CLK)
#define gccmw_gp1_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_GP1_CLK)
#define gccmw_gp2_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_GP2_CLK)
#define gccmw_hdma0_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA0_AHB_CLK)
#define gccmw_hdma1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA1_AHB_CLK)
#define gccmw_hdma2_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA2_AHB_CLK)
#define gccmw_hdma3_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA3_AHB_CLK)
#define gccmw_hdma_sfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA_SFPB_ALWAYS_ON_CLK)
#define gccmw_hdma_sfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA_SFPB_CLK)
#define gccmw_hdma_vbu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA_VBU_CFG_AHB_CLK)
#define gccmw_hdma0_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA0_QSMMUV3_CLIENT_CLK)
#define gccmw_hdma0_sys_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA0_SYS_CLK)
#define gccmw_hdma1_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA1_QSMMUV3_CLIENT_CLK)
#define gccmw_hdma1_sys_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA1_SYS_CLK)
#define gccmw_hdma2_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA2_QSMMUV3_CLIENT_CLK)
#define gccmw_hdma2_sys_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA2_SYS_CLK)
#define gccmw_hdma3_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA3_QSMMUV3_CLIENT_CLK)
#define gccmw_hdma3_sys_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA3_SYS_CLK)
#define gccmw_hdma_vbu_core_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HDMA_VBU_CORE_CLK)
#define gccmw_hmss_pbsu3_clk		gccmw_hdma_vbu_core_clk
#define gccmw_hmss_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HMSS_AXI_CLK)
#define gccmw_hmss_w_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HMSS_W_AHB_CLK)
#define gccmw_hmss_sw_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HMSS_SW_AHB_CLK)
#define gccmw_hmss_nw_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HMSS_NW_AHB_CLK)
#define gccmw_sfab_hmss_axi_clk		gccmw_hmss_axi_clk
#define gccmw_sfab_hmss_ahb_clk		gccmw_hmss_w_ahb_clk
#define gccmw_mddr2_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_MDDR2_AHB_CLK)
#define gccmw_mddr3_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_MDDR3_AHB_CLK)
#define gccmw_tsens2_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS2_AHB_CLK)
#define gccmw_tsens3_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS3_AHB_CLK)
#define gccmw_tsens4_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS4_AHB_CLK)
#define gccmw_n_gccms_ao_pgm_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_N_GCCMS_AO_PGM_CLK)
#define gccmw_cprc_nvdda_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_NVDDA_AHB_CLK)
#define gccmw_n1_csfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_N1_CSFPB_ALWAYS_ON_CLK)
#define gccmw_n1_csfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_N1_CSFPB_CLK)
#define gccmw_n2_csfpb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_N2_CSFPB_AHB_CLK)
#define gccmw_n_gccms_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_N_GCCMS_AHB_CLK)
#define gccmw_sata_ss_sfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_SS_SFPB_CLK)
#define gccmw_parb0_ser_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_PARB0_SER_CLK)
#define gccmw_parb1_ser_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_PARB1_SER_CLK)
#define gccmw_parb2_ser_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_PARB2_SER_CLK)
#define gccmw_at_w1_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AT_W1_CLK)
#define gccmw_at_w2_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AT_W2_CLK)
#define gccmw_at_w3_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AT_W3_CLK)
#define gccmw_at_w5_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AT_W5_CLK)
#define gccmw_at_w7_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AT_W7_CLK)
#define gccmw_at_w9_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_AT_W9_CLK)
#define gccmw_hmss_at_w_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_HMSS_AT_W_CLK)
#define gccmw_qdss_traceclkin_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_TRACECLKIN_CLK)
#define gccmw_mddr0_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_MDDR0_AHB_CLK)
#define gccmw_mddr1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_MDDR1_AHB_CLK)
#define gccmw_tsens9_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS9_AHB_CLK)
#define gccmw_tsens10_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS10_AHB_CLK)
#define gccmw_s_gccms_ao_pgm_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_S_GCCMS_AO_PGM_CLK)
#define gccmw_cprc_svdda_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CPRC_SVDDA_AHB_CLK)
#define gccmw_s1_csfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_S1_CSFPB_ALWAYS_ON_CLK)
#define gccmw_s1_csfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_S1_CSFPB_CLK)
#define gccmw_s2_csfpb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_S2_CSFPB_AHB_CLK)
#define gccmw_s_gccms_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_S_GCCMS_AHB_CLK)
#define gccmw_sata_ln0_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN0_AHB_CLK)
#define gccmw_sata_ln1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN1_AHB_CLK)
#define gccmw_sata_ln2_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN2_AHB_CLK)
#define gccmw_sata_ln3_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN3_AHB_CLK)
#define gccmw_sata_ln4_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN4_AHB_CLK)
#define gccmw_sata_ln5_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN5_AHB_CLK)
#define gccmw_sata_ln6_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN6_AHB_CLK)
#define gccmw_sata_ln7_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN7_AHB_CLK)
#define gccmw_sata_ln0_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN0_ASIC_CLK)
#define gccmw_sata_ln0_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN0_RX_CLK)
#define gccmw_sata_ln1_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN1_ASIC_CLK)
#define gccmw_sata_ln1_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN1_RX_CLK)
#define gccmw_sata_ln2_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN2_ASIC_CLK)
#define gccmw_sata_ln2_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN2_RX_CLK)
#define gccmw_sata_ln3_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN3_ASIC_CLK)
#define gccmw_sata_ln3_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN3_RX_CLK)
#define gccmw_sata_ln4_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN4_ASIC_CLK)
#define gccmw_sata_ln4_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN4_RX_CLK)
#define gccmw_sata_ln5_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN5_ASIC_CLK)
#define gccmw_sata_ln5_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN5_RX_CLK)
#define gccmw_sata_ln6_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN6_ASIC_CLK)
#define gccmw_sata_ln6_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN6_RX_CLK)
#define gccmw_sata_ln7_asic_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN7_ASIC_CLK)
#define gccmw_sata_ln7_rx_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN7_RX_CLK)
#define gccmw_sata_ln0_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN0_PMALIVE_CLK)
#define gccmw_sata_ln1_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN1_PMALIVE_CLK)
#define gccmw_sata_ln2_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN2_PMALIVE_CLK)
#define gccmw_sata_ln3_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN3_PMALIVE_CLK)
#define gccmw_sata_ln4_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN4_PMALIVE_CLK)
#define gccmw_sata_ln5_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN5_PMALIVE_CLK)
#define gccmw_sata_ln6_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN6_PMALIVE_CLK)
#define gccmw_sata_ln7_pmalive_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN7_PMALIVE_CLK)
#define gccmw_sata_ln0_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN0_AXI_CLK)
#define gccmw_sata_ln1_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN1_AXI_CLK)
#define gccmw_sata_ln2_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN2_AXI_CLK)
#define gccmw_sata_ln3_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN3_AXI_CLK)
#define gccmw_sata_ln4_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN4_AXI_CLK)
#define gccmw_sata_ln5_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN5_AXI_CLK)
#define gccmw_sata_ln6_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN6_AXI_CLK)
#define gccmw_sata_ln7_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN7_AXI_CLK)
#define gccmw_sata_ln0_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN0_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln1_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN1_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln2_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN2_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln3_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN3_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln4_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN4_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln5_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN5_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln6_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN6_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln7_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN7_QSMMUV3_CLIENT_CLK)
#define gccmw_sata_ln0_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN0_RXOOB_CLK)
#define gccmw_sata_ln1_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN1_RXOOB_CLK)
#define gccmw_sata_ln2_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN2_RXOOB_CLK)
#define gccmw_sata_ln3_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN3_RXOOB_CLK)
#define gccmw_sata_ln4_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN4_RXOOB_CLK)
#define gccmw_sata_ln5_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN5_RXOOB_CLK)
#define gccmw_sata_ln6_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN6_RXOOB_CLK)
#define gccmw_sata_ln7_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_LN7_RXOOB_CLK)
#define gccmw_sata_phy0_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_PHY0_RXOOB_CLK)
#define gccmw_sata_phy1_rxoob_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_PHY1_RXOOB_CLK)
#define gccmw_sata_vbu_ahb_cfg_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_VBU_AHB_CFG_CLK)
#define gccmw_sata_vbu_core_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SATA_VBU_CORE_CLK)
#define gccmw_hmss_pbsu4_clk		gccmw_sata_vbu_core_clk
#define gccmw_sec_ctrl_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SEC_CTRL_CLK)
#define gccmw_sec_ctrl_sense_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SEC_CTRL_SENSE_CLK)
#define gccmw_fan_tach_sleep_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_FAN_TACH_SLEEP_CLK)
#define gccmw_im_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_IM_SLEEP_CLK)
#define gccmw_imem_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_IMEM_SLEEP_CLK)
#define gccmw_tsens10_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS10_SLEEP_CLK)
#define gccmw_tsens1_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS1_SLEEP_CLK)
#define gccmw_tsens2_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS2_SLEEP_CLK)
#define gccmw_tsens3_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS3_SLEEP_CLK)
#define gccmw_tsens4_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS4_SLEEP_CLK)
#define gccmw_tsens9_sleep_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS9_SLEEP_CLK)
#define gccmw_ce0_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE0_AHB_CLK)
#define gccmw_sfab_ce0_ahb_clk		gccmw_ce0_ahb_clk
#define gccmw_ce0_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE0_AXI_CLK)
#define gccmw_sfab_ce0_axi_clk		gccmw_ce0_axi_clk
#define gccmw_ce0_iommu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE0_IOMMU_CFG_AHB_CLK)
#define gccmw_ce0_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE0_QSMMUV3_CLIENT_CLK)
#define gccmw_ce1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE1_AHB_CLK)
#define gccmw_sfab_ce1_ahb_clk		gccmw_ce1_ahb_clk
#define gccmw_ce1_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE1_AXI_CLK)
#define gccmw_sfab_ce1_axi_clk		gccmw_ce1_axi_clk
#define gccmw_ce1_iommu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE1_IOMMU_CFG_AHB_CLK)
#define gccmw_ce1_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_CE1_QSMMUV3_CLIENT_CLK)
#define gccmw_emac0_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_AXI_CLK)
#define gccmw_sfab_emac0_axi_clk	gccmw_emac0_axi_clk
#define gccmw_emac0_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC0_QSMMUV3_CLIENT_CLK)
#define gccmw_emac1_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_AXI_CLK)
#define gccmw_sfab_emac1_axi_clk	gccmw_emac1_axi_clk
#define gccmw_emac1_qsmmuv3_client_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC1_QSMMUV3_CLIENT_CLK)
#define gccmw_imem_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_IMEM_AXI_CLK)
#define gccmw_sfab_imem_axi_clk		gccmw_imem_axi_clk
#define gccmw_msg_ram_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_MSG_RAM_AXI_CLK)
#define gccmw_sfab_msg_ram_axi_clk	gccmw_msg_ram_axi_clk
#define gccmw_qdss_etr_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_ETR_AXI_CLK)
#define gccmw_sfab_qdss_etr_axi_clk	gccmw_qdss_etr_axi_clk
#define gccmw_qdss_fclk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_FCLK)
#define gccmw_qdss_stm_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_STM_CLK)
#define gccmw_sfab_qdss_stm_clk		gccmw_qdss_stm_clk
#define gccmw_sfab_ce0_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CE0_FCLK)
#define gccmw_sfab_ce1_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CE1_FCLK)
#define gccmw_sfab_cfab_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CFAB_AXI_CLK)
#define gccmw_sfab_cfab_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CFAB_FCLK)
#define gccmw_sfab_core_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CORE_CLK)
#define gccmw_sfab_emac0_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_EMAC0_FCLK)
#define gccmw_sfab_emac1_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_EMAC1_FCLK)
#define gccmw_sfab_hmss_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_HMSS_FCLK)
#define gccmw_sfab_ce0_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CE0_FCLK)
#define gccmw_sfab_ce1_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CE1_FCLK)
#define gccmw_sfab_imcfab_axi_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_IMCFAB_AXI_CLK)
#define gccmw_sfab_imcfab_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_IMCFAB_FCLK)
#define gccmw_sfab_imem_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_IMEM_FCLK)
#define gccmw_sfab_msg_ram_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_MSG_RAM_FCLK)
#define gccmw_sfab_pfab_axi_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_PFAB_AXI_CLK)
#define gccmw_sfab_pfab_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_PFAB_FCLK)
#define gccmw_sfab_qdss_stm_fclk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_QDSS_STM_FCLK)
#define gccmw_sfab_vbu_fclk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_VBU_FCLK)
#define gccmw_sfvbu_ce0_dvm_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFVBU_CE0_DVM_CLK)
#define gccmw_sfvbu_ce1_dvm_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFVBU_CE1_DVM_CLK)
#define gccmw_sfvbu_emac0_dvm_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFVBU_EMAC0_DVM_CLK)
#define gccmw_sfvbu_emac1_dvm_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFVBU_EMAC1_DVM_CLK)
#define gccmw_sfvbu_periph_dvm_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFVBU_PERIPH_DVM_CLK)
#define gccmw_sfvbu_q22m_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFVBU_Q22M_CLK)
#define gccmw_sfab_sfvbu_q22m_clk	gccmw_sfvbu_q22m_clk
#define gccmw_sfvbu_q22s_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFVBU_Q22S_CLK)
#define gccmw_sfab_sfvbu_q22s_clk	gccmw_sfvbu_q22s_clk
#define gccmw_tic_ahb2ahb_tic_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TIC_AHB2AHB_TIC_CLK)
#define gccmw_sfab_tic_clk		gccmw_tic_ahb2ahb_tic_clk
#define gccmw_sfab_vbu_core_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_VBU_CORE_CLK)
#define gccmw_hmss_pbsu2_clk		gccmw_sfab_vbu_core_clk
#define gccmw_emac_ahb2phy_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_EMAC_AHB2PHY_CFG_AHB_CLK)
#define gccmw_imc_iommu_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_IMC_IOMMU_CFG_AHB_CLK)
#define gccmw_imem_cfg_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_IMEM_CFG_AHB_CLK)
#define gccmw_msg_ram_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_MSG_RAM_CFG_AHB_CLK)
#define gccmw_qdss_cfg_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_CFG_AHB_CLK)
#define gccmw_qdss_dap_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_DAP_AHB_CLK)
#define gccmw_qdss_rbcpr_xpu_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_QDSS_RBCPR_XPU_AHB_CLK)
#define gccmw_s1_csfpb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_S1_CSFPB_AHB_CLK)
#define gccmw_sec_ctrl_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SEC_CTRL_AHB_CLK)
#define gccmw_sfab_cfg_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_CFG_AHB_CLK)
#define gccmw_sfab_vbu_cfg_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_SFAB_VBU_CFG_CLK)
#define gccmw_tsens1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TSENS1_AHB_CLK)
#define gccmw_wcsfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCMW_WCSFPB_ALWAYS_ON_CLK)
#define gccmw_wcsfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCMW_WCSFPB_CLK)
#define gccmw_tic_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCMW_TIC_CLK)
#define gcce_hmss_ne_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_HMSS_NE_AHB_CLK)
#define gcce_hmss_se_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_HMSS_SE_AHB_CLK)
#define gcce_mddr4_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_MDDR4_AHB_CLK)
#define gcce_n2_csfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_N2_CSFPB_ALWAYS_ON_CLK)
#define gcce_n2_csfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_N2_CSFPB_CLK)
#define gcce_tsens5_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS5_AHB_CLK)
#define gcce_pcie_ss_0_vbu_core_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_PCIE_SS_0_VBU_CORE_CLK)
#define gcce_hmss_pbsu0_clk		gcce_pcie_ss_0_vbu_core_clk
#define gcce_pcie_ss_1_vbu_core_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_PCIE_SS_1_VBU_CORE_CLK)
#define gcce_hmss_pbsu1_clk		gcce_pcie_ss_1_vbu_core_clk
#define gcce_at_e1_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_AT_E1_CLK)
#define gcce_at_e2_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_AT_E2_CLK)
#define gcce_at_e3_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_AT_E3_CLK)
#define gcce_at_e5_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_AT_E5_CLK)
#define gcce_at_e7_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_AT_E7_CLK)
#define gcce_at_e9_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_AT_E9_CLK)
#define gcce_hmss_at_e_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_HMSS_AT_E_CLK)
#define gcce_mddr5_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_MDDR5_AHB_CLK)
#define gcce_s2_csfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_S2_CSFPB_ALWAYS_ON_CLK)
#define gcce_s2_csfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_S2_CSFPB_CLK)
#define gcce_seccsfpb_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_SECCSFPB_AHB_CLK)
#define gcce_tsens8_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS8_AHB_CLK)
#define gcce_ahb_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_AHB_CLK)
#define gcce_pcie_ss_0_secsfpb_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_PCIE_SS_0_SECSFPB_AHB_CLK)
#define gcce_pcie_ss_1_secsfpb_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_PCIE_SS_1_SECSFPB_AHB_CLK)
#define gcce_psclkgen_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_PSCLKGEN_CFG_AHB_CLK)
#define gcce_secsfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, GCCE_SECSFPB_ALWAYS_ON_CLK)
#define gcce_secsfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_SECSFPB_CLK)
#define gcce_tsens6_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS6_AHB_CLK)
#define gcce_tsens7_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS7_AHB_CLK)
#define gcce_xo_clk			\
	clkid(CLK_BRANCH_CLOCK, GCCE_XO_CLK)
#define gcce_tsens5_ext_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS5_EXT_CLK)
#define gcce_tsens6_ext_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS6_EXT_CLK)
#define gcce_tsens7_ext_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS7_EXT_CLK)
#define gcce_tsens8_ext_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_TSENS8_EXT_CLK)
#define gcce_xo_div4_clk		\
	clkid(CLK_BRANCH_CLOCK, GCCE_XO_DIV4_CLK)
#define pcie_cc_0_pcie_x16_aux_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X16_AUX_CLK)
#define pcie_cc_0_pcie_x8_0_aux_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_0_AUX_CLK)
#define pcie_cc_0_pcie_x8_1_aux_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_1_AUX_CLK)
#define pcie_cc_0_phy_a_aux_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PHY_A_AUX_CLK)
#define pcie_cc_0_phy_b_aux_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PHY_B_AUX_CLK)
#define pcie_cc_0_phy_c_aux_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PHY_C_AUX_CLK)
#define pcie_cc_0_pcie_a_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_A_PCLK)
#define pcie_cc_0_pcie_x16_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X16_PCLK)
#define pcie_cc_0_pcie_x8_0_pclk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_0_PCLK)
#define pcie_cc_0_pcie_b_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_PCIE_B_PCLK)
#define pcie_cc_0_pcie_x8_1_pclk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_1_PCLK)
#define pcie_cc_0_pcie_c_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_C_PCLK)
#define pcie_cc_0_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_AHB_CLK)
#define pcie_cc_0_pcie_phy_aa_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_PHY_AA_CFG_AHB_CLK)
#define pcie_cc_0_pcie_phy_ab_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_PHY_AB_CFG_AHB_CLK)
#define pcie_cc_0_pcie_phy_b_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_PHY_B_CFG_AHB_CLK)
#define pcie_cc_0_pcie_phy_c_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_PHY_C_CFG_AHB_CLK)
#define pcie_cc_0_pcie_sfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_SFPB_ALWAYS_ON_CLK)
#define pcie_cc_0_pcie_sfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_SFPB_CLK)
#define pcie_cc_0_pcie_x16_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X16_AHB_CLK)
#define pcie_cc_0_pcie_x16_mmu_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X16_MMU_AHB_CLK)
#define pcie_cc_0_pcie_x8_0_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_0_AHB_CLK)
#define pcie_cc_0_pcie_x8_0_mmu_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_0_MMU_AHB_CLK)
#define pcie_cc_0_pcie_x8_1_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_1_AHB_CLK)
#define pcie_cc_0_pcie_x8_1_mmu_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_1_MMU_AHB_CLK)
#define pcie_cc_0_vbu_ahb_cfg_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_VBU_AHB_CFG_CLK)
#define pcie_cc_0_pcie_x16_mmu_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X16_MMU_MSTR_Q23_CLK)
#define pcie_cc_0_pcie_x16_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X16_MSTR_Q23_CLK)
#define pcie_cc_0_pcie_x16_slv_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X16_SLV_Q23_CLK)
#define pcie_cc_0_pcie_x8_0_mmu_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_0_MMU_MSTR_Q23_CLK)
#define pcie_cc_0_pcie_x8_0_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_0_MSTR_Q23_CLK)
#define pcie_cc_0_pcie_x8_1_mmu_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_1_MMU_MSTR_Q23_CLK)
#define pcie_cc_0_pcie_x8_1_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_1_MSTR_Q23_CLK)
#define pcie_cc_0_pcie_x8_0_slv_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_0_SLV_Q23_CLK)
#define pcie_cc_0_pcie_x8_1_slv_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_0_PCIE_X8_1_SLV_Q23_CLK)
#define pcie_cc_1_pcie_x8_0_aux_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_0_AUX_CLK)
#define pcie_cc_1_pcie_x8_1_aux_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_1_AUX_CLK)
#define pcie_cc_1_phy_a_aux_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PHY_A_AUX_CLK)
#define pcie_cc_1_phy_b_aux_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PHY_B_AUX_CLK)
#define pcie_cc_1_phy_c_aux_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PHY_C_AUX_CLK)
#define pcie_cc_1_pcie_a_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_A_PCLK)
#define pcie_cc_1_pcie_x16_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X16_PCLK)
#define pcie_cc_1_pcie_x8_0_pclk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_0_PCLK)
#define pcie_cc_1_pcie_b_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_B_PCLK)
#define pcie_cc_1_pcie_x8_1_pclk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_1_PCLK)
#define pcie_cc_1_pcie_c_pclk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_C_PCLK)
#define pcie_cc_1_ahb_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_AHB_CLK)
#define pcie_cc_1_pcie_phy_aa_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_PHY_AA_CFG_AHB_CLK)
#define pcie_cc_1_pcie_phy_ab_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_PHY_AB_CFG_AHB_CLK)
#define pcie_cc_1_pcie_phy_b_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_PHY_B_CFG_AHB_CLK)
#define pcie_cc_1_pcie_phy_c_cfg_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_PHY_C_CFG_AHB_CLK)
#define pcie_cc_1_pcie_sfpb_always_on_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_SFPB_ALWAYS_ON_CLK)
#define pcie_cc_1_pcie_sfpb_clk		\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_SFPB_CLK)
#define pcie_cc_1_pcie_x16_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X16_AHB_CLK)
#define pcie_cc_1_pcie_x16_mmu_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X16_MMU_AHB_CLK)
#define pcie_cc_1_pcie_x8_0_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_0_AHB_CLK)
#define pcie_cc_1_pcie_x8_0_mmu_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_0_MMU_AHB_CLK)
#define pcie_cc_1_pcie_x8_1_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_1_AHB_CLK)
#define pcie_cc_1_pcie_x8_1_mmu_ahb_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_1_MMU_AHB_CLK)
#define pcie_cc_1_vbu_ahb_cfg_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_VBU_AHB_CFG_CLK)
#define pcie_cc_1_pcie_x16_mmu_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X16_MMU_MSTR_Q23_CLK)
#define pcie_cc_1_pcie_x16_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X16_MSTR_Q23_CLK)
#define pcie_cc_1_pcie_x16_slv_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X16_SLV_Q23_CLK)
#define pcie_cc_1_pcie_x8_0_mmu_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_0_MMU_MSTR_Q23_CLK)
#define pcie_cc_1_pcie_x8_0_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_0_MSTR_Q23_CLK)
#define pcie_cc_1_pcie_x8_1_mmu_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_1_MMU_MSTR_Q23_CLK)
#define pcie_cc_1_pcie_x8_1_mstr_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_1_MSTR_Q23_CLK)
#define pcie_cc_1_pcie_x8_0_slv_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_0_SLV_Q23_CLK)
#define pcie_cc_1_pcie_x8_1_slv_q23_clk	\
	clkid(CLK_BRANCH_CLOCK, PCIE_CC_1_PCIE_X8_1_SLV_Q23_CLK)
#define NR_BRANCH_CLKS		(PCIE_CC_1_PCIE_X8_1_SLV_Q23_CLK + 1)

#include <asm/mach/gccmw.h>
#include <asm/mach/gcce.h>
#include <asm/mach/pcc.h>
#include <asm/mach/imccc.h>
#include <asm/mach/gccms.h>
#include <asm/mach/pciecc.h>
#include <asm/mach/clk_ctl.h>
#include <asm/mach/fabia.h>
#include <asm/mach/agera.h>

typedef struct clk_mux {
	clk_t output_clk;
	clk_t input_clk;
} clk_mux_t;

typedef struct freqplan_cfg {
	uint8_t major;
	uint8_t minor;
	caddr_t *cbcr;
	clk_mux_t *srcs;
	clk_t *clks;
} __packed freqplan_t;

#define CPU_BOOT_FREQ_MHZ		600
#define CBF_BOOT_FREQ_MHZ		600

int freqplan_get_plan(void);
freqplan_t *freqplan_get(void);
void clk_hw_ctrl_init(void);

#endif /* __CLK_QDF2400_H_INCLUDE__ */
