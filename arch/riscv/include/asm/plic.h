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

#include <target/arch.h>

/* Global IRQ0 is "no interrupt" */
#define IRQ_NOIRQ			0

#define PLIC_MAX_IRQS			1024
#define PLIC_MAX_CONTEXTS		15872
#define PLIC_CTX_NONE			-1

#define PLIC_REG(offset)		(PLIC_BASE + (offset))
#define PLIC_1BIT_REG(offset, irq)	\
	REG_1BIT_ADDR(PLIC_BASE + (offset), irq)

#ifdef PLIC_CTX_BASE
#define PLIC_CONTEXT_REG(ctx, offset)	\
	(PLIC_CTX_BASE + (ctx) * PLIC_CONTEXT_SIZE + (offset))
#else
#define PLIC_CONTEXT_REG(ctx, offset)	\
	PLIC_REG(PLIC_CONTEXT_BASE + (ctx) * PLIC_CONTEXT_SIZE + (offset))
#endif

#define PLIC_BLOCK_SIZE			0x1000
#define PLIC_PRIORITYR_BASE		0
#define PLIC_PENDINGR_BASE		0x1000
#define PLIC_ENABLER_BASE		0x2000
#define PLIC_ENABLER_CONTEXT		0x80
#define PLIC_CONTEXT_BASE		0x200000
#define PLIC_CONTEXT_SIZE		PLIC_BLOCK_SIZE

#define PLIC_PRIORITYR(irq)		\
	PLIC_REG(PLIC_PRIORITYR_BASE + ((irq) << 2))
#define PLIC_PENDINGR(irq)		PLIC_1BIT_REG(PLIC_PENDINGR_BASE, irq)
#define PLIC_ENABLER(irq)		PLIC_1BIT_REG(PLIC_ENABLER_BASE, irq)

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
#define PLIC_IRQ(irq)			_BV(PLIC_IRQ_OFFSET(irq))

#define plic_enable_mirq(cpu, irq)			\
	__raw_setl(PLIC_IRQ(irq), PLIC_ENABLER(irq) +	\
		   plic_hw_m_ctx(cpu) * PLIC_ENABLER_CONTEXT)
#define plic_enable_sirq(cpu, irq)			\
	__raw_setl(PLIC_IRQ(irq), PLIC_ENABLER(irq) +	\
		   plic_hw_s_ctx(cpu) * PLIC_ENABLER_CONTEXT)
#define plic_disable_mirq(cpu, irq)			\
	__raw_clearl(PLIC_IRQ(irq), PLIC_ENABLER(irq) +	\
		     plic_hw_m_ctx(cpu) * PLIC_ENABLER_CONTEXT)
#define plic_disable_sirq(cpu, irq)			\
	__raw_clearl(PLIC_IRQ(irq), PLIC_ENABLER(irq) +	\
		     plic_hw_s_ctx(cpu) * PLIC_ENABLER_CONTEXT)
#define plic_irq_pending(irq)				\
	(__raw_readl(PLIC_PENDINGR(irq)) & PLIC_IRQ(irq))
#define plic_clear_irq(irq)				\
	__raw_clearl(PLIC_IRQ(irq), PLIC_PENDINGR(irq))
#define plic_set_irq(irq)				\
	__raw_setl(PLIC_IRQ(irq), PLIC_PENDINGR(irq))
/* In case we only use 1 priority */
#define plic_mask_irq(irq)				\
	plic_configure_priority(irq, PLIC_PRI_NONE)
#define plic_unmask_irq(irq)				\
	plic_configure_priority(irq, PLIC_PRI_MIN)
#define plic_configure_priority(irq, pri)		\
	__raw_writel(pri, PLIC_PRIORITYR(irq))
#define plic_configure_threashold_m(cpu, pri)		\
	__raw_writel(pri,				\
		     PLIC_PRIORITY_THRESHOLDR(plic_hw_m_ctx(cpu)))
#define plic_configure_threashold_s(cpu, pri)		\
	__raw_writel(pri,				\
		     PLIC_PRIORITY_THRESHOLDR(plic_hw_s_ctx(cpu)))
#define plic_claim_mirq(cpu)				\
	__raw_readl(PLIC_CLAIMR(plic_hw_m_ctx(cpu)))
#define plic_claim_sirq(cpu)				\
	__raw_readl(PLIC_CLAIMR(plic_hw_s_ctx(cpu)))
#define plic_mirq_completion(cpu, irq)			\
	__raw_writel(irq, PLIC_COMPLETIONR(plic_hw_m_ctx(cpu)))
