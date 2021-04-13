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

#define PLL_FMT_PR(r)		((r) ? 'r' : 'p')

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_TRACE
#define dw_pll_trace(...)	con_dbg(__VA_ARGS__)
#else
#define dw_pll_trace(...)	do { } while (0)
#endif

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_PRSTDUR
static void dw_pll5ghz_tsmc12ffc_prstdur(uint8_t pll)
{
	uint8_t prstdur;
	uint32_t freq = DW_PLL_F_REFCLK(pll);

	if (freq < 4000000)
		freq = 4000000;
	prstdur = __fls8(__roundup32(div32u(freq + 999999, 1000000))) - 2;
	dw_pll_write(pll, PLL_ANAREG07, PLL_PRSTDUR(prstdur));
}
#else
#define dw_pll5ghz_tsmc12ffc_prstdur(pll)	do { } while (0)
#endif

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_SOC_TIMING
static void dw_pll5ghz_tsmc12ffc_wait(uint8_t pll, uint32_t timing)
{
	while (__dw_pll_wait_timing(pll, timing));
}
#else
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_ACCEL
#define dw_pll5ghz_tsmc12ffc_ndelay(us)	wmb()
#else
#define dw_pll5ghz_tsmc12ffc_ndelay(us)	udelay(((us) + 999) / 1000)
#endif

#ifndef DW_PLL_T_SPO
#define DW_PLL_T_SPO			5
#endif
#ifndef DW_PLL_T_GS
#define DW_PLL_T_GS			4
#endif
#ifndef DW_PLL_T_PRST
#define DW_PLL_T_PRST			3
#endif
#ifndef DW_PLL_T_PWRON
#define DW_PLL_T_PWRON			2
#endif
#ifndef DW_PLL_T_TRST
#define DW_PLL_T_TRST			1
#endif
#ifndef DW_PLL_T_PWRSTB
#define DW_PLL_T_PWRSTB			0
#endif

static void dw_pll5ghz_tsmc12ffc_wait(uint8_t pll, uint32_t timing)
{
	switch (timing) {
	case DW_PLL_T_TRST:
		/* 50ns */
		dw_pll5ghz_tsmc12ffc_ndelay(50);
		break;
	case DW_PLL_T_PWRON:
		/* The IP complains PWRON and RST_N set simultaneously. */
		wmb();
		break;
	case DW_PLL_T_PRST:
		/* Relationship of prstdur[2:0] and cmpclk cycles
		 *
		 * Internally controlled by prstdur at ANAREG07, at least
		 * 1us. So instead of wait cycles,
		 * dw_pll5ghz_tsmc12ffc_prstdur() should be invoked.
		 *
		 * Default 64 cycles of cmpclk (means 3.2us at 20MHz
		 * cmpclk and 1.282us at 50MHz cmpclk) which works for
		 * most of the cases..
		 * +--------------+---------------+
		 * | prstdur[2:0] | cmpclk cycles |
		 * +--------------+---------------+
		 * | 0            | 4             |
		 * | 1            | 8             |
		 * | 2            | 16            |
		 * | 3            | 32            |
		 * | 4            | 64            |
		 * | 5            | 128           |
		 * | 6            | 128           |
		 * | 7            | 128           |
		 * +--------------+---------------+
		 */
		dw_pll5ghz_tsmc12ffc_prstdur(pll);
		break;
	case DW_PLL_T_GS:
		/* Fast startup:
		 * The t_gs time interval corresponds to the phase
		 * settling time loop at gear_shift state, after the
		 * internal preset state and should last for minimum of
		 * Relock:
		 * The t_gs time interval can be shorter than the times
		 * above, but at least 2us should be used during this
		 * period.
		 * The PLL only leaves the transition states and gets to
		 * normal operation after gear_shift is set to low and
		 * lock flag is asserted high. During the Fast Startup
		 * transition state the gear_shift must be set to high
		 * during 1 minimum of 100 loops comparison clock cycles
		 * or 3us (3us includes 1us t_prstdur).
		 */
		__dw_pll_wait_cmpclk(pll, 100);
		break;
	case DW_PLL_T_SPO:
		/* The output enabling enp/enr can be asserted high either
		 * before or after the lock flag is activated. But at
		 * least t_spo of 40 loop cmpclk cycles should be reserved
		 * after the gear_shift falling transition to allow for
		 * normal phase settling.
		 */
		__dw_pll_wait_cmpclk(pll, 40);
		break;
	case DW_PLL_T_PWRSTB:
		/* 500ns */
		dw_pll5ghz_tsmc12ffc_ndelay(500);
		break;
	}
}
#endif

