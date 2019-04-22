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
 * @(#)fabia.h: FABIA PLL definitions
 * $Id: fabia.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __QDF2400_FABIA_H_INCLUDE__
#define __QDF2400_FABIA_H_INCLUDE__

/* Fabia register offsets */
#define FABIA_REG(base, off)		((base) + (off))

#define FABIA_MODE(base)		FABIA_REG(base, 0x0000)
#define FABIA_L_VAL(base)		FABIA_REG(base, 0x0004)
#define FABIA_CAL_L_VAL(base)		FABIA_REG(base, 0x0008)
#define FABIA_USER_CTL(base)		FABIA_REG(base, 0x000C)
#define FABIA_USER_CTL_U(base)		FABIA_REG(base, 0x0010)
#define FABIA_CONFIG_CTL(base)		FABIA_REG(base, 0x0014)
#define FABIA_CONFIG_CTL_U(base)	FABIA_REG(base, 0x0018)
#define FABIA_TEST_CTL(base)		FABIA_REG(base, 0x001C)
#define FABIA_TEST_CTL_U(base)		FABIA_REG(base, 0x0020)
#define FABIA_STATUS(base)		FABIA_REG(base, 0x0024)
#define FABIA_FREQ_CTL(base)		FABIA_REG(base, 0x0028)
#define FABIA_OPMODE(base)		FABIA_REG(base, 0x002C)
#define FABIA_STATE(base)		FABIA_REG(base, 0x0030)
#define FABIA_DROOP(base)		FABIA_REG(base, 0x0034)
#define FABIA_FRAC_VAL(base)		FABIA_REG(base, 0x0038)
#define FABIA_SPARE(base)		FABIA_REG(base, 0x003C)

/* FABIA_MODE */
#define FABIA_PLL_LOCK_DET			_BV(31)
#define FABIA_PLL_ACTIVE_FLAG			_BV(30)
#define FABIA_PLL_ACK_LATCH			_BV(29)
#define FABIA_PLL_LOCK_DET_FINE			_BV(28)
#define FABIA_PLL_HW_UPDATE_LOGIC_BYPASS	_BV(23)
#define FABIA_PLL_UPDATE			_BV(22)
#define FABIA_PLL_VOTE_FSM_RESET		_BV(21)
#define FABIA_PLL_VOTE_FSM_ENA			_BV(20)
#define FABIA_PLL_BIAS_COUNT_OFFSET		14
#define FABIA_PLL_BIAS_COUNT_MASK		0x3F
#define FABIA_PLL_BIAS_COUNT_SET(value)		\
	_SET_FV(FABIA_PLL_BIAS_COUNT, value)
#define FABIA_PLL_BIAS_COUNT_GET(value)		\
	_GET_FV(FABIA_PLL_MODE_BIAS_COUNT, value)
#define FABIA_PLL_LOCK_COUNT_OFFSET		8
#define FABIA_PLL_LOCK_COUNT_MASK		0x3F
#define FABIA_PLL_LOCK_COUNT_SET(value)		\
	_SET_FV(FABIA_PLL_LOCK_COUNT, value)
#define FABIA_PLL_LOCK_COUNT_GET(value)		\
	_GET_FV(FABIA_PLL_LOCK_COUNT, value)
#define FABIA_PLL_RESET_N			_BV(2)
#define FABIA_PLL_OUTCTRL			_BV(0)

#define FABIA_PLL_ENABLED			\
	(FABIA_PLL_OUTCTRL | FABIA_PLL_RESET_N)

/* FABIA_USER_CTL */
#define FABIA_PRE_DIV_RATIO_OFFSET		16
#define FABIA_PRE_DIV_RATIO_MASK		0x07
#define FABIA_PRE_DIV_RATIO(value)		\
	_SET_FV(FABIA_PRE_DIV_RATIO, value)
#define FABIA_POST_DIV_RATIO_ODD_OFFSET	12
#define FABIA_POST_DIV_RATIO_ODD_MASK		0x0F
#define __FABIA_POST_DIV_RATIO_ODD(value)	\
	_SET_FV(FABIA_POST_DIV_RATIO_ODD, value)
#define FABIA_POST_DIV_RATIO_EVEN_OFFSET	8
#define FABIA_POST_DIV_RATIO_EVEN_MASK		0x0F
#define __FABIA_POST_DIV_RATIO_EVEN(value)	\
	_SET_FV(FABIA_POST_DIV_RATIO_EVEN, value)
#define FABIA_POST_DIV_RATIO_ODD(n)		\
	__FABIA_POST_DIV_RATIO_ODD(n)
#define FABIA_POST_DIV_RATIO_EVEN(n)		\
	__FABIA_POST_DIV_RATIO_EVEN((n)-1)
#define FABIA_OUT_CLK_POLARITY			_BV(7)
#define FABIA_PLLOUT_TEST			_BV(4)
#define FABIA_PLLOUT_ODD			_BV(2)
#define FABIA_PLLOUT_EVEN			_BV(1)
#define FABIA_PLLOUT_MAIN			_BV(0)
#define FABIA_PLLOUT_ALL			\
	(FABIA_PLLOUT_ODD | FABIA_PLLOUT_EVEN |	\
	 FABIA_PLLOUT_TEST | FABIA_PLLOUT_MAIN)

/* FABIA_USER_CTL_U */
#define FABIA_USE_PREDIV_WHEN_CAL		_BV(18)
#define FABIA_USE_EXT_SAVERESTORE		_BV(17)
#define FABIA_LOW_JITTER_MODE_EN		_BV(16)
#define FABIA_FRAC_FORMAT_SEL			_BV(15)
#define FABIA_BIAS_ON_IN_STANDBY		_BV(14)
#define FABIA_REF_CLK_AT_OUT			_BV(13)
#define FABIA_SCALE_FREQ_ON_RESTART_OFFSET	11
#define FABIA_SCALE_FREQ_ON_RESTART_MASK	0x03
#define FABIA_SCALE_FREQ_ON_RESTART(value)	\
	_SET_FV(FABIA_SCALE_FREQ_ON_RESTART, value)
#define FABIA_SCALE_FREQ_DOWN_NONE		0
#define FABIA_SCALE_FREQ_DOWN_6_PERCENT		1
#define FABIA_SCALE_FREQ_DOWN_12_PERCENT	2
#define FABIA_SCALE_FREQ_DOWN_25_PERCENT	3
#define FABIA_SCALE_FREQ_DOWN(percent)		\
	FABIA_SCALE_FREQ_ON_RESTART(FABIA_SCALE_FREQ_DOWN_##percent)
#define FABIA_LATCH_INTERFACE_BYPASS		_BV(10)
#define FABIA_STATUS_REGISTER_OFFSET		5
#define FABIA_STATUS_REGISTER_MASK		0x1F
#define FABIA_STATUS_REGISTER(value)		\
	_SET_FV(FABIA_STATUS_REGISTER, value)
#define FABIA_WRITE_STATE_EN			_BV(4)
#define FABIA_CALIB_CTRL_OFFSET			1
#define FABIA_CALIB_CTRL_MASK			0x07
#define FABIA_CALIB_CTRL(value)			\
	_SET_FV(FABIA_CALIB_CTRL, value)
#define FABIA_CALIB_ALWAYS			0
#define FABIA_CALIB_NEVER			1
#define FABIA_CALIB_AUTOMATIC			2
#define FABIA_CALIB(ctrl)			\
	FABIA_CALIB_CTRL(FABIA_CALIB_##ctrl)
#define FABIA_USE_ONLY_FINE_LOCK_DET		_BV(0)

/* FABIA_CONFIG_CTL */
#define FABIA_FINE_LDC_THRESHOLD_OFFSET			26
#define FABIA_FINE_LDC_THRESHOLD_MASK			0x3F
#define __FABIA_FINE_LDC_THRESHOLD(value)		\
	_SET_FV(FABIA_FINE_LDC_THRESHOLD, value)
#define FABIA_FINE_LDC_THRESHOLD(value)			\
	__FABIA_FINE_LDC_THRESHOLD((value) >> 4)
#define FABIA_COARSE_LDC_THRESHOLD_OFFSET		22
#define FABIA_COARSE_LDC_THRESHOLD_MASK			0x0F
#define __FABIA_COARSE_LDC_THRESHOLD(value)		\
	_SET_FV(FABIA_COARSE_LDC_THRESHOLD, value)
#define FABIA_COARSE_LDC_THRESHOLD(value)		\
	__FABIA_COARSE_LDC_THRESHOLD((value) >> 6)
#define FABIA_COARSE_LOCK_DET_NEG_THRESHOLD_OFFSET	18
#define FABIA_COARSE_LOCK_DET_NEG_THRESHOLD_MASK	0x0F
#define __FABIA_COARSE_LOCK_DET_NEG_THRESHOLD(value)	\
	_SET_FV(FABIA_COARSE_LOCK_DET_NEG_THRESHOLD, value)
#define FABIA_COARSE_LOCK_DET_NEG_THRESHOLD(value)	\
	__FABIA_COARSE_LOCK_DET_NEG_THRESHOLD((value) >> 5)
#define FABIA_COARSE_LOCK_DET_POS_THRESHOLD_OFFSET	14
#define FABIA_COARSE_LOCK_DET_POS_THRESHOLD_MASK	0x0F
#define __FABIA_COARSE_LOCK_DET_POS_THRESHOLD(value)	\
	_SET_FV(FABIA_COARSE_LOCK_DET_POS_THRESHOLD, value)
#define FABIA_COARSE_LOCK_DET_POS_THRESHOLD(value)	\
	__FABIA_COARSE_LOCK_DET_POS_THRESHOLD((value) >> 6)
#define FABIA_FINE_LOCK_DET_THRESHOLD_OFFSET		11
#define FABIA_FINE_LOCK_DET_THRESHOLD_MASK		0x07
#define __FABIA_FINE_LOCK_DET_THRESHOLD(value)		\
	_SET_FV(FABIA_FINE_LOCK_DET_THRESHOLD, value)
#define FABIA_FINE_LOCK_DET_THRESHOLD(value)		\
	__FABIA_FINE_LOCK_DET_THRESHOLD(((value) - 3) >> 1)
#define FABIA_FINE_LOCK_DET_SAMPLE_SIZE_OFFSET		8
#define FABIA_FINE_LOCK_DET_SAMPLE_SIZE_MASK		0x07
#define __FABIA_FINE_LOCK_DET_SAMPLE_SIZE(value)	\
	_SET_FV(FABIA_FINE_LOCK_DET_SAMPLE_SIZE, value)
#define FABIA_FINE_LOCK_DET_SAMPLE_SIZE(value)		\
	__FABIA_FINE_LOCK_DET_SAMPLE_SIZE(((value) >> 2) - 1)
#define FABIA_FWD_GAIN_SLEWING_KFN_OFFSET		4
#define FABIA_FWD_GAIN_SLEWING_KFN_MASK			0x0F
#define FABIA_FWD_GAIN_SLEWING_KFN(value)		\
	_SET_FV(FABIA_FWD_GAIN_SLEWING_KFN, value)
#define FABIA_FWD_GAIN_KFN_OFFSET			0
#define FABIA_FWD_GAIN_KFN_MASK				0x0F
#define FABIA_FWD_GAIN_KFN(value)			\
	_SET_FV(FABIA_FWD_GAIN_KFN, value)
/* TODO: Kf = 2Kf? */
#define FABIA_FWD_GAIN_KFN_20MHZ	10
#define FABIA_FWD_GAIN_KFN_32KHZ	3

/* FABIA_CONFIG_CTL_U */
#define FABIA_BIST_CFG_OFFSET			20
#define FABIA_BIST_CFG_MASK			0x0FFF
#define FABIA_BIST_CFG(value)			\
	_SET_FV(FABIA_BIST_CFG, value)
#define FABIA_ENABLE_OVER_PST			_BV(19)
#define FABIA_BIAS_SEL_PST			_BV(18)
#define FABIA_FORCE_2_MSB_COARSE_DAC_OFFSET	16
#define FABIA_FORCE_2_MSB_COARSE_DAC_MASK	0x03
#define FABIA_FORCE_2_MSB_COARSE_DAC(value)	\
	_SET_FV(FABIA_FORCE_2_MSB_COARSE_DAC, value)
#define FABIA_DISABLE_DMET_IN_DROOP_DETECTOR_DIVIDOR	_BV(15)
#define FABIA_BYPASS_DROOP_DETECTOR_DIVIDOR		_BV(14)
#define FABIA_OSC_WARMUP_TIME_OFFSET		12
#define FABIA_OSC_WARMUP_TIME_MASK		0x03
#define FABIA_OSC_WARMUP_TIME(value)		\
	_SET_FV(FABIA_OSC_WARMUP_TIME, value)
#define FABIA_OSC_WARMUP_TIME_CNT_0_IE_NO_WAIT	0
#define FABIA_OSC_WARMUP_TIME_CNT_10_REF_CYCS	1
#define FABIA_OSC_WARMUP_TIME_CNT_25_REF_CYCS	2
#define FABIA_OSC_WARMUP_TIME_CNT_50_REF_CYCS	3
#define FABIA_NUM_STEPS_FAST_LOCK_OFFSET	10
#define FABIA_NUM_STEPS_FAST_LOCK_MASK		0x03
#define __FABIA_NUM_STEPS_FAST_LOCK(value)	\
	_SET_FV(FABIA_NUM_STEPS_FAST_LOCK, value)
#define FABIA_NUM_STEPS_FAST_LOCK(value)	\
	__FABIA_NUM_STEPS_FAST_LOCK((12 - (value)) / 2)
#define FABIA_PREDIV_WHEN_CAL_OFFSET		7
#define FABIA_PREDIV_WHEN_CAL_MASK		0x07
#define __FABIA_PREDIV_WHEN_CAL(value)		\
	_SET_FV(FABIA_PREDIV_WHEN_CAL, value)
#define FABIA_PREDIV_WHEN_CAL(value)		\
	__FABIA_PREDIV_WHEN_CAL((value) - 1)
#define FABIA_INC_MIN_GLITCH_THRESHOLD_4X	_BV(6)
#define FABIA_MIN_GLITCH_THRESHOLD_OFFSET	4
#define FABIA_MIN_GLITCH_THRESHOLD_MASK		0x03
#define FABIA_MIN_GLITCH_THRESHOLD(value)	\
	_SET_FV(FABIA_MIN_GLITCH_THRESHOLD, value)
#define FABIA_MIN_GLITCH_THRESHOLD_MIN_4	0x0
#define FABIA_MIN_GLITCH_THRESHOLD_MIN_16	0x1
#define FABIA_MIN_GLITCH_THRESHOLD_MIN_128	0x2
#define FABIA_MIN_GLITCH_THRESHOLD_MIN_256	0x3
#define FABIA_USE_BOTH_REF_CLK_EDGE		_BV(4)
#define FABIA_BIAS_WARMUP_TIME_OFFSET		1
#define FABIA_BIAS_WARMUP_TIME_MASK		0x03
#define FABIA_BIAS_WARMUP_TIME(value)		\
	_SET_FV(FABIA_BIAS_WARMUP_TIME, value)
#define FABIA_BIAS_WARMUP_TIME_CNT_0_IE_NO_WAIT	0x0
#define FABIA_BIAS_WARMUP_TIME_CNT_1_REF_CYCS	0x1
#define FABIA_BIAS_WARMUP_TIME_CNT_7_REF_CYCS	0x2
#define FABIA_BIAS_WARMUP_TIME_CNT_15_REF_CYCS	0x3
#define FABIA_DIV_LOCK_DET_THRESHOLDS		_BV(0)

/* FABIA_TEST_CTL */
#define FABIA_ADD_REF_CYC_CALIB_STEPS_OFFSET	30
#define FABIA_ADD_REF_CYC_CALIB_STEPS_MASK	0x03
#define __FABIA_ADD_REF_CYC_CALIB_STEPS(value)	\
	_SET_FV(FABIA_ADD_REF_CYC_CALIB_STEPS, value)
#define FABIA_ADD_REF_CYC_CALIB_STEPS(value)	\
	__FABIA_ADD_REF_CYC_CALIB_STEPS((value) ? ilog2_const(value) + 1 : 0)
#define FABIA_GLITCH_DETECTOR_COUNT_LIMIT_OFFSET	28
#define FABIA_GLITCH_DETECTOR_COUNT_LIMIT_MASK		0x03
#define FABIA_GLITCH_DETECTOR_COUNT_LIMIT(value)	\
	_SET_FV(FABIA_GLITCH_DETECTOR_COUNT_LIMIT, value)
#define FABIA_GLITCH_DETECTOR_COUNT_LIMIT_4		0x0
#define FABIA_GLITCH_DETECTOR_COUNT_LIMIT_16		0x1
#define FABIA_GLITCH_DETECTOR_COUNT_LIMIT_32		0x2
#define FABIA_GLITCH_DETECTOR_COUNT_LIMIT_128		0x3
#define FABIA_GLITCH_PREVENTION_DIS		_BV(27)
#define FABIA_FINE_FCW_OFFSET			21
#define FABIA_FINE_FCW_MASK			0x3F
#define FABIA_FINE_FCW(value)			\
	_SET_FV(FABIA_FINE_FCW, value)
#define FABIA_OVERRIDE_FINE_FCW			_BV(20)
#define FABIA_COARSE_FCW_OFFSET			12
#define FABIA_COARSE_FCW_MASK			0xFF
#define FABIA_COARSE_FCW(value)			\
	_SET_FV(FABIA_COARSE_FCW, value)
#define FABIA_OVERRIDE_COARSE_FCW		_BV(11)
#define FABIA_PROCESS_CALIB_WORD_OFFSET		8
#define FABIA_PROCESS_CALIB_WORD_MASK		0x07
#define FABIA_PROCESS_CALIB_WORD(value)		\
	_SET_FV(FABIA_PROCESS_CALIB_WORD, value)
#define FABIA_OVERRIDE_CALIB_WORD		_BV(7)
#define FABIA_DISABLE_LFSR			_BV(6)
#define FABIA_ATEST1_SEL_OFFSET			4
#define FABIA_ATEST1_SEL_MASK			0x03
#define FABIA_ATEST1_SEL(value)			\
	_SET_FV(FABIA_ATEST1_SEL, value)
#define FABIA_ATEST0_SEL_OFFSET			2
#define FABIA_ATEST0_SEL_MASK			0x03
#define FABIA_ATEST0_SEL(value)			\
	_SET_FV(FABIA_ATEST0_SEL, value)
#define FABIA_ATEST1_EN				_BV(1)
#define FABIA_ATEST0_EN				_BV(2)

/* FABIA_TEST_CTL_U */
#define FABIA_ENABLE_TRAINING_SEQ		_BV(31)
#define FABIA_BYPASS_LOGIC_DEP			_BV(30)
#define FABIA_BYPASS_BIAS_DET			_BV(29)
#define FABIA_MSB_LOCK_SELECT			_BV(28)
#define FABIA_BYPASS_FIRST_ORDER_DSM		_BV(27)
#define FABIA_DCO_OFFSET_CURRENT_ADJUST_OFFSET	25
#define FABIA_DCO_OFFSET_CURRENT_ADJUST_MASK	0x03
#define __FABIA_DCO_OFFSET_CURRENT_ADJUST(value)	\
	_SET_FV(FABIA_DCO_OFFSET_CURRENT_ADJUST, value)
#define FABIA_DCO_OFFSET_CURRENT_ADJUST(value)		\
	__FABIA_DCO_OFFSET_CURRENT_ADJUST((value) - 26)
#define FABIA_MIRROR_DEVICE_ADJUST_OFFSET	22
#define FABIA_MIRROR_DEVICE_ADJUST_MASK		0x07
#define FABIA_MIRROR_DEVICE_ADJUST(value)	\
	_SET_FV(FABIA_MIRROR_DEVICE_ADJUST, value)
#define FABIA_BIAS_STARTUP_CIR_DIS		_BV(21)
#define FABIA_DISABLE_CLAMP			_BV(20)
#define FABIA_BYPASS_MODE_OF_BIAS		_BV(19)
#define FABIA_BYPASS_MODE_FOR_BIAS_EN		_BV(18)
#define FABIA_BIAS_ADJUST_OFFSET		16
#define FABIA_BIAS_ADJUST_MASK			0x03
#define FABIA_BIAS_ADJUST(value)		\
	_SET_FV(FABIA_BIAS_ADJUST, value)
#define FABIA_DIV2_NMO_EN			_BV(15)
#define FABIA_DIS_LEAK_CMP			_BV(14)
#define FABIA_SINGLE_DMET_EN			_BV(13)
#define FABIA_DEMET_WINDOW_DIS			_BV(12)
#define FABIA_NMO_OSC_SEL_OFFSET		10
#define FABIA_NMO_OSC_SEL_MASK			0x03
#define FABIA_NMO_OSC_SEL(value)		\
	_SET_FV(FABIA_NMO_OSC_SEL, value)
#define FABIA_NMO_OSC_SEL_OFF			0x0
#define FABIA_NMO_OSC_SEL_FREQ_350MHZ		0x1
#define FABIA_NMO_OSC_SEL_FREQ_700MHZ		0x2
#define FABIA_NMO_OSC_SEL_FREQ_1400MHZ		0x3
#define FABIA_NOISE_MAG_OFFSET			7
#define FABIA_NOISE_MAG_MASK			0x07
#define FABIA_NOISE_MAG(value)			\
	_SET_FV(FABIA_NOISE_MAG, value)
#define FABIA_NOISE_MAG_OFF			0x0
#define FABIA_NOISE_MAG_34MVRMS			0x1
#define FABIA_NOISE_MAG_61MVRMS			0x2
#define FABIA_NOISE_MAG_72MVRMS			0x3
#define FABIA_NOISE_GEN_EN			_BV(6)
#define FABIA_OSC_BIAS_GND			_BV(5)
#define FABIA_PLL_TEST_OUT_SEL_OFFSET		3
#define FABIA_PLL_TEST_OUT_SEL_MASK		0x03
#define FABIA_PLL_TEST_OUT_SEL(value)		\
	_SET_FV(FABIA_PLL_TEST_OUT_SEL, value)
/* Select output on pll_test, select which output on pllout_lv_test */
#define FABIA_PLL_TEST_OUT_SEL_RESETN		0x0
#define FABIA_PLL_TEST_OUT_SEL_NMO_OUTPUT	0x1
#define FABIA_PLL_TEST_OUT_SEL_DCO_CLOCK_WITH_POST_DIVISION	0x2
#define FABIA_PLL_TEST_OUT_SEL_PLL_OUTPUT_SAME_AS_PLL_LV_MAIN	0x3
#define FABIA_STATUS_REG_EN			_BV(2)
#define FABIA_SKIP_FINE_TUNE			_BV(1)
#define FABIA_SKIP_FAST_LOCK			_BV(0)

/* FABIA_OPMODE */
#define FABIA_PLL_OPMODE_OFFSET		0
#define FABIA_PLL_OPMODE_MASK		0x7
#define FABIA_PLL_OPMODE(m)		\
	_SET_FV(FABIA_PLL_OPMODE, FABIA_PLL_OPMODE_##m)
#define FABIA_PLL_OPMODE_STANDBY	0x0
#define FABIA_PLL_OPMODE_RUN		0x1
#define FABIA_PLL_OPMODE_NA		0x2
#define FABIA_PLL_OPMODE_FREEZE		0x3

#define FABIA_PLL_CALIB_MODE_ALWAYS	0x0
#define FABIA_PLL_CALIB_MODE_NEVER	0x1
#define FABIA_PLL_CALIB_MODE_AUTOMATIC	0x2

#define FABIA_PLL_FRAC_MODE_ALPHA	0x0
#define FABIA_PLL_FRAC_MODE_M_OVER_N	0x1

#define FABIA_PLL_M_N_FRAC(M, N)	\
	((((uint8_t)(M)) << 8) | ((uint8_t)(N)))

#define fabia_pll_is_enabled(base)	\
	((__raw_readl(FABIA_MODE(base)) & FABIA_MODE_PLL_ENABLED) == \
	 FABIA_MODE_PLL_ENABLED)

/* Parameters used for configuring a source that is a PLL.
 * L     - The L value for the PLL.  The PLL output frequency is deriv
 *         as out_freq = ((in_freq / nPreDiv) * (L + M/N)) / nPostDiv.
 * alpha - The Alpha(prog) value. For the 20nm PLLs, the output freque
 *         derived as :
 *         out_freq = ((in_freq / nPreDiv) * (L + ALPHAfrac)) / nPostD
 *         ALPHAprog = 2^b x ALPHAfrac where 'b' is 40 for Prius and 4
 * nPreDiv  - The pre-divider value (generally 1 or 2).
 * nPostDiv - The post-divider value (TODO: split into eve/odd for Fabia).
 * nL       - The L value for the PLL.  The PLL output frequency is
 *            derived as:
 *            out_freq = ((in_freq / nPreDiv) * (L + M/N)) / nPostDiv
 * nM       - The M value (see above).
 * nN       - The N value (see above).
 * nAlpha   - The Alpha(prog) value. For the 20nm PLLs, the output
 *            frequency is derived as :
 *            out_freq = ((in_freq / nPreDiv) * (L + ALPHAfrac)) / nPostDiv
 *            ALPHAprog = 2^b x ALPHAfrac
 *            where 'b' is 40 for Prius and 40 for Tesla.
 */
struct fabia_pll {
	caddr_t base;
	clk_t input_clk;
	uint32_t def_output_hz[NR_FREQPLANS];
	uint32_t output_hz;
	uint8_t refcnt;
};

bool __fabia_enable_pll(caddr_t base,
			uint32_t input_hz, uint32_t output_hz);

#endif /* __QDF2400_FABIA_H_INCLUDE__ */
