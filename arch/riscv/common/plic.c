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

void plic_sbi_init_cold(void)
{
	irq_t irq;

	for (irq = 0; irq < NR_IRQS; irq++)
		plic_configure_priority(irq, PLIC_PRI_MIN);
}

void plic_sbi_init_warm(cpu_t cpu)
{
	irq_t irq;

	if (plic_hw_m_ctx(cpu) != PLIC_CTX_NONE) {
		for (irq = 0; irq < NR_IRQS; irq++)
			plic_disable_mirq(cpu, irq);
		plic_configure_threashold_m(cpu, PLIC_PRI_MIN);
	}
	if (plic_hw_s_ctx(cpu) != PLIC_CTX_NONE) {
		for (irq = 0; irq < NR_IRQS; irq++)
			plic_disable_sirq(cpu, irq);
		plic_configure_threashold_s(cpu, PLIC_PRI_NONE);
	}
}

void irqc_hw_enable_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_enable_irq(irq - IRQ_PLATFORM);
	else
		plic_hw_enable_int(irq);
}

void irqc_hw_disable_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_disable_irq(irq - IRQ_PLATFORM);
	else
		plic_hw_disable_int(irq);
}

void irqc_hw_mask_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_mask_irq(irq - IRQ_PLATFORM);
	else
		plic_hw_disable_int(irq);
}

void irqc_hw_unmask_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_unmask_irq(irq - IRQ_PLATFORM);
	else
		plic_hw_enable_int(irq);
}

void irqc_hw_clear_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_clear_irq(irq - IRQ_PLATFORM);
	else
		plic_hw_clear_int(irq);
}

void irqc_hw_trigger_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		plic_set_irq(irq - IRQ_PLATFORM);
	else
		plic_hw_trigger_int(irq);
}

void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger)
{
	if (irq >= IRQ_PLATFORM)
		plic_configure_priority(irq - IRQ_PLATFORM,
					prio + PLIC_PRI_MIN);
}

#ifdef CONFIG_PLIC_COMPLETION
#define plic_completion(cpu, irq)	plic_irq_completion(cpu, irq)
#else
#define plic_completion(cpu, irq)	do { } while (0)

void irqc_hw_ack_irq(irq_t irq)
{
	uint8_t cpu = smp_processor_id();

	plic_irq_completion(cpu, irq);
}
#endif

void irqc_hw_handle_irq(void)
{
	irq_t irq;
	uint8_t cpu = smp_processor_id();

	plic_hw_disable_int(IRQ_EXT);
	irq = plic_claim_irq(cpu);
	if (irq >= NR_EXT_IRQS) {
		/* Invalid IRQ */
		plic_disable_irq(irq);
		plic_irq_completion(cpu, irq);
	} else {
		if (!do_IRQ(irq + IRQ_PLATFORM)) {
			/* No IRQ handler registered, disabling... */
			plic_disable_irq(irq);
			plic_irq_completion(cpu, irq);
		} else
			plic_completion(cpu, irq);
	}
	plic_hw_enable_int(IRQ_EXT);
}
