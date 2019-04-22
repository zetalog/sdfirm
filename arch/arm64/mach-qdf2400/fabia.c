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
 * @(#)fabia.c: FABIA PLL implementation
 * $Id: fabia.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/delay.h>

/* Fabia PLL definitions (GPLL0 is Fabia type in 8998) */
/* PLL lock timeout (2x max locking time) */
#define CLOCK_PLL_LOCK_TIMEOUT_US	200
/* Warm-up time in microseconds after turning on the PLL */
#define CLOCK_PLL_WARMUP_TIME_US	25

#define GCC_IS_EAST_PLL(base)		((base) >= (caddr_t)GCCE_BASE)
#define FABIA_PLL_IS_ENABLED(mode)	\
	(((mode) & FABIA_PLL_ENABLED) == FABIA_PLL_ENABLED)

#ifdef CONFIG_FABIA_PLL_LOCK
static bool __fabia_wait_pll_lock(caddr_t base)
{
	uint32_t tout = 0;

	while (!(__raw_readl(FABIA_MODE(base)) &
	         FABIA_PLL_LOCK_DET)) {
		if (tout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
			return false;
		udelay(1);
	}
	return true;
}
#else
static bool __fabia_wait_pll_lock(caddr_t base)
{
	return true;
}
#endif

bool __fabia_enable_pll(caddr_t base,
			uint32_t input_hz, uint32_t output_hz)
{
	uint32_t mode;
	uint16_t L;

	mode = __raw_readl(FABIA_MODE(base));
	/* Return if the PLL is manually enabled. */
	if (FABIA_PLL_IS_ENABLED(mode))
		return true;

	/* Update PLL configuration */
	__raw_writel(FABIA_OSC_WARMUP_TIME(FABIA_OSC_WARMUP_TIME_CNT_25_REF_CYCS) |
		     FABIA_INC_MIN_GLITCH_THRESHOLD_4X |
		     FABIA_MIN_GLITCH_THRESHOLD(FABIA_MIN_GLITCH_THRESHOLD_MIN_128) |
		     FABIA_BIAS_WARMUP_TIME(FABIA_BIAS_WARMUP_TIME_CNT_15_REF_CYCS) |
		     FABIA_DIV_LOCK_DET_THRESHOLDS,
		     FABIA_CONFIG_CTL_U(base));
	__raw_writel(FABIA_FINE_LDC_THRESHOLD(128) |
		     FABIA_COARSE_LDC_THRESHOLD(64) |
		     FABIA_COARSE_LOCK_DET_NEG_THRESHOLD(64) |
		     FABIA_COARSE_LOCK_DET_POS_THRESHOLD(64) |
		     FABIA_FINE_LOCK_DET_THRESHOLD(7) |
		     FABIA_FINE_LOCK_DET_SAMPLE_SIZE(28) |
		     FABIA_FWD_GAIN_SLEWING_KFN(9) |
		     FABIA_FWD_GAIN_KFN(9),
		     FABIA_CONFIG_CTL(base));
	if (GCC_IS_EAST_PLL(base)) {
		__raw_writel(FABIA_GLITCH_DETECTOR_COUNT_LIMIT(FABIA_GLITCH_DETECTOR_COUNT_LIMIT_16) |
			     FABIA_ATEST1_SEL(1) |
			     FABIA_ATEST0_SEL(2),
			     FABIA_TEST_CTL_U(base));
		__raw_writel(FABIA_POST_DIV_RATIO_ODD(3) |
			     FABIA_POST_DIV_RATIO_EVEN(2) |
			     FABIA_PLLOUT_ALL,
			     FABIA_USER_CTL(base));
	} else {
		__raw_writel(FABIA_GLITCH_DETECTOR_COUNT_LIMIT(FABIA_GLITCH_DETECTOR_COUNT_LIMIT_4) |
			     FABIA_ATEST1_SEL(0) |
			     FABIA_ATEST0_SEL(0),
			     FABIA_TEST_CTL_U(base));
		__raw_writel(0x0, FABIA_TEST_CTL_U(base));
		__raw_writel(FABIA_PLLOUT_ALL, FABIA_USER_CTL(base));
	}
	__raw_writel(FABIA_ADD_REF_CYC_CALIB_STEPS(1), FABIA_TEST_CTL(base));
	__raw_writel(FABIA_BIAS_ON_IN_STANDBY |
		     FABIA_REF_CLK_AT_OUT |
		     FABIA_SCALE_FREQ_DOWN(6_PERCENT) |
		     FABIA_CALIB(AUTOMATIC),
		     FABIA_USER_CTL_U(base));

	/* Configure the PLL (if not in FSM mode). */
	if (!(mode & FABIA_PLL_VOTE_FSM_ENA)) {
		/* Program L and CAL_L. */
		L = output_hz / input_hz;
		__raw_writel(L, FABIA_L_VAL(base));
		__raw_writel(L, FABIA_CAL_L_VAL(base));
	}

	/* No voting register--follow the manual PLL enable sequence */
	__raw_writel(FABIA_PLL_OPMODE(STANDBY), FABIA_OPMODE(base));
	/* Assert the reset_n field to put the PLL in standby state */
	__raw_setl(FABIA_PLL_RESET_N, FABIA_MODE(base));

	/* Set OPMODE to run state. */
	__raw_writel(FABIA_PLL_OPMODE(RUN), FABIA_OPMODE(base));
	udelay(CLOCK_PLL_WARMUP_TIME_US);

	/* Wait for the PLL to lock within the given timeout
	 * period.
	 */
	if (!__fabia_wait_pll_lock(base))
		return false;

	/* Enable PLLOUT_MAIN */
	__raw_setl(FABIA_PLLOUT_MAIN, FABIA_USER_CTL(base));
	/* Assert the PLL Output control bit */
	__raw_setl(FABIA_PLL_OUTCTRL, FABIA_MODE(base));
	return true;
}

#if 0
static bool fabia_disable_pll(int pll)
{
	/* De-assert the OUTCTL field */
	__raw_clearl(FABIA_PLL_OUTCTRL, FABIA_MODE(pll));
	udelay(1);
	/* Put the PLL in reset mode */
	__raw_clearl(FABIA_PLL_RESET_N, FABIA_MODE(pll));
	/* Set OPMODE to standby state */
	__raw_writel(FABIA_PLL_OPMODE(STANDBY), FABIA_OPMODE(pll));
	return true;
}
#endif
