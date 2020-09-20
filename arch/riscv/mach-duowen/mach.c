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
 * @(#)mach.c: DUOWEN specific board initialization
 * $Id: mach.c,v 1.1 2019-09-02 15:13:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/irq.h>
#include <target/clk.h>
#include <target/noc.h>
#include <target/uefi.h>
#include <target/cmdline.h>

#define __imc_boot_flash() (IMC_BOOT_FLASH_TYPE(__raw_readl(SCSR_BOOT_MODE)))

uint8_t imc_boot_mode(void)
{
	uint8_t flash;

	if (imc_sim_mode() == IMC_BOOT_SIM)
		return __raw_readl(SCSR_BOOT_MODE) & IMC_BOOT_DDR;

	flash = __imc_boot_flash();
	return flash == IMC_BOOT_FLASH ? IMC_BOOT_FLASH : IMC_BOOT_ROM;
}

uint8_t imc_boot_flash(void)
{
	if (imc_sim_mode() == IMC_BOOT_SIM)
		return __raw_readl(SCSR_BOOT_MODE) & IMC_BOOT_SSI;

	return __imc_boot_flash();
}

#ifdef CONFIG_DUOWEN_PMA
void duowen_pma_init(void)
{
	int n = 0;

	/* PMA configured for vaisra_beta test bench */
	n += pma_set(n, PMA_AT_NORMAL | PMA_S_INNER, DDR_BASE,
		     ilog2_const(max(SZ_2M, DDR_SIZE)));
	n += pma_set(n, PMA_AT_DEVICE,               DEV_BASE,
		     ilog2_const(max(SZ_2M, DEV_SIZE)));
}
#endif

#ifdef CONFIG_SHUTDOWN
void board_shutdown(void)
{
	msg_imc_shutdown();
}
#endif

#ifdef CONFIG_REBOOT
void board_reboot(void)
{
	msg_imc_shutdown();
}
#endif

#ifdef CONFIG_FINISH
void board_finish(int code)
{
	if (code)
		msg_imc_failure();
	else
		msg_imc_success();
}
#endif

#ifdef CONFIG_DUOWEN_LOAD
#ifdef CONFIG_DUOWEN_LOAD_SPI_FLASH
void duowen_load_spi(void)
{
	void (*boot_entry)(void) = (void *)FLASH_BASE;

	printf("Booting from SPI flash...\n");
	clk_enable(spi_flash_pclk);
	duowen_flash_set_frequency(min(DUOWEN_FLASH_FREQ,
				       APB_CLK_FREQ));
	boot_entry();
	unreachable();
}
#else
#define duowen_load_spi()		do { } while (0)
#endif
#ifdef CONFIG_DUOWEN_LOAD_SSI_FLASH
void duowen_load_ssi(void)
{
#ifdef CONFIG_DUOWEN_ZSBL
	void (*boot_entry)(void) = (void *)RAM_BASE;
#endif
#ifdef CONFIG_DUOWEN_FSBL
	void (*boot_entry)(void) = (void *)0x80;
#endif
	uint32_t addr = 0;
	uint32_t size = 500000;
	unsigned char boot_file[] = "fsbl.bin";
	int ret;

	ret = gpt_pgpt_init();
	if (ret != 0)
		printf("SSI: primary GPT failure.\n");
	ret = gpt_get_file_by_name(boot_file, &addr, &size);
	if (ret <= 0)
		printf("SSI: %s missing.\n", boot_file);
	printf("Booting from SSI flash addr=0x%lx, size=0x%lx...\n",
	       addr, size);
#ifdef CONFIG_DUOWEN_FSBL
	ddr_init();
#endif
	duowen_ssi_flash_boot(boot_entry, addr, size);
#if defined(CONFIG_DUOWEN_IMC) && defined(CONFIG_DUOWEN_FSBL)
	duowen_clk_apc_init();
#else
	boot_entry();
	unreachable();
#endif
}
#else
#define duowen_load_ssi()		do { } while (0)
#endif

#if defined(CONFIG_DUOWEN_APC) && defined(CONFIG_DUOWEN_ZSBL)
void duowen_load_ddr(void)
{
	void (*boot_entry)(void) = (void *)0x80;

	printf("Booting from DDR...\n");
	boot_entry();
	unreachable();
}

void board_boot(void)
{
	duowen_load_ddr();
}
#else
void board_boot(void)
{
	__unused uint8_t flash_sel = imc_boot_flash();

	board_init_clock();
	if (flash_sel == IMC_FLASH_SPI_LOAD)
		duowen_load_spi();
	if (flash_sel == IMC_FLASH_SSI_LOAD)
		duowen_load_ssi();
}
#endif
#else
#define board_boot()		do { } while (0)
#endif

void board_init_clock(void)
{
	duowen_clk_init();
}

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
	board_init_timestamp();
	crcntl_power_up();
}

void board_late_init(void)
{
	/* TODO: DDR/CFAB/APC AXI clocks need to be enabled before
	 *       initializing NoC.
	 */
	duowen_imc_noc_init();
#ifndef CONFIG_SMP
	board_boot();
#endif
}

void board_smp_init(void)
{
	board_boot();
}

static int do_duowen_shutdown(int argc, char *argv[])
{
	board_shutdown();
	return 0;
}

static int do_duowen_reboot(int argc, char *argv[])
{
	board_reboot();
	return 0;
}

const char *imc_boot2name(uint8_t boot_mode)
{
	switch (boot_mode) {
	case IMC_BOOT_ROM:
		return "rom";
	case IMC_BOOT_FLASH:
		return "flash";
	case IMC_BOOT_RAM:
		return "ram";
	case IMC_BOOT_DDR:
		return "ddr";
	default:
		return "unknown";
	}
}

const char *imc_flash2name(uint8_t boot_flash)
{
	switch (boot_flash) {
	case IMC_BOOT_SD:
		return "sd";
	case IMC_BOOT_SPI:
		return "spi";
	case IMC_BOOT_SSI:
		return "ssi";
	default:
		return "unknown";
	}
}

static int do_duowen_info(int argc, char *argv[])
{
	printf("SIM  : %s\n", imc_sim_mode() ? "sim" : "asic");
	printf("CPU  : %s\n", imc_boot_cpu() ? "APC" : "IMC");
	printf("Mode : %s\n", imc_boot2name(imc_boot_mode()));
	printf("Flash: %s\n", imc_flash2name(imc_boot_flash()));
	return 0;
}

static int do_duowen(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "shutdown") == 0)
		return do_duowen_shutdown(argc, argv);
	if (strcmp(argv[1], "reboot") == 0)
		return do_duowen_reboot(argc, argv);
	if (strcmp(argv[1], "info") == 0)
		return do_duowen_info(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(duowen, do_duowen, "DUOWEN SoC global commands",
	"duowen shutdown\n"
	"    -shutdown board\n"
	"duowen reboot\n"
	"    -reboot board\n"
	"duowen info\n"
	"    -simulation/boot mode information\n"
);
