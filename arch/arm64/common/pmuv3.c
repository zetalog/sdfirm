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
 * @(#)pmuv3.c: ARM64 performance monitor unit v3 (PMUv3) implementation
 * $Id: pmuv3.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <stdio.h>
#include <target/perf.h>
#include <target/irq.h>
#include <asm/reg.h>

#ifndef CONFIG_IRQ_POLLING
void pmu_handle_irq(void)
{
	uint32_t ovs = read_sysreg(PMOVSCLR_EL0);
	int evt;

	for (evt = 0; evt < PERF_HW_MAX_COUNTERS; evt++) {
		if (ovs & PMOVS_P(evt))
			printf("Event %d counter overflow\n", evt);
	}
	write_sysreg(ovs, PMOVSCLR_EL0);
}

int pmu_irq_init(void)
{
	irqc_configure_irq(IRQ_PMU, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PMU, pmu_handle_irq);
	irqc_enable_irq(IRQ_PMU);
	return 0;
}
#else
static inline int pmu_irq_init(void)
{
	return 0;
}
#endif

void pmu_reset(void)
{
	perf_hw_reset_events();
}

int pmu_init(void)
{
	uint32_t reg;

	write_sysreg(read_sysreg(PMUSERENR_EL0) | PMUSERENR_EN,
		     PMUSERENR_EL0);
	reg = read_sysreg(PMCR_EL0);
	reg &= ~(PMCR_LC | PMCR_DP | PMCR_D);
	write_sysreg(reg | PMCR_E | PMCR_X, PMCR_EL0);
	write_sysreg(read_sysreg(MDCR_EL3) | MDCR_SPME, MDCR_EL3);
	write_sysreg(read_sysreg(MDCR_EL3) | ~MDCR_TPM, MDCR_EL3);
	write_sysreg(read_sysreg(MDCR_EL2) | ~MDCR_TPM, MDCR_EL2);

	pmu_irq_init();
	return 0;
}
