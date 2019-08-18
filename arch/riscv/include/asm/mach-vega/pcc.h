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

#define PCC0_BASE		UL(0x4002B000)
#define PCC1_BASE		UL(0x41027000)
#define PCC_REG(n, offset)	(PCC##n##_BASE + (offset))

/* PCC 0 */
#define PCC_Mscm		PCC_REG(0, 0x004)
#define PCC_Syspm		PCC_REG(0, 0x00C)
#define PCC_Max0		PCC_REG(0, 0x010)
#define PCC_Edma0		PCC_REG(0, 0x020)
#define PCC_Flexbus		PCC_REG(0, 0x030)
#define PCC_Xrdc0Mgr		PCC_REG(0, 0x050)
#define PCC_Xrdc0Pac		PCC_REG(0, 0x058)
#define PCC_Xrdc0Mrc		PCC_REG(0, 0x05C)
#define PCC_Sema420		PCC_REG(0, 0x06C)
#define PCC_Dmamux0		PCC_REG(0, 0x084)
#define PCC_Ewm0		PCC_REG(0, 0x088)
#define PCC_MuA			PCC_REG(0, 0x094)
#define PCC_Crc0		PCC_REG(0, 0x0BC)
#define PCC_Lpit0		PCC_REG(0, 0x0C0)
#define PCC_Tpm0		PCC_REG(0, 0x0D4)
#define PCC_Tpm1		PCC_REG(0, 0x0D8)
#define PCC_Tpm2		PCC_REG(0, 0x0DC)
#define PCC_Emvsim0		PCC_REG(0, 0x0E0)
#define PCC_Flexio0		PCC_REG(0, 0x0E4)
#define PCC_Lpi2c0		PCC_REG(0, 0x0E8)
#define PCC_Lpi2c1		PCC_REG(0, 0x0EC)
#define PCC_Lpi2c2		PCC_REG(0, 0x0F0)
#define PCC_Sai0		PCC_REG(0, 0x0F4)
#define PCC_Sdhc0		PCC_REG(0, 0x0F8)
#define PCC_Lpspi0		PCC_REG(0, 0x0FC)
#define PCC_Lpspi1		PCC_REG(0, 0x100)
#define PCC_Lpspi2		PCC_REG(0, 0x104)
#define PCC_Lpuart0		PCC_REG(0, 0x108)
#define PCC_Lpuart1		PCC_REG(0, 0x10C)
#define PCC_Lpuart2		PCC_REG(0, 0x110)
#define PCC_Usb0		PCC_REG(0, 0x114)
#define PCC_PortA		PCC_REG(0, 0x118)
#define PCC_PortB		PCC_REG(0, 0x11C)
#define PCC_PortC		PCC_REG(0, 0x120)
#define PCC_PortD		PCC_REG(0, 0x124)
#define PCC_Lpadc0		PCC_REG(0, 0x128)
#define PCC_Dac0		PCC_REG(0, 0x130)
#define PCC_Vref		PCC_REG(0, 0x134)
#define PCC_Atx			PCC_REG(0, 0x138)
#define PCC_Intmux0		PCC_REG(0, 0x13C)
#define PCC_Trace		PCC_REG(0, 0x200)
/* PCC 1 */
#define PCC_Edma1		PCC_REG(1, 0x020)
#define PCC_Rgpio1		PCC_REG(1, 0x03C)
#define PCC_Xrdc0PacB		PCC_REG(1, 0x058)
#define PCC_Xrdc0MrcB		PCC_REG(1, 0x05C)
#define PCC_Sema421		PCC_REG(1, 0x06C)
#define PCC_Dmamux1		PCC_REG(1, 0x084)
#define PCC_Intmux1		PCC_REG(1, 0x088)
#define PCC_Intmux0		PCC_REG(1, 0x088)
#define PCC_MuB			PCC_REG(1, 0x090)
#define PCC_Cau3		PCC_REG(1, 0x0A0)
#define PCC_Trng		PCC_REG(1, 0x0A4)
#define PCC_Lpit1		PCC_REG(1, 0x0A8)
#define PCC_Tpm3		PCC_REG(1, 0x0B4)
#define PCC_Lpi2c3		PCC_REG(1, 0x0B8)
#define PCC_Lpspi3		PCC_REG(1, 0x0D4)
#define PCC_Lpuart3		PCC_REG(1, 0x0D8)
#define PCC_PortE		PCC_REG(1, 0x0DC)
#define PCC_Ext0		PCC_REG(1, 0x200)
#define PCC_Ext1		PCC_REG(1, 0x204)

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

#endif /* __PCC_VEGA_H_INCLUDE__ */