static void __dw_pll5ghz_tsmc12ffc_set_gear(uint8_t pll)
{
	__raw_setl(PLL_GEAR_SHIFT, DW_PLL_CFG1(pll));
}

static void __dw_pll5ghz_tsmc12ffc_clear_gear(uint8_t pll)
{
	/* t_gs: gearshift */
	dw_pll5ghz_tsmc12ffc_wait(pll, DW_PLL_T_GS);
	__raw_clearl(PLL_GEAR_SHIFT, DW_PLL_CFG1(pll));
	/* t_spo: static phase offset */
	dw_pll5ghz_tsmc12ffc_wait(pll, DW_PLL_T_SPO);
}

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_GEAR
#define dw_pll5ghz_tsmc12ffc_set_gear(pll)	\
	__dw_pll5ghz_tsmc12ffc_set_gear(pll)
#define dw_pll5ghz_tsmc12ffc_clear_gear(pll)	\
	__dw_pll5ghz_tsmc12ffc_clear_gear(pll)
#else
#define dw_pll5ghz_tsmc12ffc_set_gear(pll)	do { } while (0)
#define dw_pll5ghz_tsmc12ffc_clear_gear(pll)	do { } while (0)
#endif

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_BYPASS_TEST
void dw_pll5ghz_tsmc12ffc_bypass_test(uint8_t pll, bool bypass)
{
	dw_pll_write(pll, PLL_ANAREG05, bypass ? PLL_TEST_BYPASS : 0);
}
#else
#define dw_pll5ghz_tsmc12ffc_bypass_test(pll, bypass)	do { } while (0)
#endif

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_BYPASS_SYNC
void dw_pll5ghz_tsmc12ffc_bypass_sync(uint8_t pll, bool r, bool bypass)
{
	uint8_t sync;

	sync = dw_pll_read(pll, PLL_ANAREG06);
	sync |= r ? PLL_BYPASS_SYNC_R : PLL_BYPASS_SYNC_P;
	dw_pll_write(pll, PLL_ANAREG06, sync);
}
#else
#define dw_pll5ghz_tsmc12ffc_bypass_sync(pll, r, bypass)	\
	do { } while (0)
#endif

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_DYNAMIC
static void dw_pll5ghz_tsmc12ffc_dynamic(uint8_t pll, uint32_t cfg,
					 uint32_t mask)
{
	con_log("pll(%d): dynamically changing static PINs!\n", pll);
	dw_pll5ghz_tsmc12ffc_pwrdn(pll);
	__raw_writel_mask(cfg, mask, DW_PLL_CFG1(pll));
	dw_pll5ghz_tsmc12ffc_pwrup(pll);
}
#else
static void dw_pll5ghz_tsmc12ffc_dynamic(uint8_t pll, uint32_t cfg,
					 uint32_t mask)
{
	/* Warn P/R changes
	 *
	 * P/R/DIVVCOP/DIVVCOR are marked as: "Synchronous to: Static" in
	 * "Table 2-5 Control register pins". So users are warned if these
	 * pins are meant to be dynamic driven by the high-level
	 * applications (allowed but may not work correct).
	 *
	 * NOTE that the issue cannot be detected unless console is ready.
	 */
	con_err("pll(%d): statically changing static PINs!\n", pll);
	__raw_writel_mask(cfg, mask, DW_PLL_CFG1(pll));
}
#endif

