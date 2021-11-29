/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)dw_xpcs.h: Synopsys DesignWare ethernet PCS interface
 * $Id: dw_xpcs.h,v 1.0 2020-12-17 17:20:00 zhenglv Exp $
 */

#ifndef __DW_XPCS_H_INCLUDE__
#define __DW_XPCS_H_INCLUDE__

#ifdef CONFIG_DW_XPCS_XGXS_DTE
#define XS_PCS_MMD			XS_DTE_MMD
#define XS_PMA_MMD			XS_DTE_MMD
#endif
#ifdef CONFIG_DW_XPCS_XGXS_PHY
#define XS_PCS_MMD			XS_PHY_MMD
#define XS_PMA_MMD			XS_PHY_MMD
#endif
#ifdef CONFIG_DW_XPCS_PCS
#define XS_PCS_MMD			PCS_MMD
#define XS_PMA_MMD			PMA_MMD
#endif
#define VS_MII_1_MMD			0x1A
#define VS_MII_2_MMD			0x1B
#define VS_MII_3_MMD			0x1C
#define VS_MMD1				0x1E
#define VS_MII_MMD			0x1F
#define SR(offset)			(0x0000 + (offset))
#define VR(offset)			(0x8000 + (offset))

#define DWCXS_REG(offset)		(DW_XPCS_BASE + (offset))

#ifdef CONFIG_DW_XPCS_PMA_PMD_MMD
/* PMA MMD Standard (SR) and Vendor Specific (VR) Register (except SERDES
 * Registers) address block
 */
#define SR_PMA_CTRL1			SR(0x0000)
#define SR_PMA_STATUS1			SR(0x0001)
#define SR_PMA_DEV_ID_1			SR(0x0002)
#define SR_PMA_DEV_ID_2			SR(0x0003)
#define SR_PMA_SPD_ABL			SR(0x0004)
#define SR_PMA_DEV_PKG1			SR(0x0005)
#define SR_PMA_DEV_PKG2			SR(0x0006)
#define SR_PMA_CTRL2			SR(0x0007)
#define SR_PMA_STATUS2			SR(0x0008)
#define SR_PMA_TX_DIS			SR(0x0009)
#define SR_PMA_RX_SIG_DET		SR(0x000A)
#define SR_PMA_EXT_ABL			SR(0x000B)
#define SR_PMA_PKG1			SR(0x000E)
#define SR_PMA_PKG2			SR(0x000F)
#define SR_PMA_2PT5G_5G_EXT_ABL		SR(0x0015)
#define SR_PMA_KX_CTRL			SR(0x00A0)
#define SR_PMA_KX_STS			SR(0x00A1)
#define SR_PMA_TIME_SYNC_PMA_ABL	SR(0x0708)
#define SR_PMA_TIME_SYNC_TX_MAX_DLY_LWR	SR(0x0709)
#define SR_PMA_TIME_SYNC_TX_MAX_DLY_UPR	SR(0x070A)
#define SR_PMA_TIME_SYNC_TX_MIN_DLY_LWR	SR(0x070B)
#define SR_PMA_TIME_SYNC_TX_MIN_DLY_UPR	SR(0x070C)
#define SR_PMA_TIME_SYNC_RX_MAX_DLY_LWR	SR(0x070D)
#define SR_PMA_TIME_SYNC_RX_MAX_DLY_UPR	SR(0x070E)
#define SR_PMA_TIME_SYNC_RX_MIN_DLY_LWR	SR(0x070F)
#define SR_PMA_TIME_SYNC_RX_MIN_DLY_UPR	SR(0x0710)
#define VR_PMA_DIG_CTRL1		VR(0x0000)
#define VR_PMA_DIG_STS			VR(0x0010)

/* SR_PMA_CTRL1 */
#define SR_PMA_RST			_BV(15)
#define SR_PMA_SS13			_BV(13)
#define SR_PMA_LPM			_BV(11)
#define SR_PMA_SS6			_BV(6)
#define SR_PMA_SS52_OFFSET		2
#define SR_PMA_SS52_MASK		REG_4BIT_MASK
#define SR_PMA_SS52(value)		_SET_FV(SR_PMA_SS52, value)
#define SR_PMA_LB			_BV(0)
#define SR_PMA_SS_1G			SR_PMA_SS6
#define SR_PMA_SS_10G			\
	(SR_PMA_SS13 | SR_PMA_SS6 | SR_PMA_SS52(0))

#define dw_xpcs_enable_loopback()		\
	dw_xpcs_set(PMA_MMD, SR_PMA_CTRL1, SR_PMA_LB)
