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
#include <target/console.h>
#include <target/cmdline.h>
#include <target/ddr.h>
#include <target/sbi.h>
#include <target/dma.h>
#include <target/pci.h>
#include <target/eth.h>
#include <target/mmc.h>
#include <target/spi.h>
#include <target/iommu.h>
#include <asm/mach/boot.h>

#define APC_JUMP_ENTRY		(__DDR_BASE + 0x80)
#define APC_BOOT_ENTRY		APC_ROM_BASE
#define IMC_BOOT_ENTRY		(RAM_BASE + BOOT_OFFSET)

unsigned long duowen_soc_base = SOC0_BASE;
unsigned long duowen_hart_base = SOC0_HART;

void duowen_dual_init(void)
{
	/* NOTE: Boot Core Specific Code in SBI Dual
	 *
	 * Even if a boot core is booting from socket 1, we still need the
	 * SBI to prompt from socket 0. Thus the following logic is not
	 * boot core bug free. It assumes that the code is always executed
	 * by a socket 0 boot core in the SBI dual-socket evironment.
	 */
	if (imc_socket_id() == 1) {
		duowen_soc_base = SOC1_BASE;
		duowen_hart_base = SOC1_HART;
	}
	duowen_pll_init();
	duowen_mmu_init();
}

#ifdef CONFIG_DUOWEN_PMA_DEBUG
static void duowen_pma_debug(void)
{
	console_init();
}
#else
#define duowen_pma_debug()			do { } while (0)
#endif

#ifdef CONFIG_DUOWEN_PMA_DDR_NONCACHEABLE
#define PMA_AT_DDR				(PMA_AT_NORM_NC | PMA_S_INNER)
#else
#define PMA_AT_DDR				(PMA_AT_NORMAL | PMA_S_INNER)
#endif

#ifdef CONFIG_DUOWEN_PMA_SOC
static void __duowen_pma_soc_init(void)
{
	int n = 0;

	duowen_pma_debug();
	/* Enable SoC PMA */
	n += imc_pma_set(n, PMA_AT_DDR,
			 SOC0_BASE + __DDR_BASE, ilog2_const(__DDR_SIZE));
	n += imc_pma_set(n, PMA_AT_DEVICE,
			 SOC0_BASE + __DEV_BASE, ilog2_const(DEV_SIZE));
	n += imc_pma_set(n, PMA_AT_DEVICE,
			 SOC0_BASE + __PCIE_BASE, ilog2_const(PCIE_SIZE));
	n += imc_pma_set(n, PMA_AT_DDR,
			 SOC1_BASE + __DDR_BASE, ilog2_const(__DDR_SIZE));
	n += imc_pma_set(n, PMA_AT_DEVICE,
			 SOC1_BASE + __DEV_BASE, ilog2_const(DEV_SIZE));
	n += imc_pma_set(n, PMA_AT_DEVICE,
			 SOC1_BASE + __PCIE_BASE, ilog2_const(PCIE_SIZE));
}

void duowen_pma_soc_init(void)
{
	if (!rom_get_pma_configured()) {
		__duowen_pma_soc_init();
		rom_set_pma_configured();
	}
}
#endif

#ifdef CONFIG_DUOWEN_PMA_CPU
void duowen_pma_cpu_init(void)
{
	int n = 0;

	duowen_pma_debug();
	/* Enable CPU PMA */
	n += pma_set(n, PMA_AT_DDR,
		     SOC0_BASE + __DDR_BASE, ilog2_const(__DDR_SIZE));
	n += pma_set(n, PMA_AT_DEVICE,
		     SOC0_BASE + __DEV_BASE, ilog2_const(DEV_SIZE));
	n += pma_set(n, PMA_AT_DEVICE,
		     SOC0_BASE + __PCIE_BASE, ilog2_const(PCIE_SIZE));
	n += pma_set(n, PMA_AT_DDR,
		     SOC1_BASE + __DDR_BASE, ilog2_const(__DDR_SIZE));
	n += pma_set(n, PMA_AT_DEVICE,
		     SOC1_BASE + __DEV_BASE, ilog2_const(DEV_SIZE));
	n += pma_set(n, PMA_AT_DEVICE,
		     SOC1_BASE + __PCIE_BASE, ilog2_const(PCIE_SIZE));
}
#endif

#ifdef CONFIG_DUOWEN_APC_PARTIAL_GOOD
#define duowen_get_partial_good()	CONFIG_DUOWEN_APC_PARTIAL_GOOD_MASK
#else
/* TODO: Read from a flash cfg.bin */
#define duowen_get_partial_good()	GOOD_CPU_MASK
#endif

