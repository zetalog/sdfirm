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

#ifdef CONFIG_HPM
#define PMU_HPM_COUNTERS	32
#else
#define PMU_HPM_COUNTERS	0
#endif
#ifdef CONFIG_SMMU_PMCG
#define PMU_PMCG_COUNTERS	32
#else
#define PMU_PMCG_COUNTERS	0
#endif
#ifdef CONFIG_K1MXLITE_BMU
#define PMU_BMU_COUNTERS	8
#endif

#define PMU_HW_MAX_COUNTERS	\
	(PMU_HPM_COUNTERS + PMU_PMCG_COUNTERS + PMU_BMU_COUNTERS)

#include <asm/hpm.h>
#include <driver/smmu_pmcg.h>
#include <asm/mach/bmu.h>

#endif /* __PMU_K1MXLITE_H_INCLUDE__ */
