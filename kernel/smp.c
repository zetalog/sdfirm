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
 * @(#)smp.c: symmetrical multi-processing (SMP) implementation
 * $Id: smp.c,v 1.1 2019-12-17 16:35:00 zhenglv Exp $
 */

#include <target/smp.h>
#include <target/bh.h>
#include <target/irq.h>
#include <target/timer.h>
#include <target/task.h>
#include <target/jiffies.h>
#include <target/percpu.h>
#include <target/atomic.h>
#include <target/bench.h>
#include <target/paging.h>
#include <target/cmdline.h>
#include <target/arch.h>
#include <target/console.h>
#include <target/cache.h>

#ifdef CONFIG_SMP
cpu_t smp_boot_cpu;
cpu_mask_t smp_online_cpus;

#define smp_set_online(cpu)	cpumask_set_cpu((cpu), &smp_online_cpus)
#define smp_test_online(cpu)	cpumask_test_cpu((cpu), &smp_online_cpus)
#else /* CONFIG_SMP */
#define smp_set_online(cpu)		do { } while (0)
#define smp_test_online(cpu)		false
#endif /* CONFIG_SMP */

#ifdef CONFIG_SMP_WAIT_BOOT
#define SMP_WAIT_BOOT_MS		2000

static void smp_wait_secondary_cpus(void)
{
	cpu_t cpu;
	cpu_t nr_online_cpus;
	tick_t smp_wait;

	smp_wait = tick_get_counter() + SMP_WAIT_BOOT_MS;
	do {
		nr_online_cpus = 0;
		for (cpu = 0; cpu < NR_CPUS; cpu++) {
			if (smp_test_online(cpu))
				nr_online_cpus++;
		}
		if (nr_online_cpus >= NR_CPUS)
			break;
		if (time_after_eq(tick_get_counter(), smp_wait)) {
			con_err("smp: Bring up CPUs timeout - %d/%d!\n",
				nr_online_cpus, NR_CPUS);
			break;
		}
	} while (1);
}
#else /* CONFIG_SMP_WAIT_BOOT */
#define smp_wait_secondary_cpus()	do { } while (0)
#endif /* CONFIG_SMP_WAIT_BOOT */

#ifdef CONFIG_SMP_BOOT
void smp_map_init(void)
{
	smp_hw_map_init();
	smp_set_online(smp_boot_cpu);
}

void smp_boot_secondary_cpus(caddr_t context)
{
	cpu_t cpu;

	smp_hw_ctrl_init();
	for (cpu = 0; cpu < NR_CPUS; cpu++) {
		if (cpu != smp_boot_cpu)
			smp_cpu_boot(cpu, context);
	}
	smp_wait_secondary_cpus();
}

void smp_idle(void *arg)
{
	bh_loop();
}

void smp_init_idle(void)
{
	task_create(smp_idle, NULL, PERCPU_STACK_SIZE);
}

void smp_init(void)
{
	__unused cpu_t cpu = smp_processor_id();

	con_log("smp: Bringing up CPU %d...\n", cpu);

	if (smp_processor_id() != smp_boot_cpu) {
		smp_set_online(cpu);
		irq_smp_init();
		mmu_smp_init();
		bh_init();
		timer_init();
		task_init();
		smp_hw_ctrl_init();
		board_smp_init();
		bench_init();
	} else {
		smp_boot_secondary_cpus((caddr_t)smp_init);
		board_smp_init();
		bench_init();
		cmd_init();
	}
	bh_loop();
}
#else /* CONFIG_SMP_BOOT */
void smp_init(void)
{
	bench_init();
	cmd_init();
	bh_loop();
}
#endif /* CONFIG_SMP_BOOT */
