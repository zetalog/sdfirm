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
 * @(#)mach.c: VAISRA specific board initialization
 * $Id: mach.c,v 1.1 2020-06-08 16:59:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/bench.h>
#include <target/smp.h>
#include <target/paging.h>
#include <target/barrier.h>
#include <target/percpu.h>
#include <target/cmdline.h>

#ifdef CONFIG_MMU
#define vaisra_disable_mmu()	mmu_hw_ctrl_exit()
#else
#define vaisra_disable_mmu()	do { } while (0)
#endif

static void vaisra_success(void)
{
	vaisra_disable_mmu();
	tbox_finish();
	vaisra_sim_shutdown();
}

static void vaisra_failure(void)
{
	vaisra_disable_mmu();
	tbox_error();
	vaisra_sim_shutdown();
}

#ifdef CONFIG_SHUTDOWN
void board_shutdown(void)
{
	vaisra_success();
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

static int do_vaisra_shutdown(int argc, char *argv[])
{
	board_shutdown();
	return 0;
}

static int do_vaisra(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "shutdown") == 0)
		return do_vaisra_shutdown(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(vaisra, do_vaisra, "VAISRA simulation global commands",
	"vaisra shutdown\n"
	"    -shutdown board\n"
);

/* Percpu specific VAISRA shutdown */
static DEFINE_PERCPU(bool, vaisra_err);

void vaisra_error(void)
{
	this_cpu_var(vaisra_err) = true;
}

int vaisra_finish(caddr_t percpu_area)
{
	if (this_cpu_var(vaisra_err))
		vaisra_failure();
	else
		vaisra_success();
	return 1;
}

__define_testfn(vaisra_finish, 0, SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
