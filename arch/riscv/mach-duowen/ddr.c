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
	uint64_t f_pll_vco;
	uint32_t f_pll_clk;
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

static struct ddr_speed *ddr_get_speed(int speed)
{
	if (speed > NR_DDR_SPEEDS)
		return NULL;
	return &ddr_speeds[speed];
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

void __ddr_hw_enable_speed(uint8_t speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	if (ds) {
		if (speed > DDR2_667)
			clk_enable(ddr_clk);
		else
			clk_enable(ddr_bypass_pclk);
	}
}

void ddr_hw_config_speed(uint8_t speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	if (ds) {
		clk_apply_vco(DDR_VCO, ds->f_pll_vco);
		clk_apply_pll(DDR_PLL, ds->f_pll_clk);
	}
#ifdef CONFIG_DUOWEN_DDR_EARLY_CLOCK
	__ddr_hw_enable_speed(speed);
	crcntl_clk_assert(DDR_CLK);
	crcntl_clk_assert(DDR_ACLK);
	crcntl_clk_assert(DDR_PCLK);
	crcntl_clk_assert(DDR_POR);
	ddr_wait_dfi(8);
#endif
}

#ifndef CONFIG_DUOWEN_DDR_EARLY_CLOCK
void ddr_hw_enable_speed(uint8_t speed)
{
	__ddr_hw_enable_speed(speed);
}

void ddr_hw_ctrl_init(void)
{
	crcntl_clk_assert(DDR_CLK);
	crcntl_clk_assert(DDR_ACLK);
	crcntl_clk_assert(DDR_PCLK);
	crcntl_clk_assert(DDR_POR);
	/* No DDR_CLK is enabled, ddr_wait_dfi() is not working here. */
	udelay(1);
	dw_umctl2_init();
}
#endif

void ddr_hw_wait_dfi(uint32_t cycles)
{
	uint32_t ratio;
	tsc_count_t last;

	ratio = DIV_ROUND_UP(clk_get_frequency(ddr_clk), TSC_FREQ);
	last = DIV_ROUND_UP(cycles, ratio) + tsc_read_counter();
	while (time_before(tsc_read_counter(), last));
}
