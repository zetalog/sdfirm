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

#include <target/clk.h>
#include <target/panic.h>

static uint32_t const_clks[] = {
	[LPOSC_CLK] = FREQ_LPOSC_CLK,
};

struct input_clk {
	uint32_t freq;
	uint32_t flags;
};

struct input_clk input_clks[NR_INPUT_CLKS] = {
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

struct output_clk output_clks[NR_OUTPUT_CLKS] = {
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
	caddr_t pcc;
	clk_t intfc;	/* bus interface clock */
};

struct interface_clk interface_clks[NR_INTERFACE_CLKS] = {
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

struct functional_clk {
	caddr_t pcc;
	clk_t *funcs;
	uint8_t pcs;
	uint32_t freq;
};

clk_t div1_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = invalid_clk,
	[PCC_PCS_SOSC] = soscdiv1_clk,
	[PCC_PCS_SIRC] = sircdiv1_clk,
	[PCC_PCS_FIRC] = fircdiv1_clk,
	[PCC_PCS_LPFLL] = lpflldiv1_clk,
};

clk_t div2_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = invalid_clk,
	[PCC_PCS_SOSC] = soscdiv2_clk,
	[PCC_PCS_SIRC] = sircdiv2_clk,
	[PCC_PCS_FIRC] = fircdiv2_clk,
	[PCC_PCS_LPFLL] = lpflldiv2_clk,
};

clk_t div3_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = invalid_clk,
	[PCC_PCS_SOSC] = soscdiv3_clk,
	[PCC_PCS_SIRC] = sircdiv3_clk,
	[PCC_PCS_FIRC] = fircdiv3_clk,
	[PCC_PCS_LPFLL] = lpflldiv3_clk,
};

clk_t rtcosc_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = rtcosc_clk,
	[PCC_PCS_SOSC] = soscdiv2_clk,
	[PCC_PCS_SIRC] = sircdiv2_clk,
	[PCC_PCS_FIRC] = fircdiv2_clk,
	[PCC_PCS_LPFLL] = lpflldiv2_clk,
};

clk_t sai_m_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = sai_mclk,
	[PCC_PCS_SOSC] = soscdiv2_clk,
	[PCC_PCS_SIRC] = sircdiv2_clk,
	[PCC_PCS_FIRC] = fircdiv2_clk,
	[PCC_PCS_LPFLL] = lpflldiv2_clk,
};

clk_t sdhc_d_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = sdhc_dclk,
	[PCC_PCS_SOSC] = soscdiv1_clk,
	[PCC_PCS_SIRC] = sircdiv1_clk,
	[PCC_PCS_FIRC] = fircdiv1_clk,
	[PCC_PCS_LPFLL] = lpflldiv1_clk,
};

clk_t irc48m_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = irc48m_clk,
	[PCC_PCS_SOSC] = soscdiv1_clk,
	[PCC_PCS_SIRC] = sircdiv1_clk,
	[PCC_PCS_FIRC] = fircdiv1_clk,
	[PCC_PCS_LPFLL] = lpflldiv1_clk,
};

clk_t lpadcirc_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = lpadcirc_clk,
	[PCC_PCS_SOSC] = soscdiv2_clk,
	[PCC_PCS_SIRC] = sircdiv2_clk,
	[PCC_PCS_FIRC] = fircdiv2_clk,
	[PCC_PCS_LPFLL] = lpflldiv2_clk,
};

clk_t sys_funcs[NR_PCC_PCS_CLOCKS] = {
	[PCC_PCS_EXT] = sys_clk,
	[PCC_PCS_SOSC] = soscdiv1_clk,
	[PCC_PCS_SIRC] = sircdiv1_clk,
	[PCC_PCS_FIRC] = fircdiv1_clk,
	[PCC_PCS_LPFLL] = lpflldiv1_clk,
};

