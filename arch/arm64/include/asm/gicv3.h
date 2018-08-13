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

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/gic.h>

/* ARM Generic Interrupt Controller Architecture Specification v3 and v4 */

#include <asm/sysreg_gicv3.h>

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
#define GICD_INTID_OFFSET	0
#define GICD_INTID_MASK		0x3FF
#define GICD_INTID(value)	GICD_SET_FV(INTID, value)

/* GICD_IROUTER */
#define GICD_AFF0_OFFSET	0
#define GICD_AFF0_MASK		0xFF
#define GICD_AFF0(value)	GICD_SET_FV(AFF0, value)
#define GICD_AFF1_OFFSET	8
#define GICD_AFF1_MASK		0xFF
#define GICD_AFF1(value)	GICD_SET_FV(AFF1, value)
#define GICD_AFF2_OFFSET	16
#define GICD_AFF2_MASK		0xFF
#define GICD_AFF2(value)	GICD_SET_FV(AFF2, value)
#define GICD_AFF3_OFFSET	32
#define GICD_AFF3_MASK		0xFF
#define GICD_AFF3(value)	GICD_SET_FV(AFF3, value)
#define GICD_IRQ_ROUTING_MODE_OFFSET	31
#define GICD_IRQ_ROUTING_MODE_MASK	0x01
#define GICD_IRQ_ROUTING_MODE(value)	GICD_SET_FV(IRQ_ROUTING_MODE, value)
#define GICD_IRQ_ROUTING_DIRECT		0x00
#define GICD_IRQ_ROUTING_1_OF_N		0x01

/* GICD_IPRIORITYR */
#define GIC_PRIORITY_S_HIGH		0
#define GIC_PRIORITY_NS_HIGH(max_prio)	(((uint16_t)(max_prio) + 1) / 2)
#define GIC_PRIORITY_S_LOW(max_prio)	(GIC_PRIORITY_NS_HIGH(max_prio) - 1)
#define GIC_PRIORITY_NS_LOW(max_prio)	((max_prio) - 1)

/* Redistributor register map */
#define GICR_REG(cpu, offset)				\
	(GICR_BASE+((cpu)<<GICR_CPU_SHIFT)+(offset))
#define GICR_BASE_REG(cpu, base, offset)		\
	(GICR_BASE+ ((cpu)<<GICR_CPU_SHIFT)+		\
	 (base)+(offset))
#define GICR_2BIT_BASE_REG(cpu, base, offset, n)	\
	(GIC_2BIT_REG(GICR_BASE+			\
		      ((cpu)<<GICR_CPU_SHIFT)+		\
		      (base)+(offset), n))
#define GICR_8BIT_BASE_REG(cpu, base, offset, n)	\
	(GIC_8BIT_REG(GICR_BASE+			\
		      ((cpu)<<GICR_CPU_SHIFT)+		\
		      (base)+(offset), n))
#define GICR_SET_FV(name, value)	GIC_SET_FV(GICR_##name, value)
#define GICR_GET_FV(name, value)	GIC_GET_FV(GICR_##name, value)
#define GICRn_SET_FV(n, name, value)	GICn_SET_FV(n, GICR_##name, value)
#define GICRn_GET_FV(n, name, value)	GICn_GET_FV(n, GICR_##name, value)

#define GICR_SPI_BASE			(1 << 0x10)
#define GICR_VLPI_BASE			(2 << 0x10)

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

#define gic_begin_irq(irq, cpu)
#define gic_end_irq(irq, cpu)

void gicv3_init_gicd(void);
void gicv3_init_gits(void);
void gicv3_init_gicr(uint8_t cpu);
void gicv3_init_gicc(uint8_t cpu);

uint32_t gicv3_hw_init_spis(void);
uint32_t gicv3_hw_init_sgis(uint8_t cpu);	/* PPIs and SGIs */

#endif /* __GICV3_ARM64_H_INCLUDE__ */