#define dw_xpcs_disable_loopback()		\
	dw_xpcs_clear(PMA_MMD, SR_PMA_CTRL1, SR_PMA_LB)

/* SR_PMA_CTRL2 */
#define SR_PMA_TYPE_OFFSET		0
#define SR_PMA_TYPE_MASK		REG_6BIT_MASK
#define SR_PMA_TYPE(value)		_SET_FV(SR_PMA_TYPE, value)
#define sr_pma_type(value)		_GET_FV(SR_PMA_TYPE, value)
#define SR_PMA_10000BASE_CX4		0x00
#define SR_PMA_10000BASE_EW		0x01
#define SR_PMA_10000BASE_LW		0x02
#define SR_PMA_10000BASE_SW		0x03
#define SR_PMA_10000BASE_LX4		0x04
#define SR_PMA_10000BASE_ER		0x05
#define SR_PMA_10000BASE_LR		0x06
#define SR_PMA_10000BASE_SR		0x07
#define SR_PMA_10000BASE_LRM		0x08
#define SR_PMA_10000BASE_T		0x09
#define SR_PMA_10000BASE_KX4		0x0A
#define SR_PMA_10000BASE_KR		0x0B
#define SR_PMA_1000BASE_T		0x0C
#define SR_PMA_1000BASE_KX		0x0D
#define SR_PMA_100BASE_TX		0x0E
#define SR_PMA_10BASE_T			0x0F
#define SR_PMA_2500BASE_KX		0x3B
#define SR_PMA_5000BASE_KR		0x3C

/* Optional registers */
#ifdef CONFIG_DW_XPCS_KR
#define SR_PMA_KR_PMD_CTRL		SR(0x0096)
#define SR_PMA_KR_PMD_STS		SR(0x0097)
#define SR_PMA_KR_LP_CEU		SR(0x0098)
#define SR_PMA_KR_LP_CESTS		SR(0x0099)
#define SR_PMA_KR_LD_CEU		SR(0x009A)
#define SR_PMA_KR_LD_CESTS		SR(0x009B)
#define VR_PMA_KRTR_PRBS_CTRL0		VR(0x0003)
#define VR_PMA_KRTR_PRBS_CTRL1		VR(0x0004)
#define VR_PMA_KRTR_PRBS_CTRL2		VR(0x0005)
#define VR_PMA_KRTR_TIMER_CTRL0		VR(0x0006)
#define VR_PMA_KRTR_TIMER_CTRL1		VR(0x0007)
#define VR_PMA_KRTR_TIMER_CTRL2		VR(0x0008)
#define VR_PMA_KRTR_RX_EQ_CTRL		VR(0x0009)
#define VR_PMA_KRTR_TX_EQ_STS_CTRL	VR(0x000B)
#define VR_PMA_KRTR_TX_EQ_CFF_CTRL	VR(0x000C)
#define VR_PMA_KRTR_TX_EQ_STS		VR(0x000D)
#define VR_PMA_KRTR_RX_EQ_CEU		VR(0x000E)
#define VR_PMA_KRTR_RX_EQ_STS_CTRL	VR(0x000F)
#endif /* CONFIG_DW_XPCS_KR */
#ifdef CONFIG_DW_XPCS_FEC
#define SR_PMA_KR_FEC_ABL		SR(0x00AA)
#define SR_PMA_KR_FEC_CTRL		SR(0x00AB)
#define SR_PMA_KR_FEC_CORR_BLK1		SR(0x00AC)
#define SR_PMA_KR_FEC_CORR_BLK2		SR(0x00AD)
#endif /* CONFIG_DW_XPCS_FEC */
#ifdef CONFIG_DW_XPCS_GPIO
#define VR_PMA_GPIO			VR(0x0015)
#endif /* CONFIG_DW_XPCS_GPIO */
#endif

#if defined(CONFIG_DW_XPCS_PCS_MMD) || defined(CONFIG_DW_XPCS_XGXS)
/* PCS MMD/DTE XS MMD/PHY XS MMD Standard (SR) and Vendor Specifc (VR)
 * Registers address block
 */
