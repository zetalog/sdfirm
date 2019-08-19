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
 * @(#)scg.h: RV32M1 (VEGA) SCG (system clock generator) driver
 * $Id: scg.h,v 1.1 2019-08-18 12:45:00 zhenglv Exp $
 */

#ifndef __SCG_VEGA_H_INCLUDE__
#define __SCG_VEGA_H_INCLUDE__

#define SCG_BASE		UL(0x4002C000)
#define SCG_REG(offset)		(SCG_BASE + (offset))

#define SCG_VERID		SCG_REG(0x000)
#define SCG_PARAM		SCG_REG(0x004)
#define SCG_CSR			SCG_REG(0x010)
#define SCG_RCCR		SCG_REG(0x014)
#define SCG_VCCR		SCG_REG(0x018)
#define SCG_HCCR		SCG_REG(0x01C)
#define SCG_CLKOUTCNFG		SCG_REG(0x020)
#define SCG_SOSCCSR		SCG_REG(0x100)
#define SCG_SOSCDIV		SCG_REG(0x104)
#define SCG_SIRCCSR		SCG_REG(0x200)
#define SCG_SIRCDIV		SCG_REG(0x204)
#define SCG_SIRCCFG		SCR_REG(0x208)
#define SCG_FIRCCSR		SCG_REG(0x300)
#define SCG_FIRCDIV		SCG_REG(0x304)
#define SCG_FIRCCFG		SCG_REG(0x308)
#define SCG_FIRCTCFG		SCG_REG(0x30C)
#define SCG_FIRCSTAT		SCG_REG(0x318)
#define SCG_ROSCCSR		SCG_REG(0x400)
#define SCG_LPFLLCSR		SCG_REG(0x500)
#define SCG_LPFLLDIV		SCG_REG(0x504)
#define SCG_LPFLLCFG		SCG_REG(0x508)
#define SCG_LPFLLTCFG		SCG_REG(0x50C)
#define SCG_LPFLLSTAT		SCG_REG(0x514)

/* VERID - Version ID Register */
#define SCG_VERID_VERSION_MASK		(0xFFFFFFFFU)
#define SCG_VERID_VERSION_SHIFT		(0U)
#define SCG_VERID_VERSION(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_VERID_VERSION_SHIFT)) & SCG_VERID_VERSION_MASK)

/* PARAM - Parameter Register */
#define SCG_PARAM_CLKPRES_MASK		(0xFFU)
#define SCG_PARAM_CLKPRES_SHIFT		(0U)
/* CLKPRES - Clock Present
 *  0b00000000-0b00000001..Reserved.
 *  0bxxxxxx1x..System OSC (SOSC) is present.
 *  0bxxxxx1xx..Slow IRC (SIRC) is present.
 *  0bxxxx1xxx..Fast IRC (FIRC) is present.
 *  0bxxx1xxxx..RTC OSC (ROSC) is present.
 *  0bxx1xxxxx..Low Power FLL (LPFLL) is present.
 */
#define SCG_PARAM_CLKPRES(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_PARAM_CLKPRES_SHIFT)) & SCG_PARAM_CLKPRES_MASK)
#define SCG_PARAM_DIVPRES_MASK		(0xF8000000U)
#define SCG_PARAM_DIVPRES_SHIFT		(27U)
/* DIVPRES - Divider Present
 *  0bxxxx1..System DIVSLOW is present.
 *  0bxxx1x..System DIVBUS is present.
 *  0bxx1xx..System DIVEXT is present.
 *  0b1xxxx..System DIVCORE is present.
 */
#define SCG_PARAM_DIVPRES(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_PARAM_DIVPRES_SHIFT)) & SCG_PARAM_DIVPRES_MASK)

/* CSR - Clock Status Register */
#define SCG_CSR_DIVSLOW_MASK		(0xFU)
#define SCG_CSR_DIVSLOW_SHIFT		(0U)
/* DIVSLOW - Slow Clock Divide Ratio
 *  0b0000..Reserved
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_CSR_DIVSLOW(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_CSR_DIVSLOW_SHIFT)) & SCG_CSR_DIVSLOW_MASK)
#define SCG_CSR_DIVBUS_MASK		(0xF0U)
#define SCG_CSR_DIVBUS_SHIFT		(4U)
/* DIVBUS - Bus Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_CSR_DIVBUS(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_CSR_DIVBUS_SHIFT)) & SCG_CSR_DIVBUS_MASK)
#define SCG_CSR_DIVEXT_MASK		(0xF00U)
#define SCG_CSR_DIVEXT_SHIFT		(8U)
/* DIVEXT - External Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_CSR_DIVEXT(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_CSR_DIVEXT_SHIFT)) & SCG_CSR_DIVEXT_MASK)
#define SCG_CSR_DIVCORE_MASK		(0xF0000U)
#define SCG_CSR_DIVCORE_SHIFT		(16U)
/* DIVCORE - Core Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_CSR_DIVCORE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_CSR_DIVCORE_SHIFT)) & SCG_CSR_DIVCORE_MASK)
#define SCG_CSR_SCS_MASK		(0xF000000U)
#define SCG_CSR_SCS_SHIFT		(24U)
/* SCS - System Clock Source
 *  0b0000..Reserved
 *  0b0001..System OSC (SOSC_CLK)
 *  0b0010..Slow IRC (SIRC_CLK)
 *  0b0011..Fast IRC (FIRC_CLK)
 *  0b0100..RTC OSC (ROSC_CLK)
 *  0b0101..Low Power FLL (LPFLL_CLK)
 *  0b0110..Reserved
 *  0b0111..Reserved
 */
#define SCG_CSR_SCS(x)			\
	(((uint32_t)(((uint32_t)(x)) << SCG_CSR_SCS_SHIFT)) & SCG_CSR_SCS_MASK)

/* RCCR - Run Clock Control Register */
#define SCG_RCCR_DIVSLOW_MASK		(0xFU)
#define SCG_RCCR_DIVSLOW_SHIFT		(0U)
/* DIVSLOW - Slow Clock Divide Ratio
 *  0b0000..Reserved
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_RCCR_DIVSLOW(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_RCCR_DIVSLOW_SHIFT)) & SCG_RCCR_DIVSLOW_MASK)
#define SCG_RCCR_DIVBUS_MASK		(0xF0U)
#define SCG_RCCR_DIVBUS_SHIFT		(4U)
/* DIVBUS - Bus Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_RCCR_DIVBUS(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_RCCR_DIVBUS_SHIFT)) & SCG_RCCR_DIVBUS_MASK)
#define SCG_RCCR_DIVEXT_MASK		(0xF00U)
#define SCG_RCCR_DIVEXT_SHIFT		(8U)
/* DIVEXT - External Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_RCCR_DIVEXT(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_RCCR_DIVEXT_SHIFT)) & SCG_RCCR_DIVEXT_MASK)
#define SCG_RCCR_DIVCORE_MASK		(0xF0000U)
#define SCG_RCCR_DIVCORE_SHIFT		(16U)
/* DIVCORE - Core Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_RCCR_DIVCORE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_RCCR_DIVCORE_SHIFT)) & SCG_RCCR_DIVCORE_MASK)
#define SCG_RCCR_SCS_MASK		(0x7000000U)
#define SCG_RCCR_SCS_SHIFT		(24U)
/* SCS - System Clock Source
 *  0b000..Reserved
 *  0b001..System OSC (SOSC_CLK)
 *  0b010..Slow IRC (SIRC_CLK)
 *  0b011..Fast IRC (FIRC_CLK)
 *  0b100..RTC OSC (ROSC_CLK)
 *  0b101..Low Power FLL (LPFLL_CLK)
 *  0b110..Reserved
 *  0b111..Reserved
 */
#define SCG_RCCR_SCS(x)			\
	(((uint32_t)(((uint32_t)(x)) << SCG_RCCR_SCS_SHIFT)) & SCG_RCCR_SCS_MASK)

/* VCCR - VLPR Clock Control Register */
#define SCG_VCCR_DIVSLOW_MASK		(0xFU)
#define SCG_VCCR_DIVSLOW_SHIFT		(0U)
/* DIVSLOW - Slow Clock Divide Ratio
 *  0b0000..Reserved
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_VCCR_DIVSLOW(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_VCCR_DIVSLOW_SHIFT)) & SCG_VCCR_DIVSLOW_MASK)
#define SCG_VCCR_DIVBUS_MASK		(0xF0U)
#define SCG_VCCR_DIVBUS_SHIFT		(4U)
/* DIVBUS - Bus Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_VCCR_DIVBUS(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_VCCR_DIVBUS_SHIFT)) & SCG_VCCR_DIVBUS_MASK)
#define SCG_VCCR_DIVEXT_MASK		(0xF00U)
#define SCG_VCCR_DIVEXT_SHIFT		(8U)
/* DIVEXT - External Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_VCCR_DIVEXT(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_VCCR_DIVEXT_SHIFT)) & SCG_VCCR_DIVEXT_MASK)
#define SCG_VCCR_DIVCORE_MASK		(0xF0000U)
#define SCG_VCCR_DIVCORE_SHIFT		(16U)
/* DIVCORE - Core Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_VCCR_DIVCORE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_VCCR_DIVCORE_SHIFT)) & SCG_VCCR_DIVCORE_MASK)
#define SCG_VCCR_SCS_MASK		(0xF000000U)
#define SCG_VCCR_SCS_SHIFT		(24U)
/* SCS - System Clock Source
 *  0b0000..Reserved
 *  0b0001..System OSC (SOSC_CLK)
 *  0b0010..Slow IRC (SIRC_CLK)
 *  0b0011..Reserved
 *  0b0100..RTC OSC (ROSC_CLK)
 *  0b0101..Reserved
 *  0b0110..Reserved
 *  0b0111..Reserved
 */
#define SCG_VCCR_SCS(x)			\
	(((uint32_t)(((uint32_t)(x)) << SCG_VCCR_SCS_SHIFT)) & SCG_VCCR_SCS_MASK)

/* HCCR - HSRUN Clock Control Register */
#define SCG_HCCR_DIVSLOW_MASK		(0xFU)
#define SCG_HCCR_DIVSLOW_SHIFT		(0U)
/* DIVSLOW - Slow Clock Divide Ratio
 *  0b0000..Reserved
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_HCCR_DIVSLOW(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_HCCR_DIVSLOW_SHIFT)) & SCG_HCCR_DIVSLOW_MASK)
#define SCG_HCCR_DIVBUS_MASK		(0xF0U)
#define SCG_HCCR_DIVBUS_SHIFT		(4U)
/* DIVBUS - Bus Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_HCCR_DIVBUS(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_HCCR_DIVBUS_SHIFT)) & SCG_HCCR_DIVBUS_MASK)
#define SCG_HCCR_DIVEXT_MASK		(0xF00U)
#define SCG_HCCR_DIVEXT_SHIFT		(8U)
/* DIVEXT - External Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_HCCR_DIVEXT(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_HCCR_DIVEXT_SHIFT)) & SCG_HCCR_DIVEXT_MASK)
#define SCG_HCCR_DIVCORE_MASK		(0xF0000U)
#define SCG_HCCR_DIVCORE_SHIFT		(16U)
/* DIVCORE - Core Clock Divide Ratio
 *  0b0000..Divide-by-1
 *  0b0001..Divide-by-2
 *  0b0010..Divide-by-3
 *  0b0011..Divide-by-4
 *  0b0100..Divide-by-5
 *  0b0101..Divide-by-6
 *  0b0110..Divide-by-7
 *  0b0111..Divide-by-8
 *  0b1000..Divide-by-9
 *  0b1001..Divide-by-10
 *  0b1010..Divide-by-11
 *  0b1011..Divide-by-12
 *  0b1100..Divide-by-13
 *  0b1101..Divide-by-14
 *  0b1110..Divide-by-15
 *  0b1111..Divide-by-16
 */
#define SCG_HCCR_DIVCORE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_HCCR_DIVCORE_SHIFT)) & SCG_HCCR_DIVCORE_MASK)
#define SCG_HCCR_SCS_MASK		(0xF000000U)
#define SCG_HCCR_SCS_SHIFT		(24U)
/* SCS - System Clock Source
 *  0b0000..Reserved
 *  0b0001..System OSC (SOSC_CLK)
 *  0b0010..Slow IRC (SIRC_CLK)
 *  0b0011..Fast IRC (FIRC_CLK)
 *  0b0100..RTC OSC (ROSC_CLK)
 *  0b0101..Low Power FLL (LPFLL_CLK)
 *  0b0110..Reserved
 *  0b0111..Reserved
 */
#define SCG_HCCR_SCS(x)			\
	(((uint32_t)(((uint32_t)(x)) << SCG_HCCR_SCS_SHIFT)) & SCG_HCCR_SCS_MASK)

/* CLKOUTCNFG - SCG CLKOUT Configuration Register */
#define SCG_CLKOUTCNFG_CLKOUTSEL_MASK	(0xF000000U)
#define SCG_CLKOUTCNFG_CLKOUTSEL_SHIFT	(24U)
/* CLKOUTSEL - SCG Clkout Select
 *  0b0000..SCG EXTERNAL Clock
 *  0b0001..System OSC (SOSC_CLK)
 *  0b0010..Slow IRC (SIRC_CLK)
 *  0b0011..Fast IRC (FIRC_CLK)
 *  0b0100..RTC OSC (ROSC_CLK)
 *  0b0101..Low Power FLL (LPFLL_CLK)
 *  0b0110..Reserved
 *  0b0111..Reserved
 *  0b1111..Reserved
 */
#define SCG_CLKOUTCNFG_CLKOUTSEL(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_CLKOUTCNFG_CLKOUTSEL_SHIFT)) & SCG_CLKOUTCNFG_CLKOUTSEL_MASK)

/* SOSCCSR - System OSC Control Status Register */
#define SCG_SOSCCSR_SOSCEN_MASK		(0x1U)
#define SCG_SOSCCSR_SOSCEN_SHIFT	(0U)
/* SOSCEN - System OSC Enable
 *  0b0..System OSC is disabled
 *  0b1..System OSC is enabled
 */
#define SCG_SOSCCSR_SOSCEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCEN_SHIFT)) & SCG_SOSCCSR_SOSCEN_MASK)
#define SCG_SOSCCSR_SOSCSTEN_MASK	(0x2U)
#define SCG_SOSCCSR_SOSCSTEN_SHIFT	(1U)
/* SOSCSTEN - System OSC Stop Enable
 *  0b0..System OSC is disabled in Stop modes
 *  0b1..System OSC is enabled in Stop modes if SOSCEN=1. In VLLS0, system oscillator is disabled even if SOSCSTEN=1 and SOSCEN=1.
 */
#define SCG_SOSCCSR_SOSCSTEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCSTEN_SHIFT)) & SCG_SOSCCSR_SOSCSTEN_MASK)
#define SCG_SOSCCSR_SOSCLPEN_MASK	(0x4U)
#define SCG_SOSCCSR_SOSCLPEN_SHIFT	(2U)
/* SOSCLPEN - System OSC Low Power Enable
 *  0b0..System OSC is disabled in VLP modes
 *  0b1..System OSC is enabled in VLP modes
 */
#define SCG_SOSCCSR_SOSCLPEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCLPEN_SHIFT)) & SCG_SOSCCSR_SOSCLPEN_MASK)
#define SCG_SOSCCSR_SOSCCM_MASK		(0x10000U)
#define SCG_SOSCCSR_SOSCCM_SHIFT	(16U)
/* SOSCCM - System OSC Clock Monitor
 *  0b0..System OSC Clock Monitor is disabled
 *  0b1..System OSC Clock Monitor is enabled
 */
#define SCG_SOSCCSR_SOSCCM(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCCM_SHIFT)) & SCG_SOSCCSR_SOSCCM_MASK)
#define SCG_SOSCCSR_SOSCCMRE_MASK	(0x20000U)
#define SCG_SOSCCSR_SOSCCMRE_SHIFT	(17U)
/* SOSCCMRE - System OSC Clock Monitor Reset Enable
 *  0b0..Clock Monitor generates interrupt when error detected
 *  0b1..Clock Monitor generates reset when error detected
 */
#define SCG_SOSCCSR_SOSCCMRE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCCMRE_SHIFT)) & SCG_SOSCCSR_SOSCCMRE_MASK)
#define SCG_SOSCCSR_LK_MASK		(0x800000U)
#define SCG_SOSCCSR_LK_SHIFT		(23U)
/* LK - Lock Register
 *  0b0..This Control Status Register can be written.
 *  0b1..This Control Status Register cannot be written.
 */
#define SCG_SOSCCSR_LK(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_LK_SHIFT)) & SCG_SOSCCSR_LK_MASK)
#define SCG_SOSCCSR_SOSCVLD_MASK	(0x1000000U)
#define SCG_SOSCCSR_SOSCVLD_SHIFT	(24U)
/* SOSCVLD - System OSC Valid
 *  0b0..System OSC is not enabled or clock is not valid
 *  0b1..System OSC is enabled and output clock is valid
 */
#define SCG_SOSCCSR_SOSCVLD(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCVLD_SHIFT)) & SCG_SOSCCSR_SOSCVLD_MASK)
#define SCG_SOSCCSR_SOSCSEL_MASK	(0x2000000U)
#define SCG_SOSCCSR_SOSCSEL_SHIFT	(25U)
/* SOSCSEL - System OSC Selected
 *  0b0..System OSC is not the system clock source
 *  0b1..System OSC is the system clock source
 */
#define SCG_SOSCCSR_SOSCSEL(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCSEL_SHIFT)) & SCG_SOSCCSR_SOSCSEL_MASK)
#define SCG_SOSCCSR_SOSCERR_MASK	(0x4000000U)
#define SCG_SOSCCSR_SOSCERR_SHIFT	(26U)
/* SOSCERR - System OSC Clock Error
 *  0b0..System OSC Clock Monitor is disabled or has not detected an error
 *  0b1..System OSC Clock Monitor is enabled and detected an error
 */
#define SCG_SOSCCSR_SOSCERR(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCERR_SHIFT)) & SCG_SOSCCSR_SOSCERR_MASK)

/* SOSCDIV - System OSC Divide Register */
#define SCG_SOSCDIV_SOSCDIV1_MASK	(0x7U)
#define SCG_SOSCDIV_SOSCDIV1_SHIFT	(0U)
/* SOSCDIV1 - System OSC Clock Divide 1
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_SOSCDIV_SOSCDIV1(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCDIV_SOSCDIV1_SHIFT)) & SCG_SOSCDIV_SOSCDIV1_MASK)
#define SCG_SOSCDIV_SOSCDIV2_MASK	(0x700U)
#define SCG_SOSCDIV_SOSCDIV2_SHIFT	(8U)
/* SOSCDIV2 - System OSC Clock Divide 2
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_SOSCDIV_SOSCDIV2(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCDIV_SOSCDIV2_SHIFT)) & SCG_SOSCDIV_SOSCDIV2_MASK)
#define SCG_SOSCDIV_SOSCDIV3_MASK	(0x70000U)
#define SCG_SOSCDIV_SOSCDIV3_SHIFT	(16U)
/* SOSCDIV3 - System OSC Clock Divide 3
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_SOSCDIV_SOSCDIV3(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SOSCDIV_SOSCDIV3_SHIFT)) & SCG_SOSCDIV_SOSCDIV3_MASK)

/* SIRCCSR - Slow IRC Control Status Register */
#define SCG_SIRCCSR_SIRCEN_MASK		(0x1U)
#define SCG_SIRCCSR_SIRCEN_SHIFT	(0U)
/* SIRCEN - Slow IRC Enable
 *  0b0..Slow IRC is disabled
 *  0b1..Slow IRC is enabled
 */
#define SCG_SIRCCSR_SIRCEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_SIRCEN_SHIFT)) & SCG_SIRCCSR_SIRCEN_MASK)
#define SCG_SIRCCSR_SIRCSTEN_MASK	(0x2U)
#define SCG_SIRCCSR_SIRCSTEN_SHIFT	(1U)
/* SIRCSTEN - Slow IRC Stop Enable
 *  0b0..Slow IRC is disabled in Stop modes
 *  0b1..Slow IRC is enabled in Stop modes
 */
#define SCG_SIRCCSR_SIRCSTEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_SIRCSTEN_SHIFT)) & SCG_SIRCCSR_SIRCSTEN_MASK)
#define SCG_SIRCCSR_SIRCLPEN_MASK	(0x4U)
#define SCG_SIRCCSR_SIRCLPEN_SHIFT	(2U)
/* SIRCLPEN - Slow IRC Low Power Enable
 *  0b0..Slow IRC is disabled in VLP modes
 *  0b1..Slow IRC is enabled in VLP modes
 */
#define SCG_SIRCCSR_SIRCLPEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_SIRCLPEN_SHIFT)) & SCG_SIRCCSR_SIRCLPEN_MASK)
#define SCG_SIRCCSR_LK_MASK		(0x800000U)
#define SCG_SIRCCSR_LK_SHIFT		(23U)
/* LK - Lock Register
 *  0b0..Control Status Register can be written.
 *  0b1..Control Status Register cannot be written.
 */
#define SCG_SIRCCSR_LK(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_LK_SHIFT)) & SCG_SIRCCSR_LK_MASK)
#define SCG_SIRCCSR_SIRCVLD_MASK	(0x1000000U)
#define SCG_SIRCCSR_SIRCVLD_SHIFT	(24U)
/* SIRCVLD - Slow IRC Valid
 *  0b0..Slow IRC is not enabled or clock is not valid
 *  0b1..Slow IRC is enabled and output clock is valid
 */
#define SCG_SIRCCSR_SIRCVLD(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_SIRCVLD_SHIFT)) & SCG_SIRCCSR_SIRCVLD_MASK)
#define SCG_SIRCCSR_SIRCSEL_MASK	(0x2000000U)
#define SCG_SIRCCSR_SIRCSEL_SHIFT	(25U)
/* SIRCSEL - Slow IRC Selected
 *  0b0..Slow IRC is not the system clock source
 *  0b1..Slow IRC is the system clock source
 */
#define SCG_SIRCCSR_SIRCSEL(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_SIRCSEL_SHIFT)) & SCG_SIRCCSR_SIRCSEL_MASK)

/* SIRCDIV - Slow IRC Divide Register */
#define SCG_SIRCDIV_SIRCDIV1_MASK	(0x7U)
#define SCG_SIRCDIV_SIRCDIV1_SHIFT	(0U)
/*! SIRCDIV1 - Slow IRC Clock Divide 1
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_SIRCDIV_SIRCDIV1(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCDIV_SIRCDIV1_SHIFT)) & SCG_SIRCDIV_SIRCDIV1_MASK)
#define SCG_SIRCDIV_SIRCDIV2_MASK	(0x700U)
#define SCG_SIRCDIV_SIRCDIV2_SHIFT	(8U)
/*! SIRCDIV2 - Slow IRC Clock Divide 2
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_SIRCDIV_SIRCDIV2(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCDIV_SIRCDIV2_SHIFT)) & SCG_SIRCDIV_SIRCDIV2_MASK)
#define SCG_SIRCDIV_SIRCDIV3_MASK	(0x70000U)
#define SCG_SIRCDIV_SIRCDIV3_SHIFT	(16U)
/*! SIRCDIV3 - Slow IRC Clock Divider 3
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_SIRCDIV_SIRCDIV3(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCDIV_SIRCDIV3_SHIFT)) & SCG_SIRCDIV_SIRCDIV3_MASK)

/* SIRCCFG - Slow IRC Configuration Register */
#define SCG_SIRCCFG_RANGE_MASK		(0x1U)
#define SCG_SIRCCFG_RANGE_SHIFT		(0U)
/* RANGE - Frequency Range
 *  0b0..Slow IRC low range clock (2MHz)
 *  0b1..Slow IRC high range clock (8 MHz)
 */
#define SCG_SIRCCFG_RANGE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_SIRCCFG_RANGE_SHIFT)) & SCG_SIRCCFG_RANGE_MASK)

/* FIRCCSR - Fast IRC Control Status Register */
#define SCG_FIRCCSR_FIRCEN_MASK		(0x1U)
#define SCG_FIRCCSR_FIRCEN_SHIFT	(0U)
/* FIRCEN - Fast IRC Enable
 *  0b0..Fast IRC is disabled
 *  0b1..Fast IRC is enabled
 */
#define SCG_FIRCCSR_FIRCEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCEN_SHIFT)) & SCG_FIRCCSR_FIRCEN_MASK)
#define SCG_FIRCCSR_FIRCSTEN_MASK	(0x2U)
#define SCG_FIRCCSR_FIRCSTEN_SHIFT	(1U)
/* FIRCSTEN - Fast IRC Stop Enable
 *  0b0..Fast IRC is disabled in Stop modes.
 *  0b1..Fast IRC is enabled in Stop modes
 */
#define SCG_FIRCCSR_FIRCSTEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCSTEN_SHIFT)) & SCG_FIRCCSR_FIRCSTEN_MASK)
#define SCG_FIRCCSR_FIRCLPEN_MASK	(0x4U)
#define SCG_FIRCCSR_FIRCLPEN_SHIFT	(2U)
/* FIRCLPEN - Fast IRC Low Power Enable
 *  0b0..Fast IRC is disabled in VLP modes
 *  0b1..Fast IRC is enabled in VLP modes
 */
#define SCG_FIRCCSR_FIRCLPEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCLPEN_SHIFT)) & SCG_FIRCCSR_FIRCLPEN_MASK)
#define SCG_FIRCCSR_FIRCREGOFF_MASK	(0x8U)
#define SCG_FIRCCSR_FIRCREGOFF_SHIFT	(3U)
/* FIRCREGOFF - Fast IRC Regulator Enable
 *  0b0..Fast IRC Regulator is enabled.
 *  0b1..Fast IRC Regulator is disabled.
 */
#define SCG_FIRCCSR_FIRCREGOFF(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCREGOFF_SHIFT)) & SCG_FIRCCSR_FIRCREGOFF_MASK)
#define SCG_FIRCCSR_FIRCTREN_MASK	(0x100U)
#define SCG_FIRCCSR_FIRCTREN_SHIFT	(8U)
/* FIRCTREN - Fast IRC Trim Enable
 *  0b0..Disable trimming Fast IRC to an external clock source
 *  0b1..Enable trimming Fast IRC to an external clock source
 */
#define SCG_FIRCCSR_FIRCTREN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCTREN_SHIFT)) & SCG_FIRCCSR_FIRCTREN_MASK)
#define SCG_FIRCCSR_FIRCTRUP_MASK	(0x200U)
#define SCG_FIRCCSR_FIRCTRUP_SHIFT	(9U)
/* FIRCTRUP - Fast IRC Trim Update
 *  0b0..Disable Fast IRC trimming updates
 *  0b1..Enable Fast IRC trimming updates
 */
#define SCG_FIRCCSR_FIRCTRUP(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCTRUP_SHIFT)) & SCG_FIRCCSR_FIRCTRUP_MASK)
#define SCG_FIRCCSR_LK_MASK		(0x800000U)
#define SCG_FIRCCSR_LK_SHIFT		(23U)
/* LK - Lock Register
 *  0b0..Control Status Register can be written.
 *  0b1..Control Status Register cannot be written.
 */
#define SCG_FIRCCSR_LK(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_LK_SHIFT)) & SCG_FIRCCSR_LK_MASK)
#define SCG_FIRCCSR_FIRCVLD_MASK	(0x1000000U)
#define SCG_FIRCCSR_FIRCVLD_SHIFT	(24U)
/* FIRCVLD - Fast IRC Valid status
 *  0b0..Fast IRC is not enabled or clock is not valid.
 *  0b1..Fast IRC is enabled and output clock is valid. The clock is valid once there is an output clock from the FIRC analog.
 */
#define SCG_FIRCCSR_FIRCVLD(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCVLD_SHIFT)) & SCG_FIRCCSR_FIRCVLD_MASK)
#define SCG_FIRCCSR_FIRCSEL_MASK	(0x2000000U)
#define SCG_FIRCCSR_FIRCSEL_SHIFT	(25U)
/* FIRCSEL - Fast IRC Selected status
 *  0b0..Fast IRC is not the system clock source
 *  0b1..Fast IRC is the system clock source
 */
#define SCG_FIRCCSR_FIRCSEL(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCSEL_SHIFT)) & SCG_FIRCCSR_FIRCSEL_MASK)
#define SCG_FIRCCSR_FIRCERR_MASK	(0x4000000U)
#define SCG_FIRCCSR_FIRCERR_SHIFT	(26U)
/* FIRCERR - Fast IRC Clock Error
 *  0b0..Error not detected with the Fast IRC trimming.
 *  0b1..Error detected with the Fast IRC trimming.
 */
#define SCG_FIRCCSR_FIRCERR(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCERR_SHIFT)) & SCG_FIRCCSR_FIRCERR_MASK)

/* FIRCDIV - Fast IRC Divide Register */
#define SCG_FIRCDIV_FIRCDIV1_MASK	(0x7U)
#define SCG_FIRCDIV_FIRCDIV1_SHIFT	(0U)
/* FIRCDIV1 - Fast IRC Clock Divide 1
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_FIRCDIV_FIRCDIV1(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCDIV_FIRCDIV1_SHIFT)) & SCG_FIRCDIV_FIRCDIV1_MASK)
#define SCG_FIRCDIV_FIRCDIV2_MASK	(0x700U)
#define SCG_FIRCDIV_FIRCDIV2_SHIFT	(8U)
/*! FIRCDIV2 - Fast IRC Clock Divide 2
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_FIRCDIV_FIRCDIV2(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCDIV_FIRCDIV2_SHIFT)) & SCG_FIRCDIV_FIRCDIV2_MASK)
#define SCG_FIRCDIV_FIRCDIV3_MASK	(0x70000U)
#define SCG_FIRCDIV_FIRCDIV3_SHIFT	(16U)
/*! FIRCDIV3 - Fast IRC Clock Divider 3
 *  0b000..Clock disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_FIRCDIV_FIRCDIV3(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCDIV_FIRCDIV3_SHIFT)) & SCG_FIRCDIV_FIRCDIV3_MASK)

/* FIRCCFG - Fast IRC Configuration Register */
#define SCG_FIRCCFG_RANGE_MASK		(0x3U)
#define SCG_FIRCCFG_RANGE_SHIFT		(0U)
/* RANGE - Frequency Range
 *  0b00..Fast IRC is trimmed to 48 MHz
 *  0b01..Fast IRC is trimmed to 52 MHz
 *  0b10..Fast IRC is trimmed to 56 MHz
 *  0b11..Fast IRC is trimmed to 60 MHz
 */
#define SCG_FIRCCFG_RANGE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCCFG_RANGE_SHIFT)) & SCG_FIRCCFG_RANGE_MASK)

/* FIRCTCFG - Fast IRC Trim Configuration Register */
#define SCG_FIRCTCFG_TRIMSRC_MASK	(0x3U)
#define SCG_FIRCTCFG_TRIMSRC_SHIFT	(0U)
/* TRIMSRC - Trim Source
 *  0b00..Reserved
 *  0b01..Reserved
 *  0b10..System OSC. This option requires that SOSC be divided using the TRIMDIV field to get a frequency slower than 32kHz.
 *  0b11..RTC OSC (32.768 kHz)
 */
#define SCG_FIRCTCFG_TRIMSRC(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCTCFG_TRIMSRC_SHIFT)) & SCG_FIRCTCFG_TRIMSRC_MASK)
#define SCG_FIRCTCFG_TRIMDIV_MASK	(0x700U)
#define SCG_FIRCTCFG_TRIMDIV_SHIFT	(8U)
/* TRIMDIV - Fast IRC Trim Predivide
 *  0b000..Divide by 1
 *  0b001..Divide by 128
 *  0b010..Divide by 256
 *  0b011..Divide by 512
 *  0b100..Divide by 1024
 *  0b101..Divide by 2048
 *  0b110..Reserved. Writing this value will result in Divide by 1.
 *  0b111..Reserved. Writing this value will result in a Divide by 1.
 */
#define SCG_FIRCTCFG_TRIMDIV(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCTCFG_TRIMDIV_SHIFT)) & SCG_FIRCTCFG_TRIMDIV_MASK)

/* FIRCSTAT - Fast IRC Status Register */
#define SCG_FIRCSTAT_TRIMFINE_MASK	(0x7FU)
#define SCG_FIRCSTAT_TRIMFINE_SHIFT	(0U)
#define SCG_FIRCSTAT_TRIMFINE(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCSTAT_TRIMFINE_SHIFT)) & SCG_FIRCSTAT_TRIMFINE_MASK)
#define SCG_FIRCSTAT_TRIMCOAR_MASK	(0x3F00U)
#define SCG_FIRCSTAT_TRIMCOAR_SHIFT	(8U)
#define SCG_FIRCSTAT_TRIMCOAR(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_FIRCSTAT_TRIMCOAR_SHIFT)) & SCG_FIRCSTAT_TRIMCOAR_MASK)

/* ROSCCSR - RTC OSC Control Status Register */
#define SCG_ROSCCSR_ROSCCM_MASK		(0x10000U)
#define SCG_ROSCCSR_ROSCCM_SHIFT	(16U)
/* ROSCCM - RTC OSC Clock Monitor
 *  0b0..RTC OSC Clock Monitor is disabled
 *  0b1..RTC OSC Clock Monitor is enabled
 */
#define SCG_ROSCCSR_ROSCCM(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_ROSCCSR_ROSCCM_SHIFT)) & SCG_ROSCCSR_ROSCCM_MASK)
#define SCG_ROSCCSR_ROSCCMRE_MASK	(0x20000U)
#define SCG_ROSCCSR_ROSCCMRE_SHIFT	(17U)
/* ROSCCMRE - RTC OSC Clock Monitor Reset Enable
 *  0b0..Clock Monitor generates interrupt when error detected
 *  0b1..Clock Monitor generates reset when error detected
 */
#define SCG_ROSCCSR_ROSCCMRE(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_ROSCCSR_ROSCCMRE_SHIFT)) & SCG_ROSCCSR_ROSCCMRE_MASK)
#define SCG_ROSCCSR_LK_MASK		(0x800000U)
#define SCG_ROSCCSR_LK_SHIFT		(23U)
/* LK - Lock Register
 *  0b0..Control Status Register can be written.
 *  0b1..Control Status Register cannot be written.
 */
#define SCG_ROSCCSR_LK(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_ROSCCSR_LK_SHIFT)) & SCG_ROSCCSR_LK_MASK)
#define SCG_ROSCCSR_ROSCVLD_MASK	(0x1000000U)
#define SCG_ROSCCSR_ROSCVLD_SHIFT	(24U)
/* ROSCVLD - RTC OSC Valid
 *  0b0..RTC OSC is not enabled or clock is not valid
 *  0b1..RTC OSC is enabled and output clock is valid
 */
#define SCG_ROSCCSR_ROSCVLD(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_ROSCCSR_ROSCVLD_SHIFT)) & SCG_ROSCCSR_ROSCVLD_MASK)
#define SCG_ROSCCSR_ROSCSEL_MASK	(0x2000000U)
#define SCG_ROSCCSR_ROSCSEL_SHIFT	(25U)
/* ROSCSEL - RTC OSC Selected
 *  0b0..RTC OSC is not the system clock source
 *  0b1..RTC OSC is the system clock source
 */
#define SCG_ROSCCSR_ROSCSEL(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_ROSCCSR_ROSCSEL_SHIFT)) & SCG_ROSCCSR_ROSCSEL_MASK)
#define SCG_ROSCCSR_ROSCERR_MASK	(0x4000000U)
#define SCG_ROSCCSR_ROSCERR_SHIFT	(26U)
/* ROSCERR - RTC OSC Clock Error
 *  0b0..RTC OSC Clock Monitor is disabled or has not detected an error
 *  0b1..RTC OSC Clock Monitor is enabled and detected an RTC loss of clock error
 */
#define SCG_ROSCCSR_ROSCERR(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_ROSCCSR_ROSCERR_SHIFT)) & SCG_ROSCCSR_ROSCERR_MASK)

/* LPFLLCSR - Low Power FLL Control Status Register */
#define SCG_LPFLLCSR_LPFLLEN_MASK	(0x1U)
#define SCG_LPFLLCSR_LPFLLEN_SHIFT	(0U)
/* LPFLLEN - LPFLL Enable
 *  0b0..LPFLL is disabled
 *  0b1..LPFLL is enabled
 */
#define SCG_LPFLLCSR_LPFLLEN(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLEN_SHIFT)) & SCG_LPFLLCSR_LPFLLEN_MASK)
#define SCG_LPFLLCSR_LPFLLSTEN_MASK	(0x2U)
#define SCG_LPFLLCSR_LPFLLSTEN_SHIFT	(1U)
/* LPFLLSTEN - LPFLL Stop Enable
 *  0b0..LPFLL is disabled in Stop modes.
 *  0b1..LPFLL is enabled in Stop modes
 */
#define SCG_LPFLLCSR_LPFLLSTEN(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLSTEN_SHIFT)) & SCG_LPFLLCSR_LPFLLSTEN_MASK)
#define SCG_LPFLLCSR_LPFLLTREN_MASK	(0x100U)
#define SCG_LPFLLCSR_LPFLLTREN_SHIFT	(8U)
/* LPFLLTREN - LPFLL Trim Enable
 *  0b0..Disable trimming LPFLL to an reference clock source
 *  0b1..Enable trimming LPFLL to an reference clock source
 */
#define SCG_LPFLLCSR_LPFLLTREN(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLTREN_SHIFT)) & SCG_LPFLLCSR_LPFLLTREN_MASK)
#define SCG_LPFLLCSR_LPFLLTRUP_MASK	(0x200U)
#define SCG_LPFLLCSR_LPFLLTRUP_SHIFT	(9U)
/* LPFLLTRUP - LPFLL Trim Update
 *  0b0..Disable LPFLL trimming updates. LPFLL frequency determined by AUTOTRIM written value.
 *  0b1..Enable LPFLL trimming updates. LPFLL frequency determined by reference clock multiplication
 */
#define SCG_LPFLLCSR_LPFLLTRUP(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLTRUP_SHIFT)) & SCG_LPFLLCSR_LPFLLTRUP_MASK)
#define SCG_LPFLLCSR_LPFLLTRMLOCK_MASK	(0x400U)
#define SCG_LPFLLCSR_LPFLLTRMLOCK_SHIFT	(10U)
/* LPFLLTRMLOCK - LPFLL Trim LOCK
 *  0b0..LPFLL not Locked
 *  0b1..LPFLL trimmed and Locked
 */
#define SCG_LPFLLCSR_LPFLLTRMLOCK(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLTRMLOCK_SHIFT)) & SCG_LPFLLCSR_LPFLLTRMLOCK_MASK)
#define SCG_LPFLLCSR_LPFLLCM_MASK	(0x10000U)
#define SCG_LPFLLCSR_LPFLLCM_SHIFT	(16U)
/* LPFLLCM - LPFLL Clock Monitor
 *  0b0..LPFLL Clock Monitor is disabled
 *  0b1..LPFLL Clock Monitor is enabled
 */
#define SCG_LPFLLCSR_LPFLLCM(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLCM_SHIFT)) & SCG_LPFLLCSR_LPFLLCM_MASK)
#define SCG_LPFLLCSR_LPFLLCMRE_MASK	(0x20000U)
#define SCG_LPFLLCSR_LPFLLCMRE_SHIFT	(17U)
/* LPFLLCMRE - LPFLL Clock Monitor Reset Enable
 *  0b0..Clock Monitor generates interrupt when error detected
 *  0b1..Clock Monitor generates reset when error detected
 */
#define SCG_LPFLLCSR_LPFLLCMRE(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLCMRE_SHIFT)) & SCG_LPFLLCSR_LPFLLCMRE_MASK)
#define SCG_LPFLLCSR_LK_MASK		(0x800000U)
#define SCG_LPFLLCSR_LK_SHIFT		(23U)
/* LK - Lock Register
 *  0b0..Control Status Register can be written.
 *  0b1..Control Status Register cannot be written.
 */
#define SCG_LPFLLCSR_LK(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LK_SHIFT)) & SCG_LPFLLCSR_LK_MASK)
#define SCG_LPFLLCSR_LPFLLVLD_MASK	(0x1000000U)
#define SCG_LPFLLCSR_LPFLLVLD_SHIFT	(24U)
/* LPFLLVLD - LPFLL Valid
 *  0b0..LPFLL is not enabled or clock is not valid.
 *  0b1..LPFLL is enabled and output clock is valid.
 */
#define SCG_LPFLLCSR_LPFLLVLD(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLVLD_SHIFT)) & SCG_LPFLLCSR_LPFLLVLD_MASK)
#define SCG_LPFLLCSR_LPFLLSEL_MASK	(0x2000000U)
#define SCG_LPFLLCSR_LPFLLSEL_SHIFT	(25U)
/* LPFLLSEL - LPFLL Selected
 *  0b0..LPFLL is not the system clock source
 *  0b1..LPFLL is the system clock source
 */
#define SCG_LPFLLCSR_LPFLLSEL(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLSEL_SHIFT)) & SCG_LPFLLCSR_LPFLLSEL_MASK)
#define SCG_LPFLLCSR_LPFLLERR_MASK	(0x4000000U)
#define SCG_LPFLLCSR_LPFLLERR_SHIFT	(26U)
/* LPFLLERR - LPFLL Clock Error
 *  0b0..Error not detected with the LPFLL trimming.
 *  0b1..Error detected with the LPFLL trimming.
 */
#define SCG_LPFLLCSR_LPFLLERR(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCSR_LPFLLERR_SHIFT)) & SCG_LPFLLCSR_LPFLLERR_MASK)

/* LPFLLDIV - Low Power FLL Divide Register */
#define SCG_LPFLLDIV_LPFLLDIV1_MASK	(0x7U)
#define SCG_LPFLLDIV_LPFLLDIV1_SHIFT	(0U)
/* LPFLLDIV1 - LPFLL Clock Divide 1
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_LPFLLDIV_LPFLLDIV1(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLDIV_LPFLLDIV1_SHIFT)) & SCG_LPFLLDIV_LPFLLDIV1_MASK)
#define SCG_LPFLLDIV_LPFLLDIV2_MASK	(0x700U)
#define SCG_LPFLLDIV_LPFLLDIV2_SHIFT	(8U)
/* LPFLLDIV2 - LPFLL Clock Divide 2
 *  0b000..Output disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_LPFLLDIV_LPFLLDIV2(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLDIV_LPFLLDIV2_SHIFT)) & SCG_LPFLLDIV_LPFLLDIV2_MASK)
#define SCG_LPFLLDIV_LPFLLDIV3_MASK	(0x70000U)
#define SCG_LPFLLDIV_LPFLLDIV3_SHIFT	(16U)
/* LPFLLDIV3 - LPFLL Clock Divide 3
 *  0b000..Clock disabled
 *  0b001..Divide by 1
 *  0b010..Divide by 2
 *  0b011..Divide by 4
 *  0b100..Divide by 8
 *  0b101..Divide by 16
 *  0b110..Divide by 32
 *  0b111..Divide by 64
 */
#define SCG_LPFLLDIV_LPFLLDIV3(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLDIV_LPFLLDIV3_SHIFT)) & SCG_LPFLLDIV_LPFLLDIV3_MASK)

/* LPFLLCFG - Low Power FLL Configuration Register */
#define SCG_LPFLLCFG_FSEL_MASK		(0x3U)
#define SCG_LPFLLCFG_FSEL_SHIFT		(0U)
/* FSEL - Frequency Select
 *  0b00..LPFLL is trimmed to 48 MHz.
 *  0b01..LPFLL is trimmed to 72 MHz.
 *  0b10..Reserved
 *  0b11..Reserved
 */
#define SCG_LPFLLCFG_FSEL(x)		\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLCFG_FSEL_SHIFT)) & SCG_LPFLLCFG_FSEL_MASK)

/* LPFLLTCFG - Low Power FLL Trim Configuration Register */
#define SCG_LPFLLTCFG_TRIMSRC_MASK	(0x3U)
#define SCG_LPFLLTCFG_TRIMSRC_SHIFT	(0U)
/* TRIMSRC - Trim Source
 *  0b00..SIRC
 *  0b01..FIRC
 *  0b10..System OSC
 *  0b11..RTC OSC
 */
#define SCG_LPFLLTCFG_TRIMSRC(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLTCFG_TRIMSRC_SHIFT)) & SCG_LPFLLTCFG_TRIMSRC_MASK)
#define SCG_LPFLLTCFG_TRIMDIV_MASK	(0x1F00U)
#define SCG_LPFLLTCFG_TRIMDIV_SHIFT	(8U)
#define SCG_LPFLLTCFG_TRIMDIV(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLTCFG_TRIMDIV_SHIFT)) & SCG_LPFLLTCFG_TRIMDIV_MASK)
#define SCG_LPFLLTCFG_LOCKW2LSB_MASK	(0x10000U)
#define SCG_LPFLLTCFG_LOCKW2LSB_SHIFT	(16U)
/* LOCKW2LSB - Lock LPFLL with 2 LSBS
 *  0b0..LPFLL locks within 1LSB (0.4%)
 *  0b1..LPFLL locks within 2LSB (0.8%)
 */
#define SCG_LPFLLTCFG_LOCKW2LSB(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLTCFG_LOCKW2LSB_SHIFT)) & SCG_LPFLLTCFG_LOCKW2LSB_MASK)

/* LPFLLSTAT - Low Power FLL Status Register */
#define SCG_LPFLLSTAT_AUTOTRIM_MASK	(0xFFU)
#define SCG_LPFLLSTAT_AUTOTRIM_SHIFT	(0U)
#define SCG_LPFLLSTAT_AUTOTRIM(x)	\
	(((uint32_t)(((uint32_t)(x)) << SCG_LPFLLSTAT_AUTOTRIM_SHIFT)) & SCG_LPFLLSTAT_AUTOTRIM_MASK)

#define BOARD_BOOTCLOCKRUN_CORE_CLOCK		U(48000000)
#define BOARD_BOOTCLOCKHSRUN_CORE_CLOCK		U(72000000)
#define BOARD_BOOTCLOCKVLPR_CORE_CLOCK		U(4000000)

void BOARD_InitBootClocks(void);
void BOARD_BootClockRUN(void);
void BOARD_BootClockHSRUN(void);
void BOARD_BootClockVLPR(void);

#if 0
/* Boot clocks */
extern const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockRUN;
extern const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockRUN;
extern const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockRUN;
extern const scg_firc_config_t g_scgFircConfigBOARD_BootClockRUN;
extern const scg_lpfll_config_t g_scgLpFllConfigBOARD_BootClockRUN;

/* HS clocks */
extern const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockHSRUN;
extern const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockHSRUN;
extern const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockHSRUN;
extern const scg_firc_config_t g_scgFircConfigBOARD_BootClockHSRUN;
extern const scg_lpfll_config_t g_scgLpFllConfigBOARD_BootClockHSRUN;

/* LPR clocks */
extern const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockVLPR;
extern const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockVLPR;
extern const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockVLPR;
extern const scg_firc_config_t g_scgFircConfigBOARD_BootClockVLPR;
extern const scg_lpfll_config_t g_scgLpFllConfigBOARD_BootClockVLPR;
#endif

#endif /* __SCG_VEGA_H_INCLUDE__ */
