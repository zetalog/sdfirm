/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
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
 * @(#)gicv2.c: generic interrupt controller v2 implementation
 * $Id: gicv2.c,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#include <stdio.h>
#include <target/irq.h>

#ifdef CONFIG_GICv2_IRQ_NS
#define gicd_config_irq_security(irq)	gicd_group_nonsecure_irq(irq)
#else
#define gicd_config_irq_security(irq)	gicd_group_secure_irq(irq)
#endif

void gicv2_init_gicd(void)
{
	irq_t irq;

	/* Disable distributor */
	printf("GICD_CTLR: %016llx\n", GICD_CTLR);
	__raw_clearl(GICD_ENABLE_NS, GICD_CTLR);
	__raw_clearl(GICD_ENABLE, GICD_CTLR);
	/* Disable all IRQs */
	gicd_disable_all_irqs();
	for (irq = 0; irq < NR_IRQS; irq++) {
		__raw_setl(GICD_MODEL(GICD_MODEL_1_N), GICD_ICFGR(irq));
		gicd_config_irq_security(irq);
	}
	/* Enable distributor */
	__raw_setl(GICD_ENABLE, GICD_CTLR);
	__raw_setl(GICD_ENABLE_NS, GICD_CTLR);
}

void gicv2_init_gicc(void)
{
	/* Enable CPU interface */
	__raw_clearl(GICC_ENABLE, GICC_CTLR);
	/* Disable IRQ/FIQ bypass */
	__raw_setl(GICC_IRQ_BYP_DIS, GICC_CTLR);
	__raw_setl(GICC_FIQ_BYP_DIS, GICC_CTLR);
	/* Set priority mask to the lowest to allow all interrupts. */
	__raw_writel(GICC_PRIORITY(GIC_PRIORITY_MAX), GICC_PMR);
	/* Set the binary point register to indicate that every priority
	 * level is it's own priority group. See table 3-2 in the ARM GIC
	 * specification.
	 */
	__raw_writel(GICC_BINARY_POINT(2), GICC_BPR);
	/* Enable CPU interface */
	__raw_setl(GICC_ENABLE, GICC_CTLR);
}

void gicv2_enable_irq(irq_t irq)
{
	uint8_t cpu_mask;

	gicd_enable_irq(irq);
	cpu_mask = __raw_readl(GICD_ITARGETSR(irq)) &
		   GIC_CPU_TARGETS(irq, GIC_CPU_TARGETS_MASK);
	cpu_mask |= _BV(smp_processor_id());
	__raw_writel_mask(GIC_CPU_TARGETS(irq, cpu_mask),
			  GIC_CPU_TARGETS(irq, GIC_CPU_TARGETS_MASK),
			  GICD_ITARGETSR(irq));
}

void gicv2_disable_irq(irq_t irq)
{
	uint8_t cpu_mask;

	cpu_mask = __raw_readl(GICD_ITARGETSR(irq)) &
		   GIC_CPU_TARGETS(irq, GIC_CPU_TARGETS_MASK);
	cpu_mask &= ~_BV(smp_processor_id());
	__raw_writel_mask(GIC_CPU_TARGETS(irq, cpu_mask),
			  GIC_CPU_TARGETS(irq, GIC_CPU_TARGETS_MASK),
			  GICD_ITARGETSR(irq));
	gicd_disable_irq(irq);
}