/* XXX: Since standby incorporates t_spo phase settling, it should be
 *      invoked with enp/enr cleared.
 */
void dw_pll5ghz_tsmc12ffc_standby(uint8_t pll)
{
	if (PLL_OPMODE(__raw_readl(DW_PLL_STATUS(pll))) == PLL_LOCKED) {
		__raw_writel_mask(PLL_STANDBY | PLL_PWRON, PLL_STATE_MASK,
				  DW_PLL_CFG1(pll));
		while (!(__raw_readl(DW_PLL_STATUS(pll)) & PLL_STANDBYEFF));
	}
}

void dw_pll5ghz_tsmc12ffc_relock(uint8_t pll)
{
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_STANDBYEFF) {
		__dw_pll5ghz_tsmc12ffc_set_gear(pll);
		__raw_writel_mask(PLL_RESET | PLL_PWRON,
				  PLL_RESET | PLL_STATE_MASK,
				  DW_PLL_CFG1(pll));
		dw_pll5ghz_tsmc12ffc_wait(pll, DW_PLL_T_PRST);
		__dw_pll5ghz_tsmc12ffc_clear_gear(pll);
		while (!(__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED));
	}
}

static void dw_pll5ghz_tsmc12ffc_output_fclk(uint8_t pll, bool r,
					     uint32_t divvcopr, uint32_t pr,
					     bool enable, bool locked)
{
	uint32_t mask = r ? PLL_R(PLL_R_MASK) |
			    PLL_DIVVCOR(PLL_DIVVCOR_MASK) :
			    PLL_P(PLL_P_MASK) |
			    PLL_DIVVCOP(PLL_DIVVCOP_MASK);
	uint32_t cfg = r ? PLL_R(pr) | PLL_DIVVCOR(divvcopr) :
			   PLL_P(pr) | PLL_DIVVCOP(divvcopr);
	uint32_t enpr = r ? PLL_ENR : PLL_ENP;

	if ((__raw_readl(DW_PLL_CFG1(pll)) & mask) != cfg) {
		dw_pll_trace("pll(%d): CFG1(%08llx) divvco%c=%d %c=%d\n",
			     pll, cfg, PLL_FMT_PR(r), divvcopr,
			     PLL_FMT_PR(r), pr);
		if (locked)
			dw_pll5ghz_tsmc12ffc_dynamic(pll, cfg, mask);
		else
			__raw_writel_mask(cfg, mask, DW_PLL_CFG1(pll));
	}
	if (enable)
		__raw_setl(enpr, DW_PLL_CFG1(pll));
}

/* Normal oper: Fclkout = Fvco / (divvco{p|r} * {p|r})
 * Bypass core: Fclkout = Fcmpclk / (divvco{p|r}[1:0] * {p|r})
 * Bypass all:  Fclkout = Fref_clk
 */
#define next_div(fvco, div, div32, div10)		\
	if ((div) < 8)					\
		(div32) = (div32) ? 3 : 2;		\
	else if ((div) < 64)				\
		(div10) += 1;				\
	else						\
		BUG();					\
	(div) <<= 1;					\
	(fvco) >>= 1;					\

static void ____dw_pll5ghz_tsmc12ffc_config_fclk(uint8_t pll, uint64_t fvco,
						 uint64_t freq, bool r,
						 uint64_t div,
						 uint32_t divvco32,
						 bool locked)
{
	uint32_t divvco10 = 0;
	uint32_t pr;

	fvco = div64u(fvco, div);
	do {
		/* XXX: Fvco/Fpr Cannot Divide Exactly
		 *
		 * Due to the limitation of divvco 3:2 bits, we may not
		 * be able to divide exactly here. Obtain a possible big
		 * enough P/R value to ensure the output frequency is
		 * always slower than expected to meet the timing
		 * requirement.
		 */
		pr = (uint32_t)div64u(fvco + (freq - 1), freq);
		if (pr <= 64)
			break;
		next_div(fvco, divvco32, divvco10, div);
	} while (1);
	dw_pll5ghz_tsmc12ffc_output_fclk(pll, r, divvco32 << 2 | divvco10,
					 pr - 1, false, locked);
}