#define SR_XS_PCS_CTRL1			SR(0x0000)
#define SR_XS_PCS_STS1			SR(0x0001)
#define SR_XS_PCS_DEV_ID1		SR(0x0002)
#define SR_XS_PCS_DEV_ID2		SR(0x0003)
#define SR_XS_PCS_SPD_ABL		SR(0x0004)
#define SR_XS_PCS_DEV_PKG1		SR(0x0005)
#define SR_XS_PCS_DEV_PKG2		SR(0x0006)
#define SR_XS_PCS_CTRL2			SR(0x0007)
#define SR_XS_PCS_STS2			SR(0x0008)
#define SR_XS_PCS_STS3			SR(0x0009)
#define SR_XS_PCS_PKG1			SR(0x000E)
#define SR_XS_PCS_PKG2			SR(0x000F)
#define SR_XS_PCS_LSTS			SR(0x0018)
#define SR_PCS_TIME_SYNC_PCS_ABL	SR(0x0708)
#define SR_PCS_TIME_SYNC_TX_MAX_DLY_LWR	SR(0x0709)
#define SR_PCS_TIME_SYNC_TX_MAX_DLY_UPR	SR(0x070A)
#define SR_PCS_TIME_SYNC_TX_MIN_DLY_LWR	SR(0x070B)
#define SR_PCS_TIME_SYNC_TX_MIN_DLY_UPR	SR(0x070C)
#define SR_PCS_TIME_SYNC_RX_MAX_DLY_LWR	SR(0x070D)
#define SR_PCS_TIME_SYNC_RX_MAX_DLY_UPR	SR(0x070E)
#define SR_PCS_TIME_SYNC_RX_MIN_DLY_LWR	SR(0x070F)
#define SR_PCS_TIME_SYNC_RX_MIN_DLY_UPR	SR(0x0710)
#define VR_XS_PCS_DIG_CTRL1		VR(0x0000)
#define VR_XS_PCS_DIG_CTRL2		VR(0x0001)
#define VR_XS_PCS_XAUI_CTRL		VR(0x0004)
#define VR_XS_PCS_DEBUG_CTRL		VR(0x0005)
#define VR_XS_PCS_KR_CTRL		VR(0x0007)
#define VR_XS_PCS_DIG_STS		VR(0x0010)

/* VR_XS_PCS_KR_CTRL */
#define USXG_MODE_OFFSET		10
#define USXG_MODE_MASK			REG_3BIT_MASK
#define USXG_MODE(value)		_GET_FV(USXG_MODE, value)
#define USXG_MODE_10G_SXGMII		0

/* VR_XS_PCS_DIG_CTRL1 */
#define VR_RST				_BV(15)
#define USXG_EN				_BV(9)

/* VR_XS_PCS_XAUI_CTRL */
#define XAUI_MODE			0
#define RXAUI_MODE			1

/* SR_XS_PCS_CTRL1 */
#define SR_PCS_RST			_BV(15)
#define SR_PCS_LBE			_BV(14)
#define SR_PCS_SS13			_BV(13)
#define SR_PCS_LPM			_BV(11)
#define SR_PCS_CS_EN			_BV(10)
#define SR_PCS_XAUI_STOP		_BV(9)
#define SR_PCS_SS6			_BV(6)
#define SR_PCS_SS52_OFFSET		2
#define SR_PCS_SS52_MASK		REG_4BIT_MASK
#define SR_PCS_SS52(value)		_GET_FV(SR_PCS_SS52, value)
#define KX_MODE_1G			0
#define KX4_MODE_10G			SR_PCS_SS13

#ifdef CONFIG_DUOWEN_ETH_10000BASE_RXAUI
#define PCS_SPEED_SELECTION		KX4_MODE_10G
#endif

#ifdef CONFIG_DUOWEN_ETH_SGMII_1000M
#define PCS_SPEED_SELECTION		KX_MODE_1G
#endif



/* SR_XS_PCS_STS1 */
#define SR_PCS_TXLPIR			_BV(11) /* Tx received LPI */
#define SR_PCS_RXLPIR			_BV(10) /* Rx received LPI */
#define SR_PCS_TXLPII			_BV(9)  /* Tx LPI indication */
#define SR_PCS_RXLPII			_BV(8)  /* Rx LPI indication */
#define SR_PCS_FLT			_BV(7)  /* Fault condition detected */
#define SR_PCS_CSC			_BV(6)  /* Clock stop capable */
#define SR_PCS_RLU			_BV(2)  /* Rx link up */
#define SR_PCS_LPMS			_BV(1)  /* Low-power mode support */

/* SR_XS_PCS_CTRL2 */
#define SR_PCS_TYPE_OFFSET		0
#define SR_PCS_TYPE_MASK		REG_4BIT_MASK
#define SR_PCS_TYPE(value)		_SET_FV(SR_PCS_TYPE, value)
#define sr_pcs_type(value)		_GET_FV(SR_PCS_TYPE, value)
#define SR_PCS_10000BASE_R		0x00
#define SR_PCS_10000BASE_X		0x01
#define SR_PCS_25000BASE_X		0x0E
#define SR_PCS_50000BASE_R		0x0F

