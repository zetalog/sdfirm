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
 * @(#)perf.c: performance counting implementation
 * $Id: perf.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/perf.h>
#include <target/smp.h>
#include <target/cmdline.h>

struct perf_event {
	perf_evt_t hw_event_id;
	perf_cnt_t hw_counter;
};

struct perf_desc {
	struct perf_event events[NR_PERF_EVTS];
	perf_evt_t max_counters;
	perf_evt_t next_event;
} __cache_aligned;

struct perf_desc perf_smp_descs[NR_CPUS];

int perf_event_id(perf_evt_t event)
{
	uint8_t cpu = smp_processor_id();
	int evt;

	for (evt = 0; evt < perf_smp_descs[cpu].next_event; evt++) {
		if (perf_smp_descs[cpu].events[evt].hw_event_id == event)
			return evt;
	}
	return INVALID_PERF_EVT;
}

void perf_remove_all_events(void)
{
	uint8_t cpu = smp_processor_id();
	perf_evt_t event;
	int evt;

	for (evt = 0; evt < perf_smp_descs[cpu].next_event; evt++) {
		event = perf_smp_descs[cpu].events[evt].hw_event_id;
		pmu_hw_disable_event(event);
		pmu_hw_configure_event(PMU_HW_DEFAULT_EVENT);
		perf_smp_descs[cpu].events[evt].hw_event_id = INVALID_PERF_EVT;
		perf_smp_descs[cpu].events[evt].hw_counter = 0;
	}
	perf_smp_descs[cpu].next_event = 0;
}

int perf_add_event(perf_evt_t event)
{
	uint8_t cpu = smp_processor_id();
	int evt;

	if (perf_smp_descs[cpu].next_event >= perf_smp_descs[cpu].max_counters)
		return INVALID_PERF_EVT;

	evt = perf_smp_descs[cpu].next_event;
	perf_smp_descs[cpu].events[evt].hw_event_id = event;
	perf_smp_descs[cpu].next_event++;
	pmu_hw_configure_event(event);
	pmu_hw_enable_event(event);
	return evt;
}

void perf_start(void)
{
	uint8_t cpu = smp_processor_id();

	perf_remove_all_events();
	pmu_hw_task_start();
}

void perf_stop(void)
{
	pmu_hw_task_stop();
	perf_remove_all_events();
}

void perf_init(void)
{
	uint8_t cpu;

	for_each_cpu(cpu, smp_online_cpus) {
		perf_smp_descs[cpu].max_counters = PMU_HW_MAX_COUNTERS;
		perf_smp_descs[cpu].next_event = 0;
	}
	pmu_hw_ctrl_init();
}

static int do_perf_cmd(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "off") == 0)
		cmd_clear_all_flags(CMD_FLAG_PERF);
	else if (!cmd_set_flags(argv[2], CMD_FLAG_PERF)) {
		printf("Command not found: %s\n", argv[2]);
		return -EINVAL;
	}
	return 0;
}

static int do_perf_bench(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;

#if 0
	if (strcmp(argv[2], "off") == 0)
		bench_clear_all_flags(BENCH_FLAG_PERF);
	else if (!bench_set_flags(argv[2], BENCH_FLAG_PERF)) {
		printf("Command not found: %s\n", argv[2]);
		return -EINVAL;
	}
#endif
	return 0;
}

static int do_perf(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "cmd") == 0)
		return do_perf_cmd(argc, argv);
	if (strcmp(argv[1], "benc") == 0)
		return do_perf_bench(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(perf, do_perf, "Performance monitor commands",
	"perf cmd <cmd>\n"
	"    -turn on command execution monitor\n"
	"perf cmd off\n"
	"    -turn off command execution monitor\n"
	"perf bench <test>\n"
	"    -turn on bench test monitor\n"
	"perf bench off\n"
	"    -turn off bench test monitor\n"
);
