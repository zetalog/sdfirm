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
 * @(#)vic_gic.c: generic interrupt controller implementation
 * $Id: vic_gic.c,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#include <target/irq.h>

static void irqc_hw_begin_irq(irq_t *irq, uint8_t *cpu)
{
	uint32_t iar = __raw_readl(GICC_IAR);

	*irq = GICC_GET_IRQ(iar);
	*cpu = GICC_GET_CPU(iar);
}

static void irqc_hw_end_irq(irq_t irq, uint8_t cpu)
{
	__raw_writel(GICC_SET_IRQ(irq) | GICC_SET_CPU(cpu),
		     GICC_EOIR);
}

void irqc_hw_ack_irq(irq_t irq)
{
	/* CPU ID is 0 */
	irqc_hw_end_irq(irq, 0);
}

void irqc_hw_handle_irq(void)
{
	irq_t irq;
	uint8_t cpu;

	irqc_hw_begin_irq(&irq, &cpu);
	if (irq >= NR_IRQS) {
		irqc_hw_end_irq(irq, cpu);
		return;
	}
	if (!do_IRQ(irq)) {
		irqc_hw_disable_irq(irq);
		irqc_hw_end_irq(irq, cpu);
	}
}

void irqc_hw_configure_irq(irq_t irq, uint8_t prio,
			   uint8_t trigger)
{
	uint32_t cfg;

	__raw_writel_mask(GICD_PRIORITY(irq, prio),
			  GICD_PRIORITY_MASK,
			  GICD_IPRIORITYR(irq));
	cfg = GICD_MODEL(GICD_MODEL_1_N);
	if (trigger == IRQ_LEVEL_TRIGGERED)
		cfg |= GICD_TRIGGER(GICD_TRIGGER_LEVEL);
	else
		cfg |= GICD_TRIGGER(GICD_TRIGGER_EDGE);
	__raw_writel(GICD_INT_CONFIG(irq, cfg), GICD_ICFGR(irq));
}

void irqc_hw_ctrl_init(void)
{
	gic_hw_ctrl_init();
}
