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
 * @(#)dw_pll5ghz_tsmc12ffc.c: DWC PLL5GHz TSMC12FFC implementation
 * $Id: dw_pll5ghz_tsmc12ffc.c,v 1.1 2019-11-14 09:12:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/delay.h>
#include <target/panic.h>
#include <target/bitops.h>
#include <target/barrier.h>
#include <target/cmdline.h>
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_ACCEL
#define dw_pll5ghz_tsmc12ffc_delay(us)	wmb()
#else
#define dw_pll5ghz_tsmc12ffc_delay(us)	udelay(us)
#endif

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_GEAR
static void dw_pll5ghz_tsmc12ffc_gear(uint8_t pll)
{
	/* The PLL only leaves the transition states and gets to normal
	 * operation after gear_shift is set to low and lock flag is
	 * asserted high. During the Fast Startup transition state the
	 * gear_shift must be set to high during 1 minimum of 100 loops
	 * comparison clock cycles or 3us.
	 * NOTE: 3us includes 1us preset duration.
	 */
	/* t_prst: preset */
	udelay(1);
	/* t_gs: gearshift */
	udelay(2);
	__raw_clearl(PLL_GEAR_SHIFT, DW_PLL_CFG1(pll));
}
#else
#define dw_pll5ghz_tsmc12ffc_gear(pll)	do { } while (0)
#endif

void dw_pll5ghz_tsmc12ffc_standby(uint8_t pll)
{
	__raw_writel(PLL_STANDBY | PLL_PWRON, DW_PLL_CFG1(pll));
	while (!(__raw_readl(DW_PLL_STATUS(pll)) & PLL_STANDBYEFF));
}

void dw_pll5ghz_tsmc12ffc_relock(uint8_t pll)
{
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_STANDBYEFF) {
		__raw_writel_mask(PLL_RESET | PLL_PWRON | PLL_GEAR_SHIFT,
				  PLL_RESET | PLL_STANDBY |
				  PLL_PWRON | PLL_GEAR_SHIFT,
				  DW_PLL_CFG1(pll));
		dw_pll5ghz_tsmc12ffc_gear(pll);
		while (!(__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED));

		/* P/R outputs:
		 *  1'b0: Fclkout = 0 or Fclkref/(P|R)
		 *  1'b1: Fclkout = PLL output
		 */
		__raw_setl(PLL_ENP, DW_PLL_CFG1(pll));
		__raw_setl(PLL_ENR, DW_PLL_CFG1(pll));
	}
}

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_PRSTDUR
static void dw_pll5ghz_tsmc12ffc_prstdur(uint8_t pll)
{
	uint8_t prstdur;

	prstdur = __fls8(div32u(DW_PLL_REFCLK_FREQ, 1000000) - 1) - 1;
	dw_pll_write(pll, PLL_ANAREG07, PLL_PRSTDUR(prstdur));
}
#else
#define dw_pll5ghz_tsmc12ffc_prstdur(pll)	do { } while (0)
#endif

void dw_pll5ghz_tsmc12ffc_pwron(uint8_t pll, uint64_t fvco)
{
	uint16_t mint, mfrac;
	uint8_t prediv = 0;
	uint32_t cfg = PLL_RANGE3;
	uint64_t fbdiv;

	if (fvco <= ULL(3750000000))
		cfg = PLL_RANGE1;
	else if (fvco > ULL(3750000000) && fvco <= ULL(4000000000))
		cfg = PLL_RANGE2;
	else if (fvco > ULL(4000000000) && fvco <= ULL(5000000000))
		cfg = PLL_RANGE23;
	else if (fvco <= ULL(6000000000))
		cfg = PLL_RANGE3;
	else
		BUG();

	do {
		prediv++;
		BUG_ON(prediv > 32);
		fbdiv = div64u(fvco << 16,
			       div32u(DW_PLL_REFCLK_FREQ, prediv));
		mint = HIWORD(fbdiv);
		mfrac = LOWORD(fbdiv);
	} while (mint < 16);
	__raw_writel(PLL_PREDIV(prediv - 1) |
		     PLL_MINT(mint - 16) | PLL_MFRAC(mfrac),
		     DW_PLL_CFG0(pll));

	cfg |= PLL_GEAR_SHIFT;
	__raw_writel(cfg, DW_PLL_CFG1(pll));
	/* t_pwrstb */
	dw_pll5ghz_tsmc12ffc_delay(1);
	/* ndelay(500); */
	cfg |= PLL_TEST_RESET;
	__raw_writel(cfg, DW_PLL_CFG1(pll));
	/* t_trst */
	dw_pll5ghz_tsmc12ffc_delay(1);
	/* ndelay(50); */
	cfg |= PLL_PWRON;
	__raw_writel(cfg, DW_PLL_CFG1(pll));
	/* XXX: t_pwron
	 *      In the databook, PWRON and RST_N seem to be set
	 *      simultaneously, while the IP complains that PWRON must
	 *      be enabled before setting RST_N. And the process takes
	 *      time, otherwise, RST_N setting and GEAR_SHIFT unsetting
	 *      can go wrong.
	 */
	dw_pll5ghz_tsmc12ffc_delay(1);
	cfg |= PLL_RESET;
	__raw_writel(cfg, DW_PLL_CFG1(pll));
	dw_pll5ghz_tsmc12ffc_gear(pll);
	while (!(__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED));

	/* P/R outputs:
	 *  1'b0: Fclkout = 0 or Fclkref/(P|R)
	 *  1'b1: Fclkout = PLL output
	 */
	__raw_setl(PLL_ENP, DW_PLL_CFG1(pll));
	__raw_setl(PLL_ENR, DW_PLL_CFG1(pll));
}

