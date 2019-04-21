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
 * @(#)pmu.h: performance monitor unit interfaces
 * $Id: pmu.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __PMU_ARM64_H_INCLUDE__
#define __PMU_ARM64_H_INCLUDE__

#include <asm/io.h>
#include <asm/sysreg_pmuv3.h>

/* =================================================================
 * D10.4 Performance Monitors registers
 * ================================================================= */

/* D10.4.1 PMCCFILTR_EL0,
 * Performance Monitors Cycle Count Filter Register
 */
#define PMCCFILTR_P	_BV(31)
#define PMCCFILTR_U	_BV(30)
#define PMCCFILTR_NSK	_BV(29)
#define PMCCFILTR_NSU	_BV(28)
#define PMCCFILTR_NSH	_BV(27)
#define PMCCFILTR_M	_BV(26)

/* D10.4.2 PMCCNTR_EL0, Performance Monitors Cycle Count Register */

/* D10.4.3 PMCEID0_EL0,
 * Performance Monitors Common Event Identification register 0
 * D10.4.4 PMCEID1_EL0,
 * Performance Monitors Common Event Identification register 1
 * Low 32-bit: IDlo, High 32-bit: IDhi
 */

/* D10.4.5 PMCNTENCLR_EL0,
 * Performance Monitors Count Enable Clear register
 * D10.4.6 PMCNTENSET_EL0,
 * Performance Monitors Count Enable Set register
 */
#define PMCNTEN_C		_BV(31)
#define PMCNTEN_P(n)		_BV(n)

/* D10.4.7 PMCR_EL0, Performance Monitors Control Register */
#define PMCR_IMP_OFFSET		24
#define PMCR_IMP_MASK		0xFF
#define PMCR_IMP(value)		_GET_FV(PMCR_IMP, value)
#define PMCR_IDCODE_OFFSET	16
#define PMCR_IDCODE_MASK	0xFF
#define PMCR_IDCODE(value)	_GET_FV(PMCR_IDCODE, value)
#define PMCR_N_OFFSET		11
#define PMCR_N_MASK		0x1F
#define PMCR_N(value)		_GET_FV(PMCR_N, value)
#define PMCR_LC			_BV(6) /* deprecated by ARM */
#define PMCR_DP			_BV(5)
#define PMCR_X			_BV(4)
#define PMCR_D			_BV(3)
#define PMCR_C			_BV(2)
#define PMCR_P			_BV(1)
#define PMCR_E			_BV(0)

/* D10.4.8 PMEVCNTR<n>_EL0,
 * Performance Monitors Event Count Registers, n = 0 - 30
 */

/* D10.4.9 PMEVTYPER<n>_EL0,
 * Performance Monitors Event Type Registers, n = 0 - 30
 */
#define PMEVTYPER_P		_BV(31)
#define PMEVTYPER_U		_BV(30)
#define PMEVTYPER_NSK		_BV(29)
#define PMEVTYPER_NSU		_BV(28)
#define PMEVTYPER_NSH		_BV(27)
#define PMEVTYPER_M		_BV(26)
#define PMEVTYPER_MT		_BV(25)

/* D10.4.10 PMINTENCLR_EL1,
 * Performance Monitors Interrupt Enable Clear register
 * D10.4.11 PMINTENSET_EL1,
 * Performance Monitors Interrupt Enable Set register
 */
#define PMINTEN_C		_BV(31)
#define PMINTEN_P(n)		_BV(n)

/* D10.4.12 PMOVSCLR_EL0,
 * Performance Monitors Overflow Flag Status Clear Register
 * D10.4.13 PMOVSSET_EL0,
 * Performance Monitors Overflow Flag Status Set register
 */
#define PMOVS_C			_BV(31)
#define PMOVS_P(n)		_BV(n)

/* D10.4.14 PMSELR_EL0,
 * Performance Monitors Event Counter Selection Register
 */

/* D10.4.15 PMSWINC_EL0,
 * Performance Monitors Software Increment register
 */
#define PMSWINC_P(n)		_BV(n)

/* D10.4.16 PMUSERENR_EL0,
 * Performance Monitors User Enable Register
 */
#define PMUSERENR_ER		_BV(3)
#define PMUSERENR_CR		_BV(2)
#define PMUSERENR_SW		_BV(1)
#define PMUSERENR_EN		_BV(0)

/* D10.4.17 PMXEVCNTR_EL0,
 * Performance Monitors Selected Event Count Register
 */

/* D10.4.18 PMXEVTYPER_EL0,
 * Performance Monitors Selected Event Type Register
 */

/* Required events */
#define ARMV8_PMUV3_SW_INCR		0x0000
#define ARMV8_PMUV3_L1D_CACHE_REFILL	0x0003
#define ARMV8_PMUV3_L1D_CACHE		0x0004
#define ARMV8_PMUV3_BR_MIS_PRED		0x0010
#define ARMV8_PMUV3_CPU_CYCLES		0x0011
#define ARMV8_PMUV3_BR_PRED		0x0012
#define ARMV8_PMUV3_INST_RETIRED	0x0008
#define ARMV8_PMUV3_INST_SPEC		0x001B
#define ARMV81_PMUV3_STALL_FRONTEND	0x0023
#define ARMV81_PMUV3_STALL_BACKEND	0x0024

#define pmu_get_counters()		PMCR_N(read_sysreg(PMCR_EL0))
#define pmu_reset_cycle_counter()	write_sysreg(PMCR_C, PMCR_EL0)
#define pmu_reset_event_counters()	write_sysreg(PMCR_P, PMCR_EL0)

#define pmu_enable_overflow(count)			\
	do {						\
		pmu_set_overflow(count);		\
		write_sysreg(PMINTEN_P(count),		\
			     PMINTENSET_EL1);		\
		write_sysreg(PMCNTEN_P(count),		\
			     PMCNTENSET_EL0);		\
	} while (0)
#define pmu_disable_overflow(count)			\
	do {						\
		write_sysreg(PMCNTEN_P(count),		\
			     PMCNTENCLR_EL0);		\
		write_sysreg(PMINTEN_P(count),		\
			     PMINTENCLR_EL1);		\
	} while (0)
#define pmu_clear_overflow(count)			\
	write_sysreg(PMOVS_P(count), PMOVSCLR_EL0)
#define pmu_set_overflow(count)				\
	write_sysreg(PMOVS_P(count), PMOVSSET_EL0)
#define pmu_configure_overflow(count, event)		\
	do {						\
		write_sysreg(count, PMSELR_EL0);	\
		write_sysreg(event, PMXEVTYPER_EL0);	\
	} while (0)
#define pmu_get_count(count)				\
	({						\
		perf_cnt_t __cnt;			\
		write_sysreg(count, PMSELR_EL0);	\
		__cnt = read_sysreg(PMXEVCNTR_EL0);	\
		__cnt;					\
	})
#define pmu_increment_software()			\
	write_sysreg(PMSWINC_P(perf_event_id(ARMV8_PMUV3_SW_INCR)), PMSWINC_EL0)

#define pmu_reset_events()				\
	write_sysreg(read_sysreg(PMCR_EL0) |		\
		     PMCR_C | PMCR_P, PMCR_EL0)

#define pmu_enable_event(event)		\
	pmu_enable_overflow(perf_event_id(event))
#define pmu_disable_event(event)	\
	pmu_disable_overflow(perf_event_id(event))
#define pmu_configure_event(event)	\
	pmu_configure_overflow(perf_event_id(event), event)
#define pmu_get_event_count(event)	\
	pmu_get_count(perf_event_id(event))

int pmu_init(void);

#endif /* __PMU_ARM64_H_INCLUDE__ */