/* VR_XS_PCS_DIG_STS */
#define VR_PCS_LTX_STATE_OFFSET		13
#define VR_PCS_LTX_STATE_MASK		REG_3BIT_MASK
#define VR_PCS_LTX_STATE(value)		_GET_FV(VR_PCS_LTX_STATE, value)
#define VR_PCS_LTX_ACTIVE		0
#define VR_PCS_LTX_SLEEP		1
#define VR_PCS_LTX_QUIET		2
#define VR_PCS_LTX_REF_WAKE		3
#define VR_PCS_LTX_ALERT		4
#define VR_PCS_LTX_SCR_BYP		5
#define VR_PCS_LRX_STATE_OFFSET		10
#define VR_PCS_LRX_STATE_MASK		REG_3BIT_MASK
#define VR_PCS_LRX_STATE(value)		_GET_FV(VR_PCS_LRX_STATE, value)
#define VR_PCS_LRX_ACTIVE		0
#define VR_PCS_LRX_SLEEP		1
#define VR_PCS_LRX_QUIET		2
#define VR_PCS_LRX_WAKE			3
#define VR_PCS_LRX_WTF			4
#define VR_PCS_LRX_LPI_K		5
#define VR_PCS_INV_XGM_CHAR		_BV(9)
#define VR_PCS_INV_XGM_T		_BV(8)
#define VR_PCS_INV_XGM_SOP		_BV(7)
#define VR_PCS_RXFIFO_OVF		_BV(6)
#define VR_PCS_RXFIFO_UNDF		_BV(5)
#define VR_PCS_PSEQ_STATE_OFFSET	2
#define VR_PCS_PSEQ_STATE_MASK		REG_3BIT_MASK
#define VR_PCS_PSEQ_STATE(value)	_GET_FV(VR_PCS_PSEQ_STATE, value)
#define VR_PCS_LB_ACTIVE		_BV(0)

/* Optional registers */
#ifdef CONFIG_DW_XPCS_EEE
#define SR_XS_PCS_EEE_ABL		SR(0x0014)
#define SR_XS_PCS_EEE_ABL2		SR(0x0015)
#define SR_XS_PCS_EEE_WKERR		SR(0x0016)
#define VR_XS_PCS_EEE_MCTRL0		VR(0x0006)
#define VR_XS_PCS_EEE_TXTIMER		VR(0x0008)
#define VR_XS_PCS_EEE_RXTIMER		VR(0x0009)
#define VR_XS_PCS_EEE_MCTRL1		VR(0x000B)
#endif /* CONFIG_DW_XPCS_EEE */
#ifdef CONFIG_DW_XPCS_RPCS
#define SR_XS_PCS_KR_STS1		SR(0x0020)
#define SR_XS_PCS_KR_STS2		SR(0x0021)
#define SR_XS_PCS_TP_A0			SR(0x0022)
#define SR_XS_PCS_TP_A1			SR(0x0023)
#define SR_XS_PCS_TP_A2			SR(0x0024)
#define SR_XS_PCS_TP_A3			SR(0x0025)
#define SR_XS_PCS_TP_B0			SR(0x0026)
#define SR_XS_PCS_TP_B1			SR(0x0027)
#define SR_XS_PCS_TP_B2			SR(0x0028)
#define SR_XS_PCS_TP_B3			SR(0x0029)
#define SR_XS_PCS_TP_CTRL		SR(0x002A)
#define SR_XS_PCS_TP_ERRCTR		SR(0x002B)
#else /* CONFIG_DW_XPCS_RPCS */
#define SR_XS_PCS_TCTRL			SR(0x0019)
#define VR_XS_PCS_DIG_ERRCNT_SEL	VR(0x0002)
#define VR_XS_PCS_ICG_ERRCNT1		VR(0x0011)
#endif /* CONFIG_DW_XPCS_RPCS */
#ifdef CONFIG_ARCH_HAS_DW_XPCS_RSVDPARAM_19
#define VR_XS_PCS_AM_CTRL		VR(0x000A)
#endif /* CONFIG_ARCH_HAS_DW_XPCS_RSVDPARAM_19 */
#ifndef CONFIG_DW_XPCS_SINGLE_LANE
#define VR_XS_PCS_ICG_ERRCNT2		VR(0x0012)
#define VR_XS_PCS_DSKW_ERRCTR		VR(0x0013)
#define VR_XS_PCS_TPM_ERRCTR		VR(0x0014)
#endif /* CONFIG_DW_XPCS_SINGLE_LANE */
#ifdef CONFIG_DW_XPCS_GPIO
#define VR_XS_PCS_GPIO			VR(0x0015)
#endif /* CONFIG_DW_XPCS_GPIO */
#endif

