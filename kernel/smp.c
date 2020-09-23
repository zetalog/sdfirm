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
#include <target/bench.h>
#include <target/atomic.h>
#include <target/paging.h>
#include <target/cmdline.h>
#include <target/arch.h>
#include <target/console.h>

#ifdef CONFIG_SMP
cpu_t smp_boot_cpu;
cpu_mask_t smp_online_cpus;

void smp_boot(void)
{
	smp_hw_cpu_boot();
	cpumask_set_cpu(smp_boot_cpu, &smp_online_cpus);
}

void smp_init(void)
{
	cpu_t cpu = smp_processor_id();

	con_log("SMP initializing CPU %d.\n", cpu);

	if (smp_processor_id() != smp_boot_cpu) {
		cpumask_set_cpu(cpu, &smp_online_cpus);
		irq_smp_init();
		mmu_smp_init();
		bh_init();
		timer_init();
		task_init();
		smp_hw_ctrl_init();
		board_smp_init();
		bench_init();
	} else {
		cpu_t cpu;

		smp_hw_ctrl_init();
		for (cpu = 0; cpu < NR_CPUS; cpu++) {
			if (cpu != smp_boot_cpu) {
				smp_cpu_on(cpu, (caddr_t)smp_init);
				while (!cpumask_test_cpu(cpu, &smp_online_cpus));
			}
		}
		board_smp_init();
		bench_init();
		cmd_init();
	}
	bh_loop();
}
#else
void smp_init(void)
{
	bench_init();
	cmd_init();
	bh_loop();
}
#endif

#ifdef CONFIG_RISCV
int ipi_sanity(caddr_t percpu_area)
{
	cpu_t cpu;

#ifdef CONFIG_SBI
	sbi_enable_log();
#endif
	for_each_cpu(cpu, &smp_online_cpus) {
		printf("SMP: %d sending IPI to %d\n", smp_processor_id(), cpu);
		if (cpu != smp_processor_id())
			smp_cpu_off(cpu);
	}
#ifdef CONFIG_SBI
	sbi_disable_log();
#endif
	return 1;
}
__define_testfn(ipi_sanity, 0, SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
#endif