void duowen_hart_map_init(void)
{
	uint16_t harts;

	harts = duowen_get_partial_good();
	rom_set_apc_map(harts);
	plic_init_default(harts);
	con_log("partial_good(%d): Applying partial goods: 0x%04x.\n",
		imc_socket_id(), harts);
	apc_set_cpu_map(harts);
}

#ifdef CONFIG_DUOWEN_PLIC_DUAL
void duowen_plic_init(void)
{
	if (!rom_get_pliccntl_done()) {
		if (soc_chip_link()) {
			imc_plic_enable_dual();
			if (imc_socket_id() == 0)
				plic_socket_connect(1);
			else
				plic_socket_connect(0);
		}
		rom_set_pliccntl_done();
	}
}
#else /* CONFIG_DUOWEN_PLIC_DUAL */
void duowen_plic_init(void)
{
	if (!rom_get_pliccntl_done()) {
		imc_plic_disable_dual();
		rom_set_pliccntl_done();
	}
}
#endif /* CONFIG_DUOWEN_PLIC_DUAL */

#ifdef CONFIG_SHUTDOWN
#ifdef CONFIG_SBI
void board_shutdown(void)
{
	sbi_shutdown();
}
#else
void board_shutdown(void)
{
	msg_imc_shutdown();
}
#endif
#endif

#ifdef CONFIG_REBOOT
#ifdef CONFIG_SBI
void board_reboot(void)
{
	sbi_shutdown();
}
#else
void board_reboot(void)
{
	msg_imc_shutdown();
}
#endif
#endif

#ifdef CONFIG_FINISH
#ifdef CONFIG_SBI
void board_finish(int code)
{
	sbi_finish(code);
}
#else
void board_finish(int code)
{
	if (code)
		msg_imc_failure();
	else
		msg_imc_success();
}
#endif
#endif

#if defined(CONFIG_DUOWEN_ZSBL) && !defined(CONFIG_DUOWEN_BOOT_PROT)
/* CONFIG_DUOWEN_BOOT_PROT is used, but our program layout is:
 *  stack
 *  .bss
 *  .data
 *  .rodata
 *  .text
 * Where stack is located right after .bss section, so by configuring
 * CONFIG_LOAD_BASE, ZSBL can stop using lower SRAM and thus FSBL can use
 * 0x0 BOOT_OFFSET. Care must be taken to maintain ZSBL CONFIG_LOAD_BASE,
 * currently 0x8000 is safe for about 0x2000 sized data section and 0x2000
 * (maximum 8KB) sized stack. Please tune CONFIG_LOAD_BASE for ZSBL when
 * its data section is extended.
 */
#ifdef CONFIG_DUOWEN_BOOT_OFFSET
#define BOOT_OFFSET		CONFIG_DUOWEN_BOOT_OFFSET
#else
#define BOOT_OFFSET		0x0
#endif
#else
#define BOOT_OFFSET		0x0
#endif

#ifdef CONFIG_DUOWEN_FSBL
/* TODO: Sort DDR APIs
 *
 * Note that ddr_init() has already been invoked by the subsystem
 * initialization stuffs and it handles all DDR functionalities. However
 * ddr_init() should only initialize DDR subsystem, with no
 * functionalities embedded, while a training and initialization related
 * API (e.x., ddr_config_memory_banks()) might be invoked here.
 *
 * #define duowen_boot_ddr()			ddr_init()
 */
#define duowen_boot_ddr()			do { } while (0)
#else /* CONFIG_DUOWEN_FSBL */
#define duowen_boot_ddr()			do { } while (0)
#endif /* CONFIG_DUOWEN_FSBL */

#ifdef CONFIG_DUOWEN_LOAD_FLASH
typedef void (*boot_cb)(void *, uint32_t, uint32_t, bool);

#ifdef CONFIG_DUOWEN_LOAD_GPT
#ifdef CONFIG_DUOWEN_SIM_ROM_FINISH
static void __duowen_load_gpt(mtd_t mtd, const char *file,
			      const char *name, uint32_t *addr,
			      uint32_t *size)
{
	if (gpt_mtd_test(mtd))
		msg_imc_success();
	else
		msg_imc_failure();
}
#else /* CONFIG_DUOWEN_SIM_ROM_FINISH */
static void __duowen_load_gpt(mtd_t mtd, const char *file,
			      const char *name, uint32_t *addr,
			      uint32_t *size)
{
	int ret;

	ret = gpt_get_file_by_name(mtd, file, addr, size);
	if (ret <= 0) {
		con_err("boot(%s): %s missing.\n", name, file);
		bh_panic();
	}
	con_log("boot(%s): Booting %s from addr=0x%llx, size=0x%llx...\n",
		name, file, (uint64_t)(*addr), (uint64_t)(*size));
}
#endif /* CONFIG_DUOWEN_SIM_ROM_FINISH */
#else /* CONFIG_DUOWEN_LOAD_GPT */
#define __duowen_load_gpt(mtd, file, name, addr, size)	do { } while (0)
#endif /* CONFIG_DUOWEN_LOAD_GPT */

