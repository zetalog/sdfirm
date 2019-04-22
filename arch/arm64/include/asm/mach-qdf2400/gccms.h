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
 * @(#)gccms.h: GCCMS (GCC Memory Slice) definitions
 * $Id: gccms.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __GCCMS_QDF2400_H_INCLUDE__
#define __GCCMS_QDF2400_H_INCLUDE__

/* Accepts the clock outputs from GCCMW AHB clocks, controls clock
 * of the DDR channels, generates DDR clocks and resets.
 */
#ifndef N_GCCMS_BASE
#define N_GCCMS_BASE			ULL(0x0FF08200000)
#endif
#ifndef S_GCCMS_BASE
#define S_GCCMS_BASE			ULL(0x0FF1E400000)
#endif
#define GCCMS_BASE(x)			x##_GCCMS_BASE
#define GCCMS_REG(n, off)		((caddr_t)GCCMS_BASE(n) + (off))

/* Memory slice PLLs */
#define GCCMS_PLL_BLOCK_SHIFT		16
#define GCCMS_PLL_REG(n, blk, off)	\
	(GCCMS_BASE(n) + ((blk) << GCCMS_PLL_BLOCK_SHIFT) + (off))

/* GCCMS_MDDR_PLL */
#define GCCMS_DDR_PLL(ddr)		(1 + ((ddr) << 1))
#define GCCMS_MDDR_PLL_BASE(ns, ddr)	GCCMS_PLL_REG(ns, GCCMS_DDR_PLL(ddr), 0)

/* Memory slice CCs */
#define GCCMS_CC_BLOCK_SHIFT		16
#define GCCMS_CC_REG(n, blk, off)	\
	(GCCMS_BASE(n) + ((blk) << GCCMS_CC_BLOCK_SHIFT) + (off))

/* GCCMS_AHB */
#define GCCMS_AHB			8
#define GCCMS_AHB_CBCR(n)		GCCMS_CC_BLOCK_SHIFT(n, GCCMS_AHB, 0)
#define GCCMS_IM_SLEEP_CBCR(n)		GCCMS_CC_BLOCK_SHIFT(n, GCCMS_AHB, 4)
#define GCCMS_XO_DIV4_CBCR(n)		GCCMS_CC_BLOCK_SHIFT(n, GCCMS_AHB, 8)

/* GCCMS_MDDR_CC */
#define GCCMS_DDR0			0
#define GCCMS_DDR1			1
#define GCCMS_DDR2			2
#define GCCMS_DDR_CC(ddr)		(10 + (ddr))
#define GCCMS_MDDR_BCR(n, ddr)		GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x00)
#define GCCMS_MDDR_RESETR(n, ddr)	GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x14)
#define GCCMS_MDDR_CGF4R(n, ddr)	GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x18)
#define GCCMS_MDDR_2X_CBCR(n, ddr)	GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x20)
#define GCCMS_MDDR_MISC(n, ddr)		GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x30)
#define GCCMS_MDDR_CLKGEN_MISC(n, ddr)	GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x38)
#define GCCMS_MDDR_SLEEP_CBCR(n, ddr)	GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x40)
#define GCCMS_MDDR_XO_CBCR(n, ddr)	GCCMS_CC_REG(n, GCCMS_DDR_CC(ddr), 0x44)
#define GCCMS_MDDR_0_CGF4R(n)		GCCMS_CC_REG(n, GCCMS_DDR_CC(0), 0x18)
#define GCCMS_MDDR_1_CGF4R(n)		GCCMS_CC_REG(n, GCCMS_DDR_CC(1), 0x48)
#define GCCMS_MDDR_2_CGF4R(n)		GCCMS_CC_REG(n, GCCMS_DDR_CC(2), 0x48)

/* GCCMS_MDDR_x_CGF4R */
#define GCCMS_MDDR_MUX_SEL_OFFSET	0
#define GCCMS_MDDR_MUX_SEL_MASK		REG_2BIT_MASK
#define GCCMS_MDDR_MUX_SEL(value)	_SET_FV(GCCMS_MDDR_MUX_SEL, value)

#define GCCMS_MDDR_MUX_SEL_XO		0
#define GCCMS_MDDR_MUX_SEL_CLKGEN	1
#define GCCMS_MDDR_MUX_SEL_PLL		2
#define GCCMS_MDDR_MUX_SEL_PLL_TEST	3

/* GCCMS_TEST */
#define GCCMS_TEST			14
#define GCCMS_DEBUG_CLK_CTL(n)		GCCMS_CC_REG(n, GCCMS_TEST, 0x000)
#define GCCMS_DEBUG_CDIVR(n)		GCCMS_CC_REG(n, GCCMS_TEST, 0x004)
#define GCCMS_TEST_BUS_CTL(n)		GCCMS_CC_REG(n, GCCMS_TEST, 0x008)
#define GCCMS_PLLTEST_PAD_CFG(n)	GCCMS_CC_REG(n, GCCMS_TEST, 0x100)
#define GCCMS_CLOCK_FRQ_MEASURE_CTL(n)	GCCMS_CC_REG(n, GCCMS_TEST, 0x104)
#define GCCMS_CLOCK_FRQ_MEASURE_STATUS(n)	\
	GCCMS_CC_REG(n, GCCMS_TEST, 0x108)

/* GCCMS_SPARE */
#define GCCMS_SPARE			15
#define GCCMS_SPARE_0(n)		GCCMS_CC_REG(n, GCCMS_SPARE, 0x00)

#endif /* __GCCMS_QDF2400_H_INCLUDE__ */
