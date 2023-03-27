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
#include <target/smp.h>
#include <asm/mach/boot.h>

#ifdef CONFIG_SHUTDOWN
void board_shutdown(void)
{
	imc_sim_finish(true);
}
#endif /* CONFIG_SHUTDOWN */

#ifdef CONFIG_FINISH
void board_finish(int code)
{
	imc_sim_finish(!!(code == 0));
}
#endif /* CONFIG_FINISH */

#ifdef CONFIG_REBOOT
void board_reboot(void)
{
}
#endif /* CONFIG_REBOOT */

#ifdef CONFIG_DPU_LOAD
#ifdef CONFIG_DPU_LOAD_SPI_FLASH
static void dpu_boot_spi(void)
{
	void (*boot_entry)(void);

	printf("boot(spi): %sbooting...\n", BOOT_PREFIX);
	boot_entry = (void *)CONFIG_DPU_BOOT_ADDR;
	__boot_init();
	clk_enable(srst_flash);
	/* Allow maximum 1/25 APB frequency */
	dpu_flash_set_frequency(min(DPU_FLASH_FREQ, APB_CLK_FREQ / 25));
	__boot_msg(smp_processor_id());
	smp_boot_secondary_cpus((caddr_t)boot_entry);
	__boot_fini();
	local_flush_icache_all();
	boot_entry();
}
#else /* CONFIG_DPU_LOAD_SPI_FLASH */
#define dpu_boot_spi()				do { } while (0)
#endif /* CONFIG_DPU_LOAD_SPI_FLASH */

#if defined(CONFIG_DPU_LOAD_SSI_FLASH) || \
    defined(CONFIG_DPU_LOAD_PCIE_EP)
/* ======================================================================
 * Boot definitions
 * ====================================================================== */
#ifdef CONFIG_DPU_BOOT_DDR0
#define DDR_BOOT_BASE		((void *)DDR0_DATA_BASE)
#else /* CONFIG_DPU_BOOT_DDR0 */
#define DDR_BOOT_BASE		((void *)DDR1_DATA_BASE)
#endif /* CONFIG_DPU_BOOT_DDR0 */
#ifdef CONFIG_DPU_LOAD_FSBL
#define DPU_BOOT_FILE		"bbl.bin"
#define DPU_BOOT_ADDR		DDR_BOOT_BASE
#else /* CONFIG_DPU_LOAD_FSBL */
#ifdef CONFIG_DPU_LOAD_ZSBL
#define DPU_BOOT_FILE		"fsbl.bin"
#ifdef CONFIG_DPU_LOAD_PCIE_EP
#define DPU_BOOT_ADDR		DDR_BOOT_BASE
#else /* CONFIG_DPU_LOAD_PCIE_EP */
#ifndef CONFIG_DPU_BOOT_STACK
/* Can't load fsbl to SRAM_BASE where zsbl .data/.bss sections are
 * required to persist during loading.
 */
#define DPU_BOOT_ADDR		((void *)(SRAM_BASE + 0x8000))
#else /* CONFIG_DPU_BOOT_STACK */
#define DPU_BOOT_ADDR		((void *)SRAM_BASE)
#endif /* CONFIG_DPU_BOOT_STACK */
#endif /* CONFIG_DPU_LOAD_PCIE_EP */
#endif /* CONFIG_DPU_LOAD_ZSBL */
#endif /* CONFIG_DPU_LOAD_FSBL */
#endif /* CONFIG_DPU_LOAD_SSI_FLASH || CONFIG_DPU_LOAD_PCIE_EP */