#define plic_sirq_completion(cpu, irq)			\
	__raw_writel(irq, PLIC_COMPLETIONR(plic_hw_s_ctx(cpu)))

#ifdef CONFIG_RISCV_EXIT_M
#define plic_enable_irq(irq)		\
	plic_enable_mirq(smp_processor_id(), irq)
#define plic_disable_irq(irq)		\
	plic_disable_mirq(smp_processor_id(), irq)
#define plic_claim_irq(cpu)		plic_claim_mirq(cpu)
#define plic_irq_completion(cpu, irq)	plic_mirq_completion(cpu, irq)
#endif
#ifdef CONFIG_RISCV_EXIT_S
#define plic_enable_irq(irq)		\
	plic_enable_sirq(smp_processor_id(), irq)
#define plic_disable_irq(irq)		\
	plic_disable_sirq(smp_processor_id(), irq)
#define plic_claim_irq(cpu)		plic_claim_sirq(cpu)
#define plic_irq_completion(cpu, irq)	plic_sirq_completion(cpu, irq)
#endif

#ifdef CONFIG_PLIC
void plic_init_default(void);
void plic_sbi_init_cold(void);
void plic_sbi_init_warm(cpu_t cpu);
#else
#define plic_init_default()		do { } while (0)
#define plic_sbi_init_cold()		do { } while (0)
#define plic_sbi_init_warm(cpu)		do { } while (0)
#endif

#ifdef ARCH_HAVE_IRQC
#ifdef CONFIG_PLIC
void irqc_hw_enable_irq(irq_t irq);
void irqc_hw_disable_irq(irq_t irq);
#ifdef CONFIG_PLIC_MASK_PRIORITY
void irqc_hw_mask_irq(irq_t irq);
void irqc_hw_unmask_irq(irq_t irq);
#endif
void irqc_hw_clear_irq(irq_t irq);
void irqc_hw_trigger_irq(irq_t irq);
void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger);
void irqc_hw_handle_irq(void);
#ifndef CONFIG_PLIC_COMPLETION
void irqc_hw_ack_irq(irq_t irq);
#endif
#ifdef CONFIG_SBI
/* PLIC requires no special initialization other than that is done
 * in SBI.
 */
#define irqc_hw_ctrl_init()	plic_hw_ctrl_init()
#ifdef CONFIG_SMP
#define irqc_hw_smp_init()	do { } while (0)
#endif /* CONFIG_SMP */
#else /* CONFIG_SBI */
#define irqc_hw_ctrl_init()				\
	do {						\
		plic_hw_ctrl_init();			\
		plic_sbi_init_cold();			\
	} while (0)
#ifdef CONFIG_SMP
#define irqc_hw_smp_init()	plic_sbi_init_warm(smp_processor_id())
#endif /* CONFIG_SMP */
#endif /* CONFIG_SBI */
#ifdef CONFIG_MMU
#define irqc_hw_mmu_init()	plic_hw_mmu_init()
#endif /* CONFIG_MMU */
#else /* CONFIG_PLIC */
/* Specially when a platform supports CLINT and uses that as IRQC */
#define irqc_hw_ctrl_init()	do { } while (0)
#define irqc_hw_handle_irq()	do { } while (0)
#define irqc_hw_enable_irq(irq)		do { } while (0)
#define irqc_hw_disable_irq(irq)	do { } while (0)
#ifdef CONFIG_PLIC_MASK_PRIORITY
#define irqc_hw_mask_irq(irq)		do { } while (0)
#define irqc_hw_unmask_irq(irq)		do { } while (0)
#endif
#define irqc_hw_clear_irq(irq)		do { } while (0)
#define irqc_hw_trigger_irq(irq)	do { } while (0)
#define irqc_hw_configure_irq(irq, prio, trigger)	\
					do { } while (0)
#define irqc_hw_handle_irq()		do { } while (0)
#ifdef CONFIG_PLIC_MASK_PRIORITY
#define irqc_hw_ack_irq(irq)		do { } while (0)
#endif
#ifdef CONFIG_SMP
#define irqc_hw_smp_init()		do { } while (0)
#endif /* CONFIG_SMP */
#ifdef CONFIG_MMU
#define irqc_hw_mmu_init()		do { } while (0)
#endif /* CONFIG_SMP */
#endif /* CONFIG_PLIC */
#endif /* ARCH_HAVE_IRQC */

#endif /* __PLIC_RISCV_H_INCLUDE__ */
