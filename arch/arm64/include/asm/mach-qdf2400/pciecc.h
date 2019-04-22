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
 * @(#)pciecc.h: PCIe_CC (PCIe Clock Controller) definitions
 * $Id: pciecc.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __PCIECC_QDF2400_H_INCLUDE__
#define __PCIECC_QDF2400_H_INCLUDE__

/* Instantiated in the 2 pcie_quad hard macros, accepts the clock outputs
 * from the pad pcie_xo, also accepts PLL outputs from GCCE, and generates
 * clocks and resets for the pci_quad hard macros.
 */
#define PCIE_CC_BLOCK_SHIFT		16
#define PCIE0_CC_BASE			ULL(0x0FF23800000)
#define PCIE1_CC_BASE			ULL(0x0FF25000000)
#define PCIE_CC_REG(n, blk, off)		\
	((caddr_t)PCIE0_CC_BASE + (off) +	\
	 ((n)*(PCIE1_CC_BASE-PCIE0_CC_BASE)) +	\
	 ((blk) << PCIE_CC_BLOCK_SHIFT))

/* PCIE_PHY */
#define PCIE_CC_PCIE_PHY_AA		0
#define PCIE_CC_PCIE_PHY_AB		1
#define PCIE_CC_PCIE_PHY_B		2
#define PCIE_CC_PCIE_PHY_C		3
#define PCIE_CC_PCIE_PHY_RESETR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0000)
#define PCIE_CC_PCIE_PHY_CFG_AHB_CBCR(n, blk)	\
	PCIE_CC_REG(n, blk, 0x0004)
#define PCIE_CC_PCIE_PHY_MISC(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0008)

/* PCIE */
#define PCIE_CC_PCIE_A			4
#define PCIE_CC_PCIE_B			5
#define PCIE_CC_PCIE_C			6
#define PCIE_CC_PCIE_PCLK_CMD_RCGR(n, blk)	\
	PCIE_CC_REG(n, blk, 0x0004)
#define PCIE_CC_PCIE_PCLK_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x000C)
#define PCIE_CC_PHY_AUX_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0010)

/* PCIE_SFPB */
#define PCIE_CC_PCIE_SFPB			7
#define PCIE_CC_PCIE_SFPB_CMD_RCGR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_SFPB, 0x0004)
#define PCIE_CC_PCIE_SFPB_ALWAYS_ON_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_SFPB, 0x000C)
#define PCIE_CC_PCIE_SFPB_CBCR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_SFPB, 0x0010)

/* PCIE_X8 */
#define PCIE_CC_X8_MSTR			8
#define PCIE_CC_X8_SLV			9
#define PCIE_CC_PCIE_X8_MSTR_Q23_CMD_RCGR(n)		\
	PCIE_CC_REG(n, PCIE_CC_X8_MSTR, 0x0004)
#define PCIE_CC_PCIE_X8_SLV_Q23_CMD_RCGR(n)		\
	PCIE_CC_REG(n, PCIE_CC_X8_SLV, 0x0004)
#define PCIE_CC_X8_0					16
#define PCIE_CC_X8_1					17
#define PCIE_CC_PCIE_X8_BCR(n, blk)			\
	PCIE_CC_REG(n, blk, 0x0000)
#define PCIE_CC_PCIE_X8_RESETR(n, blk)			\
	PCIE_CC_REG(n, blk, 0x0004)
#define PCIE_CC_PCIE_X8_AHB_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0008)
#define PCIE_CC_PCIE_X8_AUX_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x000C)
#define PCIE_CC_PCIE_X8_MMU_AHB_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0010)
#define PCIE_CC_PCIE_X8_MMU_MSTR_Q23_CBCR(n, blk)	\
	PCIE_CC_REG(n, blk, 0x0014)
#define PCIE_CC_PCIE_X8_MSTR_Q23_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0018)
#define PCIE_CC_PCIE_X8_SLV_Q23_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x001C)
#define PCIE_CC_PCIE_X8_PCLK_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0020)
#define PCIE_CC_PCIE_X8_MSTR_AXI_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0024)
#define PCIE_CC_PCIE_X8_SLV_AXI_CBCR(n, blk)		\
	PCIE_CC_REG(n, blk, 0x0028)

/* PCIE_CC_PCIE_X16 */
#define PCIE_CC_PCIE_X16			18
#define PCIE_CC_PCIE_X16_BCR(n)			\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0000)
#define PCIE_CC_PCIE_X16_RESETR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0004)
#define PCIE_CC_PCIE_X16_MSTR_Q23_CMD_RCGR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0014)
#define PCIE_CC_PCIE_X16_SLV_Q23_CMD_RCGR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0024)
#define PCIE_CC_PCIE_X16_AHB_CBCR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0038)
#define PCIE_CC_PCIE_X16_AUX_CBCR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x003C)
#define PCIE_CC_PCIE_X16_MMU_AHB_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0040)
#define PCIE_CC_PCIE_X16_MMU_MSTR_Q23_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0044)
#define PCIE_CC_PCIE_X16_MSTR_Q23_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0048)
#define PCIE_CC_PCIE_X16_SLV_Q23_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x004C)
#define PCIE_CC_PCIE_X16_PCLK_CBCR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0050)
#define PCIE_CC_PCIE_X16_PCLK_MUXR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x005C)
#define PCIE_CC_PCIE_X16_MSTR_AXI_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0060)
#define PCIE_CC_PCIE_X16_SLV_AXI_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_X16, 0x0064)

/* PCIE_VBU */
#define PCIE_CC_PCIE_VBU		19
#define PCIE_CC_PCIE_VBU_BCR(n)		\
	PCIE_CC_REG(n, PCIE_CC_PCIE_VBU, 0x0000)
#define PCIE_CC_VBU_AHB_CFG_CBCR(n)	\
	PCIE_CC_REG(n, PCIE_CC_PCIE_VBU, 0x0004)

/* PCIE_AUX */
#define PCIE_CC_PCIE_AUX		20
#define PCIE_CC_PCIE_AUX_CMD_RCGR(n)	PCIE_CC_REG(n, PCIE_CC_PCIE_AUX, 0x0004) /* MND */
#define PCIE_CC_PCIE_AUX_MND_MASK	0xFFFF

/* IM_SLEEP */
#define PCIE_CC_IM_SLEEP		21
#define PCIE_CC_IM_SLEEP_CBCR(n)	PCIE_CC_REG(n, PCIE_CC_IM_SLEEP, 0x0004)

/* DEBUG */
#define PCIE_CC_DEBUG			22
#define PCIE_CC_DEBUG_CLK_CTL(n)	PCIE_CC_REG(n, PCIE_CC_DEBUG, 0x0000)
#define PCIE_CC_DEBUG_CDIVR(n)		PCIE_CC_REG(n, PCIE_CC_DEBUG, 0x0004)

/* MISC */
#define PCIE_CC_MISC			23
#define PCIE_CC_PCIE_CLKGEN_MISC	PCIE_CC_REG(n, PCIE_CC_MISC, 0x0004)

#endif /* __PCIECC_QDF2400_H_INCLUDE__ */
