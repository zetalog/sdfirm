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
 * @(#)clk.c: RV32M1 (VEGA) specific clock tree driver
 * $Id: clk.c,v 1.1 2019-08-19 14:24:00 zhenglv Exp $
 */

#include <errno.h>
#include <target/clk.h>

static uint32_t const_clks[] = {
	[LPOSC_CLK] = FREQ_LPOSC_CLK,
};

struct input_clk {
	uint32_t freq;
	uint32_t flags;
};

struct input_clk input_clks[] = {
	[SOSC_CLK] = {
		.freq = SCG_SOSC_FREQ,
		.flags = SCG_EN,
	},
	[SIRC_CLK] = {
		.freq = CORE_CLK_FREQ_VLPR * 2,
		.flags = SCG_EN | SCG_LPEN,
	},
	[FIRC_CLK] = {
		.freq = CORE_CLK_FREQ_RUN,
		.flags = SCG_EN,
	},
	[ROSC_CLK] = {
		.freq = SCG_ROSC_FREQ,
		.flags = 0,
	},
	[LPFLL_CLK] = {
		.freq = 72000000,
		.flags = SCG_EN,
	},
};

struct output_clk {
	uint32_t freq;
};

struct output_clk output_clks[] = {
	[SOSCDIV1_CLK] = {
		.freq = INVALID_FREQ,
	},
	[SOSCDIV2_CLK] = {
		.freq = INVALID_FREQ,
	},
	[SOSCDIV3_CLK] = {
		.freq = INVALID_FREQ,
	},
	[SIRCDIV1_CLK] = {
		.freq = INVALID_FREQ,
	},
	[SIRCDIV2_CLK] = {
		.freq = INVALID_FREQ,
	},
	[SIRCDIV3_CLK] = {
		.freq = INVALID_FREQ,
	},
	[FIRCDIV1_CLK] = {
		.freq = INVALID_FREQ,
	},
	[FIRCDIV2_CLK] = {
		.freq = INVALID_FREQ,
	},
	[FIRCDIV3_CLK] = {
		.freq = INVALID_FREQ,
	},
	[LPFLLDIV1_CLK] = {
		.freq = INVALID_FREQ,
	},
	[LPFLLDIV2_CLK] = {
		.freq = INVALID_FREQ,
	},
	[LPFLLDIV3_CLK] = {
		.freq = INVALID_FREQ,
	},
};

struct interface_clk {
	clk_t intfc;	/* bus interface clock */
	caddr_t pcc;
};

struct interface_clk interface_clks[] = {
	[MSMC_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_MSCM,
	},
	[AXBS_CLK] = {
		.intfc = plat_clk,
		.pcc = PCC_AXBS0,
	},
	[DMA0_CLK] = {
		.intfc = sys_clk,
		.pcc = PCC_DMA0,
	},
	[XRDC_MGR_CLK] = {
		.intfc = plat_clk,
		.pcc = PCC_XRDC_MGR,
	},
	[XRDC_PAC_CLK] = {
		.intfc = plat_clk,
		.pcc = PCC_XRDC_PAC,
	},
	[XRDC_MRC_CLK] = {
		.intfc = plat_clk,
		.pcc = PCC_XRDC_MRC,
	},
	[SEMA42_0_CLK] = {
		.intfc = plat_clk,
		.pcc = PCC_SEMA42_0,
	},
	[DMAMUX0_CLK] = {
		.intfc = bus_clk,
		.pcc = PCC_DMAMUX0,
	},
	[EWM_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_EWM,
	},
	[MUA_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_MUA,
	},
	[CRC_CLK] = {
		.intfc = bus_clk,
		.pcc = PCC_CRC0,
	},
	[PORTA_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_PORTA,
	},
	[PORTB_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_PORTB,
	},
	[PORTC_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_PORTC,
	},
	[PORTD_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_PORTD,
	},
	[LPDAC_CLK] = {
		.intfc = bus_clk,
		.pcc = PCC_LPDAC0,
	},
	[VREF_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_VREF,
	},
	[DMA1_CLK] = {
		.intfc = sys_clk,
		.pcc = PCC_DMA1,
	},
	[GPIOE_CLK] = {
		.intfc = plat_clk,
		.pcc = PCC_GPIOE,
	},
	[SEMA42_1_CLK] = {
		.intfc = plat_clk,
		.pcc = PCC_SEMA42_1,
	},
	[DMAMUX1_CLK] = {
		.intfc = bus_clk,
		.pcc = PCC_DMAMUX1,
	},
	[INTMUX1_CLK] = {
		.intfc = bus_clk,
		.pcc = PCC_INTMUX1,
	},
	[MUB_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_MUB,
	},
	[CAU3_CLK] = {
		.intfc = sys_clk,
		.pcc = PCC_CAU3,
	},
	[TRNG_CLK] = {
		.intfc = bus_clk,
		.pcc = PCC_TRNG,
	},
	[PORTE_CLK] = {
		.intfc = slow_clk,
		.pcc = PCC_PORTE,
	},
#if 0
	[MTB_CLK] = {
		.intfc = sys_clk,
		.pcc = PCC_MTB,
	},
	[EXT_CLK_CLK] = {
		.intfc = ext_clk,
		.pcc = PCC_EXT_CLK,
	},
#endif
};

uint8_t sys_mode = SYS_MODE_RUN;
clk_t scs_clk = sirc_clk;
clk_t scs_clkout = sirc_clk;

static uint8_t __freqplan_clk2scs(clk_t src);

static uint32_t get_const_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_CONST_CLKS)
		return INVALID_FREQ;
	return const_clks[clk];
}

