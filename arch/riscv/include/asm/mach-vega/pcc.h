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
 * @(#)pcc.h: RV32M1 (VEGA) PCC (peripheral clock control) driver
 * $Id: pcc.h,v 1.1 2019-08-17 16:30:00 zhenglv Exp $
 */

#ifndef __PCC_VEGA_H_INCLUDE__
#define __PCC_VEGA_H_INCLUDE__

#define PCC_REG(base, offset)	((base) + (offset))

#define PCC_CLKCFG_PCD_MASK	REG_3BIT_MASK
#define PCC_CLKCFG_PCD_SHIFT	0
/* PCD - Peripheral Clock Divider Select
 *  0b000..Divide by 1.
 *  0b001..Divide by 2.
 *  0b010..Divide by 3.
 *  0b011..Divide by 4.
 *  0b100..Divide by 5.
 *  0b101..Divide by 6.
 *  0b110..Divide by 7.
 *  0b111..Divide by 8.
 */
#define PCC_CLKCFG_PCD(x)	\
	(((uint32_t)(((uint32_t)(x)) << PCC_CLKCFG_PCD_SHIFT)) & PCC_CLKCFG_PCD_MASK)
#define PCC_CLKCFG_FRAC_MASK	(0x8U)
#define PCC_CLKCFG_FRAC_SHIFT	3
/* FRAC - Peripheral Clock Divider Fraction
 *  0b0..Fractional value is 0.
 *  0b1..Fractional value is 1.
 */
#define PCC_CLKCFG_FRAC(x)	\
	(((uint32_t)(((uint32_t)(x)) << PCC_CLKCFG_FRAC_SHIFT)) & PCC_CLKCFG_FRAC_MASK)

#define PCC_CLKCFG_PCS_MASK	(0x7000000U)
#define PCC_CLKCFG_PCS_SHIFT	24
/* PCS - Peripheral Clock Source Select
 *  0b000..Clock is off. An external clock can be enabled for this peripheral.
 *  0b001..Clock option 1
 *  0b010..Clock option 2
 *  0b011..Clock option 3
 *  0b100..Clock option 4
 *  0b101..Clock option 5
 *  0b110..Clock option 6
 *  0b111..Clock option 7
 */
#define PCC_CLKCFG_PCS(x)	\
	(((uint32_t)(((uint32_t)(x)) << PCC_CLKCFG_PCS_SHIFT)) & PCC_CLKCFG_PCS_MASK)

#define PCC_CLKCFG_INUSE_MASK	UL(0x20000000)
#define PCC_CLKCFG_INUSE_SHIFT	29
/* INUSE - In use flag
 *  0b0..Peripheral is not being used.
 *  0b1..Peripheral is being used. Software cannot modify the existing clocking configuration.
 */
#define PCC_CLKCFG_INUSE(x)	\
	(((uint32_t)(((uint32_t)(x)) << PCC_CLKCFG_INUSE_SHIFT)) & PCC_CLKCFG_INUSE_MASK)

#define PCC_CLKCFG_CGC_MASK	(0x40000000U)
#define PCC_CLKCFG_CGC_SHIFT	30
/* CGC - Clock Gate Control
 *  0b0..Clock disabled
 *  0b1..Clock enabled. The current clock selection and divider options are locked.
 **/
#define PCC_CLKCFG_CGC(x)	\
	(((uint32_t)(((uint32_t)(x)) << PCC_CLKCFG_CGC_SHIFT)) & PCC_CLKCFG_CGC_MASK)

#define PCC_CLKCFG_PR_MASK	(0x80000000U)
#define PCC_CLKCFG_PR_SHIFT	31
/* PR - Present
 *  0b0..Peripheral is not present.
 *  0b1..Peripheral is present.
 */
#define PCC_CLKCFG_PR(x)	\
	(((uint32_t)(((uint32_t)(x)) << PCC_CLKCFG_PR_SHIFT)) & PCC_CLKCFG_PR_MASK)

/* The count of PCC_CLKCFG */
#define PCC_CLKCFG_COUNT	(130U)

/* PCC - Peripheral instance base addresses */
/* Peripheral PCC0 base address */
#define PCC0_BASE		UL(0x4002B000)
#define PCC1_BASE		UL(0x41027000)

/* PCC 0 */
#define CLOCK_Mscm		PCC_REG(PCC0_BASE, 0x4)
#define CLOCK_Syspm		PCC_REG(PCC0_BASE, 0xC)
#define CLOCK_Max0		PCC_REG(PCC0_BASE, 0x10)
#define CLOCK_Edma0		PCC_REG(PCC0_BASE, 0x20)
#define CLOCK_Flexbus		PCC_REG(PCC0_BASE, 0x30)
#define CLOCK_Xrdc0Mgr		PCC_REG(PCC0_BASE, 0x50)
#define CLOCK_Xrdc0Pac		PCC_REG(PCC0_BASE, 0x58)
#define CLOCK_Xrdc0Mrc		PCC_REG(PCC0_BASE, 0x5C)
#define CLOCK_Sema420		PCC_REG(PCC0_BASE, 0x6C)
#define CLOCK_Dmamux0		PCC_REG(PCC0_BASE, 0x84)
#define CLOCK_Ewm0		PCC_REG(PCC0_BASE, 0x88)
#define CLOCK_MuA		PCC_REG(PCC0_BASE, 0x94)
#define CLOCK_Crc0		PCC_REG(PCC0_BASE, 0xBC)
#define CLOCK_Lpit0		PCC_REG(PCC0_BASE, 0xC0)
#define CLOCK_Tpm0		PCC_REG(PCC0_BASE, 0xD4)
#define CLOCK_Tpm1		PCC_REG(PCC0_BASE, 0xD8)
#define CLOCK_Tpm2		PCC_REG(PCC0_BASE, 0xDC)
#define CLOCK_Emvsim0		PCC_REG(PCC0_BASE, 0xE0)
#define CLOCK_Flexio0		PCC_REG(PCC0_BASE, 0xE4)
#define CLOCK_Lpi2c0		PCC_REG(PCC0_BASE, 0xE8)
#define CLOCK_Lpi2c1		PCC_REG(PCC0_BASE, 0xEC)
#define CLOCK_Lpi2c2		PCC_REG(PCC0_BASE, 0xF0)
#define CLOCK_Sai0		PCC_REG(PCC0_BASE, 0xF4)
#define CLOCK_Sdhc0		PCC_REG(PCC0_BASE, 0xF8)
#define CLOCK_Lpspi0		PCC_REG(PCC0_BASE, 0xFC)
#define CLOCK_Lpspi1		PCC_REG(PCC0_BASE, 0x100)
#define CLOCK_Lpspi2		PCC_REG(PCC0_BASE, 0x104)
#define CLOCK_Lpuart0		PCC_REG(PCC0_BASE, 0x108)
#define CLOCK_Lpuart1		PCC_REG(PCC0_BASE, 0x10C)
#define CLOCK_Lpuart2		PCC_REG(PCC0_BASE, 0x110)
#define CLOCK_Usb0		PCC_REG(PCC0_BASE, 0x114)
#define CLOCK_PortA		PCC_REG(PCC0_BASE, 0x118)
#define CLOCK_PortB		PCC_REG(PCC0_BASE, 0x11C)
#define CLOCK_PortC		PCC_REG(PCC0_BASE, 0x120)
#define CLOCK_PortD		PCC_REG(PCC0_BASE, 0x124)
#define CLOCK_Lpadc0		PCC_REG(PCC0_BASE, 0x128)
#define CLOCK_Dac0		PCC_REG(PCC0_BASE, 0x130)
#define CLOCK_Vref		PCC_REG(PCC0_BASE, 0x134)
#define CLOCK_Atx		PCC_REG(PCC0_BASE, 0x138)
#define CLOCK_Intmux0		PCC_REG(PCC0_BASE, 0x13C)
#define CLOCK_Trace		PCC_REG(PCC0_BASE, 0x200)
/* PCC 1 */
#define CLOCK_Edma1		PCC_REG(PCC1_BASE, 0x20)
#define CLOCK_Rgpio1		PCC_REG(PCC1_BASE, 0x3C)
#define CLOCK_Xrdc0PacB		PCC_REG(PCC1_BASE, 0x58)
#define CLOCK_Xrdc0MrcB		PCC_REG(PCC1_BASE, 0x5C)
#define CLOCK_Sema421		PCC_REG(PCC1_BASE, 0x6C)
#define CLOCK_Dmamux1		PCC_REG(PCC1_BASE, 0x84)
#define CLOCK_Intmux1		PCC_REG(PCC1_BASE, 0x88)
#define CLOCK_Intmux0		PCC_REG(PCC1_BASE, 0x88)
#define CLOCK_MuB		PCC_REG(PCC1_BASE, 0x90)
#define CLOCK_Cau3		PCC_REG(PCC1_BASE, 0xA0)
#define CLOCK_Trng		PCC_REG(PCC1_BASE, 0xA4)
#define CLOCK_Lpit1		PCC_REG(PCC1_BASE, 0xA8)
#define CLOCK_Tpm3		PCC_REG(PCC1_BASE, 0xB4)
#define CLOCK_Lpi2c3		PCC_REG(PCC1_BASE, 0xB8)
#define CLOCK_Lpspi3		PCC_REG(PCC1_BASE, 0xD4)
#define CLOCK_Lpuart3		PCC_REG(PCC1_BASE, 0xD8)
#define CLOCK_PortE		PCC_REG(PCC1_BASE, 0xDC)
#define CLOCK_Ext0		PCC_REG(PCC1_BASE, 0x200)
#define CLOCK_Ext1		PCC_REG(PCC1_BASE, 0x204)

#endif /* __PCC_VEGA_H_INCLUDE__ */