void dw_pll5ghz_tsmc12ffc_pwrdn(uint8_t pll)
{
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_STANDBYEFF)
		dw_pll5ghz_tsmc12ffc_relock(pll);
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED) {
		__raw_clearl(PLL_PWRON, DW_PLL_CFG1(pll));
		while (__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED);
	}
}

void dw_pll5ghz_tsmc12ffc_bypass(uint8_t pll, uint8_t mode)
{
	switch (mode) {
	case PLL_BYPASS_CORE:
		__raw_setl(PLL_BYPASS, DW_PLL_CFG1(pll));
		dw_pll_write(pll, PLL_ANAREG05, 0);
		break;
	case PLL_BYPASS_ALL:
		__raw_setl(PLL_BYPASS, DW_PLL_CFG1(pll));
		dw_pll_write(pll, PLL_ANAREG05, PLL_TEST_BYPASS);
		break;
	case PLL_BYPASS_NONE:
	default:
		__raw_clearl(PLL_BYPASS, DW_PLL_CFG1(pll));
		break;
	}
}

static int do_pll_reg_access(int argc, char * argv[])
{
	int pll;
	uint8_t reg;
	uint8_t val;

	if (argc < 4)
		return -EINVAL;

	pll = strtoul(argv[2], NULL, 0);
	reg = strtoul(argv[3], NULL, 0);
	if (argc > 4) {
		val = strtoul(argv[4], NULL, 0);
		dw_pll_write(pll, reg, val);
		printf("done.\n");
		return 0;
	} else {
		val = dw_pll_read(pll, reg);
		return mem_print_data(reg, &val, 1, 1);
	}
}

static int do_pll_operation(int argc, char * argv[])
{
	int pll;
	uint32_t freq;

	if (argc < 3)
		return -EINVAL;

	pll = strtoul(argv[2], NULL, 0);
	if (strcmp(argv[1], "up")) {
		if (argc < 4)
			return -EINVAL;
		freq = strtoul(argv[3], NULL, 0);
		dw_pll5ghz_tsmc12ffc_pwron(pll, freq);
	}
	if (strcmp(argv[1], "down"))
		dw_pll5ghz_tsmc12ffc_pwrdn(pll);
	if (strcmp(argv[1], "standby"))
		dw_pll5ghz_tsmc12ffc_standby(pll);
	if (strcmp(argv[1], "relock"))
		dw_pll5ghz_tsmc12ffc_relock(pll);
	return -ENODEV;
}

static int do_pll(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "access") == 0)
		return do_pll_reg_access(argc, argv);
	return do_pll_operation(argc, argv);
}

DEFINE_COMMAND(pll, do_pll, "Control DWC PLL5GHz TSMC12FFCNS",
	"pll access pll reg\n"
	"    -read pll register\n"
	"pll access pll reg val\n"
	"    -write pll register\n"
	"pll up pll freq\n"
	"    -power up pll with specified frequency\n"
	"pll down pll\n"
	"    -power down pll\n"
	"pll standby pll\n"
	"    -put pll into standby state\n"
	"pll relock pll\n"
	"    -get pll out of standby state\n"
	"\n"
);