#if defined(CONFIG_DW_XPCS_PMA_PMD_MMD) || defined(CONFIG_DW_XPCS_XGXS)
/* SERDES Registers address block */
#define VR_XS_PMA_RX_LSTS		VR(0x0020)

/* UP3 */
#ifdef CONFIG_ARCH_IS_DW_XPCS_PHY_GEN1_3G
#include <driver/xpcs_phy_gen1.h>
#endif
/* UP5 !SSP */
#ifdef CONFIG_ARCH_IS_DW_XPCS_GEN2_3G
#include <driver/xpcs_phy_gen2.h>
#endif
#ifdef CONFIG_ARCH_IS_DW_XPCS_GEN2_6G
#include <driver/xpcs_phy_gen2.h>
#endif
/* UP5 SSP */
#ifdef CONFIG_ARCH_IS_DW_XPCS_GEN4_3G
#include <driver/xpcs_phy_gen4.h>
#endif
#ifdef CONFIG_ARCH_IS_DW_XPCS_GEN4_6G
#include <driver/xpcs_phy_gen4.h>
#endif
/* KR_PHY */
#ifdef CONFIG_ARCH_IS_DW_XPCS_PHY_GEN5_10G
#include <driver/xpcs_phy_gen5_10g.h>
#endif
/* E12 */
#ifdef CONFIG_ARCH_IS_DW_XPCS_PHY_GEN5_12G
#include <driver/xpcs_phy_gen5_12g.h>
#endif
/* E16 */
#ifdef CONFIG_ARCH_IS_DW_XPCS_PHY_GEN5_16G
#include <driver/xpcs_phy_gen5_16g.h>
#endif

/* RSVEDPARAM0 */
#ifdef CONFIG_ARCH_HAS_DW_XPCS_RSVDPARAM_0
#define VR_XS_PMA_Consumer_10G_RX_IQ_CTRL0	VR(0x006B)

/* Optional registers */
#ifndef CONFIG_DW_XPCS_SINGLE_LANE
#define VR_XS_PMA_Consumer_10G_RX_IQ_CTRL1	VR(0x006C)
#define VR_XS_PMA_Consumer_10G_RX_IQ_CTRL2	VR(0x006D)
#define VR_XS_PMA_Consumer_10G_RX_IQ_CTRL3	VR(0x006E)
#endif /* CONFIG_DW_XPCS_SINGLE_LANE */
#endif

/* RSVDPARAM19 */
#ifdef CONFIG_ARCH_HAS_DW_XPCS_RSVDPARAM_19
#define VR_XS_PMA_Consumer_10G_TX_TERM_CTRL	VR(0x003C)
#define VR_XS_PMA_Consumer_10G_RX_TERM_CTRL	VR(0x0064)
#endif

#ifdef CONFIG_ARCH_IS_DW_XPCS_PHY_SNPS_XAUI
#define VR_XS_PMA_SNPS_CR_CTRL			VR(0x00A0)
#define VR_XS_PMA_SNPS_CR_ADDR			VR(0x00A1)
#define VR_XS_PMA_SNPS_CR_DATA			VR(0x00A2)
#endif
#endif

#ifdef CONFIG_DW_XPCS_AN_MMD
/* Auto-negotiation (AN) MMD Standard (SR) and Vendor Specific (VR)
 * address block
 */
