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
#include <target/uefi.h>
#include <target/spi.h>
#include <target/mmc.h>
#include <target/sbi.h>
#include <target/ddr.h>
#include <target/cmdline.h>
#include <asm/mach/boot.h>

#ifdef CONFIG_DPULP_LOAD_FSBL
#ifdef CONFIG_DPULP_FIRM_RAM
#define BOOT_LOAD_ADDR				SRAM_BASE
#define BOOT_LOAD_FILE				"dfw.bin"
#else /* CONFIG_DPULP_FIRM_RAM */
#define BOOT_LOAD_ADDR				DDR_DATA_BASE
#define BOOT_LOAD_FILE				"bbl.bin"
#endif /* CONFIG_DPULP_FIRM_RAM */
#else /* CONFIG_DPULP_LOAD_FSBL */
#if !defined(CONFIG_DPULP_LOAD_ZSBL) && !defined(CONFIG_DPULP_TB)
#warning "Only ZSBL can load to RAM!"
#endif /* !CONFIG_DPULP_LOAD_ZSBL && !CONFIG_DPULP_TB */
#define BOOT_LOAD_ADDR				SRAM_BASE
#define BOOT_LOAD_FILE				"fsbl.bin"
#endif /* CONFIG_DPULP_LOAD_FSBL */

#ifdef CONFIG_FINISH
#ifdef CONFIG_SBI
void board_finish(int code)
{
	sbi_finish(code);
}
#else /* CONFIG_SBI */
void board_finish(int code)
{
	if (code)
		tcsr_sim_finish(false);
	else
		tcsr_sim_finish(true);
}
#endif /* CONFIG_SBI */
#endif /* CONFIG_FINISH */

#ifdef CONFIG_SHUTDOWN
#ifdef CONFIG_SBI
void board_shutdown(void)
{
	sbi_shutdown();
}
#else /* CONFIG_SBI */
void board_shutdown(void)
{
	tcsr_sim_finish(true);
}
#endif /* CONFIG_SBI */
#endif /* CONFIG_SHUTDOWN */

#ifdef CONFIG_REBOOT
#ifdef CONFIG_SBI
void board_reboot(void)
{
	sbi_shutdown();
}
#else /* CONFIG_SBI */
void board_reboot(void)
{
	tcsr_sim_finish(true);
}
#endif /* CONFIG_SBI */
#endif /* CONFIG_REBOOT */

#ifdef CONFIG_DPULP_BOOT
typedef void (*boot_cb)(void *, uint32_t, uint32_t, bool);

static void dpulp_load_gpt(mtd_t mtd, const char *file,
			   const char *name, uint32_t *addr,
			   uint32_t *size)
{
	int ret;

	ret = gpt_get_file_by_name(mtd, file, addr, size);
	if (ret <= 0) {
		con_err("boot(%s): %s missing.\n", name, file);
		bh_panic();
	}
	con_log("boot(%s): Booting %s from addr=0x%lx, size=0x%lx...\n",
		name, file, addr, size);
}

static void dpulp_load_file(mtd_t mtd, boot_cb boot,
			    const char *file, void *entry,
			    const char *name, bool jump)
{
	__unused uint32_t addr = 0;
	__unused uint32_t size = 500000;

	con_log("boot(%s): Booting %s to entry=0x%lx...\n",
		name, file, (unsigned long)entry);
	dpulp_load_gpt(mtd, file, name, &addr, &size);
	boot(entry, addr, size, jump);
}

#if 0
static void __dpulp_load(mtd_t mtd, boot_cb boot, const char *file,
			 caddr_t entry, const char *name)
{
	dpulp_load_file(mtd, boot, file, (void *)entry, name, false);
}
#endif

static void __dpulp_boot(mtd_t mtd, boot_cb boot, const char *file,
			 caddr_t entry, const char *name)
{
	dpulp_load_file(mtd, boot, file, (void *)entry, name, true);
	unreachable();
}

static void dpulp_load(mtd_t mtd, boot_cb boot, const char *name)
{
	int ret;

	ret = gpt_pgpt_init();
	if (ret != 0) {
		con_err("boot(%s): primary GPT failure.\n", name);
		bh_panic();
	}
	__dpulp_boot(mtd, boot, BOOT_LOAD_FILE, BOOT_LOAD_ADDR, name);
	bh_panic();
}

static void dpulp_load_ssi(void)
{
	dpulp_load(board_flash, dpulp_ssi_boot, "ssi");
}

static void dpulp_load_sd(void)
{
	dpulp_load(board_sdcard, dpulp_sd_boot, "sd");
}

static void board_boot_early(void)
{
	uint8_t load_sel = tcsr_load_to();

	board_init_clock();
	if (load_sel == TCSR_LOAD_SSI)
		dpulp_load_ssi();
	if (load_sel == TCSR_LOAD_SD)
		dpulp_load_sd();
}
#else /* CONFIG_DPULP_BOOT */
#define board_boot_early()			do { } while (0)
#endif /* CONFIG_DPULP_BOOT */

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
	board_init_timestamp();
#ifdef CONFIG_DPULP_FSBL
	/* TODO: Move this to board specific DDR initialization */
	//tcsr_config_ddr_intlv();
#endif
}

void board_late_init(void)
{
	__unused uint8_t load_sel = tcsr_load_to();

	if (load_sel == TCSR_LOAD_SSI)
		dpulp_ssi_init();
	if (load_sel == TCSR_LOAD_SD)
		dpulp_sd_init();

	/* Non-BBL bootloader */
	board_boot_early();

	/* dpulp_eth_late_init(); */
}

#ifdef CONFIG_SMP
static void board_boot_late(void)
{
	void *load_addr = (void *)BOOT_LOAD_ADDR;

	if (smp_processor_id() == 0)
		con_log("boot(ddr): Booting %d cores...\n", MAX_CPU_NUM);
	__boot_jump(load_addr);
}

void board_smp_init(void)
{
	board_boot_late();
}
#endif /* CONFIG_SMP */

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
