/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)mach.c: VIRAPA specific board initialization
 * $Id: mach.c,v 1.1 2022-06-30 16:43:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/bench.h>
#include <target/smp.h>
#include <target/paging.h>
#include <target/barrier.h>
#include <target/percpu.h>
#include <target/cmdline.h>

#ifdef CONFIG_MMU
#define virapa_disable_mmu()	mmu_hw_ctrl_exit()
#else
#define virapa_disable_mmu()	do { } while (0)
#endif

static void virapa_success(void)
{
	virapa_disable_mmu();
	tbox_finish();
	virapa_sim_shutdown();
}

static void virapa_failure(void)
{
	virapa_disable_mmu();
	tbox_error();
	virapa_sim_shutdown();
}

#ifdef CONFIG_SHUTDOWN
void board_shutdown(void)
{
	virapa_success();
}
#endif

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
}

void board_late_init(void)
{
}

#ifdef CONFIG_SMP
void board_smp_init(void)
{
}
#endif /* CONFIG_SMP */

static int do_virapa_shutdown(int argc, char *argv[])
{
	board_shutdown();
	return 0;
}

static int do_virapa(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "shutdown") == 0)
		return do_virapa_shutdown(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(virapa, do_virapa, "VIRAPA simulation global commands",
	"virapa shutdown\n"
	"    -shutdown board\n"
);

/* Percpu specific VIRAPA shutdown */
static DEFINE_PERCPU(bool, virapa_err);

void virapa_error(void)
{
	this_cpu_var(virapa_err) = true;
}

int virapa_finish(caddr_t percpu_area)
{
	if (this_cpu_var(virapa_err))
		virapa_failure();
	else
		virapa_success();
	return 1;
}

__define_testfn(virapa_finish, 0, SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
