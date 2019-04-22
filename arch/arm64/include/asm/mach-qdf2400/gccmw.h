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
 * @(#)gccmw.h: GCCMW (GCC Main West) definitions
 * $Id: gccmw.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __GCCMW_QDF2400_H_INCLUDE__
#define __GCCMW_QDF2400_H_INCLUDE__

/* Accepts the clock outputs from pad west_xo, and sleep clk, also
 * accepts SATA/EMAC PHY output clock, generates west side devices'
 * clocks and resets. It also includes a main reset controller,
 * generates source clock for PCC and IMC_CC, and source resets for
 * GCCE, GCCMS, PCC, IMC_CC, PCIE_CC.
 */
#ifndef GCCMW_BASE
#define GCCMW_BASE			ULL(0x0FF01000000)
#endif
#define GCCMW_REG(off)			((caddr_t)GCCMW_BASE + (off))

/* West PLLs */
#define GCCMW_PLL_BASE			GCCMW_REG(0x00000)
#define GCCMW_PLL_BLOCK_SHIFT		16
#define GCCMW_PLL_REG(blk, off)		\
	(GCCMW_PLL_BASE + ((blk) << GCCMW_PLL_BLOCK_SHIFT) + (off))

#define GCCMW_WEST_PLL_BASE(blk)	GCCMW_PLL_REG(blk, 0x0000)
#define GCCMW_WEST_PLL0			0
#define GCCMW_WEST_PLL1			1

/* West CC */
#define GCCMW_CC_BASE			GCCMW_REG(0x30000)
#define GCCMW_CC_BLOCK_SHIFT		16
#define GCCMW_CC_REG(blk, off)		\
	(GCCMW_CC_BASE + ((blk) << GCCMW_CC_BLOCK_SHIFT) + (off))

/* CE */
#define GCCMW_CE0				0
#define GCCMW_CE1				1
#define GCCMW_CE_BCR(blk)			GCCMW_CC_REG(blk, 0x0000)
#define GCCMW_CE_CMD_RCGR(blk)			GCCMW_CC_REG(blk, 0x0004)
#define GCCMW_CE_AHB_CBCR(blk)			GCCMW_CC_REG(blk, 0x000C)
#define GCCMW_CE_IOMMU_CFG_AHB_CBCR(blk)	GCCMW_CC_REG(blk, 0x0010)
#define GCCMW_CE_SFVBU_DVM_CBCR(blk)		GCCMW_CC_REG(blk, 0x0014)
#define GCCMW_CE_QSMMUV3_CLIENT_CBCR(blk)	GCCMW_CC_REG(blk, 0x0018)
#define GCCMW_CE_CBCR(blk)			GCCMW_CC_REG(blk, 0x001C)
#define GCCMW_CE_RESETR(blk)			GCCMW_CC_REG(blk, 0x0020)
#define GCCMW_CE_AXI_CBCR(blk)			GCCMW_CC_REG(blk, 0x0024)

/* CONFIG_FAB */
#define GCCMW_CFAB				2
#define GCCMW_CFAB_BCR				GCCMW_CC_REG(GCCMW_CFAB, 0x0000)
#define GCCMW_CFAB_CMD_RCGR			GCCMW_CC_REG(GCCMW_CFAB, 0x0004)
#define GCCMW_CFAB_AHB2MSM_CFG_AHB_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x000C)
#define GCCMW_CFAB_CORE_CBCR			GCCMW_CC_REG(GCCMW_CFAB, 0x0010)
#define GCCMW_CFAB_SATA_PHY_CFG_FCLK_CBCR	GCCMW_CC_REG(GCCMW_CFAB, 0x0030)
#define GCCMW_CFAB_SFAB_FCLK_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0038)
#define GCCMW_CFAB_SFAB_M_AXI_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x003C)
#define GCCMW_CFAB_TOPCFG1_FCLK_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0040)
#define GCCMW_CFAB_TOPCFG2_FCLK_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0044)
#define GCCMW_CFAB_WCSFPB_FCLK_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0048)
#define GCCMW_CFAB_SFAB_AXI_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0064)
#define GCCMW_CFAB_TOPCFG1_AHB_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0068)
#define GCCMW_CFAB_TOPCFG1_XPU_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_CFAB, 0x006C)
#define GCCMW_CFAB_TOPCFG2_AHB_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0070)
#define GCCMW_CFAB_HDMA_SFPB_FCLK_CBCR		GCCMW_CC_REG(GCCMW_CFAB, 0x0078)

/* CPRC_APCS */
#define GCCMW_CPRC_APCS			3
#define GCCMW_CPRC_APCS_BCR		GCCMW_CC_REG(GCCMW_CPRC_APCS, 0x0000)
#define GCCMW_CPRC_APCS_CMD_RCGR	GCCMW_CC_REG(GCCMW_CPRC_APCS, 0x0004)
#define GCCMW_CPRC_APCS_HMSS_CBCR	GCCMW_CC_REG(GCCMW_CPRC_APCS, 0x000C)

/* CPRC_DIG */
#define GCCMW_CPRC_DIG			4
#define GCCMW_CPRC_DIG_BCR		GCCMW_CC_REG(GCCMW_CPRC_DIG, 0x0000)
#define GCCMW_CPRC_DIG_CMD_RCGR		GCCMW_CC_REG(GCCMW_CPRC_DIG, 0x0004)
#define GCCMW_CPRC_DIG_AHB_CBCR		GCCMW_CC_REG(GCCMW_CPRC_DIG, 0x000C)
#define GCCMW_CPRC_DIG_CBCR		GCCMW_CC_REG(GCCMW_CPRC_DIG, 0x0010)

/* CPRC_MEM */
#define GCCMW_CPRC_MEM			5
#define GCCMW_CPRC_MEM_BCR		GCCMW_CC_REG(GCCMW_CPRC_MEM, 0x0000)
#define GCCMW_CPRC_MEM_CMD_RCGR		GCCMW_CC_REG(GCCMW_CPRC_MEM, 0x0004)
#define GCCMW_CPRC_MEM_CBCR		GCCMW_CC_REG(GCCMW_CPRC_MEM, 0x0010)

/* CTCSR */
#define GCCMW_CTCSR			6
#define GCCMW_CTCSR_BCR			GCCMW_CC_REG(GCCMW_CTCSR, 0x0000)
#define GCCMW_CTCSR_AHB_CBCR		GCCMW_CC_REG(GCCMW_CTCSR, 0x0004)

/* EMAC */
#define GCCMW_EMAC0				7
#define GCCMW_EMAC1				8
#define GCCMW_EMAC_BCR(blk)			GCCMW_CC_REG(blk, 0x0000)
#define GCCMW_EMAC_125M_CMD_RCGR(blk)		GCCMW_CC_REG(blk, 0x0004)
#define GCCMW_EMAC_AHB_CMD_RCGR(blk)		GCCMW_CC_REG(blk, 0x000C)
#define GCCMW_EMAC_SYS_25M_CMD_RCGR(blk)	GCCMW_CC_REG(blk, 0x0014)
#define GCCMW_EMAC_TX_CMD_RCGR(blk)		GCCMW_CC_REG(blk, 0x001C) /* MND */
#define GCCMW_EMAC_TX_MND_MASK			0xFFFF

#define GCCMW_EMAC_AHB_CBCR(blk)		GCCMW_CC_REG(blk, 0x0030)
#define GCCMW_EMAC_QSMMUV3_CLIENT_CBCR(blk)	GCCMW_CC_REG(blk, 0x0034)
#define GCCMW_EMAC_IOMMU_CFG_AHB_CBCR(blk)	GCCMW_CC_REG(blk, 0x0038)
#define GCCMW_EMAC_SYS_25M_CBCR(blk)		GCCMW_CC_REG(blk, 0x003C)
#define GCCMW_EMAC_GMII_TX_CBCR(blk)		GCCMW_CC_REG(blk, 0x0040)
#define GCCMW_EMAC_SFVBU_DVM_CBCR(blk)		GCCMW_CC_REG(blk, 0x0044)
#define GCCMW_EMAC_125M_CBCR(blk)		GCCMW_CC_REG(blk, 0x0048)
#define GCCMW_EMAC_AXI_CBCR(blk)		GCCMW_CC_REG(blk, 0x004C)
#define GCCMW_EMAC_RESETR(blk)			GCCMW_CC_REG(blk, 0x0070)
#define GCCMW_EMAC_RX_MUXR(blk)			GCCMW_CC_REG(blk, 0x0080)
#define GCCMW_EMAC_RX_CBCR(blk)			GCCMW_CC_REG(blk, 0x0084)
#define GCCMW_EMAC_RAM_SYS_CBCR(blk)		GCCMW_CC_REG(blk, 0x0088)

/* FAN_TACH */
#define GCCMW_FAN_TACH			9
#define GCCMW_FAN_TACH_BCR		GCCMW_CC_REG(GCCMW_FAN_TACH, 0x0000)
#define GCCMW_FAN_TACH_CMD_RCGR		GCCMW_CC_REG(GCCMW_FAN_TACH, 0x0004)
#define GCCMW_FAN_TACH_AHB_CBCR		GCCMW_CC_REG(GCCMW_FAN_TACH, 0x000C)
#define GCCMW_FAN_TACH_PWM_CBCR		GCCMW_CC_REG(GCCMW_FAN_TACH, 0x0010)
#define GCCMW_FAN_TACH_SLEEP_CBCR	GCCMW_CC_REG(GCCMW_FAN_TACH, 0x0014)

/* SYS */
#define GCCMW_SYS			12
#define GCCMW_SLEEP_CMD_RCGR		GCCMW_CC_REG(GCCMW_SYS, 0x0000)
#define GCCMW_XO_CMD_RCGR		GCCMW_CC_REG(GCCMW_SYS, 0x0008)
#define GCCMW_IM_SLEEP_CBCR		GCCMW_CC_REG(GCCMW_SYS, 0x0010)
#define GCCMW_XO_CBCR			GCCMW_CC_REG(GCCMW_SYS, 0x0014)
#define GCCMW_XO_DIV4_CBCR		GCCMW_CC_REG(GCCMW_SYS, 0x0018)
#define GCCMW_SGMII_PHY_RESETR		GCCMW_CC_REG(GCCMW_SYS, 0x0300)
#define GCCMW_NORTH_XO_REF_CLK_MISC	GCCMW_CC_REG(GCCMW_SYS, 0x0304)
#define GCCMW_SOUTH_XO_REF_CLK_MISC	GCCMW_CC_REG(GCCMW_SYS, 0x0308)
#define GCCMW_PARB_PRE_DIV_CDIVR	GCCMW_CC_REG(GCCMW_SYS, 0x0700)
#define GCCMW_TIC_CBCR			GCCMW_CC_REG(GCCMW_SYS, 0x0704)

/* MISC */
#define GCCMW_MISC1			13
#define GCCMW_SW_SRST			GCCMW_CC_REG(GCCMW_MISC1, 0x0400)
#define GCCMW_WDOG_DEBUG		GCCMW_CC_REG(GCCMW_MISC1, 0x0404)
#define GCCMW_FLUSH_ETR_DEBUG_TIMER	GCCMW_CC_REG(GCCMW_MISC1, 0x0408)
#define GCCMW_STOP_CAPTURE_DEBUG_TIMER	GCCMW_CC_REG(GCCMW_MISC1, 0x040C)
#define GCCMW_RESET_STATUS		GCCMW_CC_REG(GCCMW_MISC1, 0x0410)
#define GCCMW_PROC_HALT			GCCMW_CC_REG(GCCMW_MISC1, 0x0414)
#define GCCMW_ACC_MISC			GCCMW_CC_REG(GCCMW_MISC1, 0x0418)
#define GCCMW_FIRST_PASS_TIMEOUT	GCCMW_CC_REG(GCCMW_MISC1, 0x0420)
#define GCCMW_RESET_FSM_CTRL		GCCMW_CC_REG(GCCMW_MISC1, 0x0424)
#define GCCMW_MDDR_PWROK		GCCMW_CC_REG(GCCMW_MISC1, 0x0600)

/* GP */
#define GCCMW_GP0			14
#define GCCMW_GP1			15
#define GCCMW_GP2			16
#define GCCMW_GP_CMD_RCGR(blk)		GCCMW_CC_REG(blk, 0x0000) /* MND */
#define GCCMW_GP_MND_MASK		0xFF
#define GCCMW_GP_CBCR(blk)		GCCMW_CC_REG(blk, 0x0014)

/* HDMA */
#define GCCMW_HDMA			17
#define GCCMW_HDMA_BCR			GCCMW_CC_REG(GCCMW_HDMA, 0x0000)
#define GCCMW_HDMA_AHB_CMD_RCGR		GCCMW_CC_REG(GCCMW_HDMA, 0x0004)
#define GCCMW_HDMA_SFPB_CMD_RCGR	GCCMW_CC_REG(GCCMW_HDMA, 0x000C)
#define GCCMW_HDMA_SYS_CMD_RCGR		GCCMW_CC_REG(GCCMW_HDMA, 0x0014)
#define GCCMW_HDMA_VBU_CMD_RCGR		GCCMW_CC_REG(GCCMW_HDMA, 0x001C)
#define GCCMW_HDMA_SFPB_ALWAYS_ON_CBCR	GCCMW_CC_REG(GCCMW_HDMA, 0x0038)
#define GCCMW_HDMA_SFPB_CBCR		GCCMW_CC_REG(GCCMW_HDMA, 0x003C)
#define GCCMW_HDMA_SFPB_AHB_CBCR	GCCMW_CC_REG(GCCMW_HDMA, 0x0040)
#define GCCMW_HDMA_VBU_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_HDMA, 0x0044)
#define GCCMW_HDMA_VBU_CORE_CBCR	GCCMW_CC_REG(GCCMW_HDMA, 0x0048)

#define GCCMW_HDMA0				18
#define GCCMW_HDMA1				19
#define GCCMW_HDMA2				20
#define GCCMW_HDMA3				21
#define GCCMW_HDMA_HDMA_BCR(blk)		GCCMW_CC_REG(blk, 0x0000)
#define GCCMW_HDMA_AHB_CBCR(blk)		GCCMW_CC_REG(blk, 0x0004)
#define GCCMW_HDMA_SYS_CBCR(blk)		GCCMW_CC_REG(blk, 0x0008)
#define GCCMW_HDMA_RESETR(blk)			GCCMW_CC_REG(blk, 0x000C)
#define GCCMW_HDMA_QSMMUV3_CLIENT_CBCR(blk)	GCCMW_CC_REG(blk, 0x0010)

#define GCCMW_HDMA_VBU			22
#define GCCMW_HDMA_VBU_BCR		GCCMW_CC_REG(GCCMW_HDMA_VBU, 0x0000)

/* HMSS_BUS */
#define GCCMW_HMSS_BUS			23
#define GCCMW_HMSS_BUS_CMD_RCGR		GCCMW_CC_REG(GCCMW_HMSS_BUS, 0x0000)
#define GCCMW_HMSS_W_AHB_CBCR		GCCMW_CC_REG(GCCMW_HMSS_BUS, 0x0018)
#define GCCMW_HMSS_AXI_CBCR		GCCMW_CC_REG(GCCMW_HMSS_BUS, 0x0020)
#define GCCMW_HMSS_NW_AHB_CBCR		GCCMW_CC_REG(GCCMW_HMSS_BUS, 0x002C)
#define GCCMW_HMSS_SW_AHB_CBCR		GCCMW_CC_REG(GCCMW_HMSS_BUS, 0x0030)

/* IMC */
#define GCCMW_IMC			25
#define GCCMW_IMC_BCR			GCCMW_CC_REG(GCCMW_IMC, 0x0000)
#define GCCMW_IMC_IOMMU_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_IMC, 0x0004)
#define GCCMW_SFAB_IMCFAB_AXI_CBCR	GCCMW_CC_REG(GCCMW_IMC, 0x0008)

/* IMEM */
#define GCCMW_IMEM			26
#define GCCMW_IMEM_BCR			GCCMW_CC_REG(GCCMW_IMEM, 0x0000)
#define GCCMW_IMEM_CFG_AHB_CBCR		GCCMW_CC_REG(GCCMW_IMEM, 0x0004)
#define GCCMW_IMEM_SLEEP_CBCR		GCCMW_CC_REG(GCCMW_IMEM, 0x0008)
#define GCCMW_IMEM_AXI_CBCR		GCCMW_CC_REG(GCCMW_IMEM, 0x000C)

/* MPM */
#define GCCMW_MPM			27
#define GCCMW_MPM_BCR			GCCMW_CC_REG(GCCMW_MPM, 0x0000)
#define GCCMW_MPM_AHB_CBCR		GCCMW_CC_REG(GCCMW_MPM, 0x0004)

/* MSG_RAM */
#define GCCMW_MSG_RAM			28
#define GCCMW_MSG_RAM_BCR		GCCMW_CC_REG(GCCMW_MSG_RAM, 0x0000)
#define GCCMW_MSG_RAM_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_MSG_RAM, 0x0004)
#define GCCMW_MSG_RAM_AXI_CBCR		GCCMW_CC_REG(GCCMW_MSG_RAM, 0x0008)

/* PARB */
#define GCCMW_PARB0			29
#define GCCMW_PARB1			30
#define GCCMW_PARB_BCR(blk)		GCCMW_CC_REG(blk, 0x0000)
#define GCCMW_PARB_SER_CMD_RCGR(blk)	GCCMW_CC_REG(blk, 0x0004)
#define GCCMW_PARB_CFG_AHB_CBCR(blk)	GCCMW_CC_REG(blk, 0x000C)
#define GCCMW_PARB_SER_CBCR(blk)	GCCMW_CC_REG(blk, 0x0010)
#define GCCMW_PARB2			31
#define GCCMW_PARB2_BCR			GCCMW_CC_REG(GCCMW_PARB2, 0x0000)
#define GCCMW_PARB2_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_PARB2, 0x0004)
#define GCCMW_PARB2_SER_CBCR		GCCMW_CC_REG(GCCMW_PARB2, 0x0008)
#define GCCMW_PARB2_CFAB_FCLK_CBCR	GCCMW_CC_REG(GCCMW_PARB2, 0x000C)
#define GCCMW_PARB2_AHB_CBCR		GCCMW_CC_REG(GCCMW_PARB2, 0x0010)
#define GCCMW_PARB2_SFAB_PFAB_AXI_CBCR	GCCMW_CC_REG(GCCMW_PARB2, 0x0014)
#define GCCMW_PARB2_SER_CMD_RCGR	GCCMW_CC_REG(GCCMW_PARB2, 0x0018)

/* QDSS */
#define GCCMW_QDSS			32
#define GCCMW_QDSS_BCR			GCCMW_CC_REG(GCCMW_QDSS, 0x0000)
#define GCCMW_QDSS_ATB_CMD_RCGR		GCCMW_CC_REG(GCCMW_QDSS, 0x0004)
#define GCCMW_QDSS_TRACE_CMD_RCGR	GCCMW_CC_REG(GCCMW_QDSS, 0x000C)
#define GCCMW_QDSS_CFG_AHB_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0014)
#define GCCMW_QDSS_DAP_AHB_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0018)
#define GCCMW_QDSS_DAP_AXI_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x001C)
#define GCCMW_QDSS_ETR_AXI_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0020)
#define GCCMW_QDSS_RBCPR_XPU_AHB_CBCR	GCCMW_CC_REG(GCCMW_QDSS, 0x0024)
#define GCCMW_QDSS_STM_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0028)
#define GCCMW_QDSS_TRACECLKIN_CBCR	GCCMW_CC_REG(GCCMW_QDSS, 0x002C)
#define GCCMW_AT_W1_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0030)
#define GCCMW_AT_W2_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0034)
#define GCCMW_AT_W3_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0038)
#define GCCMW_AT_W5_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x003C)
#define GCCMW_AT_W7_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0040)
#define GCCMW_AT_W9_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0044)
#define GCCMW_HMSS_AT_W_CBCR		GCCMW_CC_REG(GCCMW_QDSS, 0x0048)

/* SATA */
#define GCCMW_SATA0			33
#define GCCMW_SATA0_BCR			GCCMW_CC_REG(GCCMW_SATA0, 0x0000)
#define GCCMW_SATA0_AHB_CMD_RCGR	GCCMW_CC_REG(GCCMW_SATA0, 0x0004)
#define GCCMW_SATA0_LN_ASIC_CMD_RCGR	GCCMW_CC_REG(GCCMW_SATA0, 0x000C)
#define GCCMW_SATA0_LN_RX_CMD_RCGR	GCCMW_CC_REG(GCCMW_SATA0, 0x0014)
#define GCCMW_SATA0_LN_RXOOB_CBCR	GCCMW_CC_REG(GCCMW_SATA0, 0x001C)
#define GCCMW_SATA0_LN_AHB_CBCR		GCCMW_CC_REG(GCCMW_SATA0, 0x003C)
#define GCCMW_SATA0_LN_ASIC_CBCR	GCCMW_CC_REG(GCCMW_SATA0, 0x0040)
#define GCCMW_SATA0_LN_PMALIVE_CBCR	GCCMW_CC_REG(GCCMW_SATA0, 0x0048)
#define GCCMW_SATA0_LN_RX_CBCR		GCCMW_CC_REG(GCCMW_SATA0, 0x004C)
#define GCCMW_SATA_PHY0_RXOOB_CBCR	GCCMW_CC_REG(GCCMW_SATA0, 0x0050)
#define GCCMW_SATA0_LN_AXI_CBCR		GCCMW_CC_REG(GCCMW_SATA0, 0x0054)
#define GCCMW_SATA0_RESETR		GCCMW_CC_REG(GCCMW_SATA0, 0x0058)
#define GCCMW_SATA0_LN_QSMMUV3_CLIENT_CBCR	\
	GCCMW_CC_REG(GCCMW_SATA0, 0x005C)
#define GCCMW_SATA4			37
#define GCCMW_SATA4_BCR			GCCMW_CC_REG(GCCMW_SATA4, 0x0000)
#define GCCMW_SATA4_LN_ASIC_CMD_RCGR	GCCMW_CC_REG(GCCMW_SATA4, 0x0004)
#define GCCMW_SATA4_LN_RX_CMD_RCGR	GCCMW_CC_REG(GCCMW_SATA4, 0x000C)
#define GCCMW_SATA4_LN_AHB_CBCR		GCCMW_CC_REG(GCCMW_SATA4, 0x0014)
#define GCCMW_SATA4_LN_ASIC_CBCR	GCCMW_CC_REG(GCCMW_SATA4, 0x0018)
#define GCCMW_SATA4_LN_PMALIVE_CBCR	GCCMW_CC_REG(GCCMW_SATA4, 0x0020)
#define GCCMW_SATA4_LN_RX_CBCR		GCCMW_CC_REG(GCCMW_SATA4, 0x0024)
#define GCCMW_SATA_PHY1_RXOOB_CBCR	GCCMW_CC_REG(GCCMW_SATA4, 0x0028)
#define GCCMW_SATA4_LN_AXI_CBCR		GCCMW_CC_REG(GCCMW_SATA4, 0x002C)
#define GCCMW_SATA4_RESETR		GCCMW_CC_REG(GCCMW_SATA4, 0x0030)
#define GCCMW_SATA4_LN_QSMMUV3_CLIENT_CBCR	\
	GCCMW_CC_REG(GCCMW_SATA4, 0x0034)
#define GCCMW_SATA4_LN_RXOOB_CBCR	GCCMW_CC_REG(GCCMW_SATA4, 0x0038)
#define GCCMW_SATA1				34
#define GCCMW_SATA2				35
#define GCCMW_SATA3				36
#define GCCMW_SATA5				38
#define GCCMW_SATA6				39
#define GCCMW_SATA7				40
#define GCCMW_SATA_BCR(blk)			GCCMW_CC_REG(blk, 0x0000)
#define GCCMW_SATA_LN_ASIC_CMD_RCGR(blk)	GCCMW_CC_REG(blk, 0x0004)
#define GCCMW_SATA_LN_RX_CMD_RCGR(blk)		GCCMW_CC_REG(blk, 0x000C)
#define GCCMW_SATA_LN_AHB_CBCR(blk)		GCCMW_CC_REG(blk, 0x0014)
#define GCCMW_SATA_LN_ASIC_CBCR(blk)		GCCMW_CC_REG(blk, 0x0018)
#define GCCMW_SATA_LN_PMALIVE_CBCR(blk)		GCCMW_CC_REG(blk, 0x0020)
#define GCCMW_SATA_LN_RX_CBCR(blk)		GCCMW_CC_REG(blk, 0x0024)
#define GCCMW_SATA_LN_AXI_CBCR(blk)		GCCMW_CC_REG(blk, 0x0028)
#define GCCMW_SATA_RESETR(blk)			GCCMW_CC_REG(blk, 0x002C)
#define GCCMW_SATA_LN_QSMMUV3_CLIENT_CBCR(blk)	GCCMW_CC_REG(blk, 0x0030)
#define GCCMW_SATA_LN_RXOOB_CBCR(blk)		GCCMW_CC_REG(blk, 0x0034)
#define GCCMW_SATA				41
#define GCCMW_SATA_SS_BCR			GCCMW_CC_REG(GCCMW_SATA, 0x0000)
#define GCCMW_SATA_VBU_CMD_RCGR			GCCMW_CC_REG(GCCMW_SATA, 0x0004)
#define GCCMW_SATA_PHY_CFG_AHB_CBCR		GCCMW_CC_REG(GCCMW_SATA, 0x0024)
#define GCCMW_SATA_VBU_AHB_CFG_CBCR		GCCMW_CC_REG(GCCMW_SATA, 0x0028)
#define GCCMW_SATA_VBU_CORE_CBCR		GCCMW_CC_REG(GCCMW_SATA, 0x002C)
#define GCCMW_SATA_SS_SFPB_CBCR			GCCMW_CC_REG(GCCMW_SATA, 0x0030)
#define GCCMW_SATA_PMALIVE_CMD_RCGR		GCCMW_CC_REG(GCCMW_SATA, 0x0100)
#define GCCMW_SATA_Q22_CMD_RCGR			GCCMW_CC_REG(GCCMW_SATA, 0x0200)
#define GCCMW_SATA_RXOOB_CMD_RCGR		GCCMW_CC_REG(GCCMW_SATA, 0x0300)
#define GCCMW_SATA_SS_CSFPB_CMD_RCGR		GCCMW_CC_REG(GCCMW_SATA, 0x0400)

/* SATAPHY */
#define GCCMW_SATAPHY0				42
#define GCCMW_SATAPHY1				43
#define GCCMW_SATAPHY_AUX_CBCR(blk)		GCCMW_CC_REG(blk, 0x0040)
#define GCCMW_SATAPHY_CFG_AHB_CBCR(blk)		GCCMW_CC_REG(blk, 0x0044)
#define GCCMW_SATAPHY_RESETR(blk)		GCCMW_CC_REG(blk, 0x0048)

/* SEC_CTRL */
#define GCCMW_SEC			44
#define GCCMW_SEC_CTRL_BCR		GCCMW_CC_REG(GCCMW_SEC, 0x0000)
#define GCCMW_ACC_CMD_RCGR		GCCMW_CC_REG(GCCMW_SEC, 0x0004)
#define GCCMW_SEC_CTRL_CMD_RCGR		GCCMW_CC_REG(GCCMW_SEC, 0x000C)
#define GCCMW_SEC_CTRL_AHB_CBCR		GCCMW_CC_REG(GCCMW_SEC, 0x0014)
#define GCCMW_SEC_CTRL_SENSE_CBCR	GCCMW_CC_REG(GCCMW_SEC, 0x001C)
#define GCCMW_SEC_CTRL_ACC_CBCR		GCCMW_CC_REG(GCCMW_SEC, 0x0020)
#define GCCMW_SEC_CTRL_CBCR		GCCMW_CC_REG(GCCMW_SEC, 0x0024)

/* SYSTEM_FAB */
#define GCCMW_SFAB			45
#define GCCMW_SFAB_BCR			GCCMW_CC_REG(GCCMW_SFAB, 0x0000)
#define GCCMW_SFAB_CMD_RCGR		GCCMW_CC_REG(GCCMW_SFAB, 0x0004)
#define GCCMW_SFAB_AHBMSM_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x000C)
#define GCCMW_QDSS_FCLK_CBCR		GCCMW_CC_REG(GCCMW_SFAB, 0x0010)
#define GCCMW_SFAB_CE0_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0014)
#define GCCMW_SFAB_CE1_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0018)
#define GCCMW_SFAB_CFAB_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x001C)
#define GCCMW_SFAB_CFG_AHB_CBCR		GCCMW_CC_REG(GCCMW_SFAB, 0x0020)
#define GCCMW_SFAB_CORE_CBCR		GCCMW_CC_REG(GCCMW_SFAB, 0x0024)
#define GCCMW_SFAB_EMAC0_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0028)
#define GCCMW_SFAB_EMAC1_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x002C)
#define GCCMW_SFAB_HMSS_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0030)
#define GCCMW_SFAB_IMEM_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0034)
#define GCCMW_SFAB_PFAB_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0038)
#define GCCMW_SFAB_QDSS_STM_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x003C)
#define GCCMW_SFAB_VBU_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0040)
#define GCCMW_SFAB_VBU_Q22M_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0044)
#define GCCMW_SFAB_VBU_Q22S_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0048)
#define GCCMW_TIC_AHB2AHB_TIC_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x004C)
#define GCCMW_SFAB_IMCFAB_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0050)
#define GCCMW_SFAB_MSG_RAM_FCLK_CBCR	GCCMW_CC_REG(GCCMW_SFAB, 0x0054)

/* SFAB_VBU */
#define GCCMW_SFAB_VBU			46
#define GCCMW_SFAB_VBU_BCR		GCCMW_CC_REG(GCCMW_SFAB_VBU, 0x0000)
#define GCCMW_SFAB_VBU_CMD_RCGR		GCCMW_CC_REG(GCCMW_SFAB_VBU, 0x0004)
#define GCCMW_SFAB_VBU_CFG_CBCR		GCCMW_CC_REG(GCCMW_SFAB_VBU, 0x000C)
#define GCCMW_SFAB_VBU_CORE_CBCR	GCCMW_CC_REG(GCCMW_SFAB_VBU, 0x0010)
#define GCCMW_SFAB_VBU_PERIPH_DVM_CBCR	GCCMW_CC_REG(GCCMW_SFAB_VBU, 0x0014)

/* TLMM */
#define GCCMW_TLMM			48
#define GCCMW_TLMM_BCR			GCCMW_CC_REG(GCCMW_TLMM, 0x0000)
#define GCCMW_TLMM_AHB_CBCR		GCCMW_CC_REG(GCCMW_TLMM, 0x0004)
#define GCCMW_TLMM_CBCR			GCCMW_CC_REG(GCCMW_TLMM, 0x0008)

/* WCSFPB (W_CONF_SFPB) */
#define GCCMW_W_CSFPB			49
#define GCCMW_W_CSFPB_BCR		GCCMW_CC_REG(GCCMW_W_CSFPB, 0x0000)
#define GCCMW_W_CSFPB_ALWAYS_ON_CBCR	GCCMW_CC_REG(GCCMW_W_CSFPB, 0x000C)
#define GCCMW_W_CSFPB_CBCR		GCCMW_CC_REG(GCCMW_W_CSFPB, 0x0010)
#define GCCMW_W_CSFPB_XPU_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_W_CSFPB, 0x0014)
#define GCCMW_W_CSFPB_AHB_CBCR		GCCMW_CC_REG(GCCMW_W_CSFPB, 0x0018)
#define GCCMW_EMAC_AHB2PHY_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_W_CSFPB, 0x001C)
#define GCCMW_W_CSFPB_CMD_RCGR		GCCMW_CC_REG(GCCMW_W_CSFPB, 0x0020)

/* IMC */
#define GCCMW_IMC2			50
#define GCCMW_IMC_WARM_RESET_STATUS	GCCMW_CC_REG(GCCMW_IMC2, 0x0104)

/* MPM */
#define GCCMW_MPM2			52
#define GCCMW_MPM_MISC			GCCMW_CC_REG(GCCMW_MPM2, 0x0008)
#define GCCMW_MPM_RESETR		GCCMW_CC_REG(GCCMW_MPM2, 0x000C)

/* TSENS */
#define GCCMW_TSENS1			53
#define GCCMW_TSENS2			54
#define GCCMW_TSENS3			61
#define GCCMW_TSENS4			62
#define GCCMW_TSENS9			63
#define GCCMW_TSENS10			64
#define GCCMW_TSENS_BCR(blk)		GCCMW_CC_REG(blk, 0x0000)
#define GCCMW_TSENS_AHB_CBCR(blk)	GCCMW_CC_REG(blk, 0x0004)
#define GCCMW_TSENS_EXT_CBCR(blk)	GCCMW_CC_REG(blk, 0x0008)
#define GCCMW_TSENS_SLEEP_CBCR(blk)	GCCMW_CC_REG(blk, 0x000C)

/* CPRC_VDDA */
#define GCCMW_CPRC_NVDDA		65
#define GCCMW_CPRC_SVDDA		66
#define GCCMW_CPRC_VDDA_BCR(blk)	GCCMW_CC_REG(blk, 0x0000)
#define GCCMW_CPRC_VDDA_AHB_CBCR(blk)	GCCMW_CC_REG(blk, 0x0004)
#define GCCMW_CPRC_VDDA_CBCR(blk)	GCCMW_CC_REG(blk, 0x0008)
#define GCCMW_CPRC_VDDA_CMD_RCGR(blk)	GCCMW_CC_REG(blk, 0x000C)

/* NCSFPB (N_CONF_SFPB) */
#define GCCMW_N_CSFPB			67
#define GCCMW_N1_CSFPB_BCR		GCCMW_CC_REG(GCCMW_N_CSFPB, 0x0000)
#define GCCMW_N1_CSFPB_CFAB_FCLK_CBCR	GCCMW_CC_REG(GCCMW_N_CSFPB, 0x0004)
#define GCCMW_N1_CSFPB_AHB_CBCR		GCCMW_CC_REG(GCCMW_N_CSFPB, 0x0008)
#define GCCMW_N1_CSFPB_ALWAYS_ON_CBCR	GCCMW_CC_REG(GCCMW_N_CSFPB, 0x000C)
#define GCCMW_N1_CSFPB_CBCR		GCCMW_CC_REG(GCCMW_N_CSFPB, 0x0010)
#define GCCMW_N2_CSFPB_AHB_CBCR		GCCMW_CC_REG(GCCMW_N_CSFPB, 0x0014)
#define GCCMW_N_CSFPB_XPU_CFG_AHB_CBCR	GCCMW_CC_REG(GCCMW_N_CSFPB, 0x0018)
#define GCCMW_N1_CSFPB_CMD_RCGR		GCCMW_CC_REG(GCCMW_N_CSFPB, 0x001C)

/* SCSFPB (S_CONF_SFPB) */
#define GCCMW_S_CSFPB			68
#define GCCMW_S1_CSFPB_BCR		GCCMW_CC_REG(GCCMW_S_CSFPB, 0x0000)
#define GCCMW_S1_CSFPB_AHB_CBCR		GCCMW_CC_REG(GCCMW_S_CSFPB, 0x0004)
#define GCCMW_S1_CSFPB_ALWAYS_ON_CBCR	GCCMW_CC_REG(GCCMW_S_CSFPB, 0x0008)
#define GCCMW_S1_CSFPB_CBCR		GCCMW_CC_REG(GCCMW_S_CSFPB, 0x000C)
#define GCCMW_S2_CSFPB_AHB_CBCR		GCCMW_CC_REG(GCCMW_S_CSFPB, 0x0010)
#define GCCMW_S1_CSFPB_CMD_RCGR		GCCMW_CC_REG(GCCMW_S_CSFPB, 0x0014)

/* DEBUG */
#define GCCMW_DEBUG			77
#define GCCMW_DEBUG_CDIVR		GCCMW_CC_REG(GCCMW_DEBUG, 0x0100)
#define GCCMW_DEBUG_CLK_CTL		GCCMW_CC_REG(GCCMW_DEBUG, 0x0200)
#define GCCMW_PLLTEST_PAD_CFG		GCCMW_CC_REG(GCCMW_DEBUG, 0x0208)
#define GCCMW_CLOCK_FRQ_MEASURE_CTL	GCCMW_CC_REG(GCCMW_DEBUG, 0x020C)
#define GCCMW_CLOCK_FRQ_MEASURE_STATUS	GCCMW_CC_REG(GCCMW_DEBUG, 0x0210)
#define GCCMW_PLL_LOCK_DET_STATUS0	GCCMW_CC_REG(GCCMW_DEBUG, 0x0214)
#define GCCMW_PLL_LOCK_DET_EN_MASK0	GCCMW_CC_REG(GCCMW_DEBUG, 0x0218)
#define GCCMW_PLL_LOCK_DET_STATUS1	GCCMW_CC_REG(GCCMW_DEBUG, 0x0220)
#define GCCMW_PLL_LOCK_DET_EN_MASK1	GCCMW_CC_REG(GCCMW_DEBUG, 0x0224)
#define GCCMW_TEST_BUS_CTL		GCCMW_CC_REG(GCCMW_DEBUG, 0x0230)

/* MDDR */
#define GCCMW_N_MDDR			78
#define GCCMW_S_MDDR			79
#define GCCMW_MDDR2_AHB_CBCR		GCCMW_CC_REG(GCCMW_N_MDDR, 0x0000)
#define GCCMW_MDDR3_AHB_CBCR		GCCMW_CC_REG(GCCMW_N_MDDR, 0x0004)
#define GCCMW_N_GCCMS_AHB_CBCR		GCCMW_CC_REG(GCCMW_N_MDDR, 0x0008)
#define GCCMW_MDDR0_AHB_CBCR		GCCMW_CC_REG(GCCMW_S_MDDR, 0x0000)
#define GCCMW_MDDR1_AHB_CBCR		GCCMW_CC_REG(GCCMW_S_MDDR, 0x0004)
#define GCCMW_S_GCCMS_AHB_CBCR		GCCMW_CC_REG(GCCMW_S_MDDR, 0x0008)

/* RESTART */
#define GCCMW_RESTART			93
#define GCCMW_IMC_CC_RESTART		GCCMW_CC_REG(GCCMW_RESTART, 0x0100)
#define GCCMW_PCC_RESTART		GCCMW_CC_REG(GCCMW_RESTART, 0x0104)
#define GCCMW_GCCMS0_RESTART		GCCMW_CC_REG(GCCMW_RESTART, 0x0108)
#define GCCMW_GCCMS1_RESTART		GCCMW_CC_REG(GCCMW_RESTART, 0x010C)
#define GCCMW_GCCE_RESTART		GCCMW_CC_REG(GCCMW_RESTART, 0x0110)
#define GCCMW_PCIE_CC_0_RESTART		GCCMW_CC_REG(GCCMW_RESTART, 0x0114)
#define GCCMW_PCIE_CC_1_RESTART		GCCMW_CC_REG(GCCMW_RESTART, 0x0118)

/* GCCMW_WDOG_DEBUG */
#define GCCMW_PRE_ARES_DEBUG_TIMER_VAL_OFFSET		0
#define GCCMW_PRE_ARES_DEBUG_TIMER_VAL_MASK		0x7FFF
#define GCCMW_PRE_ARES_DEBUG_TIMER_VAL(value)		\
	_SET_FV(GCCMW_PRE_ARES_DEBUG_TIMER_VAL, value)
#define GCCMW_SRST_V1_MODE		_BV(15)
#define GCCMW_PROC_HALT_EN		_BV(16)
#define GCCMW_WDOG_DEBUG_EN		_BV(17)

/* GCCMW_FLUSH_ETR_DEBUG_TIMER */
#define GCCMW_FLUSH_ETR_DEBUG_TIMER_VAL_OFFSET		0
#define GCCMW_FLUSH_ETR_DEBUG_TIMER_VAL_MASK		0x3FFF
#define GCCMW_FLUSH_ETR_DEBUG_TIMER_VAL(value)		\
	_SET_FV(GCCMW_FLUSH_ETR_DEBUG_TIMER_VAL, value)
#define GCCMW_RST_DBG_WDOG_1ST_RD	_BV(14)
#define GCCMW_RST_XPU_WDOG_1ST_RD	_BV(15)

/* GCCMW_STOP_CAPTURE_DEBUG_TIMER */
#define GCCMW_STOP_CAPTURE_DEBUG_TIMER_VAL_OFFSET	0
#define GCCMW_STOP_CAPTURE_DEBUG_TIMER_VAL_MASK		0xFFFF
#define GCCMW_STOP_CAPTURE_DEBUG_TIMER_VAL(value)	\
	_SET_FV(GCCMW_STOP_CAPTURE_DEBUG_TIMER_VAL, value)

/* GCCMW_RESET_STATUS */
#define GCCMW_DEBUG_RESET_STATUS_OFFSET		0
#define GCCMW_DEBUG_RESET_STATUS_MASK		0x03
#define GCCMW_DEBUG_RESET_STATUS(value)		\
	_SET_FV(GCCMW_DEBUG_RESET_STATUS, value)
#define GCCMW_DEBUG_NORMAL_RESET		0x0
#define GCCMW_DEBUG_PERFORM_WDOG_2ND_PASS	0x2
#define GCCMW_DEBUG_WDOG_EXPIRED		0x3

#define __gccmw_set_debug_reset_status(status)				\
	__raw_writel_mask(GCCMW_DEBUG_RESET_STATUS(status),		\
		GCCMW_DEBUG_RESET_STATUS(GCCMW_DEBUG_RESET_STATUS_MASK),\
		GCCMW_RESET_STATUS)

#endif /* __GCCMW_QDF2400_H_INCLUDE__ */