struct clk_driver clk_const = {
	.max_clocks = NR_CONST_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_const_clk_freq,
	.set_freq = NULL,
};

static void apply_input_clk(clk_clk_t clk)
{
	BUG_ON(clk >= NR_INPUT_CLKS);
	scg_input_enable(CLK_SCG_SCS(clk), input_clks[clk].freq,
			 input_clks[clk].flags);
}

static int enable_input_clk(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return -EINVAL;
	apply_input_clk(clk);
	return 0;
}

static void disable_input_clk(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return;
	scg_input_disable(CLK_SCG_SCS(clk));
}

static int set_input_clk_freq(clk_clk_t clk, uint32_t freq)
{
	if (clk >= NR_INPUT_CLKS)
		return -EINVAL;
	input_clks[clk].freq = freq;
	apply_input_clk(clk);
	return 0;
}

static uint32_t get_input_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return INVALID_FREQ;
	return scg_input_get_freq(CLK_SCG_SCS(clk));
}

struct clk_driver clk_input = {
	.max_clocks = NR_INPUT_CLKS,
	.enable = enable_input_clk,
	.disable = disable_input_clk,
	.get_freq = get_input_clk_freq,
	.set_freq = set_input_clk_freq,
};

void apply_system_clk(uint8_t mode, clk_t src)
{
	BUG_ON(mode >= NR_SCG_MODES);
	clk_enable(src);
	scg_clock_select(mode, __freqplan_clk2scs(src));
}

static int enable_system_clk(clk_clk_t clk)
{
	if (clk == SYS_CLK_SRC)
		apply_system_clk(sys_mode, scs_clk);
	if (clk == CLKOUT_CLK)
		apply_system_clk(SCG_CLKOUT, scs_clkout);
	return 0;
}

static void disable_system_clk(clk_clk_t clk)
{
	if (clk == SYS_CLK_SRC)
		freqplan_config_boot();
	if (clk == CLKOUT_CLK)
		scg_clock_select(SCG_CLKOUT, SCG_SCS_EXT);
}

static uint32_t get_system_clk_freq(clk_clk_t clk)
{
	if (clk == SYS_CLK_SRC)
		return clk_get_frequency(scs_clk);
	return INVALID_FREQ;
}

struct clk_driver clk_system = {
	.max_clocks = NR_SYSTEM_CLKS,
	.enable = enable_system_clk,
	.disable = disable_system_clk,
	.get_freq = get_system_clk_freq,
	.set_freq = NULL,
};