static void __duowen_load_file(mtd_t mtd, boot_cb boot,
			       const char *file, void *entry,
			       const char *name, bool jump)
{
	__unused uint32_t addr = 0;
	__unused uint32_t size = 500000;

	con_log("boot(%s): Booting %s to entry=0x%lx...\n",
		name, file, (unsigned long)entry);
	__duowen_load_gpt(mtd, file, name, &addr, &size);
#if !defined(CONFIG_DUOWEN_LOAD_RAM_BACKDOOR) && \
    !defined(CONFIG_DUOWEN_LOAD_DDR_BACKDOOR)
	boot(entry, addr, size, jump);
#elif defined(CONFIG_DUOWEN_LOAD_RAM_BACKDOOR)
	if (jump)
		__boot_jump(entry);
#endif
}

void duowen_load_file(mtd_t mtd, boot_cb boot, const char *file,
		      caddr_t entry, const char *name)
{
	__duowen_load_file(mtd, boot, file, (void *)entry, name, false);
}

void duowen_boot_file(mtd_t mtd, boot_cb boot, const char *file,
		      caddr_t entry, const char *name)
{
	__duowen_load_file(mtd, boot, file, (void *)entry, name, true);
	unreachable();
}

static void duowen_load_flash(mtd_t mtd, boot_cb boot, const char *name)
{
	int ret;

	ret = gpt_pgpt_init(duowen_gpt_load);
	if (ret != 0) {
		con_err("boot(%s): primary GPT failure.\n", name);
		bh_panic();
	}
	duowen_boot_ddr();
#ifdef CONFIG_DUOWEN_ZSBL
	duowen_boot_file(mtd, boot, "fsbl.bin", IMC_BOOT_ENTRY, name);
#endif /* CONFIG_DUOWEN_ZSBL */
#ifdef CONFIG_DUOWEN_FSBL
	/* For APC FSBL, boot jump is done in SMP style. Thus it's always
	 * safe to load bbl.bin prior to any other boot steps.
	 */
	duowen_hart_map_init();
	duowen_load_file(mtd, boot, "bbl.bin", APC_JUMP_ENTRY, name);
	apc_set_jump_addr(APC_JUMP_ENTRY);
	duowen_clk_apc_init();
#ifdef CONFIG_DUOWEN_APC
#ifdef CONFIG_DUOWEN_LOAD_IMC_FIRMWARE
	duowen_load_file(mtd, boot, "imc.bin", IMC_BOOT_ENTRY, name);
	imc_set_boot_addr(IMC_BOOT_ENTRY);
	duowen_clk_imc_init();
#endif /* CONFIG_DUOWEN_LOAD_IMC_FIRMWARE */
#else /* CONFIG_DUOWEN_APC */
#ifdef CONFIG_DUOWEN_LOAD_IMC_FIRMWARE
	duowen_boot_file(mtd, boot, "imc.bin", IMC_BOOT_ENTRY, name);
#else /* CONFIG_DUOWEN_LOAD_IMC_FIRMWARE */
	bh_panic();
#endif /* CONFIG_DUOWEN_LOAD_IMC_FIRMWARE */
#endif /* CONFIG_DUOWEN_APC */
#endif /* CONFIG_DUOWEN_FSBL */
}

#ifdef CONFIG_DUOWEN_LOAD_SSI_FLASH
static void duowen_load_ssi(void)
{
	duowen_load_flash(board_flash, duowen_ssi_boot, "ssi");
}
#else
#define duowen_load_ssi()			do { } while (0)
#endif

#ifdef CONFIG_DUOWEN_LOAD_SD
static void duowen_load_sd(void)
{
	duowen_load_flash(board_sdcard, duowen_sd_boot, "sd");
}
#else
#define duowen_load_sd()			do { } while (0)
#endif

