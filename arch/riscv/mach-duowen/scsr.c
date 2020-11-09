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
 * @(#)scsr.h: DUOWEN SysCSR register usage models
 * $Id: scsr.h,v 1.1 2020-11-09 22:46:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/cmdline.h>

static void __pma_cfg(int n, unsigned long attr)
{
	unsigned long cfgmask, pmacfg;
	int pmacfg_index, pmacfg_shift;
	bool tor = ((attr & PMA_A) == PMA_A_TOR);

	/* calculate PMA register and offset */
	pmacfg_index = REG64_16BIT_INDEX(tor ? n + 1 : n);
	pmacfg_shift = REG64_16BIT_OFFSET(tor ? n + 1 : n);

	cfgmask = ~(UL(0xffff) << pmacfg_shift);
	pmacfg	= imc_pma_read_cfg(pmacfg_index) & cfgmask;
	pmacfg |= ((attr << pmacfg_shift) & ~cfgmask);

	imc_pma_write_cfg(pmacfg_index, pmacfg);
}

int imc_pma_set(int n, unsigned long attr,
		phys_addr_t addr, unsigned long log2len)
{
	unsigned long addrmask, pmaaddr;
	bool tor = !IS_ALIGNED(addr, PMA_GRAIN_ALIGN) || log2len < PMA_GRAIN_SHIFT;

	/* check parameters */
	if (n >= PMA_COUNT || log2len > __riscv_xlen || log2len < PMA_SHIFT)
		return -EINVAL;

	/* encode PMA config */
	attr |= tor ? PMA_A_TOR :
		((log2len == PMA_SHIFT) ? PMA_A_NA4 : PMA_A_NAPOT);

	if (tor) {
		imc_pma_write_addr(n, addr);
		imc_pma_write_addr(n + 1, addr + (1 << log2len));
		__pma_cfg(n, attr);
		return 2;
	}

	/* encode PMA address */
	if (log2len == PMA_SHIFT) {
		pmaaddr = (addr >> PMA_SHIFT);
	} else {
		if (log2len == __riscv_xlen) {
			pmaaddr = -UL(1);
		} else {
			addrmask = (UL(1) << (log2len - PMA_SHIFT)) - 1;
			pmaaddr	 = ((addr >> PMA_SHIFT) & ~addrmask);
			pmaaddr |= (addrmask >> 1);
		}
	}

	/* write csrs */
	imc_pma_write_addr(n, pmaaddr);
	__pma_cfg(n, attr);
	return 1;
}

const char *imc_apb_names[IMC_MAX_APB_PERIPHS] = {
	"uart3_lp_req_sclk",
	"uart3_lp_req_pclk",
	"uart2_lp_req_sclk",
	"uart2_lp_req_pclk",
	"uart1_lp_req_sclk",
	"uart1_lp_req_pclk",
	"uart0_lp_req_sclk",
	"uart0_lp_req_pclk",
};

#if 0
static int imc_apb_name2periph(const char *name)
{
	int periph;

	for (periph = 0; periph < IMC_MAX_APB_PERIPHS; periph++) {
		if (strcmp(name, imc_apb_names[periph]) == 0)
			return periph;
	}
	return IMC_MAX_APB_PERIPHS;
}
#endif

static void do_scsr_apb_dump(void)
{
	int periph;

	for (periph = 0; periph < IMC_MAX_APB_PERIPHS; periph++) {
		printf("%s: %s\n", imc_apb_names[periph],
		       imc_apb_is_low_power(periph) ? "off" : "on");
	}
}

static int do_scsr_apb(int argc, char **argv)
{
	if (argc < 3)
		return -EINVAL;
	if (strcmp(argv[2], "dump") == 0) {
		do_scsr_apb_dump();
		return 0;
	}
	return -ENODEV;
}

const char *imc_axi_names[IMC_MAX_AXI_PERIPHS] = {
	"pcie_x16_mst",
	"pcie_x16_slv",
	"pcie_x8_mst",
	"pcie_x8_slv",
	"pcie_x4_0_mst",
	"pcie_x4_0_slv",
	"pcie_x4_1_mst",
	"pcie_x4_1_slv",
	"pcie_x16_dbi",
	"pcie_x8_dbi",
	"pcie_x4_0_dbi",
	"pcie_x4_1_dbi",
	"ddr0",
	"ddr0_ctrl",
	"ddr1",
	"ddr1_ctrl",
};

uint16_t imc_axi_periphs;

#define imc_axi_periph_registered(periph)	(_BV(periph) & imc_axi_periphs)

void imc_axi_register_periphs(uint16_t periphs)
{
	imc_axi_periphs |= periphs;
}

void imc_axi_unregister_periphs(uint16_t periphs)
{
	imc_axi_periphs &= ~periphs;
}

static int imc_axi_name2periph(const char *name)
{
	int periph;

	for (periph = 0; periph < IMC_MAX_AXI_PERIPHS; periph++) {
		if (strcmp(name, imc_axi_names[periph]) == 0)
			return periph;
	}
	return IMC_MAX_AXI_PERIPHS;
}

static void do_scsr_axi_dump(void)
{
	int periph;

	for (periph = 0; periph < IMC_MAX_AXI_PERIPHS; periph++) {
		printf("%s: %s\n", imc_axi_names[periph],
		       imc_axi_is_low_power(periph) ? "off" : "on");
	}
}

static int do_scsr_axi(int argc, char **argv)
{
	int periph;

	if (argc < 3)
		return -EINVAL;
	if (strcmp(argv[2], "dump") == 0) {
		do_scsr_axi_dump();
		return 0;
	}
	if (argc < 4)
		return -EINVAL;
	periph = imc_axi_name2periph(argv[3]);
	if (periph >= IMC_MAX_AXI_PERIPHS) {
		printf("%s is an unknown AXI peripheral!\n", argv[3]);
		return -EINVAL;
	}
	if (!imc_axi_periph_registered(periph)) {
		printf("%s is not in a reset active state!\n", argv[3]);
		return -EINVAL;
	}
	if (strcmp(argv[2], "on") == 0)
		imc_axi_exit_low_power(periph);
	if (strcmp(argv[2], "off") == 0)
		imc_axi_enter_low_power(periph);
	return -ENODEV;
}

static int do_scsr(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "apb") == 0)
		return do_scsr_apb(argc, argv);
	if (strcmp(argv[1], "axi") == 0)
		return do_scsr_axi(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(scsr, do_scsr, "System control/status registers",
	"scsr apb dump\n"
	"    -dump APB bus peripheral status\n"
	"scsr axi dump\n"
	"    -dump AXI bus peripheral status\n"
	"scsr axi on|off periph\n"
	"    -power on|off AXI bus peripheral\n"
);
