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
 * @(#)plic.c: platform level interrupt controller (PLIC) implementation
 * $Id: plic.c,v 1.1 2019-10-11 15:50:00 zhenglv Exp $
 */

#include <target/irq.h>
#include <target/smp.h>
#include <target/irq.h>
#include <target/sbi.h>

#ifdef CONFIG_SBI
/* Do not handle IRQs in M mode.
 * If S mode is a part of this firmware, handle IRQs.
 * If S mode is not a part of this firmware, do not handle IRQs.
 */
#define PLIC_PRI_M		PLIC_PRI_MAX
#define PLIC_THR_M		PLIC_THR_NONE
#ifdef CONFIG_SBI_PAYLOAD
#define PLIC_PRI_S		PLIC_PRI_MAX
#define PLIC_THR_S		PLIC_THR_NONE
#else /* CONFIG_SBI_PAYLOAD */
#define PLIC_PRI_S		PLIC_PRI_DEF
#define PLIC_THR_S		PLIC_THR_ALL
#endif /* CONFIG_SBI_PAYLOAD */
#else /* CONFIG_SBI */
#ifdef CONFIG_RISCV_EXIT_M
#define PLIC_PRI_M		PLIC_PRI_DEF
#define PLIC_THR_M		PLIC_THR_ALL
#else /* CONFIG_RISCV_EXIT_M */
/* No IRQs in M mode */
#define PLIC_PRI_M		PLIC_PRI_MAX
#define PLIC_THR_M		PLIC_THR_NONE
#endif /* CONFIG_RISCV_EXIT_M */
#ifdef CONFIG_RISCV_EXIT_S
#define PLIC_PRI_S		PLIC_PRI_DEF
#define PLIC_THR_S		PLIC_THR_ALL
#else /* CONFIG_RISCV_EXIT_S */
/* No IRQs in S mode */
#define PLIC_PRI_S		PLIC_PRI_MAX
#define PLIC_THR_S		PLIC_THR_NONE
#endif /* CONFIG_RISCV_EXIT_S */
#endif /* CONFIG_SBI */

void plic_init_default(void)
{
	cpu_t cpu;

	for (cpu = 0; cpu < NR_CPUS; cpu++) {
		if (plic_hw_m_ctx(cpu) != PLIC_CTX_NONE)
			plic_configure_threshold_m(cpu, PLIC_THR_NONE);
		if (plic_hw_s_ctx(cpu) != PLIC_CTX_NONE)
			plic_configure_threshold_s(cpu, PLIC_THR_NONE);
	}
}

void plic_sbi_init_cold(void)
{
	irq_t irq;

	for (irq = 0; irq < NR_EXT_IRQS; irq++)
		plic_configure_priority(irq, PLIC_PRI_MIN);
}

void plic_sbi_init_warm(cpu_t cpu)
{
	irq_t irq;

	if (plic_hw_m_ctx(cpu) != PLIC_CTX_NONE) {
		/* By default, IRQs should be disabled for all contexts,
		 * so that it is ensured that no IRQ can arrive to the bad
		 * cores.
		 * The following lines are meant to re-disable IRQs for
		 * the good cores.
		 */
		for (irq = 0; irq < NR_EXT_IRQS; irq++)
			plic_disable_mirq(cpu, irq);
		plic_configure_threshold_m(cpu, PLIC_THR_M);
	}
	if (plic_hw_s_ctx(cpu) != PLIC_CTX_NONE) {
		/* By default, IRQs should be disabled for all contexts,
		 * so that it is ensured that no IRQ can arrive to the bad
		 * cores.
		 * The following lines are meant to re-disable IRQs for
		 * the good cores.
		 */
		for (irq = 0; irq < NR_EXT_IRQS; irq++)
			plic_disable_sirq(cpu, irq);
		plic_configure_threshold_s(cpu, PLIC_THR_S);
	}
}

void irqc_hw_enable_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_enable_irq(irq_ext(irq));
	else
		plic_hw_enable_int(irq);
}

void irqc_hw_disable_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_disable_irq(irq_ext(irq));
	else
		plic_hw_disable_int(irq);
}

void irqc_hw_mask_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_mask_irq(irq_ext(irq));
	else
		plic_hw_disable_int(irq);
}

void irqc_hw_unmask_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_unmask_irq(irq_ext(irq));
	else
		plic_hw_enable_int(irq);
}

void irqc_hw_clear_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_clear_irq(irq_ext(irq));
	else
		plic_hw_clear_int(irq);
}

void irqc_hw_trigger_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_set_irq(irq_ext(irq));
	else
		plic_hw_trigger_int(irq);
}

void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger)
{
	if (irq >= IRQ_PLATFORM)
		plic_configure_priority(irq_ext(irq),
					prio + PLIC_PRI_MIN);
}

#ifdef CONFIG_RISCV_IRQ_VERBOSE
#define plic_irq_completion_verbose(cpu, irq, is_ack)		\
	do {							\
		plic_irq_completion((cpu), (irq));		\
		printf("PLIC %d %s completion.\n", (irq),	\
		       (is_ack) ? "external" : "internal");	\
	} while (0)
#else
#define plic_irq_completion_verbose(cpu, irq, is_ack)		\
	plic_irq_completion(cpu, irq)
#endif

#ifdef CONFIG_PLIC_COMPLETION
#ifdef CONFIG_PLIC_COMPLETION_ENTRY
#define plic_completion_entry(cpu, irq)	\
	plic_irq_completion_verbose(cpu, irq, false)
#define plic_completion_exit(cpu, irq)	do { } while (0)
#endif
#ifdef CONFIG_PLIC_COMPLETION_EXIT
#define plic_completion_entry(cpu, irq)	do { } while (0)
#define plic_completion_exit(cpu, irq)	\
	plic_irq_completion_verbose(cpu, irq, false)
#endif
#else
#define plic_completion_entry(cpu, irq)	do { } while (0)
#define plic_completion_exit(cpu, irq)	do { } while (0)

void irqc_hw_ack_irq(irq_t irq)
{
	__unused uint8_t cpu = smp_processor_id();

	plic_irq_completion_verbose(cpu, irq_ext(irq), true);
}
#endif

void irqc_hw_handle_irq(void)
{
	irq_t irq;
	__unused uint8_t cpu = smp_processor_id();

	plic_hw_disable_int(IRQ_EXT);
	irq = plic_claim_irq(cpu);
	if (irq >= NR_EXT_IRQS) {
		/* Invalid IRQ */
		plic_disable_irq(irq);
		plic_irq_completion(cpu, irq);
	} else {
#ifdef CONFIG_RISCV_IRQ_VERBOSE
		printf("External IRQ %d\n", irq);
#endif
		plic_completion_entry(cpu, irq);
		if (!do_IRQ(EXT_IRQ(irq))) {
			/* No IRQ handler registered, disabling... */
			plic_disable_irq(irq);
			plic_irq_completion(cpu, irq);
		} else
			plic_completion_exit(cpu, irq);
	}
	plic_hw_enable_int(IRQ_EXT);
}