#define SR_AN_CTRL				SR(0x0000)
#define SR_AN_STS				SR(0x0001)
#define SR_AN_DEV_ID1				SR(0x0002)
#define SR_AN_DEV_ID2				SR(0x0003)
#define SR_AN_DEV_PKG1				SR(0x0005)
#define SR_AN_DEV_PKG2				SR(0x0006)
#define SR_AN_PKG1				SR(0x000E)
#define SR_AN_PKG2				SR(0x000F)
#define SR_AN_ADV1				SR(0x0010)
#define SR_AN_ADV2				SR(0x0011)
#define SR_AN_ADV3				SR(0x0012)
#define SR_AN_LP_ABL1				SR(0x0013)
#define SR_AN_LP_ABL2				SR(0x0014)
#define SR_AN_LP_ABL3				SR(0x0015)
#define SR_AN_XNP_TX1				SR(0x0016)
#define SR_AN_XNP_TX2				SR(0x0017)
#define SR_AN_XNP_TX3				SR(0x0018)
#define SR_AN_LP_XNP_ABL1			SR(0x0019)
#define SR_AN_LP_XNP_ABL2			SR(0x001A)
#define SR_AN_LP_XNP_ABL3			SR(0x001B)
#define SR_AN_COMP_STS				SR(0x0030)
#define VR_AN_DIG_CTRL1				VR(0x0000)
#define VR_AN_INTR_MSK				VR(0x0001)
#define VR_AN_INTR				VR(0x0002)
#define VR_AN_KR_MODE_CTRL			VR(0x0003)
#define VR_AN_TIMER_CTRL0			VR(0x0004)
#define VR_AN_TIMER_CTRL1			VR(0x0005)

/* SR_AN_CTRL */
#define SR_AN_RST				_BV(15)
#define SR_AN_EXT_NP_CTL			_BV(13)
#define SR_AN_EN				_BV(12)
#define SR_AN_LPM				_BV(11)
#define SR_AN_RSTRT_AN				_BV(9)

#define dw_xpcs_an_enabled()			\
	(!!(dw_xpcs_read(AN_MMD, SR_AN_CTRL) & SR_AN_EN))
#define dw_xpcs_an_enable()			\
	dw_xpcs_set(AN_MMD, SR_AN_CTRL, SR_AN_EN)
#define dw_xpcs_an_disable()			\
	dw_xpcs_clear(AN_MMD, SR_AN_CTRL, SR_AN_EN)

/* Optional registers */
#ifdef CONFIG_DW_XPCS_EEE
#define SR_AN_EEE_ABL1				SR(0x003C)
#define SR_AN_EEE_LP_ABL1			SR(0x003D)
#define SR_AN_EEE_ABL2				SR(0x003E)
#define SR_AN_EEE_LP_ABL2			SR(0x003F)
#endif /* CONFIG_DW_XPCS_EEE */

#define dw_xpcs_an_init()			\
	do {					\
		if (dw_xpcs_an_enabled())	\
			dw_xpcs_an_disable();	\
	} while (0)
#else
#define dw_xpcs_an_init()			do { } while (0)
#endif

#ifdef CONFIG_DW_VS_MII_MMD
/* Vendor Specific MII_1 MMD Standard (SR) and Vendor Specific (VR)
 * Registers address block
 */
#define SR_MII_1_CTRL				SR(0x0000)
#define SR_MII_1_STS				SR(0x0001)
#define SR_MII_1_DEV_ID1			SR(0x0002)
#define SR_MII_1_DEV_ID2			SR(0x0003)
#define SR_MII_1_AN_ADV				SR(0x0004)
#define SR_MII_1_LP_BABL			SR(0x0005)
#define SR_MII_1_AN_EXPN			SR(0x0006)
#define SR_MII_1_EXT_STS			SR(0x000F)
#define VR_MII_1_DIG_CTRL1			VR(0x0000)
#define VR_MII_1_AN_CTRL			VR(0x0001)
#define VR_MII_1_AN_INTR_STS			VR(0x0002)
#define VR_MII_1_TC				VR(0x0003)
#define VR_MII_1_DBG_CTRL			VR(0x0005)
#define VR_MII_1_EEE_MCTRL0			VR(0x0006)
#define VR_MII_1_EEE_TXTIMER			VR(0x0008)
#define VR_MII_1_EEE_RXTIMER			VR(0x0009)
#define VR_MII_1_LINK_TIMER_CTRL		VR(0x000A)
#define VR_MII_1_EEE_MCTRL1			VR(0x000B)
#define VR_MII_1_DIG_STS			VR(0x0010)
#define VR_MII_1_ICG_ERRCNT1			VR(0x0011)
#define VR_MII_1_GPIO				VR(0x0015)
#define VR_MII_1_DIG_CTRL2			VR(0x00E1)
#define VR_MII_1_DIG_ERRCNT_SEL			VR(0x00E2)

/* Vendor Specific MII_2 MMD Standard (SR) and Vendor Specific (VR)
 * Registers address block
 */