#ifdef CONFIG_DPU_LOAD_SSI_FLASH
#ifdef CONFIG_DPU_BOOT_BACKDOOR
#define dpu_load_ssi(boot_entry, boot_file)	do { } while (0)
#else /* CONFIG_DPU_BOOT_BACKDOOR */
static void dpu_load_ssi(void *boot_entry, const char *boot_file)
{
	uint32_t addr = 0;
	uint32_t size = 500000;
	int ret;

	__boot_init();
	ret = gpt_pgpt_init();
	if (ret != 0) {
		__boot_msg(BOOT_ERROR_INIT);
		printf("boot(ssi): failed to init gpt.\n");
	}
	printf("boot(ssi): loading %s to 0x%016llx...\n",
	       boot_file, (uint64_t)boot_entry);
	ret = gpt_get_file_by_name(board_flash, boot_file, &addr, &size);
	if (ret <= 0) {
		__boot_msg(BOOT_ERROR_FIND);
		printf("boot(ssi): failed to load %s.\n", boot_file);
	}
	printf("boot(ssi): %sbooting - 0x%llx(0x%llx)...\n",
	       BOOT_PREFIX, (uint64_t)addr, (uint64_t)size);
	dpu_ssi_flash_boot(boot_entry, addr, size);
}
#endif /* CONFIG_DPU_BOOT_BACKDOOR */

static void dpu_boot_ssi(void)
{
	void (*boot_entry)(void) = DPU_BOOT_ADDR;

	dpu_pe_boot();
	dpu_load_ssi(boot_entry, DPU_BOOT_FILE);
}
#else /* CONFIG_DPU_LOAD_SSI_FLASH */
#define dpu_load_ssi(boot_entry, boot_file)	do { } while (0)
#define dpu_boot_ssi()				do { } while (0)
#endif /* CONFIG_DPU_LOAD_SSI_FLASH */

#ifdef CONFIG_DPU_LOAD_PCIE_EP
static void dpu_load_pcie(void *boot_entry)
{
	uint64_t pcie_ddr_addr = (uint64_t)(boot_entry);
	unsigned int pcie_inmem_addr = 0;
	unsigned int x_slice_pcie = 67108864;
	unsigned int y_slice_pcie = 1;
	unsigned int x_full_pcie = x_slice_pcie;

	printf("boot(pcie): loading program to 0x%016llx...\n",
	       (uint64_t)boot_entry);

	/* Transfer const data from memory to spm */
	Pcie_DDR_Transfer((void *)&pcie_ddr_addr, 0,
			  (void *)pcie_inmem_addr, 0,
			  x_slice_pcie, y_slice_pcie, x_full_pcie, 0, 0, 0);
	while (!(PCIE_DDR_TransferFinish(0)))
		;
}

static void dpu_boot_pcie(void)
{
	void (*boot_entry)(void) = DPU_BOOT_ADDR;

	__boot_init();
	dpu_pe_boot();
	dpu_load_pcie(boot_entry);
	printf("boot(pcie): %sbooting...\n", BOOT_PREFIX);
	__boot_msg(smp_processor_id());
	smp_boot_secondary_cpus((caddr_t)boot_entry);
	__boot_fini();
	local_flush_icache_all();
	boot_entry();
}
#else /* CONFIG_DPU_LOAD_PCIE_EP */
#define dpu_load_pcie(boot_entry)		do { } while (0)
#define dpu_boot_pcie()				do { } while (0)
#endif /* CONFIG_DPU_LOAD_PCIE_EP */

void board_boot(void)
{
	uint8_t flash_sel = imc_boot_flash();

	board_init_clock();
	if (flash_sel == IMC_FLASH_SPI) {
		/* SPI flash never loads itself, ensured by Kconfig */
		dpu_boot_spi();
		dpu_boot_ssi();
	}
	if (flash_sel == IMC_FLASH_SSI) {
		dpu_boot_pcie();
		dpu_boot_ssi();
	}
}
#else /* CONFIG_DPU_LOAD */
#define board_boot()				do { } while (0)
#endif /* CONFIG_DPU_LOAD */

