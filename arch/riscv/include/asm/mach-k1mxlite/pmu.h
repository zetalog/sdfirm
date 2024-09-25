/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)pmu.h: K1MXLite specific perf implementation
 * $Id: pmu.h,v 1.279 2023-11-25 21:19:18 zhenglv Exp $
 */

#ifndef __PMU_K1MXLITE_H_INCLUDE__
#define __PMU_K1MXLITE_H_INCLUDE__

#define PMU_HW_DEFAULT_EVENT	0
#ifdef CONFIG_HPM
#define PMU_HPM_COUNTERS	HPM_MAX_COUNTERS
#define HPM_EVENT_ID(evt)	((evt) % 32)
#else
#define PMU_HPM_COUNTERS	0
#endif
#ifdef CONFIG_K1MXLITE_BMU
#define PMU_BMU_COUNTERS	8
#else
#define PMU_BMU_COUNTERS	0
#endif
#ifdef CONFIG_SMMU_PMCG
#define NR_SMMU_TCUS		1
#define NR_SMMU_TBUS		2
#define PMU_PMCG_COUNTERS	(32 * (NR_SMMU_TCUS + NR_SMMU_TBUS))
#define PMCG_EVENT_ID(evt)	((evt) % 32)
#define PMCG_EVENT_IS_TCU(evt)	(((evt) / 32) < NR_SMMU_TCUS)
#define PMCG_EVENT_IS_TBU(evt)	(((evt) / 32) >= NR_SMMU_TCUS)
#define SMMU_HW_IRQ_PMU(dev)	IRQ_SMMU_PMCG
#else
#define PMU_PMCG_COUNTERS	0
#endif

#define PMU_IS_HPM_EVENT(evt)	((evt) < PMU_HPM_COUNTERS)
#define PMU_IS_PMCG_EVENT(evt)	\
	(!PMU_IS_HPM_EVENT(evt) && ((evt) < (PMU_HPM_COUNTERS + PMU_PMCG_COUNTERS)))

#define PMU_HW_MAX_COUNTERS	\
	(PMU_HPM_COUNTERS + PMU_PMCG_COUNTERS + PMU_BMU_COUNTERS)

#if PMU_HW_MAX_COUNTERS > 0
#define ARCH_HAVE_PMU 1

#include <asm/hpm.h>
#include <driver/smmu_pmcg.h>
#include <asm/mach/bmu.h>

#define pmu_hw_get_event_count(event)		hpm_get_event_count(event)
#define pmu_hw_set_event_count(event, count)	hpm_set_event_count(event, count)

void pmu_hw_configure_event(perf_evt_t evt);
void pmu_hw_enable_event(perf_evt_t evt);
void pmu_hw_disable_event(perf_evt_t evt);
void pmu_hw_task_start(void);
void pmu_hw_task_stop(void);
void pmu_hw_ctrl_init(void);
#endif

#endif /* __PMU_K1MXLITE_H_INCLUDE__ */