static void dw_pll5ghz_tsmc12ffc_config_sync(uint8_t pll, uint64_t fvco,
					     uint64_t freq, bool r,
					     bool locked)
{
	uint64_t div;
	uint32_t divvco32;

	BUG_ON(freq > ULL(1000000000));

	if (fvco <= ULL(2000000000)) {
		divvco32 = 0;
		div = 2;
	} else if (fvco <= ULL(4000000000)) {
		divvco32 = 2;
		div = 4;
	} else {
		divvco32 = 3;
		div = 8;
	}
	____dw_pll5ghz_tsmc12ffc_config_fclk(pll, fvco, freq, r,
					     div, divvco32, locked);
}

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_BYPASS_SYNC
static void dw_pll5ghz_tsmc12ffc_config_async(uint8_t pll, uint64_t fvco,
					      uint64_t freq, bool r,
					      bool locked)
{
	uint64_t div = 2;
	uint32_t divvco32 = 0;

	____dw_pll5ghz_tsmc12ffc_config_fclk(pll, fvco, freq, r,
					     div, divvco32, locked);
}

static void __dw_pll5ghz_tsmc12ffc_config_fclk(uint8_t pll, uint64_t fvco,
					       uint64_t freq, bool r,
					       bool locked)
{
	if (freq > ULL(1000000000))
		dw_pll5ghz_tsmc12ffc_config_async(pll, fvco, freq, r, locked);
	else
		dw_pll5ghz_tsmc12ffc_config_sync(pll, fvco, freq, r, locked);
}
#else
static void __dw_pll5ghz_tsmc12ffc_config_fclk(uint8_t pll, uint64_t fvco,
					       uint64_t freq, bool r,
					       bool locked)
{
	dw_pll5ghz_tsmc12ffc_config_sync(pll, fvco, freq, r, locked);
}
#endif /* CONFIG_DW_PLL5GHZ_TSMC12FFC_BYPASS_SYNC */

static void dw_pll5ghz_tsmc12ffc_config_fclk_default(uint8_t pll, bool r,
						     bool locked)
{
	dw_pll5ghz_tsmc12ffc_output_fclk(pll, r, 0xf, 0x3f, false, locked);
}

void dw_pll5ghz_tsmc12ffc_enable(uint8_t pll, uint64_t fvco,
				 uint64_t freq, bool r)
{
	con_log("pll(%d): Fvco=%lldHz, F%c=%lldHz\n",
		pll, fvco, PLL_FMT_PR(r), freq);
	__dw_pll5ghz_tsmc12ffc_config_fclk(pll, fvco, freq, r, true);
	if (freq > ULL(1000000000))
		dw_pll5ghz_tsmc12ffc_bypass_sync(pll, r, true);
	__raw_setl(r ? PLL_ENR : PLL_ENP, DW_PLL_CFG1(pll));
}

void dw_pll5ghz_tsmc12ffc_disable(uint8_t pll, bool r)
{
	dw_pll5ghz_tsmc12ffc_config_fclk_default(pll, r, true);
}