#define SR_MII_2_CTRL				SR(0x0000)
#define SR_MII_2_STS				SR(0x0001)
#define SR_MII_2_DEV_ID1			SR(0x0002)
#define SR_MII_2_DEV_ID2			SR(0x0003)
#define SR_MII_2_AN_ADV				SR(0x0004)
#define SR_MII_2_LP_BABL			SR(0x0005)
#define SR_MII_2_AN_EXPN			SR(0x0006)
#define SR_MII_2_EXT_STS			SR(0x000F)
#define VR_MII_2_DIG_CTRL1			VR(0x0000)
#define VR_MII_2_AN_CTRL			VR(0x0001)
#define VR_MII_2_AN_INTR_STS			VR(0x0002)
#define VR_MII_2_TC				VR(0x0003)
#define VR_MII_2_DBG_CTRL			VR(0x0005)
#define VR_MII_2_EEE_MCTRL0			VR(0x0006)
#define VR_MII_2_EEE_TXTIMER			VR(0x0008)
#define VR_MII_2_EEE_RXTIMER			VR(0x0009)
#define VR_MII_2_LINK_TIMER_CTRL		VR(0x000A)
#define VR_MII_2_EEE_MCTRL1			VR(0x000B)
#define VR_MII_2_DIG_STS			VR(0x0010)
#define VR_MII_2_ICG_ERRCNT1			VR(0x0011)
#define VR_MII_2_GPIO				VR(0x0015)
#define VR_MII_2_DIG_CTRL2			VR(0x00E1)
#define VR_MII_2_DIG_ERRCNT_SEL			VR(0x00E2)

/* Vendor Specific MII_3 MMD Standard (SR) and Vendor Specific (VR)
 * Registers address block
 */
#define SR_MII_3_CTRL				SR(0x0000)
#define SR_MII_3_STS				SR(0x0001)
#define SR_MII_3_DEV_ID1			SR(0x0002)
#define SR_MII_3_DEV_ID2			SR(0x0003)
#define SR_MII_3_AN_ADV				SR(0x0004)
#define SR_MII_3_LP_BABL			SR(0x0005)
#define SR_MII_3_AN_EXPN			SR(0x0006)
#define SR_MII_3_EXT_STS			SR(0x000F)
#define VR_MII_3_DIG_CTRL1			VR(0x0000)
#define VR_MII_3_AN_CTRL			VR(0x0001)
#define VR_MII_3_AN_INTR_STS			VR(0x0002)
#define VR_MII_3_TC				VR(0x0003)
#define VR_MII_3_DBG_CTRL			VR(0x0005)
#define VR_MII_3_EEE_MCTRL0			VR(0x0006)
#define VR_MII_3_EEE_TXTIMER			VR(0x0008)
#define VR_MII_3_EEE_RXTIMER			VR(0x0009)
#define VR_MII_3_LINK_TIMER_CTRL		VR(0x000A)
#define VR_MII_3_EEE_MCTRL1			VR(0x000B)
#define VR_MII_3_DIG_STS			VR(0x0010)
#define VR_MII_3_ICG_ERRCNT1			VR(0x0011)
#define VR_MII_3_GPIO				VR(0x0015)
#define VR_MII_3_DIG_CTRL2			VR(0x00E1)
#define VR_MII_3_DIG_ERRCNT_SEL			VR(0x00E2)
#endif

/* Vendor Specific MMD1 Registers address block */
#define SR_VSMMD_PMA_ID1			SR(0x0000)
#define SR_VSMMD_PMA_ID2			SR(0x0001)
#define SR_VSMMD_DEV_ID1			SR(0x0002)
#define SR_VSMMD_DEV_ID2			SR(0x0003)
#define SR_VSMMD_PCS_ID1			SR(0x0004)
#define SR_VSMMD_PCS_ID2			SR(0x0005)
#define SR_VSMMD_AN_ID1				SR(0x0006)
#define SR_VSMMD_AN_ID2				SR(0x0007)
#define SR_VSMMD_STS				SR(0x0008)
#define SR_VSMMD_CTRL				SR(0x0009)
#define SR_VSMMD_PKGID1				SR(0x000E)
#define SR_VSMMD_PKGID2				SR(0x000F)

#ifdef CONFIG_DW_XPCS_VS_MII_MMD
/* Vendor Specific MII MMD Standard (SR) and Vendor Specific (VR)
 * Registers address block
 */
#if defined(CONFIG_ARCH_IS_DW_XPCS_1000BASE_X) || \
    defined(CONFIG_DW_XPCS_AN)
