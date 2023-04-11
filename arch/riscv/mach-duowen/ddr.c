/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)ddr.c: DUOWEN ASIC DDR frequency plan implementation
 * $Id: ddr.c,v 1.1 2020-08-08 09:52:00 zhenglv Exp $
 */

#include <target/ddr.h>
#include <target/delay.h>
#include <target/jiffies.h>

struct ddr_speed {
	clk_freq_t f_pll_vco;
	clk_freq_t f_pll_clk;
};

struct ddr_speed ddr_speeds[NR_DDR_SPEEDS] = {
	[DDR4_3200] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_clk = UL(800000000),
	},
	[DDR4_2933] = {
		.f_pll_vco = ULL(2933333333),
		.f_pll_clk = UL(733333333),
	},
	[DDR4_2666] = {
		.f_pll_vco = ULL(2666666666),
		.f_pll_clk = UL(666666666),
	},
	[DDR4_2400] = {
		.f_pll_vco = ULL(4800000000),
		.f_pll_clk = UL(600000000),
	},
	[DDR4_2133] = {
		.f_pll_vco = ULL(4266666666),
		.f_pll_clk = UL(533333333),
	},
	[DDR4_1866] = {
		.f_pll_vco = ULL(3733333333),
		.f_pll_clk = UL(466666666),
	},
	[DDR4_1600] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_clk = UL(400000000),
	},
	[DDR3_1333] = {
		.f_pll_vco = ULL(2666666666),
		.f_pll_clk = UL(333333333),
	},
	[DDR3_1066] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_clk = UL(266666666),
	},
	[DDR3_800] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_clk = UL(200000000),
	},
	[DDR2_667] = {
		.f_pll_vco = ULL(2666666666),
		.f_pll_clk = UL(666666666),
	},
	[DDR2_533] = {
		.f_pll_vco = ULL(4266666666),
		.f_pll_clk = UL(533333333),
	},
	[DDR2_400] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_clk = UL(400000000),
	},
	[DDR_266] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_clk = UL(266666666),
	},
	[DDR_200] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_clk = UL(200000000),
	},
};

static ddr_cid_t ddr_cids[NR_DDR_SLOTS] = {
	0, 0, 1, 1,
};

/* TODO: Obtain correct SPD address */
static uint8_t ddr_spd_addrs[NR_DDR_SLOTS] = {
	0x50, 0x50, 0x50, 0x50
};

static struct ddr_speed *ddr_get_speed(int speed)
{
	if (speed > NR_DDR_SPEEDS)
		return NULL;
	return &ddr_speeds[speed];
}

int ddr_clk_speed(clk_freq_t freq)
{
	int speed;

	for (speed = 0; speed < NR_DDR_SPEEDS; speed++) {
		if (ddr_speeds[speed].f_pll_clk == freq)
			return speed;
	}
	return DDR_SPEED_DEFAULT;
}

clk_freq_t ddr_get_fvco(int speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	return ds ? ds->f_pll_vco : INVALID_FREQ;
}

clk_freq_t ddr_get_fclk(int speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	return ds ? ds->f_pll_clk: INVALID_FREQ;
}

/* XXX: Dual Socket Runtime
 *
 * DDR speed APIs are developed for being used for single socket runtime,
 * or dual socket bootstrap, cannot be applied to dual socket runtime.
 * With the following untested interfaces, the APIs now can be applied to
 * dual socket runtime.
 */
#ifdef CONFIG_DUOWEN_SBI_DUAL
static void __ddr_hw_enable_speed2(uint8_t speed)
{
	if (speed > DDR2_667)
		clk_enable(ddr_clk2);
	else
		clk_enable(ddr_bypass_pclk2);
}

static void __ddr_hw_config_speed2(clk_freq_t fvco, clk_freq_t fpll)
{
	clk_apply_vco(DDR_VCO2, DDR_VCO, fvco);
	clk_apply_pll(DDR_PLL2, DDR_PLL, fpll);
}
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define __ddr_hw_enable_speed2(speed)		do { } while (0)
#define __ddr_hw_config_speed2(fvco, fpll)	do { } while (0)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

void ddr_hw_enable_speed(uint8_t speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	if (ds) {
		if (speed > DDR2_667)
			clk_enable(ddr_clk);
		else
			clk_enable(ddr_bypass_pclk);
		__ddr_hw_enable_speed2(speed);
	}
}

void ddr_hw_config_speed(uint8_t speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	if (ds) {
		clk_apply_vco(DDR_VCO, DDR_VCO, ds->f_pll_vco);
		clk_apply_pll(DDR_PLL, DDR_PLL, ds->f_pll_clk);
		__ddr_hw_config_speed2(ds->f_pll_vco, ds->f_pll_clk);
	}
}

void ddr_hw_wait_dfi(uint32_t cycles)
{
	uint32_t ratio;
	tsc_count_t last;

	ratio = DIV_ROUND_UP(clk_get_frequency(ddr_clk), TSC_FREQ);
	last = DIV_ROUND_UP(cycles, ratio) + tsc_read_counter();
	while (time_before(tsc_read_counter(), last));
}

void ddr_hw_slot_reset(void)
{
	ddr_slot_ctrl.cid = ddr_cids[ddr_sid];
	ddr_slot_ctrl.smbus = 0;
	ddr_slot_ctrl.spd_addr = ddr_spd_addrs[ddr_sid];
}

void ddr_hw_slot_select(ddr_sid_t sid)
{
}

void ddr_hw_chan_reset(void)
{
}

void ddr_hw_chan_select(ddr_cid_t cid)
{
}
