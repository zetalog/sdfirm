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
 * @(#)dw_pll5ghz.c: DesignWare 5GHz PLL (TSMC 12FFC) implementation
 * $Id: dw_pll5ghz.c,v 1.1 2019-11-14 09:12:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/delay.h>
#include <target/panic.h>

void dwc_pll5ghz_tmffc12_enable(uint8_t pll, uint64_t fvco)
{
	uint16_t mint, mfrac;
	uint8_t prediv = 0;
	uint32_t vco_cfg;
	uint64_t fbdiv;

	if (fvco <= ULL(3750000000))
		vco_cfg = PLL_VCO_MODE | PLL_LOWFREQ;
	else if (fvco >= ULL(4000000000))
		vco_cfg = PLL_LOWFREQ;
	else
		vco_cfg = PLL_VCO_MODE;

	do {
		prediv++;
		BUG_ON(prediv > 32);
		fbdiv = div64u(fvco << 16,
			       div32u(DW_PLL5GHZ_REFCLK_FREQ, prediv));
		mint = HIWORD(fbdiv);
		mfrac = LOWORD(fbdiv);
	} while (mint < 16);
	__raw_writel(PLL_PREDIV(prediv - 1) |
		     PLL_MINT(mint - 16) | PLL_MFRAC(mfrac),
		     DW_PLL5GHZ_CFG0(pll));

	udelay(2);
	__raw_writel(vco_cfg | PLL_GEAR_SHIFT, DW_PLL5GHZ_CFG1(pll));
	udelay(2);

	/* step1: test_rst_n */
	__raw_setl(PLL_TEST_RESET, DW_PLL5GHZ_CFG1(pll));
	udelay(2);
	/* step2: pwron & rst_n */
	__raw_setl(PLL_PWRON, DW_PLL5GHZ_CFG1(pll));
	udelay(2);
	__raw_setl(PLL_RESET, DW_PLL5GHZ_CFG1(pll));
	/* step3: preset */
	udelay(7);
	/* step4: gearshift */
	__raw_clearl(PLL_GEAR_SHIFT, DW_PLL5GHZ_CFG1(pll));
	/* step5: spo */
	udelay(2);
	/* step6: enp/enr */
	__raw_setl(PLL_ENP, DW_PLL5GHZ_CFG1(pll));
	__raw_setl(PLL_ENR, DW_PLL5GHZ_CFG1(pll));
	/* step7: lock */
	while (__raw_readl(DW_PLL5GHZ_STATUS(pll)) != PLL_LOCKED);
}

void dwc_pll5ghz_tmffc12_disable(uint8_t pll)
{
	/* NYI: we don't use this function now */
}