#ifdef CONFIG_DPU_SIM_RAM_BOOT_DDR
static void dpu_ram_boot_ddr(void)
{
	void (*boot_entry)(void);

	imc_config_ddr_intlv();
	dpu_pll_soft_reset(SRST_DDR0_0);
	dpu_pll_soft_reset(SRST_DDR0_1);
	boot_entry = (void *)DDR1_DATA_BASE;
	boot_entry();
}
#else /* CONFIG_DPU_SIM_RAM_BOOT_DDR */
#define dpu_ram_boot_ddr()			do { } while (0)
#endif /* CONFIG_DPU_SIM_RAM_BOOT_DDR */

#if defined(CONFIG_DPU_LOAD_FSBL) || defined(CONFIG_DPU_BOOT_RAM)
static void dpu_load_ddr_early(void)
{
	imc_config_ddr_intlv();
}
#else /* CONFIG_DPU_LOAD_FSBL && CONFIG_DPU_BOOT_RAM */
#define dpu_load_ddr_early()			do { } while (0)
#endif /* CONFIG_DPU_LOAD_FSBL && CONFIG_DPU_BOOT_RAM */

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
	board_init_timestamp();
	dpu_load_ddr_early();
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

#ifdef CONFIG_DPU_APC
void board_boot_late(void)
{
}

#ifdef CONFIG_SMP
void board_smp_init(void)
{
	board_boot_late();
}
#endif /* CONFIG_SMP */

#ifdef CONFIG_DPU_PMA_DEBUG
static void dpu_pma_debug(void)
{
	console_init();
}
#else /* CONFIG_DPU_PMA_DEBUG */
#define dpu_pma_debug()			do { } while (0)
#endif /* CONFIG_DPU_PMA_DEBUG */

#ifdef CONFIG_DPU_PMA
#ifdef CONFIG_SMP_BOOT
static void __dpu_pma_cpu_init(void)
{
	int n = 0;

	dpu_pma_debug();
	/* Enable CPU PMA */
	n += pma_set(n, PMA_AT_DEVICE,
		     DEV_BASE, ilog2_const(DEV_SIZE));
	/* PMA driver automatically splits big NAPOT regions */
	n += pma_set(n, PMA_AT_NORMAL | PMA_S_INNER,
		     DDR0_DATA_BASE,
		     ilog2_const(DDR0_DATA_SIZE + DDR1_DATA_SIZE));
	n += pma_set(n, PMA_AT_DEVICE,
		     PCIE_BASE, ilog2_const(PCIE_SIZE));
}
#else /* CONFIG_SMP_BOOT */
static void __dpu_pma_cpu_init(void)
{
	int n = 0;

	dpu_pma_debug();
	/* Enable CPU PMA */
	n += pma_set(n, PMA_AT_DEVICE,
		     DEV_BASE, ilog2_const(DEV_SIZE));
	/* PMA driver automatically splits big NAPOT regions */
	n += pma_set(n, PMA_AT_DEVICE,
		     DDR0_DATA_BASE,
		     ilog2_const(DDR0_DATA_SIZE + DDR1_DATA_SIZE));
	n += pma_set(n, PMA_AT_DEVICE,
		     PCIE_BASE, ilog2_const(PCIE_SIZE));
#endif /* CONFIG_SMP_BOOT */
#else /* CONFIG_DPU_PMA */
#define __dpu_pma_cpu_init()		do { } while (0)
#endif /* CONFIG_DPU_PMA */

#ifdef CONFIG_SMP_BOOT
#define dpu_cpu_cache_init()		vaisra_cache_on()
#else /* CONFIG_SMP_BOOT */
#define dpu_cpu_cache_init()		vaisra_cache_off()
#endif /* CONFIG_SMP_BOOT */

void dpu_pma_cpu_init(void)
{
	dpu_cpu_cache_init();
	__dpu_pma_cpu_init();
	dpu_ram_boot_ddr();
}
#endif /* CONFIG_DPU_APC */

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

#ifdef CONFIG_DPU_EARLY_PUTCH
void dpu_early_putch(uint8_t ch)
{
	typedef void (*early_putch_cb)(uint8_t);
	early_putch_cb func = (early_putch_cb)EARLY_PUTCH;

	func(ch);
}
#endif

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
