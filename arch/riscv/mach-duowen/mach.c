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

uint8_t imc_boot_cpu(void)
{
	uint32_t boot_mode = __raw_readl(SCSR_BOOT_MODE);

	if (boot_mode & IMC_BOOT_SIM_IMC)
		return boot_mode & IMC_BOOT_SIM_APC;
	return boot_mode & IMC_BOOT_APC;
}

uint8_t __imc_boot_flash(void)
{
	return IMC_BOOT_FLASH(__raw_readl(SCSR_BOOT_MODE));
}

#ifdef CONFIG_DUOWEN_ZSBL
uint8_t imc_boot_flash(void)
{
	uint8_t flash = __imc_boot_flash();

	return flash > IMC_FLASH_SPI_LOAD ? IMC_FLASH_SPI_LOAD : flash;
}
#else
uint8_t imc_boot_flash(void)
{
	uint8_t flash = __imc_boot_flash();

	return flash > IMC_FLASH_SSI_LOAD ? IMC_FLASH_SSI_LOAD : flash;
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

#ifdef CONFIG_DUOWEN_LOAD
void board_boot(void)
{
	uint8_t flash_sel = imc_boot_flash();
	void (*boot_entry)(void);

	board_init_clock();
#ifdef CONFIG_DUOWEN_LOAD_SPI_FLASH
	if (flash_sel == IMC_FLASH_SPI_LOAD) {
		printf("Booting from SPI flash...\n");
		boot_entry = (void *)CONFIG_DUOWEN_BOOT_ADDR;
		clk_enable(spi_flash_pclk);
		duowen_flash_set_frequency(min(DUOWEN_FLASH_FREQ,
					       APB_CLK_FREQ));
	}
#endif
#ifdef CONFIG_DUOWEN_LOAD_SSI_FLASH
	if (flash_sel == IMC_FLASH_SSI_LOAD) {
		uint32_t addr = 0;
		uint32_t size = 500000;
		unsigned char boot_file[] = "fsbl.bin";
		int ret;

		boot_entry = (void *)RAM_BASE;
		ret = gpt_pgpt_init();
		if (ret != 0)
			printf("E(%d): Failed to init primary GPT.\n", ret);
		ret = gpt_get_file_by_name(boot_file, &addr, &size);
		if (ret <= 0)
			printf("E(%d): Failed to get boot file.\n", ret);
		printf("Booting from SSI flash addr=0x%lx, size=0x%lx...\n",
		       addr, size);
		duowen_ssi_flash_boot(boot_entry, addr, size);
	}
#endif
	boot_entry();
}
#else
#define board_boot()		do { } while (0)
#endif

void board_init_clock(void)
{
	crcntl_init();
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

static int do_duowen(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "shutdown") == 0)
		return do_duowen_shutdown(argc, argv);
	if (strcmp(argv[1], "reboot") == 0)
		return do_duowen_reboot(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(duowen, do_duowen, "DUOWEN SoC global commands",
	"duowen shutdown\n"
	"    -shutdown board\n"
	"duowen reboot\n"
	"    -reboot board\n"
);
