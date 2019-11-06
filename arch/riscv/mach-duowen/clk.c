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
 * @(#)clk.c: DUOWEN clock controller implementation
 * $Id: clk.c,v 1.1 2019-11-06 10:39:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/delay.h>

void pll_enable(uint8_t pll, uint8_t mint)
{
	__raw_writel(PLL_MINT(mint), CRCNTL_PLLDIV(pll));
	udelay(2);
	__raw_writel(PLL_VCO_MODE | PLL_LOWFREQ | PLL_GEAR_SHIFT,
		     CRCNTL_PLLCFG1(pll));
	udelay(2);

	/* step1: test_rst_n */
	__raw_setl(PLL_TEST_RESET, CRCNTL_PLLCFG1(pll));
	udelay(2);
	/* step2: pwron & rst_n */
	__raw_setl(PLL_PWRON, CRCNTL_PLLCFG1(pll));
	udelay(2);
	__raw_setl(PLL_RESET, CRCNTL_PLLCFG1(pll));
	/* step3: preset */
	udelay(7);
	/* step4: gearshift */
	__raw_clearl(PLL_GEAR_SHIFT, CRCNTL_PLLCFG1(pll));
	/* step5: spo */
	udelay(2);
	/* step6: enp/enr */
	__raw_setl(PLL_ENP, CRCNTL_PLLCFG1(pll));
	__raw_setl(PLL_ENR, CRCNTL_PLLCFG1(pll));
	/* step7: lock */
	while (__raw_readl(CRCNTL_PLLSTS(pll)) != PLL_LOCK);
}

#ifdef CONFIG_DUOWEN_CLK_TEST
void clk_hw_ctrl_init(void)
{
	/* TODO: wait imc_rst_n */
	sfab_power_up();
	/* TODO: wait PS_HOLD */
	ddrpll_enable();
	socpll_enable();
	pll_select();
	enable_all_clocks();
}
#else
void clk_hw_ctrl_init(void)
{
}
#endif
