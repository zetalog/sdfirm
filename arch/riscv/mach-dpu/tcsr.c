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
 * @(#)tcsr.c: DPU top control/status registers implementation
 * $Id: tcsr.c,v 1.1 2020-03-09 17:22:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/cmdline.h>

#ifdef CONFIG_DPU_TCSR_ADDR_TRANS
void imc_addr_trans(int n, uint32_t in_addr, uint64_t out_addr,
		    imc_at_attr_t attr)
{
	imc_addr_set_invalid(n);
	imc_addr_set_addr_i(n, in_addr);
	imc_addr_set_addr_o(n, out_addr);
	imc_addr_set_attr(n, attr);
	imc_addr_set_valid(n);
}

static int do_tcsr_addr(int argc, char *argv[])
{
	int index;
	uint32_t in;
	uint64_t out;
	uint32_t attr = IMC_AT_ATTR_NORMAL;

	if (argc < 6)
		return -EINVAL;

	index = strtoul(argv[2], NULL, 0);
	in = strtoul(argv[3], NULL, 0);
	out = strtoull(argv[4], NULL, 0);
	if (strcmp(argv[5], "device"))
		attr = IMC_AT_ATTR_DEVICE;
	imc_addr_trans(index, in, out, attr);
	return 0;
}
#else
static inline do_tcsr_addr(int argc, char **argv)
{
	printf("DPU_TCSR_ADDR_TRANS is not configured.\n");
	return -ENODEV;
}
#endif

#ifdef CONFIG_DPU_TCSR_LOW_POWER
const char *imc_apb_names[IMC_MAX_APB_PERIPHS] = {
	"ssi_busy",
	"ssi_sleep",
	"uart_lp_req_pclk",
	"uart_lp_req_sclk",
	"i2c0_en",
	"i2c1_en",
	"i2c2_en",
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

static void do_tcsr_apb_dump(void)
{
	int periph;

	for (periph = 0; periph < IMC_MAX_APB_PERIPHS; periph++) {
		printf("%s: %s\n", imc_apb_names[periph],
		       imc_apb_is_low_power(periph) ? "off" : "on");
	}
}

static int do_tcsr_apb(int argc, char **argv)
{
	if (argc < 3)
		return -EINVAL;
	if (strcmp(argv[2], "dump") == 0) {
		do_tcsr_apb_dump();
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

static void do_tcsr_axi_dump(void)
{
	int periph;

	for (periph = 0; periph < IMC_MAX_AXI_PERIPHS; periph++) {
		printf("%s: %s\n", imc_axi_names[periph],
		       imc_axi_is_low_power(periph) ? "off" : "on");
	}
}

static int do_tcsr_axi(int argc, char **argv)
{
	int periph;

	if (argc < 3)
		return -EINVAL;
	if (strcmp(argv[2], "dump") == 0) {
		do_tcsr_axi_dump();
		return 0;
	}
	if (argc < 4)
		return -EINVAL;
	periph = imc_axi_name2periph(argv[3]);
	if (periph >= IMC_MAX_AXI_PERIPHS)
		return -EINVAL;
	if (strcmp(argv[2], "on") == 0 && imc_axi_periph_registered(periph))
		imc_axi_exit_low_power(periph);
	if (strcmp(argv[2], "off") == 0 && imc_axi_periph_registered(periph))
		imc_axi_enter_low_power(periph);
	return -ENODEV;
}
#else
static inline int do_tcsr_apb(int argc, char **argv)
{
	printf("DPU_TCSR_LOW_POWER is not configured.\n");
	return -ENODEV;
}

static inline int do_tcsr_axi(int argc, char **argv)
{
	printf("DPU_TCSR_LOW_POWER is not configured.\n");
	return -ENODEV;
}
#endif

const char *imc_boot2name(uint8_t boot_mode)
{
	switch (boot_mode) {
	case IMC_BOOT_ROM:
		return "rom";
	case IMC_BOOT_FLASH:
		return "flash";
	default:
		return "BOOT_ADDR";
	}
}

static int do_tcsr_info(int argc, char *argv[])
{
	uint8_t mode = imc_boot_mode();

	printf("Major:     %02x\n", imc_soc_major());
	printf("Minor:     %02x\n", imc_soc_minor());
	printf("Core:      %02x\n", imc_core_id());
	printf("Cluster:   %d\n", imc_cluster_id());
	printf("Boot Mode: %s\n", imc_boot2name(mode));
	if (mode == IMC_BOOT_USE_BOOT_ADDR)
		printf("Boot Addr: %08lx\n", imc_boot_addr());
	return 0;
}

static int do_tcsr_sim(int argc, char *argv[])
{
	if (strcmp(argv[2], "fail") == 0)
		imc_sim_finish(false);
	else
		imc_sim_finish(true);
	return 0;
}

static int do_tcsr(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "info") == 0)
		return do_tcsr_info(argc, argv);
	if (strcmp(argv[1], "addr") == 0)
		return do_tcsr_addr(argc, argv);
	if (strcmp(argv[1], "sim") == 0)
		return do_tcsr_sim(argc, argv);
	if (strcmp(argv[1], "apb") == 0)
		return do_tcsr_apb(argc, argv);
	if (strcmp(argv[1], "axi") == 0)
		return do_tcsr_axi(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(tcsr, do_tcsr, "Top control/status registers",
	"tcsr info\n"
	"    -dump versions, identifiers, modes\n"
	"tcsr addr id in out normal|device\n"
	"    -map AXI address space (beyond 4G)\n"
	"tcsr apb dump\n"
	"    -dump APB bus peripheral status\n"
	"tcsr axi dump\n"
	"    -dump AXI bus peripheral status\n"
	"tcsr axi on|off periph\n"
	"    -power on|off AXI bus peripheral\n"
	"tcsr sim pass|fail\n"
	"    -finish simulation with pass/fail\n"
);
