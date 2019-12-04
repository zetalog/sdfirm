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
 * @(#)ccu.c: SUNXI clock control unit (CCU) implementation
 * $Id: ccu.c,v 1.1 2019-11-17 20:38:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/clk.h>
#include <target/delay.h>
#include <target/barrier.h>

void clock_init_safe(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)CCU_BASE;

	clock_set_pll1(408000000);

	__raw_writel(PLL6_CFG_DEFAULT, &ccm->pll6_cfg);
	while (!(__raw_readl(&ccm->pll6_cfg) & CCM_PLL6_CTRL_LOCK));

	__raw_writel(AHB1_ABP1_DIV_DEFAULT, &ccm->ahb1_apb1_div);
	__raw_writel(MBUS_CLK_DEFAULT, &ccm->mbus0_clk_cfg);
}

void clock_init_sec(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)CCU_BASE;
	struct sunxi_prcm_reg *const prcm =
		(struct sunxi_prcm_reg *)PRCM_BASE;

	__raw_setl(CCM_SEC_SWITCH_MBUS_NONSEC |
		   CCM_SEC_SWITCH_BUS_NONSEC |
		   CCM_SEC_SWITCH_PLL_NONSEC,
		   &ccm->ccu_sec_switch);
	__raw_setl(PRCM_SEC_SWITCH_APB0_CLK_NONSEC |
		   PRCM_SEC_SWITCH_PLL_CFG_NONSEC |
		   PRCM_SEC_SWITCH_PWR_GATE_NONSEC,
		   &prcm->prcm_sec_switch);
}

void clock_init_uart(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)CCU_BASE;

	/* uart clock source is apb2 */
	__raw_writel(APB2_CLK_SRC_OSC24M | APB2_CLK_RATE_N_1 |
		     APB2_CLK_RATE_M(1), &ccm->apb2_div);

	/* open the clock for uart */
	__raw_setl(CLK_GATE_OPEN <<
		   (APB2_GATE_UART_SHIFT + UART_CON_ID),
		   &ccm->apb2_gate);

	/* deassert uart reset */
	__raw_setl(1 << (APB2_RESET_UART_SHIFT + UART_CON_ID),
		   &ccm->apb2_reset_cfg);
}

void clock_set_pll1(unsigned int clk)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)CCU_BASE;
	const int p = 0;
	int k = 1;
	int m = 1;

	if (clk >= 1368000000) {
		k = 3;
	} else if (clk >= 768000000) {
		k = 2;
	}

	/* Switch to 24MHz clock while changing PLL1 */
	__raw_writel(AXI_DIV_3 << AXI_DIV_SHIFT |
		     ATB_DIV_2 << ATB_DIV_SHIFT |
		     CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT,
		     &ccm->cpu_axi_cfg);

	/*
	 * sun6i: PLL1 rate = ((24000000 * n * k) >> 0) / m   (p is ignored)
	 * sun8i: PLL1 rate = ((24000000 * n * k) >> p) / m
	 */
	__raw_writel(CCM_PLL1_CTRL_EN | CCM_PLL1_CTRL_P(p) |
		     CCM_PLL1_CTRL_N(clk / (24000000 * k / m)) |
		     CCM_PLL1_CTRL_K(k) | CCM_PLL1_CTRL_M(m),
		     &ccm->pll1_cfg);
	wmb();
	__delay(200);

	/* Switch CPU to PLL1 */
	__raw_writel(AXI_DIV_3 << AXI_DIV_SHIFT |
		     ATB_DIV_2 << ATB_DIV_SHIFT |
		     CPU_CLK_SRC_PLL1 << CPU_CLK_SRC_SHIFT,
		     &ccm->cpu_axi_cfg);
}

unsigned int clock_get_pll1(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)CCU_BASE;
	unsigned int pll1_cfg;
	int k, n;

	pll1_cfg = __raw_readl(&ccm->pll1_cfg);

	k = (pll1_cfg >>  4) & 0x3;
	n = (pll1_cfg >>  8) & 0x1f;

	return (24000000 * (n+1) * (k+1));
}

unsigned int clock_get_pll6(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)CCU_BASE;

	uint32_t rval = __raw_readl(&ccm->pll6_cfg);
	int n = ((rval & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT) + 1;
	int k = ((rval & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;
	return 24000000 * n * k / 2;
}
