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
 * @(#)mach.c: DPU-LP specific board initialization
 * $Id: mach.c,v 1.1 2021-11-01 15:39:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/irq.h>
#include <target/clk.h>
#include <target/spi.h>
#include <target/cmdline.h>

#ifdef CONFIG_SHUTDOWN
void board_shutdown(void)
{
	imc_sim_finish(true);
}
#endif

#ifdef CONFIG_REBOOT
void board_reboot(void)
{
}
#endif

#ifdef CONFIG_DPULP_LOAD
#ifdef CONFIG_DPULP_LOAD_SPI_FLASH
static void dpulp_boot_spi(void)
{
	void (*boot_entry)(void);

	printf("boot(spi): booting...\n");
	boot_entry = (void *)CONFIG_DPULP_BOOT_ADDR;
	clk_enable(srst_flash);
	/* dpulp_flash_set_frequency(min(DPULP_FLASH_FREQ, APB_CLK_FREQ)); */
	/* max spi speed is 5M,so here configure to 4M. */
	dpulp_flash_set_frequency(min(DPULP_FLASH_FREQ,
				      DPULP_FLASH_REAL_FREQ));
	boot_entry();
}
#else
#define dpulp_boot_spi()				do { } while (0)
#endif

#ifdef CONFIG_DPULP_LOAD_ROM
static void dpulp_load_rom(void *boot_entry)
{
	uint32_t addr;
	uint32_t size;

	addr = 0x14400080;
	size = 128 * 1024;
	printf("from rom\n");

	memcpy(boot_entry, (void *)addr, size);
}
#else
#define dpulp_load_rom(boot_entry)				do { } while (0)
#endif

#ifdef CONFIG_DPULP_LOAD_SSI_FLASH
static void dpulp_load_ssi(void *boot_entry, const char *boot_file)
{
	uint32_t addr = 0;
	uint32_t size = 500000;
	int ret;

	ret = gpt_pgpt_init();
	if (ret != 0)
		printf("Error: Failed to init partition.\n");
	printf("boot(ssi): loading %s...\n", boot_file);
	ret = gpt_get_file_by_name(board_flash, boot_file, &addr, &size);
	if (ret <= 0)
		printf("Error: Failed to load file.\n");
	printf("boot(ssi): validating content - 0x%lx(0x%lx)...\n",
	       addr, size);
	dpulp_ssi_flash_boot(boot_entry, addr, size);
}
#else
#define dpulp_load_ssi(boot_entry, boot_file)				do { } while (0)
#endif

#if defined(CONFIG_DPULP_LOAD_ROM) \
	|| defined(CONFIG_DPULP_LOAD_SSI_FLASH)
static void dpulp_boot_ssi(void)
{
	void (*boot_entry)(void);

#ifdef CONFIG_DPULP_LOAD_ZSBL
#define DPULP_BOOT_FILE	"fsbl.bin"
#ifndef CONFIG_DPULP_BOOT_STACK
	/* fsbl.bin can't copy to SRAM_BASE which is for .data/.bss of zsbl.bin */
	boot_entry = (void *)(SRAM_BASE + 0x8000);
#else
	boot_entry = (void *)SRAM_BASE;
#endif
#endif

#ifdef CONFIG_DPULP_LOAD_FSBL
#define DPULP_BOOT_FILE	"bbl.bin"
	boot_entry = (void *)DDR_DATA_BASE;
#endif

#if defined(CONFIG_DPULP_LOAD_ROM) \
	|| defined(CONFIG_DPULP_LOAD_FAKE_PCIE_MEM)
	boot_entry = (void *)DDR_DATA_BASE;
#endif

	dpulp_load_ssi(boot_entry, DPULP_BOOT_FILE);
	dpulp_load_rom(boot_entry);
	dpulp_load_fake_pcie_mem(boot_entry);
	printf("boot(ssi): validating SSI contents...\n");
	cmd_batch();
	printf("boot(ssi): booting...\n");
	boot_entry();
}
#else
#define dpulp_boot_ssi()				do { } while (0)
#endif

void board_boot(void)
{
	uint8_t flash_sel = imc_boot_flash();

	board_init_clock();
	if (flash_sel == IMC_FLASH_SPI)
		dpulp_boot_spi();
	if (flash_sel == IMC_FLASH_SSI)
		dpulp_boot_ssi();
}
#else
#define board_boot()				do { } while (0)
#endif

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
	board_init_timestamp();
#ifdef CONFIG_DPULP_GEN2
#ifndef CONFIG_DPULP_FIRM
	imc_config_ddr_intlv();
#endif
#endif
}

void board_late_init(void)
{
	dpulp_ssi_flash_init();
	board_boot();
}

static int do_dpulp_shutdown(int argc, char *argv[])
{
	board_shutdown();
	return 0;
}

static int do_dpulp_reboot(int argc, char *argv[])
{
	board_reboot();
	return 0;
}

static int do_dpulp(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "shutdown") == 0)
		return do_dpulp_shutdown(argc, argv);
	if (strcmp(argv[1], "reboot") == 0)
		return do_dpulp_reboot(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(dpulp, do_dpulp, "DPULP SoC global commands",
	"dpulp shutdown\n"
	"    -shutdown board\n"
	"dpulp reboot\n"
	"    -reboot board\n"
);
