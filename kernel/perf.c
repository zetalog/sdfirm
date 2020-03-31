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

struct perf_event {
	perf_evt_t hw_event_id;
	perf_cnt_t hw_counter;
};

struct perf_desc {
	struct perf_event events[NR_PERF_EVTS];
	perf_evt_t max_counters;
	perf_evt_t next_event;
} __cache_aligned;

struct perf_desc perf_descs[NR_CPUS];

int perf_event_id(perf_evt_t event)
{
	uint8_t cpu = smp_processor_id();
	int evt;

	for (evt = 0; evt < perf_descs[cpu].next_event; evt++) {
		if (perf_descs[cpu].events[evt].hw_event_id == event)
			return evt;
	}
	return INVALID_PERF_EVT;
}

void perf_unregister_all_events(void)
{
	uint8_t cpu = smp_processor_id();
	perf_evt_t event;
	int evt;

	for (evt = 0; evt < perf_descs[cpu].next_event; evt++) {
		event = perf_descs[cpu].events[evt].hw_event_id;
		pmu_hw_disable_event(event);
		pmu_hw_configure_event(PMU_HW_DEFAULT_EVENT);
		perf_descs[cpu].events[evt].hw_event_id = INVALID_PERF_EVT;
		perf_descs[cpu].events[evt].hw_counter = 0;
	}
}

int perf_register_event(perf_evt_t event)
{
	uint8_t cpu = smp_processor_id();
	int evt;

	if (perf_descs[cpu].next_event >= perf_descs[cpu].max_counters)
		return INVALID_PERF_EVT;

	evt = perf_descs[cpu].next_event;
	perf_descs[cpu].events[evt].hw_event_id = event;
	perf_descs[cpu].next_event++;
	pmu_hw_configure_event(event);
	pmu_hw_enable_event(event);
	return evt;
}

void perf_init(void)
{
	uint8_t cpu = smp_processor_id();

	perf_unregister_all_events();
	pmu_hw_ctrl_init();
	perf_descs[cpu].max_counters = pmu_hw_get_counters();
}
