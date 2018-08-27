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
 * @(#)gic.h: generic interrupt controller definitions
 * $Id: gic.h,v 1.279 2018-7-19 10:19:18 zhenglv Exp $
 */

#ifndef __GIC_ARM64_H_INCLUDE__
#define __GIC_ARM64_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

#ifndef ARCH_HAVE_IRQC
#define ARCH_HAVE_IRQC
#else
#error "Multiple IRQ controller defined"
#endif

/* ARM Generic Interrupt Controller Architecture Specification v2 */

/* base address and bit offset generator for bits-registers */
#define GIC_1BIT_MASK			0x001
#define GIC_2BIT_MASK			0x003
#define GIC_3BIT_MASK			0x007
#define GIC_4BIT_MASK			0x00F
#define GIC_5BIT_MASK			0x01F
#define GIC_6BIT_MASK			0x03F
#define GIC_7BIT_MASK			0x07F
#define GIC_8BIT_MASK			0x0FF
#define GIC_9BIT_MASK			0x1FF
#define GIC_10BIT_MASK			0x3FF
#define GIC_11BIT_MASK			0x7FF
#define GIC_12BIT_MASK			0xFFF
#define GIC_24BIT_MASK			0xFFFFFF

#define GIC_1BIT_REG(base, n)		((base)+(((n) & (~31)) >> 3))
#define GIC_2BIT_REG(base, n)		((base)+(((n) & (~15)) >> 2))
#define GIC_4BIT_REG(base, n)		((base)+(((n) & (~7 )) >> 1))
#define GIC_8BIT_REG(base, n)		((base)+(((n) & (~3 )) >> 0))

#define GIC_1BIT_OFFSET(n)		(((n) & 31) << 0)
#define GIC_2BIT_OFFSET(n)		(((n) & 15) << 1)
#define GIC_4BIT_OFFSET(n)		(((n) &  7) << 2)
#define GIC_8BIT_OFFSET(n)		(((n) &  3) << 3)

#define GIC_GET_FV(name, value)		_GET_FV(name, value)
#define GIC_SET_FV(name, value)		_SET_FV(name, value)
#define GICn_GET_FV(n, name, value)	\
	(((value) >> (name##_OFFSET(n))) & (name##_MASK))
#define GICn_SET_FV(n, name, value)	\
	(((value) & (name##_MASK)) << (name##_OFFSET(n)))

/* Distributor register map */
#define GICD_REG(offset)		(GICD_BASE+(offset))
#define GICD_1BIT_REG(offset, n)	GIC_1BIT_REG(GICD_BASE+(offset), n)
#define GICD_2BIT_REG(offset, n)	GIC_2BIT_REG(GICD_BASE+(offset), n)
#define GICD_8BIT_REG(offset, n)	GIC_8BIT_REG(GICD_BASE+(offset), n)
#define GICD_GET_FV(name, value)	GIC_GET_FV(GICD_##name, value)
#define GICD_SET_FV(name, value)	GIC_SET_FV(GICD_##name, value)
#define GICDn_GET_FV(n, name, value)	GICn_GET_FV(n, GICD_##name, value)
#define GICDn_SET_FV(n, name, value)	GICn_SET_FV(n, GICD_##name, value)

/* v2: 4.3.1 Distributor Control Register, GICD_CTLR */
/* v3: 8.9.4 GICD_CTLR, Distributor Control Register */
#define GICD_CTLR			GICD_REG(0x000)
/* v2: 4.3.2 Interrupt Controller Type Register, GICD_TYPER */
/* v3: 8.9.24 GICD_TYPER, Interrupt Controller Type Register */
#define GICD_TYPER			GICD_REG(0x004)
/* v2: 4.3.3 Distributor Implementer Identification Register, GICD_IIDR */
/* v3: 8.9.11 GICD_IIDR, Distributor Implementer Identification Register */
#define GICD_IIDR			GICD_REG(0x008)
/* v2: 4.3.4 Interrupt Group Registers, GICD_IGROUPRn */
/* v3: 8.9.9 GICD_IGROUPR<n>, Interrupt Group Registers, n = 0 - 31 */
#define GICD_IGROUPR(n)			GICD_1BIT_REG(0x080, n)
/* v2: 4.3.5 Interrupt Set-Enable Registers, GICD_ISENABLERn */
/* v3: 8.9.15 GICD_ISENABLER<n>,
 *            Interrupt Set-Enable Registers, n = 0 - 31
 */
#define GICD_ISENABLER(n)		GICD_1BIT_REG(0x100, n)
/* v2: 4.3.6 Interrupt Clear-Enable Registers, GICD_ICENABLERn */
/* v3: 8.9.6 GICD_ICENABLER<n>,
 *           Interrupt Clear-Enable Registers, n = 0 - 31
 */
#define GICD_ICENABLER(n)		GICD_1BIT_REG(0x180, n)
/* v2: 4.3.7 Interrupt Set-Pending Registers, GICD_ISPENDRn */
/* v3: 8.9.16 GICD_ISPENDR<n>,
 *            Interrupt Set-Pending Registers, n = 0 - 31
 */
#define GICD_ISPENDR(n)			GICD_1BIT_REG(0x200, n)
/* v2: 4.3.8 Interrupt Clear-Pending Registers, GICD_ICPENDRn */
/* v3: 8.9.8 GICD_ICPENDR<n>,
 *           Interrupt Clear-Pending Registers, n = 0 - 31
 */
#define GICD_ICPENDR(n)			GICD_1BIT_REG(0x280, n)
/* v2: 4.3.9 Interrupt Set-Active Registers, GICD_ISACTIVERn */
/* v3: 8.9.14 GICD_ISACTIVER<n>,
 *            Interrupt Set-Active Registers, n = 0 - 31
 */
#define GICD_ISACTIVER(n)		GICD_1BIT_REG(0x300, n)
/* v2: 4.3.10 Interrupt Clear-Active Registers, GICD_ICACTIVERn */
/* v3: 8.9.5 GICD_ICACTIVER<n>,
 *           Interrupt Clear-Active Registers, n = 0 - 31
 */
#define GICD_ICACTIVER(n)		GICD_1BIT_REG(0x380, n)
/* v2: 4.3.11 Interrupt Priority Registers, GICD_IPRIORITYRn */
/* v3: 8.9.12 GICD_IPRIORITYR<n>,
 *            Interrupt Priority Registers, n = 0 - 254
 */
#define GICD_IPRIORITYR(n)		GICD_8BIT_REG(0x400, n)
/* v2: 4.3.12 Interrupt Processor Targets Registers, GICD_ITARGETSRn */
/* v3: 8.9.17 GICD_ITARGETSR<n>,
 *            Interrupt Processor Targets Registers, n = 0 - 254
 */
#define GICD_ITARGETSR(n)		GICD_8BIT_REG(0x800, n)
/* v2: 4.3.13 Interrupt Configuration Registers, GICD_ICFGRn */
/* v3: 8.9.7 GICD_ICFGR<n>,
 *           Interrupt Configuration Registers, n = 0 - 63
 */
#define GICD_ICFGR(n)			GICD_2BIT_REG(0xC00, n)
/* v2: 4.3.14 Non-secure Access Control Registers, GICD_NSACRn */
/* v3: 8.9.18 GICD_NSACR<n>,
 *            Non-secure Access Control Registers, n = 0 - 63
 */
#define GICD_NSACR(n)			GICD_2BIT_REG(0xE00, n)
/* v2: 4.3.15 Software Generated Interrupt Register, GICD_SGIR */
/* v3: 8.9.21 GICD_SGIR, Software Generated Interrupt Register */
#define GICD_SGIR			GICD_REG(0xF00)
/* v2: 4.3.16 SGI Clear-Pending Registers, GICD_CPENDSGIRn */
/* v3: 8.9.3 GICD_CPENDSGIR<n>,
 *           SGI Clear-Pending Registers, n = 0 - 3
 */
#define GICD_CPENDSGIR(n)		GICD_8BIT_REG(0xF10, n)
/* v2: 4.3.17 SGI Set-Pending Registers, GICD_SPENDSGIRn */
/* v3: 8.9.22 GICD_SPENDSGIR<n>,
 *            SGI Set-Pending Registers, n = 0 - 3
 */
#define GICD_SPENDSGIR(inst, n)		GICD_8BIT_REG(inst, 0xF20, n)

/* GICD_CTLR */
#define GICD_ENABLE_GRP0		_BV(0)
#define GICD_ENABLE_GRP1		_BV(1)

/* GICD_TYPER */
#define GICD_IT_LINES_NUMBER_OFFSET	0
#define GICD_IT_LINES_NUMBER_MASK	GIC_5BIT_MASK
#define GICD_IT_LINES_NUMBER(value)	GICD_GET_FV(IT_LINES_NUMBER, value)
#define GICD_CPU_NUMBER_OFFSET		5
#define GICD_CPU_NUMBER_MASK		GIC_3BIT_MASK
#define GICD_CPU_NUMBER(value)		GICD_GET_FV(CPU_NUMBER, value)
#define GICD_SECURITY_EXTN		_BV(10)
#define GICD_LSPI_OFFSET		11
#define GICD_LSPI_MASK			GIC_5BIT_MASK
#define GICD_LSPI(value)		GICD_GET_FV(LSPI, value)

/* GICD_IIDR */
#define GICD_IMPLEMENTER_OFFSET		0
#define GICD_IMPLEMENTER_MASK		GIC_12BIT_MASK
#define GICD_IMPLEMENTER(value)		GICD_GET_FV(IMPLEMENTER, value)
#define GICD_REVISION_OFFSET		12
#define GICD_REVISION_MASK		GIC_4BIT_MASK
#define GICD_REVISION(value)		GICD_GET_FV(REVISION, value)
#define GICD_VARIANT_OFFSET		16
#define GICD_VARIANT_MASK		GIC_4BIT_MASK
#define GICD_VARIANT(value)		GICD_GET_FV(VARIANT, value)
#define GICD_PRODUCT_ID_OFFSET		24
#define GICD_PRODUCT_ID_MASK		GIC_8BIT_MASK
#define GICD_PRODUCT_ID(value)		GICD_GET_FV(PRODUCT_ID, value)

/* GICD/R_IGROUPR */
#define GIC_GROUP1(n)			_BV(GIC_1BIT_OFFSET(n))

/* GICD/R_ISENABLER/GICD/R_ICENABLER
 * GICD/R_ISPENDR/GICD/R_ICPENDR
 * GICD/R_ISACTIVER/GICD/R_ICACTIVER
 */
#define GIC_INTERRUPT_ID(n)		_BV(GIC_1BIT_OFFSET(n))

/* GICD/R_IPRORITYR */
#define GIC_PRIORITY_OFFSET(n)		GIC_8BIT_OFFSET(n)
#define GIC_PRIORITY_MASK		GIC_8BIT_MASK
#define GIC_PRIORITY(n, value)		\
	GICn_SET_FV(n, GIC_PRIORITY, (value) << GIC_PRIORITY_SHIFT)
#define GIC_PRIORITY_IDLE		GIC_PRIORITY_MASK

/* GICD/R_ITARGETSR */
#define GIC_CPU_TARGETS_OFFSET(n)	GIC_8BIT_OFFSET(n)
#define GIC_CPU_TARGETS_MASK		GIC_8BIT_MASK
#define GIC_CPU_TARGETS(n, value)	GICn_SET_FV(n, GIC_CPU_TARGETS, value)
#define GIC_CPU_TARGETS_MAX		0xFF

/* GICD/R_ICFGR */
#define GIC_INT_CONFIG_OFFSET(n)	GIC_2BIT_OFFSET(n)
#define GIC_INT_CONFIG_MASK		GIC_2BIT_MASK
#define GIC_INT_CONFIG(n, value)	GICn_SET_FV(n, GIC_INT_CONFIG, value)
#define GIC_TRIGGER(value)		((value & 1) << 1)
#define GIC_TRIGGER_LEVEL		0
#define GIC_TRIGGER_EDGE		1

/* GICD_NSACR */
#define GIC_NS_ACCESS_OFFSET(n)		GIC_2BIT_OFFSET(n)
#define GIC_NS_ACCESS_MASK		GIC_2BIT_MASK
#define GIC_NS_ACCESS(n, value)		GICn_SET_FV(n, GIC_NS_ACCESS, value)
#define GIC_NSAC_ALL			0
#define GIC_NSAC_WSPEND_WSGI		1
#define GIC_NSAC_WCPEND_RXACTIVE	2
#define GIC_NSAC_RWTARGETS		3

/* GICD_SGIR */
#define GICD_SGIINTID_OFFSET		0
#define GICD_SGIINTID_MASK		GIC_4BIT_MASK
#define GICD_SGIINTID(value)		GICD_SET_FV(SGIINTID, value)
#define GICD_NSATT			_BV(15)
#define GICD_CPU_TARGET_LIST_OFFSET	16
#define GICD_CPU_TARGET_LIST_MASK	GIC_8BIT_MASK
#define GICD_CPU_TARGET_LIST(value)	GICD_SET_FV(CPU_TARGET_LIST, value)
#define GICD_TARGET_LIST_FILTER_OFFSET	24
#define GICD_TARGET_LIST_FILTER_MASK	GIC_2BIT_MASK
#define GICD_TARGET_LIST_FILTER(value)	GICD_SET_FV(TARGET_LIST_FILTER, value)

/* GICD_CPENDSGIR/GICD_SPENDSGIR */
#define GICD_SGI_PENDING_OFFSET(n)	GIC_8BIT_OFFSET(n)
#define GICD_SGI_PENDING_MASK		GIC_8BIT_MASK
#define GICD_SGI_PENDING(n, value)	GICDn_SET_FV(n, SGI_PENDING, value)

/* Generic values */
#define GIC_PRIORITY0(value)		GIC_PRIORITY(0, value)
#define GIC_PRIORITY_MAX		(0xFF >> GIC_PRIORITY_SHIFT)

/* Allow implementation specific initialization */
void irqc_hw_ctrl_init(void);
void gic_hw_ctrl_init(void);

#define gicd_hw_enable_irq(irq)		\
	__raw_writel(GIC_INTERRUPT_ID(irq), GICD_ISENABLER(irq))
#define gicd_hw_disable_irq(irq)	\
	__raw_setl(GIC_INTERRUPT_ID(irq), GICD_ICENABLER(irq))
#define gicd_hw_trigger_irq(irq)	\
	__raw_setl(GIC_INTERRUPT_ID(irq), GICD_ISPENDR(irq))
#define gicd_hw_clear_irq(irq)		\
	__raw_setl(GIC_INTERRUPT_ID(irq), GICD_ICPENDR(irq))
#define gicd_configure_irq(irq, priority, trigger)			\
	do {								\
		uint32_t cfg;						\
		__raw_writel_mask(GIC_PRIORITY(irq, priority),		\
				  GIC_PRIORITY(irq, GIC_PRIORITY_MASK),	\
				  GICD_IPRIORITYR(irq));		\
		if (trigger == IRQ_LEVEL_TRIGGERED)			\
			cfg = GIC_TRIGGER(GIC_TRIGGER_LEVEL);		\
		else							\
			cfg = GIC_TRIGGER(GIC_TRIGGER_EDGE);		\
		__raw_writel_mask(GIC_INT_CONFIG(irq, cfg),		\
				  GIC_INT_CONFIG(irq,			\
						 GIC_INT_CONFIG_MASK),	\
				  GICD_ICFGR(irq));			\
	} while (0)
#define gicd_enable_all_irqs(max_irq)					\
	do {								\
		irq_t irq;						\
		for (irq = 0; irq <= NR_IRQS; irq += 32) {		\
			__raw_writel(0xFFFFFFFF, GICD_ISENABLER(irq));	\
		}							\
	} while (0)
#define gicd_disable_all_irqs(max_irq)					\
	do {								\
		irq_t irq;						\
		for (irq = 0; irq <= NR_IRQS; irq += 32) {		\
			__raw_writel(0xFFFFFFFF, GICD_ICENABLER(irq));	\
		}							\
	} while (0)
void irqc_hw_ack_irq(irq_t irq);

#endif /* __GIC_ARM64_H_INCLUDE__ */
