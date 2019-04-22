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
 * @(#)gcce.h: GCCE (GCC East) definitions
 * $Id: gcce.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __GCCE_QDF2400_H_INCLUDE__
#define __GCCE_QDF2400_H_INCLUDE__

/* Accepts the clock outputs from pad west_xo, and sleep clk, also
 * accepts SATA/EMAC PHY output clock, generates west side devices'
 * clocks and resets. It also includes a main reset controller,
 * generates source clock for PCC and IMC_CC, and source resets for
 * GCCE, GCCMS, PCC, IMC_CC, PCIE_CC.
*/
#ifndef GCCE_BASE
#define GCCE_BASE			ULL(0x0FF22400000)
#endif

#define GCCE_REG(off)			((caddr_t)GCCE_BASE + (off))

#define GCCE_PLL_BASE			GCCE_REG(0x00000)
#define GCCE_PLL_BLOCK_SHIFT		16
#define GCCE_PLL_REG(blk, off)		\
	(GCCE_PLL_BASE + ((blk) << GCCE_PLL_BLOCK_SHIFT) + (off))

/* East PLLs */
#define GCCE_EAST_PLL0			0
#define GCCE_EAST_PLL1			1
#define GCCE_EAST_PLL_BASE(blk)		GCCE_PLL_REG(blk, 0x0000)

/* East CCs */
#define GCCE_CC_BASE			GCCE_REG(0x20000)
#define GCCE_CC_BLOCK_SHIFT		16
#define GCCE_CC_REG(blk, off)		\
	(GCCE_CC_BASE + ((blk) << GCCE_CC_BLOCK_SHIFT) + (off))

/* XO */
#define GCCE_XO				0
#define GCCE_XO_CMD_RCGR		GCCE_CC_REG(GCCE_XO, 0x0000)
#define GCCE_XO_CFG_RCGR		GCCE_CC_REG(GCCE_XO, 0x0004)
#define GCCE_AHB_CBCR			GCCE_CC_REG(GCCE_XO, 0x0008)
#define GCCE_XO_CBCR			GCCE_CC_REG(GCCE_XO, 0x000C)
#define GCCE_XO_DIV4_CBCR		GCCE_CC_REG(GCCE_XO, 0x0028)
#define GCCE_IM_SLEEP_CBCR		GCCE_CC_REG(GCCE_XO, 0x0030)

/* HMSS */
#define GCCE_HMSS			1
#define GCCE_HMSS_BUS_CMD_RCGR		GCCE_CC_REG(GCCE_HMSS, 0x0000)
#define GCCE_HMSS_NE_AHB_CBCR		GCCE_CC_REG(GCCE_HMSS, 0x001C)
#define GCCE_HMSS_SE_AHB_CBCR		GCCE_CC_REG(GCCE_HMSS, 0x0020)

/* NCSFPB (N_CONF_SFPB) */
#define GCCE_NCSFPB			2
#define GCCE_N2_CSFPB_CMD_RCGR		GCCE_CC_REG(GCCE_NCSFPB, 0x0004)
#define GCCE_N2_CSFPB_ALWAYS_ON_CBCR	GCCE_CC_REG(GCCE_NCSFPB, 0x000C)
#define GCCE_N2_CSFPB_CBCR		GCCE_CC_REG(GCCE_NCSFPB, 0x0010)

/* MDDR */
#define GCCE_N_MDDR			3
#define GCCE_S_MDDR			12
#define GCCE_N_GCCMS_BCR		GCCE_CC_REG(GCCE_N_MDDR, 0x0000)
#define GCCE_MDDR4_AHB_CBCR		GCCE_CC_REG(GCCE_N_MDDR, 0x0004)
#define GCCE_S_GCCMS_BCR		GCCE_CC_REG(GCCE_S_MDDR, 0x0000)
#define GCCE_MDDR5_AHB_CBCR		GCCE_CC_REG(GCCE_S_MDDR, 0x0004)

/* PCIE_SS */
#define GCCE_PCIE_SS_0				4
#define GCCE_PCIE_SS_1				5
#define GCCE_PCIE_SS_BCR(blk)			GCCE_CC_REG(blk, 0x0000)
#define GCCE_PCIE_SS_VBU_CMD_RCGR(blk)		GCCE_CC_REG(blk, 0x0004)
#define GCCE_PCIE_SS_SECSFPB_AHB_CBCR(blk)	GCCE_CC_REG(blk, 0x000C)
#define GCCE_PCIE_SS_VBU_CORE_CBCR(blk)		GCCE_CC_REG(blk, 0x0010)

/* QDSS */
#define GCCE_QDSS			10
#define GCCE_QDSS_BCR			GCCE_CC_REG(GCCE_QDSS, 0x0000)
#define GCCE_QDSS_ATB_CMD_RCGR		GCCE_CC_REG(GCCE_QDSS, 0x0004)
#define GCCE_AT_E1_CBCR			GCCE_CC_REG(GCCE_QDSS, 0x0010)
#define GCCE_AT_E2_CBCR			GCCE_CC_REG(GCCE_QDSS, 0x0014)
#define GCCE_AT_E3_CBCR			GCCE_CC_REG(GCCE_QDSS, 0x0018)
#define GCCE_HMSS_AT_E_CBCR		GCCE_CC_REG(GCCE_QDSS, 0x001C)
#define GCCE_AT_E5_CBCR			GCCE_CC_REG(GCCE_QDSS, 0x0020)
#define GCCE_AT_E7_CBCR			GCCE_CC_REG(GCCE_QDSS, 0x0024)
#define GCCE_AT_E9_CBCR			GCCE_CC_REG(GCCE_QDSS, 0x0028)

/* SCSFPB (S_CONF_SFPB) */
#define GCCE_SCSFPB			11
#define GCCE_S2_CSFPB_CMD_RCGR		GCCE_CC_REG(GCCE_SCSFPB, 0x0004)
#define GCCE_S2_CSFPB_ALWAYS_ON_CBCR	GCCE_CC_REG(GCCE_SCSFPB, 0x000C)
#define GCCE_S2_CSFPB_CBCR		GCCE_CC_REG(GCCE_SCSFPB, 0x0010)
#define GCCE_SECSFPB_AHB_CBCR		GCCE_CC_REG(GCCE_SCSFPB, 0x0014)
#define GCCE_SECSFPB			13
#define GCCE_SECSFPB_CMD_RCGR		GCCE_CC_REG(GCCE_SECSFPB, 0x0004)
#define GCCE_PSCLKGEN_CFG_AHB_CBCR	GCCE_CC_REG(GCCE_SECSFPB, 0x000C)
#define GCCE_SECSFPB_CBCR		GCCE_CC_REG(GCCE_SECSFPB, 0x0010)
#define GCCE_SECSFPB_ALWAYS_ON_CBCR	GCCE_CC_REG(GCCE_SECSFPB, 0x0014)

/* TSENS */
#define GCCE_TSENS5			14
#define GCCE_TSENS6			15
#define GCCE_TSENS7			16
#define GCCE_TSENS8			17
/* TSENS */
#define GCCE_TSENS_BCR(blk)		GCCE_CC_REG(blk, 0x0000)
#define GCCE_TSENS_AHB_CBCR(blk)	GCCE_CC_REG(blk, 0x0004)
#define GCCE_TSENS_EXT_CBCR(blk)	GCCE_CC_REG(blk, 0x0008)
#define GCCE_TSENS_SLEEP_CBCR(blk)	GCCE_CC_REG(blk, 0x000C)

/* DEBUG */
#define GCCE_DEBUG			19
#define GCCE_DEBUG_CDIVR		GCCE_CC_REG(GCCE_DEBUG, 0x0010)
#define GCCE_DEBUG_CLK_CTL		GCCE_CC_REG(GCCE_DEBUG, 0x0014)
#define GCCE_PLLTEST_PAD_CFG		GCCE_CC_REG(GCCE_DEBUG, 0x0018)
#define GCCE_CLOCK_FRQ_MEASURE_CTL	GCCE_CC_REG(GCCE_DEBUG, 0x0020)
#define GCCE_CLOCK_FRQ_MEASURE_STATUS	GCCE_CC_REG(GCCE_DEBUG, 0x0024)
#define GCCE_TEST_BUS_CTL		GCCE_CC_REG(GCCE_DEBUG, 0x0028)

#endif /* _GCCE_QDF2400_H_INCLUDE__ */
