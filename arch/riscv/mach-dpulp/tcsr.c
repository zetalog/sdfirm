/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)tcsr.c: DPU-LP top control/status registers implementation
 * $Id: tcsr.c,v 1.1 2021-11-01 15:50:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/cmdline.h>

const char *tcsr_boot2name(uint8_t boot_mode)
{
	switch (boot_mode) {
	case TCSR_BOOT_ROM:
		return "rom";
	case TCSR_BOOT_SPI:
		return "spi";
	case TCSR_BOOT_RAM:
		return "ram";
	case TCSR_BOOT_DDR:
		return "ddr";
	default:
		return "BOOT_ADDR";
	}
}

const char *tcsr_load2name(uint8_t load_mode)
{
	switch (load_mode) {
	case TCSR_LOAD_SSI:
		return "ssi";
	case TCSR_LOAD_SD:
		return "sd";
	default:
		return "unknown";
	}
}

static int do_tcsr_info(int argc, char *argv[])
{
	uint8_t boot = tcsr_boot_from();
	uint8_t load = tcsr_load_to();

	printf("Major:      %02x\n", tcsr_soc_major());
	printf("Minor:      %02x\n", tcsr_soc_minor());
	printf("Boot from:  %s\n", tcsr_boot2name(boot));
	printf("Load to:    %s\n", tcsr_load2name(load));
	return 0;
}

static int do_tcsr_sim(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "fail") == 0)
		tcsr_sim_finish(false);
	else
		tcsr_sim_finish(true);
	return 0;
}

static int do_tcsr(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "info") == 0)
		return do_tcsr_info(argc, argv);
	if (strcmp(argv[1], "sim") == 0)
		return do_tcsr_sim(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(tcsr, do_tcsr, "Top control/status registers",
	"tcsr info\n"
	"    -dump versions, identifiers, modes\n"
	"tcsr sim pass|fail\n"
	"    -finish simulation with pass/fail\n"
);