void board_boot_early(void)
{
	__unused uint8_t load_sel = imc_load_from();

	board_init_clock();
	if (load_sel == IMC_BOOT_SD) {
		if (duowen_sd_inserted())
			duowen_load_sd();
		else
			duowen_load_ssi();
	}
	if (load_sel == IMC_BOOT_SSI)
		duowen_load_ssi();
}
#else /* CONFIG_DUOWEN_LOAD_FLASH */
void board_boot_early(void)
{
#ifdef CONFIG_DUOWEN_ASBL
	/* The default jump address is APC_BOOT_ENTRY which is the entry
	 * point of the ASBL. To avoid recursively executing ASBL, resets
	 * it to APC_JUMP_ENTRY.
	 */
	if (apc_get_jump_addr() == APC_BOOT_ENTRY)
		apc_set_jump_addr(APC_JUMP_ENTRY);
#endif /* CONFIG_DUOWEN_ASBL */
}
#endif /* CONFIG_DUOWEN_LOAD_FLASH */

#if defined(CONFIG_DUOWEN_BOOT_APC) && !defined(CONFIG_DUOWEN_TB)
#ifdef CONFIG_DUOWEN_SIM_ROM_FINISH
#define duowen_load_ddr()		msg_imc_success()
#else
void duowen_load_ddr(void)
{
	void *boot_addr = (void *)apc_get_jump_addr();

	if (smp_processor_id() == 0)
		con_log("boot(ddr): Booting %d cores...\n", MAX_CPU_NUM);
#if defined(CONFIG_SMP_BOOT) && !defined(CONFIG_SMP)
	smp_boot_secondary_cpus((caddr_t)boot_addr);
#endif
	__boot_jump(boot_addr);
}
#endif

void board_boot_late(void)
{
	duowen_load_ddr();
}
#else /* CONFIG_DUOWEN_BOOT_APC */
#define board_boot_late()			do { } while (0)
#endif /* CONFIG_DUOWEN_BOOT_APC */

#ifdef CONFIG_DUOWEN_SIM_BOOT_CLOCKS
void board_init_clock(void)
{
}
#else
void board_init_clock(void)
{
	duowen_clk_init();
}
#endif

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
	board_init_timestamp();
	crcntl_power_up();
}

void board_late_init(void)
{
	__unused uint8_t load_sel = imc_load_from();

	/* Bench test initialization */
	duowen_ssi_irq_init();

	/* Bootloader initialization */
	if (load_sel == IMC_BOOT_SD) {
		if (duowen_sd_inserted())
			duowen_sd_init();
		else
			duowen_ssi_init();
	}
	if (load_sel == IMC_BOOT_SSI)
		duowen_ssi_init();

	/* SoC PMA configuration */
	duowen_pma_soc_init();
	/* Coherence initialization */
	duowen_imc_noc_init();
	/* SMMU global bypass initialization */
	duowen_smmu_pma_init();
	/* PCIe dual socket connection, must be done prior than all other
	 * dual socket operations.
	 */
	pci_platform_init();
	/* PLIC parital goods and socket connection */
	duowen_plic_init();

	/* Non-BBL bootloader initialization */
	board_boot_early();

	/* BBL bootloader initialization */
	smmu_dma_alloc_sme();
	//smmu_pcie_alloc_sme();
	duowen_eth_late_init();

#if defined(CONFIG_SMP_BOOT) && !defined(CONFIG_SMP)
	/* Standalone SMP boot */
	board_boot_late();
#endif
}

#ifdef CONFIG_SMP
void board_smp_init(void)
{
	board_boot_late();
}
#endif /* CONFIG_SMP */

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

static int do_duowen_info(int argc, char *argv[])
{
	printf("SIM :    %s\n", imc_sim_mode() ? "sim" : "asic");
	printf("CPU :    %s\n", imc_boot_cpu() ? "APC" : "IMC");
	if (imc_sim_mode() == IMC_BOOT_ASIC)
		printf("BOOT:    %s\n", imc_boot_from() ? "SPI" : "ROM");
	else
		printf("BOOT:    %s\n", imc_sim_boot_from() ? "DDR" : "RAM");
	printf("LOAD:    %s\n", imc_load_from() ? "SSI0" : "SD");
	printf("IMCBOOT: 0x%016llx\n", imc_get_boot_addr());
	printf("APCBOOT: 0x%016llx\n", apc_get_boot_addr());
	printf("APCJUMP: 0x%016llx\n", apc_get_jump_addr());
	printf("CPU:     0x%04x\n", apc_get_cpu_map());
	printf("CLUSTER: 0x%02x\n", apc_get_cluster_map());
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