static int apply_output_clk(clk_clk_t clk)
{
	uint32_t src_freq;
	clk_t src;
	uint32_t freq;

	freq = output_clks[clk].freq;
	if (freq == INVALID_FREQ)
		return -EINVAL;
	if (clk < NR_OUTPUT_CCRS) {
		if (clk == CORE_CLK)
			src = freqplan_clk2scs();
		else
			src = core_clk;
	} else
		src = CLK_DIV_CLKID(clk);
	src_freq = clk_get_frequency(src);
	if (src_freq == INVALID_FREQ)
		clk_enable(src);
	src_freq = clk_get_frequency(src);
	if (src_freq == INVALID_FREQ)
		return -EINVAL;

	if (clk < NR_OUTPUT_CCRS)
		scg_system_set_freq(sys_mode, CLK_CCR_DIVID(clk), freq);
	else
		scg_output_set_freq(src, CLK_DIV_DIVID(clk), freq);
	return 0;
}

static int enable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;
	return apply_output_clk(clk);
}

static void disable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return;
	if (clk < NR_OUTPUT_CCRS)
		freqplan_config_boot();
	else
		scg_output_disable(CLK_DIV_SCS(clk),
				   CLK_DIV_DIVID(clk));
}

static int set_output_clk_freq(clk_clk_t clk, uint32_t freq)
{
	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;
	output_clks[clk].freq = freq;
	return apply_output_clk(clk);
}

static uint32_t get_output_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return INVALID_FREQ;
	if (clk < NR_OUTPUT_CCRS)
		return scg_system_get_freq(sys_mode,
					   CLK_CCR_DIVID(clk));
	else
		return scg_output_get_freq(CLK_DIV_SCS(clk),
					   CLK_DIV_DIVID(clk));
}

struct clk_driver clk_output = {
	.max_clocks = NR_OUTPUT_CLKS,
	.enable = enable_output_clk,
	.disable = disable_output_clk,
	.get_freq = get_output_clk_freq,
	.set_freq = set_output_clk_freq,
};

static int enable_interface_clk(clk_clk_t clk)
{
	if (clk >= NR_INTERFACE_CLKS)
		return -EINVAL;
	clk_enable(interface_clks[clk].intfc);
	pcc_enable_clk(interface_clks[clk].pcc);
	return 0;
}

static void disable_interface_clk(clk_clk_t clk)
{
	if (clk >= NR_INTERFACE_CLKS)
		return;
	pcc_disable_clk(interface_clks[clk].pcc);
	return;
}

static uint32_t get_interface_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_INTERFACE_CLKS)
		return -EINVAL;
	return clk_get_frequency(interface_clks[clk].intfc);
}

struct clk_driver clk_interface = {
	.max_clocks = NR_INTERFACE_CLKS,
	.enable = enable_interface_clk,
	.disable = disable_interface_clk,
	.get_freq = get_interface_clk_freq,
	.set_freq = NULL,
};

clk_t freqplan_scs2clk(void)
{
	uint8_t scs = __scg_clock_get_source();
	if (scs == SCG_SCS_EXT || scs >= NR_SCG_CLOCKS)
		scs = SCG_SCS_SOSC;
	return clkid(CLK_INPUT, scs - 1);
}

static uint8_t __freqplan_clk2scs(clk_t src)
{
	uint8_t clk = clk_clk(src);
	if (clk < NR_INPUT_CLKS)
		return clk + 1;
	return SCG_SCS_EXT;
}

uint8_t freqplan_clk2scs(void)
{
	return __freqplan_clk2scs(scs_clk);
}

#ifdef CONFIG_VEGA_BOOT_CPU
struct clk_src {
	clk_t clk;
	uint32_t freq;
};

struct clk_src freqplan_run[] = {
	{
		.clk = sirc_clk,
		.freq = 8000000,
	}, {
		.clk = firc_clk,
		.freq = 48000000,
	}, {
		.clk = fircdiv1_clk,
		.freq = 48000000,
	}, {
		.clk = fircdiv2_clk,
		.freq = 48000000,
	}, {
		.clk = fircdiv3_clk,
		.freq = 48000000,
	}, {
		.clk = sircdiv3_clk,
		.freq = 8000000,
	}, {
		.clk = core_clk,
		.freq = 48000000,
	}, {
		.clk = bus_clk,
		.freq = 48000000,
	}, {
		.clk = slow_clk,
		.freq = 24000000,
	}, {
		.clk = invalid_clk,
		.freq = INVALID_FREQ,
	},
};