static void __dw_pll5ghz_tsmc12ffc_pwrup(uint8_t pll)
{
	dw_pll5ghz_tsmc12ffc_set_gear(pll);
	/* t_pwrstb */
	dw_pll5ghz_tsmc12ffc_wait(pll, DW_PLL_T_PWRSTB);
	__raw_setl(PLL_TEST_RESET, DW_PLL_CFG1(pll));
	/* t_trst */
	dw_pll5ghz_tsmc12ffc_wait(pll, DW_PLL_T_TRST);
	__raw_setl(PLL_PWRON, DW_PLL_CFG1(pll));
	/* XXX: t_pwron
	 *      In the databook, PWRON and RST_N seem to be set
	 *      simultaneously, while the IP complains that PWRON must
	 *      be enabled before setting RST_N. And the process takes
	 *      time, otherwise, RST_N setting and GEAR_SHIFT unsetting
	 *      can go wrong.
	 */
	dw_pll5ghz_tsmc12ffc_wait(pll, DW_PLL_T_PWRON);
	dw_pll_trace("pll(%d): CFG1(%08llx) reset\n",
		     pll, __raw_readl(DW_PLL_CFG1(pll)));
	__raw_setl(PLL_RESET, DW_PLL_CFG1(pll));
	/* t_prst: preset */
	dw_pll5ghz_tsmc12ffc_wait(pll, DW_PLL_T_PRST);
	dw_pll5ghz_tsmc12ffc_clear_gear(pll);
	while (!(__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED));
}

static void __dw_pll5ghz_tsmc12ffc_output_fvco(uint8_t pll, uint32_t range)
{
	uint32_t cfg;

	cfg = __raw_readl(DW_PLL_CFG1(pll));
	dw_pll_trace("pll(%d): CFG1(%08x) vco_mode=%d, lowfreq=%d\n",
		     pll, cfg, !!(range & PLL_VCO_MODE),
		     !!(range & PLL_LOWFREQ));

	cfg &= ~(PLL_STATE_MASK | PLL_RESET_MASK);
	cfg |= range;
	__raw_writel(cfg, DW_PLL_CFG1(pll));
	__dw_pll5ghz_tsmc12ffc_pwrup(pll);
}

static void dw_pll5ghz_tsmc12ffc_output_fvco(uint8_t pll, uint64_t fvco)
{
	uint32_t cfg = PLL_RANGE3;

	if (fvco < ULL(2500000000))
		BUG();
	else if (fvco <= ULL(3750000000))
		cfg = PLL_RANGE1;
	else if (fvco > ULL(3750000000) && fvco <= ULL(4000000000))
		cfg = PLL_RANGE2;
	else if (fvco > ULL(4000000000) && fvco <= ULL(5000000000))
		cfg = PLL_RANGE23;
	else if (fvco <= ULL(6000000000))
		cfg = PLL_RANGE3;
	else
		BUG();

	__dw_pll5ghz_tsmc12ffc_output_fvco(pll, cfg);
}

void dw_pll5ghz_tsmc12ffc_pwrup(uint8_t pll)
{
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED)
		return;

	__dw_pll5ghz_tsmc12ffc_pwrup(pll);
}

static void dw_pll5ghz_tsmc12ffc_config_fvco(uint8_t pll, uint64_t fvco)
{
	uint16_t mint, mfrac;
	uint8_t prediv = 0;
	uint64_t fbdiv;
	uint32_t cfg;

	/* 3.4 Output frequency
	 *
	 * The VCO oscillating frequency Fvco is a function of the input
	 * reference frequency Frefclk and of the multiplication/division
	 * ratios:
	 *        fbdiv
	 * Fvco = ------ * Frefclk
	 *        prediv
	 * where:
	 * fbdiv: feedback multiplication ratio
	 * prediv: input frequency division ratio
	 *                mfrac
	 * fbdiv = mint + -----
	 *                2^16
	 * Then:
	 * mint: ranges between 16 and 1039
	 * mfrac: ranges between 0 and 6535
	 */
	do {
		prediv++;
		BUG_ON(prediv > 32);
		fbdiv = div64u(fvco << 16,
			       div32u(DW_PLL_F_REFCLK(pll), prediv));
		mint = HIWORD(fbdiv);
		mfrac = LOWORD(fbdiv);
	} while (mint < 16);
	cfg = PLL_PREDIV(prediv - 1) |
	      PLL_MINT(mint - 16) | PLL_MFRAC(mfrac);
	dw_pll_trace("pll(%d): CFG0(%08llx) prediv=%d\n",
		     pll, cfg, prediv - 1);
	__raw_writel(cfg, DW_PLL_CFG0(pll));
}

