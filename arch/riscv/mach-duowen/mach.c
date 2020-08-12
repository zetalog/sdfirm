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
#include <target/cmdline.h>

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

#ifdef CONFIG_DUOWEN_IMC
static void imc_init(void)
{
}
#else
#define imc_init()		do { } while (0)
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
	imc_init();
}

void board_late_init(void)
{
	/* TODO: DDR/CFAB/APC AXI clocks need to be enabled before
	 *       initializing NoC.
	 */
	duowen_imc_noc_init();
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
