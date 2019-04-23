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
 * @(#)gicv2.h: generic interrupt controller v2 definitions
 * $Id: gicv2.h,v 1.279 2018-7-22 10:19:18 zhenglv Exp $
 */

#ifndef __GICV2_ARM64_H_INCLUDE__
#define __GICV2_ARM64_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/gic.h>

/* Distributor interface register map */
/* 4.3.18 Identification registers - Peripheral ID2 Register, ICPIDR2 */
#define GICD_ICPIDR2			GICD_REG(0xFE8) /* Peripheral ID2 */

/* GICD_ICFGR */
#define GICD_MODEL(value)		((value & 1) << 0)
#define GICD_MODEL_N_N			0
#define GICD_MODEL_1_N			1

/* GICD_ICPIDR2 */
#define GICD_ARCH_REV_OFFSET		4
#define GICD_ARCH_REV_MASK		GIC_4BIT_MASK
#define GICD_ARCH_REV(value)		GICD_GET_FV(ARCH_REV, value)

/* CPU interface register map */
#define GICC_REG(offset)		(GICC_BASE+(offset))
#define GICC_1BIT_REG(offset, n)	GIC_1BIT_REG(GICC_BASE+(offset), n)
#define GICC_GET_FV(name, value)	GIC_GET_FV(GICC_##name, value)
#define GICC_SET_FV(name, value)	GIC_SET_FV(GICC_##name, value)
#define GICCn_GET_FV(n, name, value)	GICn_GET_FV(n, GICC_##name, value)
#define GICCn_SET_FV(n, name, value)	GICn_SET_FV(n, GICC_##name, value)

/* 4.4.1 CPU Interface Control Register, GICC_CTLR */
#define GICC_CTLR			GICC_REG(0x000)
/* 4.4.2 Interrupt Priority Mask Register, GICC_PMR */
#define GICC_PMR			GICC_REG(0x004)
/* 4.4.3 Binary Point Register, GICC_BPR */
#define GICC_BPR			GICC_REG(0x008)
/* 4.4.4 Interrupt Acknowledge Register, GICC_IAR */
#define GICC_IAR			GICC_REG(0x00C)
/* 4.4.5 End of Interrupt Register, GICC_EOIR */
#define GICC_EOIR			GICC_REG(0x010)
/* 4.4.6 Running Priority Register, GICC_RPR */
#define GICC_RPR			GICC_REG(0x014)
/* 4.4.7 Highest Priority Pending Interrupt Register, GICC_HPPIR */
#define GICC_HPPIR			GICC_REG(0x018)
/* 4.4.8 Aliased Binary Point Register, GICC_ABPR */
#define GICC_ABPR			GICC_REG(0x01C)
/* 4.4.9 Aliased Interrupt Acknowledge Register, GICC_AIAR */
#define GICC_AIAR			GICC_REG(0x020)
/* 4.4.10 Aliased End of Interrupt Register, GICC_AEOIR */
#define GICC_AEOIR			GICC_REG(0x024)
/* 4.4.11 Aliased Highest Priority Pending Interrupt Register, GICC_AHPPIR */
#define GICC_AHPPIR			GICC_REG(0x028)
/* 4.4.12 Active Priorities Registers, GICC_APRn */
#define GICC_APR(n)			GICC_1BIT_REG(0x0D0, n)
/* 4.4.13 Non-secure Active Priorities Registers, GICC_NSAPRn */
#define GICC_NSAPR(n)			GICC_1BIT_REG(0x0E0, n)
/* 4.4.14 CPU Interface Identification Register, GICC_IIDR */
#define GICC_IIDR			GICC_REG(0x0FC)
/* 4.4.15 Deactivate Interrupt Register, GICC_DIR */
#define GICC_DIR			GICC_REG(0x100)

/* GICC_CTLR */
#define GICC_ENABLE			_BV(0)
#define GICC_FIQ_BYP_DIS		_BV(5)
#define GICC_IRQ_BYP_DIS		_BV(6)
#define GICC_EOI_MODE_NS		_BV(9)

/* GICC_PMR/GICC_RPR */
#define GICC_PRIORITY(value)		GIC_PRIORITY0(value)

/* GICC_BPR/GICC_ABPR */
#define GICC_BINARY_POINT_OFFSET	0
#define GICC_BINARY_POINT_MASK		GIC_3BIT_MASK
#define GICC_BINARY_POINT(value)	GICC_SET_FV(BINARY_POINT, value)

/* GICC_IAR/GICC_EOIR/GICC_HPPIR/GICC_AIAR/GICC_AEOIR/GICC_AHPIR/GICC_DIR */
#define GICC_IRQ_ID_OFFSET		0
#define GICC_IRQ_ID_MASK		GIC_10BIT_MASK
#define GICC_GET_IRQ(value)		GICC_GET_FV(IRQ_ID, value)
#define GICC_SET_IRQ(value)		GICC_SET_FV(IRQ_ID, value)
#define GICC_CPU_ID_OFFSET		10
#define GICC_CPU_ID_MASK		GIC_3BIT_MASK
#define GICC_GET_CPU(value)		GICC_GET_FV(CPU_ID, value)
#define GICC_SET_CPU(value)		GICC_SET_FV(CPU_ID, value)

/* GICC_APR/GICC_NSAPR */
#define GICC_ACTIVE_PRIORITY_OFFSET(n)	GIC_1BIT_OFFSET(n)
#define GICC_ACTIVE_PRIORITY_MASK	GIC_1BIT_MASK
#define GICC_ACTIVE_PRIORITY(n, value)	GICCn_SET_FV(n, ACTIVE_PRIORITY, value)

/* GICC_IIDR */
#define GICC_IMPLEMENTER_OFFSET		0
#define GICC_IMPLEMENTER_MASK		GIC_12BIT_MASK
#define GICC_IMPLEMENTER(value)		GICC_GET_FV(IMPLEMENTER, value)
#define GICC_REVISION_OFFSET		12
#define GICC_REVISION_MASK		GIC_4BIT_MASK
#define GICC_REVISION(value)		GICC_GET_FV(REVISION, value)
#define GICC_ARCH_REV_OFFSET		16
#define GICC_ARCH_REV_MASK		GIC_4BIT_MASK
#define GICC_ARCH_REV(value)		GICC_GET_FV(ARCH_REV, value)
#define GICC_PRODUCT_ID_OFFSET		20
#define GICC_PRODUCT_ID_MASK		GIC_12BIT_MASK
#define GICC_PRODUCT_ID(value)		GICC_GET_FV(PRODUCT_ID, value)

/* Group0 is secure and group1 is non-secure */
#define gicd_group_secure_irq(irq)	\
	__raw_clearl(GIC_INTERRUPT_ID(irq), GICD_IGROUPR(irq))
#define gicd_group_nonsecure_irq(irq)	\
	__raw_setl(GIC_INTERRUPT_ID(irq), GICD_IGROUPR(irq))

#define gic_begin_irq(irq, cpu)				\
	do {						\
		uint32_t iar = __raw_readl(GICC_IAR);	\
		irq = GICC_GET_IRQ(iar);		\
		cpu = GICC_GET_CPU(iar);		\
	} while (0)
#define gic_end_irq(irq, cpu)				\
	do {						\
		__raw_writel(GICC_SET_IRQ(irq) |	\
			     GICC_SET_CPU(cpu),		\
			     GICC_EOIR);		\
	} while (0)

#define irqc_hw_enable_irq(irq)		gicd_enable_irq(irq)
#define irqc_hw_disable_irq(irq)	gicd_disable_irq(irq)
#define irqc_hw_trigger_irq(irq)	gicd_trigger_irq(irq)
#define irqc_hw_clear_irq(irq)		gicd_clear_irq(irq)
#define irqc_hw_configure_irq(irq, priority, trigger)	\
	gicd_configure_irq(irq, priority, trigger)

void gicv2_init_gicd(void);
void gicv2_init_gicc(void);

#endif /* __GICV2_ARM64_H_INCLUDE__ */
