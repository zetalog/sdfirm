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
 * @(#)scg.c: RV32M1 (VEGA) SCG (system clock generator) implementation
 * $Id: scg.c,v 1.1 2019-08-20 15:11:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/panic.h>

uint32_t scg_sirc_freq[SCG_SIRC_RANGE_MAX] = {
	[SCG_SIRC_2MHZ] = 2000000,
	[SCG_SIRC_8MHZ] = 8000000,
};

uint32_t scg_firc_freq[SCG_FIRC_RANGE_MAX] = {
	[SCG_FIRC_48MHZ] = 48000000,
	[SCG_FIRC_52MHZ] = 52000000,
	[SCG_FIRC_56MHZ] = 56000000,
	[SCG_FIRC_60MHZ] = 60000000,
};

uint32_t scg_lpfll_freq[SCG_LPFLL_FSEL_MAX] = {
	[SCG_LPFLL_48MHZ] = 48000000,
	[SCG_LPFLL_72MHZ] = 72000000,
};

uint32_t scg_input_get_freq(uint8_t scs)
{
	uint8_t range;

	if (!scg_clock_valid(scs))
		return INVALID_FREQ;

	switch (scs) {
	case SCG_SCS_EXT:
	case SCG_SCS_SOSC:
		return SCG_SOSC_FREQ;
	case SCG_SCS_SIRC:
		range = scg_clock_get_range(scs);
		if (range >= SCG_SIRC_RANGE_MAX)
			return INVALID_FREQ;
		return scg_sirc_freq[range];
	case SCG_SCS_FIRC:
		range = scg_clock_get_range(scs);
		if (range >= SCG_FIRC_RANGE_MAX)
			return INVALID_FREQ;
		return scg_firc_freq[range];
	case SCG_SCS_ROSC:
		return SCG_ROSC_FREQ;
	case SCG_SCS_LPFLL:
		range = scg_clock_get_range(scs);
		if (range >= SCG_LPFLL_FSEL_MAX)
			return INVALID_FREQ;
		return scg_lpfll_freq[range];
	}
	return 0;
}

void __scg_input_set_freq(uint8_t scs, uint32_t freq,
			  uint32_t *freqs, int nr_freqs)
{
	int i;

	for (i = 0; i < nr_freqs; i++) {
		if (freqs[i] == freq) {
			scg_clock_set_range(scs, i);
			return;
		}
	}
	BUG();
}

void scg_input_set_freq(uint8_t scs, uint32_t freq)
{
	switch (scs) {
	case SCG_SCS_EXT:
	case SCG_SCS_SOSC:
		BUG_ON(freq != SCG_SOSC_FREQ);
		break;
	case SCG_SCS_ROSC:
		BUG_ON(freq != SCG_ROSC_FREQ);
		break;
	case SCG_SCS_SIRC:
		__scg_input_set_freq(scs, freq, scg_sirc_freq,
				     SCG_SIRC_RANGE_MAX);
		break;
	case SCG_SCS_FIRC:
		__scg_input_set_freq(scs, freq, scg_firc_freq,
				     SCG_FIRC_RANGE_MAX);
		break;
	case SCG_SCS_LPFLL:
		__scg_input_set_freq(scs, freq, scg_lpfll_freq,
				     SCG_LPFLL_FSEL_MAX);
		break;
	}
}

int scg_input_disable(uint8_t scs)
{
	if (scs >= NR_SCG_CLOCKS)
		return -EINVAL;
	if (scs == SCG_SCS_ROSC || scs == SCG_SCS_EXT)
		return 0;
	if (scg_clock_selected(scs))
		return -EBUSY;
	if (scg_clock_locked(scs))
		return -EPERM;
	if (scs == SCG_SCS_SIRC)
		__raw_writel(0, SCG_SCSCSR(scs));
	else
		__raw_writel(SCG_ERR, SCG_SCSCSR(scs));
	return 0;
}

int scg_input_enable(uint8_t scs, uint32_t freq, uint32_t flags)
{
	uint32_t en = SCG_EN;
	uint32_t msk = SCG_EN | SCG_STEN | SCG_LPEN | SCG_TREN;

	if (scs >= NR_SCG_CLOCKS)
		return -EINVAL;
	if (scs == SCG_SCS_ROSC || scs == SCG_SCS_EXT)
		return 0;
	scg_input_set_freq(scs, freq);
	if (flags & SCG_STEN)
		en |= SCG_STEN;
	if (flags & SCG_LPEN)
		en |= SCG_LPEN;
	if (flags & (SCG_CM | SCG_CMRE)) {
		msk |= (SCG_CM | SCG_CMRE);
		if (flags & SCG_CM)
			en |= SCG_CM;
		else
			en |= SCG_CMRE;
	}
	/* TODO: trim support */
	if (scs == SCG_SCS_FIRC)
		en |= SCG_TREN;
	__raw_writel_mask(en, msk, SCG_SCSCSR(scs));
	/* TODO: parallel enabling */
	while (!scg_clock_valid(scs));
	return 0;
}

void scg_output_disable(uint8_t scs, uint8_t id)
{
	if (scs == SCG_SCS_ROSC || scs == SCG_SCS_EXT ||
	    scs >= NR_SCG_CLOCKS)
		return;
	__raw_writel_mask(SCG_DIV_DIV_SET(id, SCG_DIV_DIV_DISABLED),
			  SCG_DIV_DIV_MSK(id), SCG_SCSDIV(scs));
}

void scg_output_enable(uint8_t scs, uint8_t id, uint8_t div)
{
	if (scs == SCG_SCS_ROSC || scs == SCG_SCS_EXT ||
	    scs >= NR_SCG_CLOCKS)
		return;
	__raw_writel_mask(SCG_DIV_DIV_SET(id, SCG_DIV_DIV2VAL(div)),
			  SCG_DIV_DIV_MSK(id), SCG_SCSDIV(scs));
}

int scg_output_set_freq(uint8_t scs, uint8_t id, uint32_t freq)
{
	uint8_t div;
	uint32_t src_freq;

	src_freq = scg_input_get_freq(scs);
	if (src_freq == INVALID_FREQ)
		return -EINVAL;
	if (src_freq < freq)
		return -EINVAL;
	/* Support disabling of this clock */
	if (freq)
		div = div32u(src_freq, freq);
	else
		div = 0;
	if (div > SCG_DIV_DIV_MAX)
		div = SCG_DIV_DIV_MAX;
	scg_output_enable(scs, id, div);
	return 0;
}

uint32_t scg_output_get_freq(uint8_t scs, uint8_t id)
{
	uint8_t val, div;
	uint32_t src_freq;

	src_freq = scg_input_get_freq(scs);
	val = SCG_DIV_DIV_GET(id, __raw_readl(SCG_SCSDIV(scs)));
	if (val == 0 || src_freq == INVALID_FREQ)
		return INVALID_FREQ;
	div = SCG_DIV_VAL2DIV(val);
	return div32u(src_freq, div);
}

int scg_system_set_freq(uint8_t mode, uint8_t id, uint32_t freq)
{
	uint8_t div;
	uint32_t src_freq;

	if (mode >= NR_SCG_MODES)
		return -EINVAL;
	if (id == SCG_DIVCORE) {
		uint8_t scs = scg_clock_get_source(mode);
		src_freq = scg_input_get_freq(scs);
	} else
		src_freq = scg_system_get_freq(mode, SCG_DIVCORE);
	if (src_freq == INVALID_FREQ)
		return -EINVAL;
	if (src_freq < freq)
		return -EINVAL;
	/* Do not support disabling of this clock */
	if (freq == 0)
		return -EINVAL;
	div = div32u(src_freq, freq);
	if (div > SCG_CCR_DIV_MAX)
		div = SCG_CCR_DIV_MAX;
	__raw_writel_mask(SCG_CCR_DIV_SET(id, SCG_CCR_DIV2VAL(div)),
			  SCG_CCR_DIV_MSK(id), SCG_MODEREG(mode));
	return 0;
}

uint32_t scg_system_get_freq(uint8_t mode, uint8_t id)
{
	uint8_t val, div;
	uint32_t src_freq;

	if (mode >= NR_SCG_MODES)
		return INVALID_FREQ;
	if (id == SCG_DIVCORE) {
		uint8_t scs = scg_clock_get_source(mode);
		src_freq = scg_input_get_freq(scs);
	} else
		src_freq = scg_system_get_freq(mode, SCG_DIVCORE);
	val = SCG_CCR_DIV_GET(id, __raw_readl(SCG_MODEREG(mode)));
	div = SCG_CCR_VAL2DIV(val);
	if (div == 0 || src_freq == INVALID_FREQ)
		return INVALID_FREQ;
	return div32u(src_freq, div);
}

void scg_clock_select(uint8_t mode, uint8_t scs)
{
	scg_clock_set_source(mode, scs);
	/* set power mode */
	smc_power_select(mode);
	while (__scg_clock_get_source() != scs);
}