#define SR_MII_CTRL				SR(0x0000)
#define SR_MII_STS				SR(0x0001)
#define SR_MII_DEV_ID1				SR(0x0002)
#define SR_MII_DEV_ID2				SR(0x0003)
#define SR_MII_AN_ADV				SR(0x0004)
#define SR_MII_LP_BABL				SR(0x0005)
#define SR_MII_AN_EXPN				SR(0x0006)
#define SR_MII_EXT_STS				SR(0x000F)
#define VR_MII_DIG_CTRL1			VR(0x0000)
#define VR_MII_AN_CTRL				VR(0x0001)
#define VR_MII_AN_INTR_STS			VR(0x0002)
#define VR_MII_LINK_TIMER_CTRL			VR(0x000A)

/* SR_MII_CTRL */
#define SR_MII_SS13				_BV(13)
#define SR_MII_AN_ENABLE			_BV(12)
#define SR_MII_SS6				_BV(6)
#define SR_MII_SS5				_BV(5)

#ifdef CONFIG_DW_XPCS_GPIO
#define VR_MII_GPIO				VR(0x0015)
#endif /* CONFIG_DW_XPCS_GPIO */
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */

#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define SR_MII_TIME_SYNC_ABL			SR(0x0708)
#define SR_MII_TIME_SYNC_TX_MAX_DLY_LWR		SR(0x0709)
#define SR_MII_TIME_SYNC_TX_MAX_DLY_UPR		SR(0x070A)
#define SR_MII_TIME_SYNC_TX_MIN_DLY_LWR		SR(0x070B)
#define SR_MII_TIME_SYNC_TX_MIN_DLY_UPR		SR(0x070C)
#define SR_MII_TIME_SYNC_RX_MAX_DLY_LWR		SR(0x070D)
#define SR_MII_TIME_SYNC_RX_MAX_DLY_UPR		SR(0x070E)
#define SR_MII_TIME_SYNC_RX_MIN_DLY_LWR		SR(0x070F)
#define SR_MII_TIME_SYNC_RX_MIN_DLY_UPR		SR(0x0710)
#define VR_MII_TC				VR(0x0003)
#define VR_MII_DBG_CTRL				VR(0x0005)
#define VR_MII_DIG_STS				VR(0x0010)
#define VR_MII_ICG_ERRCNT1			VR(0x0011)
#define VR_MII_MISC_STS				VR(0x0018)
#define VR_MII_RX_LSTS				VR(0x0020)
#define VR_MII_SNPS_CR_CTRL			VR(0x00A0)
#define VR_MII_SNPS_CR_ADDR			VR(0x00A1)
#define VR_MII_SNPS_CR_DATA			VR(0x00A2)
#define VR_MII_DIG_CTRL2			VR(0x00E1)
#define VR_MII_DIG_ERRCNT_SEL			VR(0x00E2)

#ifdef CONFIG_DW_XPCS_EEE
#define VR_MII_EEE_MCTRL0			VR(0x0006)
#define VR_MII_EEE_TXTIMER			VR(0x0008)
#define VR_MII_EEE_RXTIMER			VR(0x0009)
#define VR_MII_EEE_MCTRL1			VR(0x000B)
#endif /* CONFIG_DW_XPCS_EEE */
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */

/* RSVDPARAM_0 */
#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define VR_MII_Consumer_10G_TX_TERM_CTRL	VR(0x003C)
#define VR_MII_Consumer_10G_RX_TERM_CTRL	VR(0x0064)
#define VR_MII_Consumer_10G_RX_IQ_CTRL0		VR(0x006B)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */
#endif

#ifndef CONFIG_ARCH_IS_DW_XPCS_PHY_SNPS_XAUI
#define dw_xpcs_int_sram_init()			do { } while (0)
#define dw_xpcs_ext_sram_init()			do { } while (0)
#define dw_xpcs_power_init()			do { } while (0)
#endif

uint16_t dw_xpcs_read(uint16_t mmd, uint16_t addr);
void dw_xpcs_write(uint16_t mmd, uint16_t addr, uint16_t value);

#define dw_xpcs_set(mmd, addr, value)			\
	do {						\
		uint16_t __v = dw_xpcs_read(mmd, addr);	\
		__v |= (value);				\
		dw_xpcs_write(mmd, addr, __v);		\
	} while (0)
#define dw_xpcs_clear(mmd, addr, value)			\
	do {						\
		uint16_t __v = dw_xpcs_read(mmd, addr);	\
		__v &= ~(value);			\
		dw_xpcs_write(mmd, addr, __v);		\
	} while (0)

#ifdef CONFIG_DW_XPCS
void dw_xpcs_link_up(void);
#else
#define dw_xpcs_link_up()		do { } while (0)
#endif

#endif /* __DW_XPCS_H_INCLUDE__ */