void dw_pll5ghz_tsmc12ffc_pwron(uint8_t pll, uint64_t fvco)
{
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED)
		return;

	con_log("pll(%d): Fvco=%lldHz, Fp=0Hz Fr=0Hz\n", pll, fvco);
	dw_pll5ghz_tsmc12ffc_config_fvco(pll, fvco);
	dw_pll5ghz_tsmc12ffc_config_fclk_default(pll, false, false);
	dw_pll5ghz_tsmc12ffc_config_fclk_default(pll, true, false);
	dw_pll5ghz_tsmc12ffc_output_fvco(pll, fvco);

	/* P/R outputs:
	 *  1'b0: Fclkout = 0 or Fclkref/(P|R)
	 *  1'b1: Fclkout = PLL output
	 */
	dw_pll5ghz_tsmc12ffc_bypass(pll, PLL_BYPASS_NONE);
}

static void dw_pll5ghz_tsmc12ffc_config_fclk(uint8_t pll, uint64_t fvco,
					     uint64_t fclkout, bool r)
{
	if (fclkout != INVALID_FREQ)
		__dw_pll5ghz_tsmc12ffc_config_fclk(pll, fvco, fclkout,
						   r, false);
	else
		dw_pll5ghz_tsmc12ffc_config_fclk_default(pll, r, false);
}

void dw_pll5ghz_tsmc12ffc_pwron2(uint8_t pll, uint64_t fvco,
				 uint64_t fpclk, uint64_t frclk)
{
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED)
		return;

	con_log("pll(%d): Fvco=%lldHz, Fp=%lldHz Fr=%lldHz\n",
		pll, fvco, fpclk, frclk);
	dw_pll5ghz_tsmc12ffc_config_fvco(pll, fvco);
	dw_pll5ghz_tsmc12ffc_config_fclk(pll, fvco, fpclk, false);
	dw_pll5ghz_tsmc12ffc_config_fclk(pll, fvco, frclk, true);
	dw_pll5ghz_tsmc12ffc_output_fvco(pll, fvco);

	/* P/R outputs:
	 *  1'b0: Fclkout = 0 or Fclkref/(P|R)
	 *  1'b1: Fclkout = PLL output
	 */
	dw_pll5ghz_tsmc12ffc_bypass(pll, PLL_BYPASS_NONE);
}

/* XXX: Since pwrdn incorporates t_spo phase settling, it should be
 *      invoked with enp/enr cleared.
 */
void dw_pll5ghz_tsmc12ffc_pwrdn(uint8_t pll)
{
	if (!(__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED))
		return;

	dw_pll5ghz_tsmc12ffc_bypass(pll, PLL_BYPASS_NONE);
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_STANDBYEFF)
		dw_pll5ghz_tsmc12ffc_relock(pll);
	if (__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED) {
		__raw_clearl(PLL_PWRON, DW_PLL_CFG1(pll));
		while (__raw_readl(DW_PLL_STATUS(pll)) & PLL_LOCKED);
	}
}

void dw_pll5ghz_tsmc12ffc_bypass(uint8_t pll, uint8_t mode)
{
	if (!(__raw_readl(DW_PLL_CFG1(pll)) & PLL_PWRON))
		return;
	switch (mode) {
	case PLL_BYPASS_CORE:
		__raw_setl(PLL_BYPASS, DW_PLL_CFG1(pll));
		dw_pll5ghz_tsmc12ffc_bypass_test(pll, false);
		break;
	case PLL_BYPASS_ALL:
		__raw_setl(PLL_BYPASS, DW_PLL_CFG1(pll));
		dw_pll5ghz_tsmc12ffc_bypass_test(pll, true);
		break;
	case PLL_BYPASS_NONE:
	default:
		__raw_clearl(PLL_BYPASS, DW_PLL_CFG1(pll));
		break;
	}
}

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_REG_ACCESS
void dw_pll_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	if (!(__raw_readl(DW_PLL_CFG1(pll)) & PLL_PWRON))
		return;
	__dw_pll_write(pll, reg, val);
}

