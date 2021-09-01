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
 * @(#)mach.c: DPU specific board initialization
 * $Id: mach.c,v 1.1 2020-03-04 15:19:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/irq.h>
#include <target/clk.h>
#include <target/spi.h>
#include <target/pci.h>
#include <target/uefi.h>
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

#ifdef CONFIG_DPU_LOAD
#ifdef CONFIG_DPU_LOAD_SPI_FLASH
static void dpu_boot_spi(void)
{
	void (*boot_entry)(void);

	printf("boot(spi): booting...\n");
	boot_entry = (void *)CONFIG_DPU_BOOT_ADDR;
	clk_enable(srst_flash);
	/* dpu_flash_set_frequency(min(DPU_FLASH_FREQ, APB_CLK_FREQ)); */
	/* max spi speed is 5M,so here configure to 4M. */
	dpu_flash_set_frequency(min(DPU_FLASH_FREQ, DPU_FLASH_REAL_FREQ));
	boot_entry();
}
#else
#define dpu_boot_spi()				do { } while (0)
#endif

#ifdef CONFIG_DPU_LOAD_SSI_FLASH
#ifdef CONFIG_DPU_SIM_SSI_BACKDOOR
#define dpu_load_ssi(boot_entry, boot_file)	do { } while (0)
#else /* CONFIG_DPU_SIM_SSI_BACKDOOR */
static void dpu_load_ssi(void *boot_entry, const char *boot_file)
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
	dpu_ssi_flash_boot(boot_entry, addr, size);
}
#endif /* CONFIG_DPU_SIM_SSI_BACKDOOR */

static void dpu_boot_ssi(void)
{
	void (*boot_entry)(void);

	dpu_pe_boot();
#ifdef CONFIG_DPU_LOAD_ZSBL
#define DPU_BOOT_FILE	"fsbl.bin"
#ifndef CONFIG_DPU_BOOT_STACK
	/* fsbl.bin can't copy to SRAM_BASE which is for .data/.bss of zsbl.bin */
	boot_entry = (void *)(SRAM_BASE + 0x8000);
#else
	boot_entry = (void *)SRAM_BASE;
#endif

#endif
#ifdef CONFIG_DPU_LOAD_FSBL
#define DPU_BOOT_FILE	"bbl.bin"
	boot_entry = (void *)DDR_DATA_BASE;
#endif
	dpu_load_ssi(boot_entry, DPU_BOOT_FILE);
	printf("boot(ssi): validating SSI contents...\n");
	cmd_batch();
	printf("boot(ssi): booting...\n");
	boot_entry();
}
#else
#define dpu_boot_ssi()				do { } while (0)
#endif

void board_boot(void)
{
	uint8_t flash_sel = imc_boot_flash();

	board_init_clock();
	if (flash_sel == IMC_FLASH_SPI)
		dpu_boot_spi();
	if (flash_sel == IMC_FLASH_SSI)
		dpu_boot_ssi();
}
#else
#define board_boot()				do { } while (0)
#endif

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
	board_init_timestamp();
#ifdef CONFIG_DPU_GEN2
#ifndef CONFIG_DPU_FIRM
	imc_config_ddr_intlv();
#endif
#endif
}

void board_late_init(void)
{
	dpu_gpio_irq_init();
	dpu_tsensor_irq_init();
	dpu_ssi_irq_init();
	dpu_ssi_flash_init();
#ifdef CONFIG_PCI
	pci_platform_init();
#endif
	board_boot();
}

static int do_dpu_boot(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "pe") == 0) {
		dpu_pe_boot();
		return 0;
	}
	if (strcmp(argv[2], "vpu") == 0) {
		dpu_vpu_boot();
		return 0;
	}
	return -EINVAL;
}

static int do_dpu_pll2(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "ddr0") == 0) {
		dpu_pll_reg_select_ddr0();
		return 0;
	}
	if (strcmp(argv[2], "ddr1") == 0) {
		dpu_pll_reg_select_ddr1();
		return 0;
	}
	return -EINVAL;
}

static int do_dpu_shutdown(int argc, char *argv[])
{
	board_shutdown();
	return 0;
}

static int do_dpu_reboot(int argc, char *argv[])
{
	board_reboot();
	return 0;
}

static int do_dpu(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "boot") == 0)
		return do_dpu_boot(argc, argv);
	if (strcmp(argv[1], "pll2") == 0)
		return do_dpu_pll2(argc, argv);
	if (strcmp(argv[1], "shutdown") == 0)
		return do_dpu_shutdown(argc, argv);
	if (strcmp(argv[1], "reboot") == 0)
		return do_dpu_reboot(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(dpu, do_dpu, "DPU SoC global commands",
	"dpu boot pe|vpu\n"
	"    -boot DPU PE or VPU\n"
	"dpu pll2 ddr0|ddr1\n"
	"    -select PLL2 register access read source\n"
	"dpu shutdown\n"
	"    -shutdown board\n"
	"dpu reboot\n"
	"    -reboot board\n"
);
