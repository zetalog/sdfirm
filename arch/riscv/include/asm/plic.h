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
 * @(#)plic.h: platform level interrupt controller (PLIC) definitions
 * $Id: plic.h,v 1.1 2019-10-11 15:43:00 zhenglv Exp $
 */

#ifndef __PLIC_RISCV_H_INCLUDE__
#define __PLIC_RISCV_H_INCLUDE__

#include <target/types.h>
#include <target/arch.h>

/* Global IRQ0 is "no interrupt" */
#define IRQ_NOIRQ			0

#define PLIC_MAX_IRQS			1024
#define PLIC_MAX_CONTEXTS		15872

#define PLIC_REG(offset)		(PLIC_BASE + (offset))
#define PLIC_CONTEXT_REG(ctx, offset)	\
	PLIC_REG(PLIC_CONTEXT_BASE + (ctx) * PLIC_CONTEXT_SIZE + (offset))

#define PLIC_BLOCK_SIZE			0x1000
#define PLIC_PRIORITYR_BASE		0
#define PLIC_PENDINGR_BASE		0x1000
#define PLIC_ENABLER_BASE		0x2000
#define PLIC_CONTEXT_BASE		0x200000
#define PLIC_CONTEXT_SIZE		PLIC_BLOCK_SIZE

#define PLIC_PRIORITYR(irq)		PLIC_REG(PLIC_PRIORITYR_BASE + (irq) * 4)
#define PLIC_PENDINGR(irq)		REG_1BIT_ADDR(PLIC_PENDINGR_BASE, irq)
#define PLIC_ENABLER(irq)		REG_1BIT_ADDR(PLIC_ENABLER_BASE, irq)

#define PLIC_PRIORITY_THRESHOLDR(ctx)	PLIC_CONTEXT_REG(ctx, 0x00)
#define PLIC_CLAIMR(ctx)		PLIC_CONTEXT_REG(ctx, 0x04)
#define PLIC_COMPLETIONR(ctx)		PLIC_CONTEXT_REG(ctx, 0x04)

#define PLIC_PRI_NONE			0
#define PLIC_PRI_MIN			1
#define PLIC_PRI_MAX			PLIC_HW_PRI_MAX
#ifdef PLIC_HW_PRI_DEF
#define PLIC_PRI_DEF			PLIC_HW_PRI_DEF
#else
#define PLIC_PRI_DEF			PLIC_HW_PRI_MIN
#endif

#define PLIC_IRQ_OFFSET(irq)		REG_1BIT_OFFSET(irq)
#define PLIC_IRQ_MASK			REG_1BIT_MASK
/* Macros used to program PLIC_PENDINGR/PLIC_ENABLER */
#define PLIC_GET_IRQ(irq, value)	_GET_FVn(irq, PLIC_IRQ, value)
#define PLIC_SET_IRQ(irq, value)	_SET_FVn(irq, PLIC_IRQ, value)
#define PLIC_IRQ(irq)			PLIC_IRQ_OFFSET(irq)

#define plic_enable_irq(irq)		\
	__raw_setl(PLIC_IRQ(irq), PLIC_ENABLER(irq))
#define plic_disable_irq(irq)		\
	__raw_setl(PLIC_IRQ(irq), PLIC_ENABLER(irq))
#define plic_irq_pending(irq)		\
	(__raw_readl(PLIC_PENDINGR(irq)) & PLIC_IRQ(irq))
#define plic_clear_irq(irq)		\
	__raw_clearl(PLIC_IRQ(irq), PLIC_PENDINGR(irq))
#define plic_set_irq(irq)		\
	__raw_setl(PLIC_IRQ(irq), PLIC_PENDINGR(irq))
/* In case we only use 1 priority */
#define plic_mask_irq(irq)		\
	__raw_writel(PLIC_PRI_NONE, PLIC_PRIORITYR(irq))
#define plic_unmask_irq(irq)		\
	__raw_writel(PLIC_PRI_MIN, PLIC_PRIORITYR(irq))
#define plic_configure_priority(irq, pri)		\
	__raw_writel_mask(PLIC_PRI(pri < PLIC_PRI_MIN ?	\
			  PLIC_PRI_MIN : pri),		\
			  PLIC_PRI(PLIC_PRI_MASK),	\
			  PLIC_PRIORITYR(irq))
#define plic_claim_irq(cpu)		\
	__raw_readl(PLIC_CLAIMR(cpu))
#define plic_irq_completion(cpu, irq)	\
	__raw_writel(irq, PLIC_COMPLETIONR(cpu))

#define irqc_hw_enable_irq(irq)		plic_enable_irq(irq)
#define irqc_hw_disable_irq(irq)	plic_disable_irq(irq)
#define irqc_hw_clear_irq(irq)		plic_clear_irq(irq)
#define irqc_hw_trigger_irq(irq)	plic_set_irq(irq)

#define irqc_hw_ctrl_init()		plic_hw_ctrl_init()
#define irqc_hw_configure_irq(irq, prio, trig)		\
	do {						\
		plic_configure_priority(irq, prio);	\
		plic_hw_configure_trigger(irq, trig);	\
	} while (0)

#endif /* __PLIC_RISCV_H_INCLUDE__ */