uint8_t dw_pll_read(uint8_t pll, uint8_t reg)
{
	if (!(__raw_readl(DW_PLL_CFG1(pll)) & PLL_PWRON))
		return 0;
	return __dw_pll_read(pll, reg);
}

static int do_pll_reg_access(int argc, char * argv[])
{
	int pll;
	uint8_t reg;
	uint8_t val;

	if (argc < 4)
		return -EINVAL;

	pll = strtoul(argv[2], NULL, 0);
	if (!(__raw_readl(DW_PLL_CFG1(pll)) & PLL_PWRON)) {
		printf("PLL %d is not in PWRON state.\n", pll);
		return -EINVAL;
	}
	reg = (uint8_t)strtoul(argv[3], NULL, 0);
	if (argc > 4) {
		val = (uint8_t)strtoul(argv[4], NULL, 0);
		dw_pll_write(pll, reg, val);
	} else {
		val = dw_pll_read(pll, reg);
	}
	printf("%c: %02x: %02x\n", argc > 4 ? 'W' : 'R', reg, val);
	return 0;
}
#else
static int do_pll_reg_access(int argc, char * argv[])
{
	return -EINVAL;
}
#endif

static int do_pll_operation(int argc, char * argv[])
{
	int pll;

	if (argc < 3)
		return -EINVAL;

	pll = strtoul(argv[2], NULL, 0);
	if (strcmp(argv[1], "up") == 0)
		dw_pll5ghz_tsmc12ffc_pwrup(pll);
	else if (strcmp(argv[1], "down") == 0)
		dw_pll5ghz_tsmc12ffc_pwrdn(pll);
	else if (strcmp(argv[1], "standby") == 0)
		dw_pll5ghz_tsmc12ffc_standby(pll);
	else if (strcmp(argv[1], "relock") == 0)
		dw_pll5ghz_tsmc12ffc_relock(pll);
	else
		return -ENODEV;
	return 0;
}

static int do_pll_bypass(int argc, char * argv[])
{
	int pll;

	if (argc < 4)
		return -EINVAL;

	pll = strtoul(argv[2], NULL, 0);
	if (strcmp(argv[3], "all") == 0)
		dw_pll5ghz_tsmc12ffc_bypass(pll, PLL_BYPASS_ALL);
	else if (strcmp(argv[3], "core") == 0)
		dw_pll5ghz_tsmc12ffc_bypass(pll, PLL_BYPASS_CORE);
	else if (strcmp(argv[3], "none") == 0)
		dw_pll5ghz_tsmc12ffc_bypass(pll, PLL_BYPASS_NONE);
	else
		return -ENODEV;
	return 0;
}

static int do_pll(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "access") == 0)
		return do_pll_reg_access(argc, argv);
	if (strcmp(argv[1], "bypass") == 0)
		return do_pll_bypass(argc, argv);
	return do_pll_operation(argc, argv);
}

DEFINE_COMMAND(pll, do_pll, "Control DWC PLL5GHz TSMC12FFCNS",
	"pll access pll reg\n"
	"    -read pll register\n"
	"pll access pll reg val\n"
	"    -write pll register\n"
	"pll bypass all|core|none\n"
	"    -set pll output bypass mode\n"
	"pll up pll\n"
	"    -power up pll\n"
	"pll down pll\n"
	"    -power down pll\n"
	"pll standby pll\n"
	"    -put pll into standby state\n"
	"pll relock pll\n"
	"    -get pll out of standby state\n"
);
