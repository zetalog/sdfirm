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
 * @(#)cru.h: DPU-LP clock/reset unit controller definitions
 * $Id: cru.h,v 1.1 2021-11-15 14:18:00 zhenglv Exp $
 */

#ifndef __CRU_DPULP_H_INCLUDE__
#define __CRU_DPULP_H_INCLUDE__

#include <target/arch.h>

#define CRU_REG(offset)			(CRU_BASE + (offset))
#define CRU_1BIT_REG(off, n)		REG_1BIT_ADDR(CRU_BASE+(off), n)

#include <asm/mach/pwr.h>
#include <asm/mach/pll.h>

/* CLK_CFG/RESET register */
#define CRU_CLK_CFG(n)			CRU_1BIT_REG(0x400, n)
#define CRU_RESET(n)			CRU_1BIT_REG(0x500, n)

/* CRU_1BIT_REG bits encodings */
#define CRU_B_OFFSET			0
#define CRU_B_MASK			REG_5BIT_MASK
#define CRU_B(bit)			_SET_FV(CRU_B, bit)
#define cru_b(cfg)			_GET_FV(CRU_B, cfg)
#define CRU_R_OFFSET			5
#define CRU_R_MASK			REG_10BIT_MASK
#define CRU_R(reg)			_SET_FV(CRU_R, reg)
#define cru_r(cfg)			_GET_FV(CRU_R, cfg)
#define CRU_T				_BV(15)
#define CRU_CFG(r, reg, bit)		\
	(((r) ? CRU_T : 0) | CRU_R(reg) | CRU_B(bit))
#define CRU_MSK(cfg)			_BV(cru_b(cfg))
#define CRU_BIT(cfg)			((cru_r(cfg) << 5) + cru_b(cfg))
#define CRU_ADR(cfg)			\
	(((cfg) & CRU_T) ?		\
	 CRU_RESET(CRU_BIT(cfg)) :	\
	 CRU_CLK_CFG(CRU_BIT(cfg)))
#define CRU_FLD(cfg)			REG_1BIT_OFFSET(CRU_BIT(cfg))

/* 3.4.17 CPU_CLK_CFG */
#define CRU_cpu_clksel			CRU_CFG(0, 0, 0)
/* 3.4.18 GPDPU_CLK_CFG */
#define CRU_swallow_bypass		CRU_CFG(0, 0, 1)
#define CRU_gpdpu_bus_clksel		CRU_CFG(0, 1, 0)
#define CRU_gpdpu_core_clksel		CRU_CFG(0, 1, 1)
#define CRU_gpdpu_clken			CRU_CFG(0, 1, 2)
#define CRU_bus_swallow_bypass		CRU_CFG(0, 1, 3)
#define CRU_core_swallow_bypass		CRU_CFG(0, 1, 4)
/* 3.4.19 DDR_CLK_CFG */
#define CRU_ddr_clksel			CRU_CFG(0, 2, 0)
#define CRU_ddr0_clken			CRU_CFG(0, 2, 1)
#define CRU_ddr1_clken			CRU_CFG(0, 2, 2)
#define CRU_ddr_bypassPclken		CRU_CFG(0, 2, 3)
/* 3.4.20 SoC_CLK_CFG */
#define CRU_soc_800_clksel		CRU_CFG(0, 3, 0)
#define CRU_soc_400_clksel		CRU_CFG(0, 3, 1)
#define CRU_soc_200_clksel		CRU_CFG(0, 3, 2)
#define CRU_soc_100_clksel		CRU_CFG(0, 3, 3)
/* 3.4.21 VPU_CLK_CFG */
#define CRU_vpu_cclksel			CRU_CFG(0, 4, 0)
#define CRU_vpu_bclksel			CRU_CFG(0, 4, 1)
#define CRU_vpu0_clken			CRU_CFG(0, 4, 2)
#define CRU_vpu1_clken			CRU_CFG(0, 4, 3)
#define CRU_vpu0_cclk_swallow_bypass	CRU_CFG(0, 4, 4)
#define CRU_vpu1_cclk_swallow_bypass	CRU_CFG(0, 4, 5)
#define CRU_vpu0_bclk_swallow_bypass	CRU_CFG(0, 4, 6)
#define CRU_vpu1_bclk_swallow_bypass	CRU_CFG(0, 4, 7)
/* 3.4.22 RAB_PHY_CLK_CFG */
#define CRU_rab0_phy_clksel		CRU_CFG(0, 5, 0)
#define CRU_rab1_phy_clksel		CRU_CFG(0, 5, 1)
#define CRU_rab0_clken			CRU_CFG(0, 5, 2)
#define CRU_rab1_clken			CRU_CFG(0, 5, 3)
/* 3.4.23 PCIE_CLK_CFG */
#define CRU_pcie_clken			CRU_CFG(0, 6, 0)
/* 3.4.24 CPU_RESET */
#define CRU_cpu_func_reset		CRU_CFG(1, 0, 0)
#define CRU_cpu_dbg_reset		CRU_CFG(1, 0, 1)
/* 3.4.25 GPDPU_RESET */
#define CRU_gpdpu_reset			CRU_CFG(1, 1, 0)
/* 3.4.26 DDR_RESET */
#define CRU_ddr0_pwrokin		CRU_CFG(1, 2, 0)
#define CRU_ddr0_apb_reset		CRU_CFG(1, 2, 1)
#define CRU_ddr0_axi_reset		CRU_CFG(1, 2, 2)
#define CRU_ddr0_reset			CRU_CFG(1, 2, 3)
#define CRU_ddr1_pwrokin		CRU_CFG(1, 2, 4)
#define CRU_ddr1_apb_reset		CRU_CFG(1, 2, 5)
#define CRU_ddr1_axi_reset		CRU_CFG(1, 2, 6)
#define CRU_ddr1_reset			CRU_CFG(1, 2, 7)
/* 3.4.27 VPU_RESET */
#define CRU_vpu_reset			CRU_CFG(1, 3, 0)
/* 3.4.28 PCIE_RESET */
#define CRU_pcie_pwr_up_reset		CRU_CFG(1, 4, 0)
/* 3.4.29 RAB_RESET */
#define CRU_rab0_reset			CRU_CFG(1, 5, 0)
#define CRU_rab0_rio_reset		CRU_CFG(1, 5, 1)
#define CRU_rab0_rio_logic_reset	CRU_CFG(1, 5, 2)
#define CRU_rab1_reset			CRU_CFG(1, 5, 4)
#define CRU_rab1_rio_reset		CRU_CFG(1, 5, 5)
#define CRU_rab1_rio_logic_reset	CRU_CFG(1, 5, 6)
/* 3.4.30 ETH_RESET */
#define CRU_eth0_reset			CRU_CFG(1, 6, 0)
#define CRU_eth1_reset			CRU_CFG(1, 6, 1)
/* 3.4.31 GPIO_RESET */
#define CRU_gpio_reset			CRU_CFG(1, 7, 0)
/* 3.4.32 SSI_RESET */
#define CRU_ssi_reset			CRU_CFG(1, 8, 0)
/* 3.4.33 FLASH_RESET */
#define CRU_flash_reset			CRU_CFG(1, 9, 0)
/* 3.4.34 I2C_RESET */
#define CRU_i2c0_reset			CRU_CFG(1, 10, 0)
#define CRU_i2c1_reset			CRU_CFG(1, 10, 1)
#define CRU_i2c2_reset			CRU_CFG(1, 10, 2)
#define CRU_i2c3_reset			CRU_CFG(1, 10, 3)
/* 3.4.35 UART_RESET */
#define CRU_uart0_reset			CRU_CFG(1, 11, 0)
#define CRU_uart1_reset			CRU_CFG(1, 11, 1)
#define CRU_uart2_reset			CRU_CFG(1, 11, 2)
#define CRU_uart3_reset			CRU_CFG(1, 11, 3)
/* 3.4.36 SD_RESET */
#define CRU_sd_reset			CRU_CFG(1, 12, 0)
/* 3.4.37 PLIC_RESET */
#define CRU_plic_reset			CRU_CFG(1, 13, 0)
/* 3.4.38 TMR_RESET */
#define CRU_tmr_reset			CRU_CFG(1, 14, 0)
/* 3.4.39 WDT_RESET */
#define CRU_wdt_reset			CRU_CFG(1, 15, 0)
/* 3.4.40 TSENSOR_RESET */
#define CRU_aon_tsensor_reset		CRU_CFG(1, 16, 0)
#define CRU_gpdpu_tsensor_reset		CRU_CFG(1, 16, 1)
#define CRU_ddr0_tsensor_reset		CRU_CFG(1, 16, 2)
#define CRU_ddr1_tsensor_reset		CRU_CFG(1, 16, 3)
#define CRU_rab0_tsensor_reset		CRU_CFG(1, 16, 4)
#define CRU_rab1_tsensor_reset		CRU_CFG(1, 16, 5)
#define CRU_eth0_tsensor_reset		CRU_CFG(1, 16, 6)
#define CRU_eth1_tsensor_reset		CRU_CFG(1, 16, 7)
/* 3.4.41 TCSR_RESET */
#define CRU_tcsr_reset			CRU_CFG(1, 17, 0)

#define cru_set_bit(cfg)		\
	__raw_setl(CRU_FLD(cfg), CRU_ADR(cfg))
#define cru_clear_bit(cfg)		\
	__raw_clearl(CRU_FLD(cfg), CRU_ADR(cfg))
#define cru_test_bit(cfg)		\
	(__raw_readl(CRU_ADR(cfg)) & CRU_FLD(cfg))

#define cru_clk_enable(cfg)		cru_set_bit(cfg)
#define cru_clk_disable(cfg)		cru_clear_bit(cfg)
#define cru_clk_enabled(cfg)		cru_test_bit(cfg)
#define cru_clk_disabled(cfg)		(!cru_test_bit(cfg))
#define cru_clk_select(cfg)		cru_clear_bit(cfg)
#define cru_clk_deselect(cfg)		cru_set_bit(cfg)
#define cru_clk_selected(cfg)		(!cru_test_bit(cfg))
#define cru_clk_deselected(cfg)		cru_test_bit(cfg)
#define cru_clk_swallow(cfg)		cru_clear_bit(cfg)
#define cru_clk_deswallow(cfg)		cru_set_bit(cfg)
#define cru_rst_assert(cfg)		cru_set_bit(cfg)
#define cru_rst_deassert(cfg)		cru_clear_bit(cfg)

#ifdef CONFIG_CRU_TRACE
void cru_trace(bool enabling, const char *name);
#else /* CONFIG_CRU_TRACE */
#define cru_trace(enabling, name)	do { } while (0)
#endif /* CONFIG_CRU_TRACE */

#endif /* __CRU_DPULP_H_INCLUDE__ */