struct clk_src freqplan_vlpr[] = {
	{
		.clk = sirc_clk,
		.freq = 8000000,
	}, {
		.clk = firc_clk,
		.freq = 48000000,
	}, {
		.clk = sircdiv1_clk,
		.freq = 8000000,
	}, {
		.clk = sircdiv2_clk,
		.freq = 8000000,
	}, {
		.clk = sircdiv3_clk,
		.freq = 8000000,
	}, {
		.clk = fircdiv1_clk,
		.freq = 48000000,
	}, {
		.clk = core_clk,
		.freq = 4000000,
	}, {
		.clk = bus_clk,
		.freq = 2000000,
	}, {
		.clk = slow_clk,
		.freq = 4000000 / 9,
	}, {
		.clk = invalid_clk,
		.freq = INVALID_FREQ,
	},
};

struct clk_src freqplan_hsrun[] = {
	{
		.clk = sirc_clk,
		.freq = 8000000,
	}, {
		.clk = firc_clk,
		.freq = 48000000,
	}, {
		.clk = lpfll_clk,
		.freq = 72000000,
	}, {
		.clk = sircdiv3_clk,
		.freq = 8000000,
	}, {
		.clk = fircdiv1_clk,
		.freq = 48000000,
	}, {
		.clk = fircdiv2_clk,
		.freq = 48000000,
	}, {
		.clk = fircdiv3_clk,
		.freq = 48000000,
	}, {
		.clk = core_clk,
		.freq = 72000000,
	}, {
		.clk = bus_clk,
		.freq = 72000000,
	}, {
		.clk = slow_clk,
		.freq = 8000000,
	}, {
		.clk = invalid_clk,
		.freq = INVALID_FREQ,
	},
};

void freqplan_config_boot(void)
{
	uint32_t freq = CORE_CLK_FREQ_VLPR * 2;

	/* configure SIRC as boot clock */
	scg_input_disable(SCG_SCS_SIRC);
	scg_output_disable(SCG_SCS_SIRC, SCG_DIV1);
	scg_output_set_freq(SCG_SCS_SIRC, SCG_DIV2, freq / 2);
	scg_output_set_freq(SCG_SCS_SIRC, SCG_DIV3, freq);
	scg_input_enable(SCG_SCS_SIRC, freq, SCG_EN);

	/* configure runtime clock */
	scg_clock_select(SYS_MODE_RUN, SCG_SCS_SIRC);
	scg_system_set_freq(SYS_MODE_RUN, SCG_DIVCORE, freq);
	scg_system_set_freq(SYS_MODE_RUN, SCG_DIVBUS, freq);
	scg_system_set_freq(SYS_MODE_RUN, SCG_DIVSLOW, freq / 4);
}

void freqplan_apply(struct clk_src *plan)
{
	while (plan && plan->clk != invalid_clk) {
		clk_set_frequency(plan->clk, plan->freq);
		plan++;
	}
}

void freqplan_config_run(void)
{
	freqplan_config_boot();
	sys_mode = SYS_MODE_RUN;
	scs_clk = firc_clk;
	freqplan_apply(freqplan_run);
}

void freqplan_config_vlpr(void)
{
	sys_mode = SYS_MODE_VLPR;
	scs_clk = sirc_clk;
	freqplan_apply(freqplan_vlpr);
}

void freqplan_config_hsrun(void)
{
	freqplan_config_boot();
	sys_mode = SYS_MODE_HSRUN;
	scs_clk = lpfll_clk;
	freqplan_apply(freqplan_hsrun);
}
#endif

int clk_hw_ctrl_init(void)
{
	clk_register_driver(CLK_CONST, &clk_const);
	clk_register_driver(CLK_INPUT, &clk_input);
	clk_register_driver(CLK_SYSTEM, &clk_system);
	clk_register_driver(CLK_OUTPUT, &clk_output);
	clk_register_driver(CLK_INTERFACE, &clk_interface);
	freqplan_config_run();
	return 0;
}