struct functional_clk functional_clks[NR_FUNCTIONAL_CLKS] = {
	[LPIT0_CLK] = {
		.pcc = PCC_LPIT0,
		.funcs = div3_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPIT1_CLK] = {
		.pcc = PCC_LPIT1,
		.funcs = div3_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[TPM0_CLK] = {
		.pcc = PCC_TPM0,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[TPM1_CLK] = {
		.pcc = PCC_TPM1,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[TPM2_CLK] = {
		.pcc = PCC_TPM2,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[TPM3_CLK] = {
		.pcc = PCC_TPM3,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPI2C0_CLK] = {
		.pcc = PCC_LPI2C0,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPI2C1_CLK] = {
		.pcc = PCC_LPI2C1,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPI2C2_CLK] = {
		.pcc = PCC_LPI2C2,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPI2C3_CLK] = {
		.pcc = PCC_LPI2C3,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPSPI0_CLK] = {
		.pcc = PCC_LPSPI0,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPSPI1_CLK] = {
		.pcc = PCC_LPSPI1,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPSPI2_CLK] = {
		.pcc = PCC_LPSPI2,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPSPI3_CLK] = {
		.pcc = PCC_LPSPI3,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPUART0_CLK] = {
		.pcc = PCC_LPUART0,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPUART1_CLK] = {
		.pcc = PCC_LPUART1,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPUART2_CLK] = {
		.pcc = PCC_LPUART2,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPUART3_CLK] = {
		.pcc = PCC_LPUART3,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[EMVSIM_CLK] = {
		.pcc = PCC_EMVSIM0,
		.funcs = rtcosc_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[FLEXIO_CLK] = {
		.pcc = PCC_FLEXIO0,
		.funcs = div2_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPI2S_CLK] = {
		.pcc = PCC_I2S0,
		.funcs = sai_m_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[USDHC_CLK] = {
		.pcc = PCC_USDHC0,
		.funcs = sdhc_d_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[USB_CLK] = {
		.pcc = PCC_USB0,
		.funcs = irc48m_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[LPADC_CLK] = {
		.pcc = PCC_LPADC0,
		.funcs = lpadcirc_funcs,
		.pcs = PCC_PCS_EXT,
	},
	[TRACE_CLK] = {
		.pcc = PCC_TRACE,
		.funcs = sys_funcs,
		.pcs = PCC_PCS_EXT,
	},
};

uint8_t functional_mux[NR_FUNCTIONAL_CLKS] = {
	[LPUART0_CLK] = PCC_PCS_FIRC,
};

uint8_t sys_mode = SYS_MODE_RUN;
uint8_t sys_new_mode = SYS_MODE_RUN;
clk_t scs_clk = sirc_clk;
clk_t scs_clkout = sirc_clk;

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

static int apply_input_clk(clk_clk_t clk)
{
	int ret;
	uint8_t scs;
	uint32_t freq;

	BUG_ON(clk >= NR_INPUT_CLKS);
	scs = CLK_SCG_SCS(clk);
	freq = input_clks[clk].freq;
	if (scg_clock_selected(scs)) {
		if (scg_input_get_freq(scs) == freq)
			goto exit_flags;
		return -EBUSY;
	}
	ret = scg_input_disable(scs);
	if (ret)
		return ret;
exit_flags:
	return scg_input_enable(scs, freq, input_clks[clk].flags);
}

static int enable_input_clk(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return -EINVAL;
	return apply_input_clk(clk);
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
	return apply_input_clk(clk);
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

int apply_system_clk(uint8_t mode, clk_t src)
{
	int ret;

	BUG_ON(mode >= NR_SCG_MODES);
	ret = clk_enable(src);
	if (ret)
		return ret;
	scg_clock_select(mode, freqplan_clk2scs(src));
	return 0;
}

static int enable_system_clk(clk_clk_t clk)
{
	int ret = 0;

	if (clk == SYS_CLK_SRC) {
		ret = apply_system_clk(sys_new_mode, scs_clk);
		if (ret == 0)
			sys_mode = sys_new_mode;
	}
	if (clk == CLKOUT_CLK)
		ret = apply_system_clk(SCG_CLKOUT, scs_clkout);
	return ret;
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
	if (clk == SYS_CLK_SRC && sys_mode == sys_new_mode &&
	    freqplan_clk2scs(scs_clk) == scg_clock_get_source(sys_mode))
		return clk_get_frequency(scs_clk);
	if (clk == CLKOUT_CLK &&
	    freqplan_clk2scs(scs_clkout) == scg_clock_get_source(SCG_SCS_EXT))
		return clk_get_frequency(scs_clkout);
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
			src = sys_clk_src;
		else
			src = core_clk;
	} else
		src = CLK_DIV_CLKID(clk);
	src_freq = clk_get_frequency(src);
	if (src_freq == INVALID_FREQ) {
		clk_enable(src);
		src_freq = clk_get_frequency(src);
	}
	if (src_freq == INVALID_FREQ)
		return -EINVAL;

	if (clk < NR_OUTPUT_CCRS)
		scg_system_set_freq(sys_mode, CLK_CCR_DIVID(clk), freq);
	else
		scg_output_set_freq(CLK_DIV_SCS(clk),
				   CLK_DIV_DIVID(clk), freq);
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

clk_t get_functional_pcs_clk(clk_clk_t clk)
{
	uint8_t pcs;

	if (clk >= NR_FUNCTIONAL_CLKS)
		return invalid_clk;
	pcs = functional_clks[clk].pcs;
	if (pcs >= NR_PCC_PCS_CLOCKS ||
	    ((1 << pcs) & INVALID_PCC_PCS))
		return invalid_clk;

	return functional_clks[clk].funcs[pcs];
}

static int enable_functional_clk(clk_clk_t clk)
{
	int ret;
	clk_t src_clk;

	src_clk = get_functional_pcs_clk(clk);
	if (src_clk == invalid_clk)
		return -EINVAL;
	ret = clk_enable(src_clk);
	if (ret)
		return ret;
	pcc_select_source(functional_clks[clk].pcc,
			  functional_clks[clk].pcs);
	return 0;
}

static void disable_functional_clk(clk_clk_t clk)
{
	if (clk >= NR_FUNCTIONAL_CLKS)
		return;
	pcc_disable_clk(functional_clks[clk].pcc);
}

static int set_functional_clk_freq(clk_clk_t clk, uint32_t freq)
{
	uint32_t src_freq;
	clk_t src_clk;

	src_clk = get_functional_pcs_clk(clk);
	if (src_clk == invalid_clk)
		return -EINVAL;
	src_freq = clk_get_frequency(src_clk);
	if (src_freq < freq || freq == 0)
		return -EINVAL;
	if (clk == LPADC_CLK || clk == TRACE_CLK) {
		functional_clks[clk].freq = freq;
		pcc_config_divider(functional_clks[clk].pcc,
				   src_freq, freq);
	}
	return 0;
}

static uint32_t get_functional_clk_freq(clk_clk_t clk)
{
	uint32_t src_freq;
	clk_t src_clk;

	src_clk = get_functional_pcs_clk(clk);
	if (src_clk == invalid_clk)
		return -EINVAL;
	src_freq = clk_get_frequency(src_clk);
	if (clk != LPADC_CLK && clk != TRACE_CLK)
		return src_freq;
	return functional_clks[clk].freq;
}

struct clk_driver clk_functional = {
	.max_clocks = NR_FUNCTIONAL_CLKS,
	.enable = enable_functional_clk,
	.disable = disable_functional_clk,
	.get_freq = get_functional_clk_freq,
	.set_freq = set_functional_clk_freq,
};

clk_t freqplan_scs2clk(void)
{
	uint8_t scs = __scg_clock_get_source();
	if (scs == SCG_SCS_EXT || scs >= NR_SCG_CLOCKS)
		scs = SCG_SCS_SOSC;
	return clkid(CLK_INPUT, scs - 1);
}

uint8_t freqplan_clk2scs(clk_t src)
{
	uint8_t clk = clk_clk(src);
	if (clk < NR_INPUT_CLKS)
		return CLK_SCG_SCS(clk);
	return SCG_SCS_EXT;
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

#define FREQ_SIRC_CLK		8000000
#define FREQ_FIRC_CLK		48000000

void freqplan_config_boot(void)
{
	uint32_t freq = FREQ_SIRC_CLK;

	/* No HSRUN/LP support now */
	BUG_ON(sys_mode != SYS_MODE_RUN);
	/* configure SIRC as boot clock */
	if (!scg_clock_selected(SCG_SCS_SIRC)) {
		scg_input_disable(SCG_SCS_SIRC);
		scg_input_enable(SCG_SCS_SIRC, freq, SCG_EN);
	}
	/* configure runtime clock */
	scg_clock_select(SYS_MODE_RUN, SCG_SCS_SIRC);
}

void freqplan_apply(struct clk_src *plan)
{
	int i;

	while (plan && plan->clk != invalid_clk) {
		clk_set_frequency(plan->clk, plan->freq);
		plan++;
	}

	for (i = 0; i < ARRAY_SIZE(functional_mux); i++) {
		if (functional_mux[i] != PCC_PCS_OFF)
			functional_clks[i].pcs = functional_mux[i];
	}
}

void freqplan_config_run(void)
{
	freqplan_config_boot();
	sys_new_mode = SYS_MODE_RUN;
	scs_clk = firc_clk;
	freqplan_apply(freqplan_run);
}

void freqplan_config_vlpr(void)
{
	sys_new_mode = SYS_MODE_VLPR;
	scs_clk = sirc_clk;
	freqplan_apply(freqplan_vlpr);
}

void freqplan_config_hsrun(void)
{
	freqplan_config_boot();
	sys_new_mode = SYS_MODE_HSRUN;
	scs_clk = lpfll_clk;
	freqplan_apply(freqplan_hsrun);
}
#endif

void clk_hw_ctrl_init(void)
{
	clk_register_driver(CLK_CONST, &clk_const);
	clk_register_driver(CLK_INPUT, &clk_input);
	clk_register_driver(CLK_SYSTEM, &clk_system);
	clk_register_driver(CLK_OUTPUT, &clk_output);
	clk_register_driver(CLK_INTERFACE, &clk_interface);
	clk_register_driver(CLK_FUNCTIONAL, &clk_functional);
	freqplan_config_run();
}
