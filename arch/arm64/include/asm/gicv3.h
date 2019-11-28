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
 * @(#)gicv3.h: generic interrupt controller v3 definitions
 * $Id: gicv3.h,v 1.279 2018-7-22 10:19:18 zhenglv Exp $
 */

#ifndef __GICV3_ARM64_H_INCLUDE__
#define __GICV3_ARM64_H_INCLUDE__

#include <asm/gic.h>

/* ARM Generic Interrupt Controller Architecture Specification v3 and v4 */

/* Distributor interface register map */
/* 8.9.23 GICD_STATUSR, Error Reporting Status Register */
#define GICD_STATUSR		GICD_REG(0x0010)
/* 8.9.19 GICD_SETSPI_NSR, Set Non-secure SPI Pending Register */
#define GICD_SETSPI_NSR		GICD_REG(0x0040)
/* 8.9.1 GICD_CLRSPI_NSR, Clear Non-secure SPI Pending Register */
#define GICD_CLRSPI_NSR		GICD_REG(0x0048)
/* 8.9.20 GICD_SETSPI_SR, Set Secure SPI Pending Register */
#define GICD_SETSPI_SR		GICD_REG(0x0050)
/* 8.9.2 GICD_CLRSPI_SR, Clear Secure SPI Pending Register */
#define GICD_CLRSPI_SR		GICD_REG(0x0058)
/* 8.9.10 GICD_IGRPMODR<n>, Interrupt Group Modifier Registers, n = 0 - 31 */
#define GICD_IGRPMODR(n)	GICD_1BIT_REG(0x0D00, n)
/* 8.9.13 GICD_IROUTER<n>, Interrupt Routing Registers, n = 32 - 1019 */
#define GICD_IROUTER(n)		GICD_REG(0x6100 + (n) * 8)

/* GICD_CTRL */
#define GICD_ENABLE_GRP1_NS	_BV(1)
#define GICD_ENABLE_GRP1_S	_BV(2)
#define GICD_ARE_S		_BV(4)
#define GICD_ARE_NS		_BV(5)
#define GICD_DS			_BV(6)
#define GICD_E1NWF		_BV(7)
#define GICD_RWP		_BV(31)

/* GICD_TYPER */
#define GICD_MBIS		_BV(16)
#define GICD_LPIS		_BV(17)
#define GICD_DVIS		_BV(18)
#define GICD_A3V		_BV(24)
#define GICD_NO1N		_BV(25)
#define GICD_RSS		_BV(26)

/* GICD_STATUSR */
#define GICD_RRD		_BV(0)
#define GICD_WRD		_BV(1)
#define GICD_RWOD		_BV(2)
#define GICD_WROD		_BV(3)

/* GICD_SETSPI_NSR/GICD_CLRSPI_NSR/GICD_SETSPI_SR/GICD_CLRSPI_SR */
#define GICD_SPI_OFFSET		0
#define GICD_SPI_MASK		GIC_10BIT_MASK
#define GICD_SPI(value)		GICD_SET_FV(SPI, value)

/* GICD_IROUTER */
#define GICD_AFF0_OFFSET	0
#define GICD_AFF0_MASK		GIC_8BIT_MASK
#define GICD_AFF0(value)	GICD_SET_FV(AFF0, value)
#define GICD_AFF1_OFFSET	8
#define GICD_AFF1_MASK		GIC_8BIT_MASK
#define GICD_AFF1(value)	GICD_SET_FV(AFF1, value)
#define GICD_AFF2_OFFSET	16
#define GICD_AFF2_MASK		GIC_8BIT_MASK
#define GICD_AFF2(value)	GICD_SET_FV(AFF2, value)
#define GICD_AFF3_OFFSET	32
#define GICD_AFF3_MASK		GIC_8BIT_MASK
#define GICD_AFF3(value)	GICD_SET_FV(AFF3, value)
#define GICD_IRQ_ROUTING_MODE_OFFSET	31
#define GICD_IRQ_ROUTING_MODE_MASK	GIC_1BIT_MASK
#define GICD_IRQ_ROUTING_MODE(value)	GICD_SET_FV(IRQ_ROUTING_MODE, value)
#define GICD_IRQ_ROUTING_DIRECT		0x00
#define GICD_IRQ_ROUTING_1_OF_N		0x01

/* GICD_IPRIORITYR */
#define GIC_PRIORITY_S_HIGH		0
#define GIC_PRIORITY_NS_HIGH(max_prio)	(((uint16_t)(max_prio) + 1) / 2)
#define GIC_PRIORITY_S_LOW(max_prio)	(GIC_PRIORITY_NS_HIGH(max_prio) - 1)
#define GIC_PRIORITY_NS_LOW(max_prio)	((max_prio) - 1)

/* Redistributor interface register map */
#define GICR_REG(cpu, offset)					\
	(GICR_BASE+(((uint32_t)(cpu))<<GICR_CPU_SHIFT)+(offset))
#define GICR_BASE_REG(cpu, base, offset)			\
	(GICR_BASE+ (((uint32_t)(cpu))<<GICR_CPU_SHIFT)+	\
	 (base)+(offset))
#define GICR_2BIT_BASE_REG(cpu, base, offset, n)		\
	(GIC_2BIT_REG(GICR_BASE+				\
		      (((uint32_t)(cpu))<<GICR_CPU_SHIFT)+	\
		      (base)+(offset), n))
#define GICR_8BIT_BASE_REG(cpu, base, offset, n)		\
	(GIC_8BIT_REG(GICR_BASE+				\
		      (((uint32_t)(cpu))<<GICR_CPU_SHIFT)+	\
		      (base)+(offset), n))
#define GICR_SET_FV(name, value)	GIC_SET_FV(GICR_##name, value)
#define GICR_GET_FV(name, value)	GIC_GET_FV(GICR_##name, value)
#define GICRn_SET_FV(n, name, value)	GICn_SET_FV(n, GICR_##name, value)
#define GICRn_GET_FV(n, name, value)	GICn_GET_FV(n, GICR_##name, value)

#define GICR_SPI_BASE			(ULL(1) << 0x10)
#define GICR_VLPI_BASE			(ULL(2) << 0x10)

/* 8.11.2 GICR_CTLR, Redistributor Control Register */
#define GICR_CTLR(cpu)			GICR_REG(cpu, 0x0000)
/* 8.11.10 GICR_IIDR, Redistributor Implementer Identification Register */
#define GICR_IIDR(cpu)			GICR_REG(cpu, 0x0004)
/* 8.11.23 GICR_TYPER, Redistributor Type Register */
#define GICR_TYPER(cpu)			GICR_REG(cpu, 0x0008)
/* 8.11.21 GICR_STATUSR, Error Reporting Status Register */
#define GICR_STATUSR(cpu)		GICR_REG(cpu, 0x0010)
/* 8.11.26 GICR_WAKER, Redistributor Wake Register */
#define GICR_WAKER(cpu)			GICR_REG(cpu, 0x0014)
/* 8.11.20 GICR_SETLPIR, Set LPI Pending Register */
#define GICR_SETLPIR(cpu)		GICR_REG(cpu, 0x0040)
/* 8.11.1 GICR_CLRLPIR, Clear LPI Pending Register */
#define GICR_CLRLPIR(cpu)		GICR_REG(cpu, 0x0048)
/* 8.11.19 GICR_PROPBASER,
 * Redistributor Properties Base Address Register
 */
#define GICR_PROPBASER(cpu)		GICR_REG(cpu, 0x0070)
/* 8.11.18 GICR_PENDBASER,
 * Redistributor LPI Pending Table Base Address Register
 */
#define GICR_PENDBASER(cpu)		GICR_REG(cpu, 0x0078)
/* 8.11.25 GICR_VPROPBASER,
 * Virtual Redistributor Properties Base Address Register
 */
#define GICR_VPROPBASER(cpu)		\
	GICR_BASE_REG(cpu, GICR_VLPI_BASE, 0x0070)
/* 8.11.24 GICR_VPENDBASER,
 * Virtual Redistributor LPI Pending Table Base Address Register
 */
#define GICR_VPENDBASER(cpu)		\
	GICR_BASE_REG(cpu, GICR_VLPI_BASE, 0x0078)
/* 8.11.8 GICR_IGROUPR0, Interrupt Group Register 0 */
#define GICR_IGROUPR0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0080)
/* 8.11.15 GICR_ISENABLER0, Interrupt Set-Enable Register 0 */
#define GICR_ISENABLER0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0100)
/* 8.11.4 GICR_ICENABLER0, Interrupt Clear-Enable Register 0 */
#define GICR_ICENABLER0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0180)
/* 8.11.16 GICR_ISPENDR0, Interrupt Set-Pending Register 0 */
#define GICR_ISPENDR0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0200)
/* 8.11.7 GICR_ICPENDR0, Interrupt Clear-Pending Register 0 */
#define GICR_ICPENDR0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0280)
/* 8.11.14 GICR_ISACTIVER0, Interrupt Set-Active Register 0 */
#define GICR_ISACTIVER0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0300)
/* 8.11.3 GICR_ICACTIVER0, Interrupt Clear-Active Register 0 */
#define GICR_ICACTIVER0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0380)
/* 8.11.13 GICR_IPRIORITYR<n>, Interrupt Priority Registers, n = 0 - 7 */
#define GICR_IPRIORITYR(cpu, n)		\
	GICR_8BIT_BASE_REG(cpu, GICR_SPI_BASE, 0x0400, n)
/* 8.11.5 GICR_ICFGR0, Interrupt Configuration Register 0 */
/* 8.11.6 GICR_ICFGR1, Interrupt Configuration Register 1 */
#define GICR_ICFGR(cpu, n)		\
	GICR_2BIT_BASE_REG(cpu, GICR_SPI_BASE, 0x0C00, n)
/* 8.11.9 GICR_IGRPMODR0, Interrupt Group Modifier Register 0 */
#define GICR_IGRPMODR0(cpu)		\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0D00)
/* 8.11.17 GICR_NSACR, Non-secure Access Control Register */
#define GICR_NSACR(cpu)			\
	GICR_BASE_REG(cpu, GICR_SPI_BASE, 0x0E00)

/* GICR_CTLR */
#define GICR_UWP		_BV(31)
#define GICR_DPG1S		_BV(26)
#define GICR_DPG1NS		_BV(25)
#define GICR_DPG0		_BV(24)
#define GICR_RWP		_BV(4)
#define GICR_ENABLE_LPIS	_BV(0)

/* GICR_WAKER */
#define GICR_CHILDREN_ASLEEP	_BV(2)
#define GICR_PROCESSOR_SLEEP	_BV(1)

/* CPU interface register map */
#define ICC_SET_FV(name, value)		GIC_SET_FV(ICC_##name, value)
#define ICC_GET_FV(name, value)		GIC_GET_FV(ICC_##name, value)
#define ICCn_SET_FV(n, name, value)	GICn_SET_FV(n, ICC_##name, value)
#define ICCn_GET_FV(n, name, value)	GICn_GET_FV(n, ICC_##name, value)

#include <asm/sysreg_gicv3.h>

/* ICC_PMR_EL1/ICC_RPR_EL1 */
#define ICC_PRI(value)		GIC_PRIORITY(0, value)

/* ICC_IAR0/1_EL1/ICC_EOIR0/1_EL1/ICC_HPPIR0/1_EL1/ICC_DIR_EL1 */
#define ICC_IRQ_OFFSET		0
#define ICC_IRQ_MASK		GIC_24BIT_MASK
#define ICC_SET_IRQ(value)	ICC_SET_FV(IRQ, value)
#define ICC_GET_IRQ(value)	ICC_GET_FV(IRQ, value)

/* ICC_BPR0/1_EL1 */
#define ICC_BINARY_POINT_OFFSET	0
#define ICC_BINARY_POINT_MASK	GIC_3BIT_MASK
#define ICC_BINARY_POINT(value)	ICC_SET_FV(BINARY_POINT, value)

/* ICC_SGI1R_EL1/ICC_ASGI1R_EL1/ICC_SGI0R_EL1 */
#define ICC_TARGET_LIST_OFFSET	0
#define ICC_TARGET_LIST_MASK	GIC_16BIT_MASK
#define ICC_TARGET_LIST(value)	ICC_SET_FV(TARGET_LIST, value)
#define ICC_AFF1_OFFSET		16
#define ICC_AFF1_MASK		GIC_8BIT_MASK
#define ICC_AFF1(value)		ICC_SET_FV(AFF1, value)
#define ICC_INTID_OFFSET	24
#define ICC_INTID_MASK		GIC_4BIT_MASK
#define ICC_INTID(value)	ICC_SET_FV(INTID, value)
#define ICC_AFF2_OFFSET		32
#define ICC_AFF2_MASK		GIC_8BIT_MASK
#define ICC_AFF2(value)		ICC_SET_FV(AFF2, value)
#define ICC_IRM			_BV(40)
#define ICC_RS_OFFSET		44
#define ICC_RS_MASK		GIC_4BIT_MASK
#define ICC_RS(value)		ICC_SET_FV(RS, value)
#define ICC_AFF3_OFFSET		48
#define ICC_AFF3_MASK		GIC_8BIT_MASK
#define ICC_AFF3(value)		ICC_SET_FV(AFF3, value)

/* ICC_CTLR_EL1/3 */
#define ICC_RSS			_BV(18)
#define ICC_A3V			_BV(15)
#define ICC_SEIS		_BV(14)
#define ICC_ID_BITS_OFFSET	11
#define ICC_ID_BITS_MASK	GIC_3BIT_MASK
#define ICC_ID_BITS(value)	ICC_SET_FV(ID_BITS, value)
#define ICC_PRI_BITS_OFFSET	8
#define ICC_PRI_BITS_MASK	GIC_3BIT_MASK
#define ICC_PRI_BITS(value)	ICC_SET_FV(PRI_BITS, value)
#define ICC_PMHE		_BV(6)

/* ICC_CTLR_EL1 */
#define ICC_EOI_MODE		_BV(1)
#define ICC_CBPR		_BV(0)

/* ICC_CTLR_EL3 */
#define ICC_NDS			_BV(17)
#define ICC_RM			_BV(5)
#define ICC_EOI_MODE_EL1NS	_BV(4)
#define ICC_EOI_MODE_EL1S	_BV(3)
#define ICC_EOI_MODE_EL3	_BV(2)
#define ICC_CBPR_EL1NS		_BV(1)
#define ICC_CBPR_EL1S		_BV(0)

/* ICC_SRE_EL1/2/3 */
#define ICC_SRE			_BV(0)
#define ICC_DFB			_BV(1)
#define ICC_DIB			_BV(2)
#define ICC_SRE_ENABLE		_BV(4)

/* ICC_IGRPEN0/1_EL1 */
#define ICC_ENABLE_GRP		_BV(0)

/* ICC_IGRPEN1_EL3 */
#define ICC_ENABLE_GRP1_NS	ICC_ENABLE_GRP
#define ICC_ENABLE_GRP1_S	_BV(1)

#define gicd_wait_rwp()						\
	while (__raw_readl(GICD_CTLR) & GICD_RWP)
#define gicd_clear_ctlr(value)					\
	do {							\
		__raw_clearl(value, GICD_CTLR);			\
		/* RWP tracks GICD_CTLR[2:0, 7:4] */		\
		gicd_wait_rwp();				\
	} while (0)
#define gicd_set_ctlr(value)					\
	do {							\
		__raw_setl(value, GICD_CTLR);			\
		/* RWP tracks GICD_CTLR[2:0, 7:4] */		\
		gicd_wait_rwp();				\
	} while (0)
#define gicd_set_spi(irq, secure)				\
	do {							\
		if (secure)					\
			__raw_writel(GICD_SPI(irq),		\
				     GICD_SETSPI_SR);		\
		else						\
			__raw_writel(GICD_SPI(irq),		\
				     GICD_SETSPI_NSR);		\
	} while (0)
#define gicd_clear_spi(irq, secure)				\
	do {							\
		if (secure)					\
			__raw_writel(GICD_SPI(irq),		\
				     GICD_CLRSPI_SR);		\
		else						\
			__raw_writel(GICD_SPI(irq),		\
				     GICD_CLRSPI_NSR);		\
	} while (0)

/* UWP tracks communication (SGI gen) to GICD */
#define gicr_wait_uwp(cpu)					\
	while (__raw_readl(GICR_CTLR(cpu)) & GICR_UWP)
#define gicr_wait_rwp(cpu)					\
	while (__raw_readl(GICR_CTLR(cpu)) & GICR_RWP)
#define gicr_clear_ctlr(cpu, value)				\
	do {							\
		__raw_clearl(value, GICR_CTLR(cpu));		\
		/* RWP tracks GICR_CTLR[26:24, 0] clears */	\
		gicr_wait_rwp(cpu);				\
	} while (0)
#define gicr_set_ctlr(cpu, value)				\
	do {							\
		__raw_setl(value, GICR_CTLR(cpu));		\
		/* RWP tracks GICR_CTLR[26:24] sets */		\
		gicr_wait_rwp(cpu);				\
	} while (0)
#define gicr_configure_group(cpu, irq, group)				\
	do {								\
		if (group == GIC_GROUP1NS)				\
			__raw_setl(GIC_GROUP1(irq),			\
				   GICR_IGROUPR0(cpu));			\
		else {							\
			__raw_clearl(GIC_GROUP1(irq),			\
				     GICR_IGROUPR0(cpu));		\
			if (group == GIC_GROUP1S)			\
				__raw_setl(GIC_GROUP1(irq),		\
					   GICR_IGRPMODR0(cpu));	\
			else						\
				__raw_clearl(GIC_GROUP1(irq),		\
					     GICR_IGRPMODR0(cpu));	\
		}							\
	} while (0)
#define gicr_enable_lpis(cpu)					\
	__raw_setl(GICR_ENABLE_LPIS, GICR_CTLR(cpu))
#define gicr_is_asleep(cpu)					\
	(!!(__raw_readl(GICR_WAKER(cpu)) & GICR_CHILDREN_ASLEEP))
#define gicr_enable_irq(cpu, irq)				\
	__raw_setl(GIC_INTERRUPT_ID(irq), GICR_ISENABLER0(cpu))
#define gicr_disable_irq(cpu, irq)				\
	do {							\
		__raw_setl(GIC_INTERRUPT_ID(irq),		\
			   GICR_ICENABLER0(cpu));		\
		/* RWP tracks GICR_ICENABLER0 */		\
		gicr_wait_rwp(cpu);				\
	} while (0)
#define gicr_enable_all_irqs(cpu)				\
	__raw_writel(~0, GICR_ISENABLER0(cpu))
#define gicr_disable_all_irqs(cpu)				\
	do {							\
		__raw_writel(~0, GICR_ICENABLER0(cpu));		\
		/* RWP tracks GICR_ICENABLER0 */		\
		gicr_wait_rwp(cpu);				\
	} while (0)
#define gicr_mark_awake(cpu)					\
	do {							\
		if (gicr_is_asleep(cpu)) {			\
			__raw_clearl(GICR_PROCESSOR_SLEEP,	\
				     GICR_WAKER(cpu));		\
			while (gicr_is_asleep(cpu));		\
		}						\
	} while (0)
#define gicr_mark_asleep(cpu)					\
	do {							\
		__raw_setl(GICR_PROCESSOR_SLEEP,		\
			   GICR_WAKER(cpu));			\
		while (!gicr_is_asleep(cpu));			\
	} while (0)
#define gicr_configure_irq(cpu, irq, priority, trigger)			\
	do {								\
		uint32_t cfg;						\
		__raw_writel_mask(GIC_PRIORITY(irq, priority),		\
				  GIC_PRIORITY(irq,			\
					       GIC_PRIORITY_MASK),	\
				  GICR_IPRIORITYR(cpu, irq));		\
		cfg = GIC_TRIGGER(GIC_TRIGGER_EDGE);			\
		/* Only PPI can be configured as level triggered */	\
		if (trigger == IRQ_LEVEL_TRIGGERED &&			\
		    irq >= IRQ_PPI_BASE)				\
			cfg = GIC_TRIGGER(GIC_TRIGGER_LEVEL);		\
		__raw_writel_mask(GIC_INT_CONFIG(irq, cfg),		\
				  GIC_INT_CONFIG(irq,			\
						 GIC_INT_CONFIG_MASK),	\
				  GICR_ICFGR(cpu, irq));		\
	} while (0)

#ifndef smp_processor_id
#define smp_processor_id()	0
#endif
#ifndef isb
#define isb()
#endif

#define irqc_hw_enable_irq(irq)						\
	do {								\
		if (irq < IRQ_SPI_BASE)					\
			gicr_enable_irq(smp_processor_id(), irq);	\
		else							\
			gicd_enable_irq(irq);				\
	} while (0)
#define irqc_hw_disable_irq(irq)					\
	do {								\
		if (irq < IRQ_SPI_BASE)					\
			gicr_disable_irq(smp_processor_id(), irq);	\
		else							\
			gicd_disable_irq(irq);				\
	} while (0)
#define irqc_hw_configure_irq(irq, priority, trigger)			\
	do {								\
		uint8_t cpu = smp_processor_id();			\
		gicr_configure_group(cpu, irq, GIC_GROUP1S);		\
		if (irq < IRQ_SPI_BASE)					\
			gicr_configure_irq(cpu, irq, priority, trigger);\
		else							\
			gicd_configure_irq(irq, priority, trigger);	\
	} while (0)

#define gic_begin_irq(irq, cpu)					\
	do {							\
		uint32_t iar = read_sysreg(ICC_IAR1_EL1);	\
		irq = ICC_GET_IRQ(iar);				\
		cpu = smp_processor_id();			\
	} while (0)
#define gic_end_irq(irq, cpu)					\
	write_sysreg(ICC_SET_IRQ(irq), ICC_EOIR1_EL1)

void gicv3_ack_irq(irq_t irq);
void gicv3_handle_irq(void);

void gicc_enable_priority_mask(uint8_t priority);
void gicc_disable_priority_mask(void);

void gicv3_init_gicd(void);
void gicv3_init_gits(void);
void gicv3_init_gicr(uint8_t cpu);
void gicv3_init_gicc(void);

#endif /* __GICV3_ARM64_H_INCLUDE__ */
