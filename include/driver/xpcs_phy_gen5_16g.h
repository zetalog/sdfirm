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
 * @(#)xpcs_phy_gen5_16g.h: Synopsys XAUI phy Gen5 16G interface
 * $Id: xpcs_phy_gen5_16g.h,v 1.0 2020-12-20 19:00:00 zhenglv Exp $
 */

#ifndef __XPCS_PHY_GEN5_16G_H_INCLUDE__
#define __XPCS_PHY_GEN5_16G_H_INCLUDE__

/* ======================================================================
 * Register Fields
 * ====================================================================== */
/* 6.103 VR_XS_PCS_DIG_STS */
#define VR_PCS_PSEQ_ACK_HIGH0		0x0
#define VR_PCS_PSEQ_ACK_LOW0		0x1
#define VR_PCS_PSEQ_ACK_HIGH1		0x2
#define VR_PCS_PSEQ_ACK_LOW1		0x3
#define VR_PCS_PSEQ_POWER_GOOD		0x4
#define VR_PCS_PSEQ_POWER_SAVE		0x5
#define VR_PCS_PSEQ_POWER_DOWN		0x6

#define dw_xpcs_is_power_good()			\
	(VR_PCS_PSEQ_POWER_GOOD ==		\
	 VR_PCS_PSEQ_STATE(dw_xpcs_read(XS_PCS_MMD, VR_XS_PCS_DIG_STS)))

#define dw_xpcs_power_init()			\
	while (!dw_xpcs_is_power_good())

/* ======================================================================
 * Register Addresses
 * ====================================================================== */
#define VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL0		VR(0x0030)
#define VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1		VR(0x0031)
#define VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2		VR(0x0032)
#define VR_XS_PMA_Gen5_12G_16G_TX_BOOST_CTRL		VR(0x0033)
#define VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL		VR(0x0034)
#define VR_XS_PMA_Gen5_12G_16G_TX_POWER_STATE_CTRL	VR(0x0035)
#define VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0		VR(0x0036)
#define VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1		VR(0x0037)
#define VR_XS_PMA_Gen5_12G_16G_TX_STS			VR(0x0040)
#define VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL0		VR(0x0050)
#define VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL1		VR(0x0051)
#define VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2		VR(0x0052)
#define VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL3		VR(0x0053)
#define VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL		VR(0x0054)
#define VR_XS_PMA_Gen5_12G_16G_RX_POWER_STATE_CTRL	VR(0x0055)
#define VR_XS_PMA_Gen5_12G_16G_RX_CDR_CTRL		VR(0x0056)
#define VR_XS_PMA_Gen5_12G_16G_RX_ATTN_CTRL		VR(0x0057)
#define VR_XS_PMA_Gen5_16G_RX_EQ_CTRL(l)		VR(0x0058 + (l))
#define VR_XS_PMA_Gen5_12G_16G_RX_EQ_CTRL4		VR(0x005C)
#define VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5			VR(0x005D)
#define VR_XS_PMA_Gen5_12G_16G_DFE_TAP_CTRL0		VR(0x005E)
#define VR_XS_PMA_Gen5_12G_16G_RX_STS			VR(0x0060)
#define VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1			VR(0x0064)
#define VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4			VR(0x0068)
#define VR_XS_PMA_Gen5_16G_RX_MISC_CTRL(l)		\
	VR(0x0069 + REG16_8BIT_INDEX(l))
#define VR_XS_PMA_Gen5_16G_RX_MISC_CTRL1		VR(0x006A)
#define VR_XS_PMA_Gen5_16G_RX_IQ_CTRL(l)		VR(0x006B + (l))
#define VR_XS_PMA_Gen5_12G_16G_MPLL_CMN_CTRL		VR(0x0070)
#define VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0		VR(0x0071)
#define VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2		VR(0x0073)
#define VR_XS_PMA_Gen5_12G_16G_MPLLB_CTRL0		VR(0x0074)
#define VR_XS_PMA_Gen5_12G_16G_MPLLB_CTRL2		VR(0x0076)
#define VR_XS_PMA_Gen5_12G_16G_MISC_CTRL0		VR(0x0090)
#define VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL		VR(0x0091)
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD(l)		VR(0x0092 + (l))
#define VR_XS_PMA_Gen5_16G_VCO_CAL_REF(l) 		\
	VR(0x0096 + REG16_8BIT_INDEX(l))
#define VR_XS_PMA_Gen5_12G_16G_MISC_STS			VR(0x0098)
#define VR_XS_PMA_Gen5_12G_16G_MISC_CTRL1		VR(0x0099)
#define VR_XS_PMA_Gen5_12G_16G_SRAM			VR(0x009B)

/* ======================================================================
 * Register Fields
 * ====================================================================== */
/* 6.111 VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL0 */
#define VR_PMA_TX_DT_EN(l)		_BV((l) + 12)
#define VR_PMA_TX_RST(l)		_BV((l) + 8)
#define VR_PMA_TX_INV(l)		_BV((l) + 4)
#define VR_PMA_TXBCN_EN(l)		_BV(l)

/* 6.113 VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1 */
#define VR_PMA_TX_CLK_RDY(l)		_BV((l) + 12)
#define VR_PMA_VBOOST_LVL_OFFSET	8
#define VR_PMA_VBOOST_LVL_MASK		REG_3BIT_MASK
#define VR_PMA_VBOOST_LVL(v)		_SET_FV(VR_PMA_VBOOST_LVL, v)
#define VR_PMA_VBOOST_EN(l)		_BV((l) + 4)

/* 6.114 VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2 */
#define VR_PMA_TX_WIDTH_OFFSET(l)	(REG16_2BIT_OFFSET(l) + 8)
#define VR_PMA_TX_WIDTH_MASK		REG_2BIT_MASK
#define VR_PMA_TX_WIDTH(l, v)		_SET_FVn(l, VR_PMA_TX_WIDTH, v)
#define VR_PMA_TX_LPD(l)		_BV((l) + 4)
#define VR_PMA_TX_REQ(l)		_BV(l)

/* 6.119 VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL */
#define VR_PMA_TX_RATE_OFFSET(l)	REG16_4BIT_OFFSET(l)
#define VR_PMA_TX_RATE_MASK		REG_3BIT_MASK
#define VR_PMA_TX_RATE(l, v)		_SET_FVn(l, VR_PMA_TX_RATE, v)

/* 6.121 VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0 */
#define VR_PMA_TX_EQ_MAIN_OFFSET	8
#define VR_PMA_TX_EQ_MAIN_MASK		REG_6BIT_MASK
#define VR_PMA_TX_EQ_MAIN(v)		_SET_FV(VR_PMA_TX_EQ_MAIN, v)
#define VR_PMA_TX_EQ_PRE_OFFSET		0
#define VR_PMA_TX_EQ_PRE_MASK		REG_6BIT_MASK
#define VR_PMA_TX_EQ_PRE(v)		_SET_FV(VR_PMA_TX_EQ_PRE, v)

/* 6.122 VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1 */
#define VR_PMA_CA_TX_EQ			_BV(8)
#define VR_PMA_TX_EQ_DEF_CTRL		_BV(7)
#define VR_PMA_TX_EQ_OVR_RIDE		_BV(6)
#define VR_PMA_TX_EQ_POST_OFFSET	0
#define VR_PMA_TX_EQ_POST_MASK		REG_6BIT_MASK
#define VR_PMA_TX_EQ_POST(v)		_SET_FV(VR_PMA_TX_EQ_POST, v)

/* 6.149 VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2 */
#define VR_PMA_RX_WIDTH_OFFSET(l)	(REG16_2BIT_OFFSET(l) + 8)
#define VR_PMA_RX_WIDTH_MASK		REG_2BIT_MASK
#define VR_PMA_RX_WIDTH(l, v)		_SET_FVn(l, VR_PMA_RX_WIDTH, v)
#define VR_PMA_RX_LPD(l)		_BV((l) + 4)
#define VR_PMA_RX_REQ(l)		_BV(l)

/* 6.153 VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL */
#define VR_PMA_RX_RATE_OFFSET(l)	REG16_4BIT_OFFSET(l)
#define VR_PMA_RX_RATE_MASK		REG_3BIT_MASK
#define VR_PMA_RX_RATE(l, v)		_SET_FVn(l, VR_PMA_RX_RATE, v)

/* 6.159 VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0
 * 6.162 VR_XS_PMA_Gen5_16G_RX_EQ_CTRL1
 * 6.165 VR_XS_PMA_Gen5_16G_RX_EQ_CTRL2
 * 6.163 VR_XS_PMA_Gen5_16G_RX_EQ_CTRL8
 */
#define VR_PMA_VGA1_GAIN_OFFSET		12
#define VR_PMA_VGA1_GAIN_MASK		REG_3BIT_MASK
#define VR_PMA_VGA1_GAIN(v)		_SET_FV(VR_PMA_VGA1_GAIN, v)
#define VR_PMA_VGA2_GAIN_OFFSET		8
#define VR_PMA_VGA2_GAIN_MASK		REG_3BIT_MASK
#define VR_PMA_VGA2_GAIN(v)		_SET_FV(VR_PMA_VGA2_GAIN, v)
#define VR_PMA_CTLE_POLE_OFFSET		5
#define VR_PMA_CTLE_POLE_MASK		REG_2BIT_MASK
#define VR_PMA_CTLE_POLE(v)		_SET_FV(VR_PMA_CTLE_POLE, v)
#define VR_PMA_CTLE_BOOST_OFFSET	0
#define VR_PMA_CTLE_BOOST_MASK		REG_5BIT_MASK
#define VR_PMA_CTLE_BOOST(v)		_SET_FV(VR_PMA_CTLE_BOOST, v)

/* 6.170 VR_XS_PMA_Gen5_12G_16G_RX_EQ_CTRL4 */
#define VR_PMA_RX_AD_REQ		_BV(12)
#define VR_PMA_RX_EQ_STRT_CTRL		_BV(11)
#define VR_PMA_SELF_MAIN_EN		_BV(10)
#define VR_PMA_PING_PONG_EN		_BV(9)
#define VR_PMA_SEQ_EQ_EN		_BV(8)
#define VR_PMA_CONT_OFF_CAN(l)		_BV((l) + 4)
#define VR_PMA_CONT_ADAPT(l)		_BV(l)

/* 6.171 VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5 */
#define VR_PMA_RX_ADPT_MODE_OFFSET(l)	(REG16_2BIT_OFFSET(l) + 4)
#define VR_PMA_RX_ADPT_MODE_MASK	REG_2BIT_MASK
#define VR_PMA_RX_ADPT_MODE(l, v)	_SET_FVn(l, VR_PMA_RX_ADPT_MODE, v)
#define VR_PMA_RX_ADPT_SEL(l)		_BV(l)

/* 6.172 VR_XS_PMA_Gen5_12G_AFE_DFE_EN_CTRL */
#define VR_PMA_DFE_EN(l)		_BV((l) + 4)
#define VR_PMA_AFE_EN(l)		_BV(l)

/* 6.181 VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1 */
#define VR_PMA_VCO_FRQBAND_OFFSET(l)	(REG16_2BIT_OFFSET(l) + 8)
#define VR_PMA_VCO_FRQBAND_MASK		REG_2BIT_MASK
#define VR_PMA_VCO_FRQBAND(l, v)	_SET_FVn(l, VR_PMA_VCO_FRQBAND, v)
#define VR_PMA_VCO_STEP_CTRL(l)		_BV(4 + (l))
#define VR_PMA_VCO_TEMP_COMP_EN(l)	_BV(l)

/* 6.185 VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4 */
#define VR_PMA_RX_DFE_BYP(l)		_BV((l) + 8)
#define VR_PMA_RX_TERM_OFFSET_OFFSET	0
#define VR_PMA_RX_TERM_OFFSET_MASK	REG_5BIT_MASK
#define VR_PMA_RX_TERM_OFFSET(v)	_SET_FV(VR_PMA_RX_TERM_OFFSET, v)

/* 6.186 VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0
 * 6.187 VR_XS_PMA_Gen5_16G_RX_MISC_CTRL1
 */
#define VR_PMA_RX_MISC_OFFSET(l)	REG16_8BIT_OFFSET(l)
#define VR_PMA_RX_MISC_MASK		REG_8BIT_MASK
#define VR_PMA_RX_MISC(l, v)		_SET_FVn(l, VR_PMA_RX_MISC, v)

/* 6.189 VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0
 * 6.191 VR_XS_PMA_Gen5_16G_RX_IQ_CTRL1
 * 6.193 VR_XS_PMA_Gen5_16G_RX_IQ_CTRL2
 * 6.195 VR_XS_PMA_Gen5_16G_RX_IQ_CTRL3
 */
#define VR_PMA_RX_DELTA_IQ_OFFSET	8
#define VR_PMA_RX_DELTA_IQ_MASK		REG_3BIT_MASK
#define VR_PMA_RX_DELTA_IQ(v)		_SET_FV(VR_PMA_RX_DELTA_IQ, v)
#define VR_PMA_RX_MARGIN_IQ_OFFSET	0
#define VR_PMA_RX_MARGIN_IQ_MASK	REG_7BIT_MASK
#define VR_PMA_RX_MARGIN_IQ(v)		_SET_FV(VR_PMA_RX_MARGIN_IQ, v)

/* 6.199 VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0 */
#define VR_PMA_MPLLA_CAL_DISABLE	_BV(15)
#define VR_PMA_MPLLA_SSC_CLK_SEL_OFFSET	8
#define VR_PMA_MPLLA_SSC_CLK_SEL_MASK	REG_3BIT_MASK
#define VR_PMA_MPLLA_SSC_CLK_SEL(v)	_SET_FV(VR_PMA_MPLLA_SSC_CLK_SEL, v)
#define VR_PMA_MPLLA_MULTIPLIER_OFFSET	0
#define VR_PMA_MPLLA_MULTIPLIER_MASK	REG_8BIT_MASK
#define VR_PMA_MPLLA_MULTIPLIER(v)	_SET_FV(VR_PMA_MPLLA_MULTIPLIER, v)

/* 6.203 VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2 */
#define VR_PMA_MPLLA_RECAL_BANK_SEL_OFFSET	13
#define VR_PMA_MPLLA_RECAL_BANK_SEL_MASK	REG_2BIT_MASK
#define VR_PMA_MPLLA_RECAL_BANK_SEL(v)		\
	_SET_FV(VR_PMA_MPLLA_RECAL_BANK_SEL, v)
#define VR_PMA_MPLLA_TX_CLK_DIV_OFFSET	11
#define VR_PMA_MPLLA_TX_CLK_DIV_MASK	REG_2BIT_MASK
#define VR_PMA_MPLLA_TX_CLK_DIV(v)	_SET_FV(VR_PMA_MPLLA_TX_CLK_DIV, v)
#define VR_PMA_MPLLA_DIV_CLK_EN_OFFSET	8
#define VR_PMA_MPLLA_DIV_CLK_EN_MASK	REG_3BIT_MASK
#define VR_PMA_MPLLA_DIV_CLK_EN(v)	_SET_FV(VR_PMA_MPLLA_DIV_CLK_EN, v)
#define VR_PMA_MPLLA_DIV8_CLK		_BV(0)
#define VR_PMA_MPLLA_DIV10_CLK		_BV(1)
#define VR_PMA_MPLLA_DIV16P5_CLK	_BV(2)

/* 6.225 VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL */
#define VR_PMA_REF_RPT_CLK_EN		_BV(8)
#define VR_PMA_REF_MPLLB_DIV2		_BV(7)
#define VR_PMA_REF_MPLLA_DIV2		_BV(6)
#define VR_PMA_REF_RANGE_OFFSET		3
#define VR_PMA_REF_RANGE_MASK		REG_3BIT_MASK
#define VR_PMA_REF_RANGE(v)		_SET_FV(VR_PMA_REF_RANGE, v)
#define VR_PMA_REF_CLK_DIV2		_BV(2)
#define VR_PMA_REF_USE_PAD		_BV(1)
#define VR_PMA_REF_CLK_EN		_BV(0)

/* 6.227 VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0 */
#define VR_PMA_VCO_LD_VAL_OFFSET	0
#define VR_PMA_VCO_LD_VAL_MASK		REG_13BIT_MASK
#define VR_PMA_VCO_LD_VAL(v)		_SET_FV(VR_PMA_VCO_LD_VAL, v)

/* 6.234 VR_XS_PMA_Gen5_16G_VCO_CAL_REF0
 * 6.235 VR_XS_PMA_Gen5_16G_VCO_CAL_REF1
 */
#define VR_PMA_VCO_REF_LD_OFFSET(l)	REG16_8BIT_OFFSET(l)
#define VR_PMA_VCO_REF_LD_MASK		REG_7BIT_MASK
#define VR_PMA_VCO_REF_LD(l, v)		_SET_FVn(l, VR_PMA_VCO_REF_LD, v)

/* 6.244 VR_XS_PMA_Gen5_12G_16G_SRAM */
#define VR_PMA_EXT_LD_DN	_BV(1)
#define VR_PMA_INIT_DN		_BV(0)

/* ======================================================================
 * Field Values
 * ====================================================================== */
/* 6.114 VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2
 * 6.149 VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2
 */
#define VR_PMA_WIDTH_8BIT		0
#define VR_PMA_WIDTH_10BIT		1
#define VR_PMA_WIDTH_16BIT		2
#define VR_PMA_WIDTH_20BIT		3

/* 6.119 VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL
 * 6.153 VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL
 */
#define VR_PMA_RATE_BAUD		0
#define VR_PMA_RATE_BAUD_DIV2		1
#define VR_PMA_RATE_BAUD_DIV4		2
#define VR_PMA_RATE_BAUD_DIV8		3
#define VR_PMA_RATE_BAUD_DIV10		7

/* 6.171 VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5 */
#define VR_PMA_RX_ADPT_RPCS		3
#define VR_PMA_RX_ADPT_PCS		0

/* 6.225 VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL */
#define VR_PMA_REF_20_26MHZ		0
#define VR_PMA_REF_26P1_52MHZ		1
#define VR_PMA_REF_52P1_78MHZ		2
#define VR_PMA_REF_78P1_104MHZ		3
#define VR_PMA_REF_104P1_130MHZ		4
#define VR_PMA_REF_130P1_156MHZ		5
#define VR_PMA_REF_156P1_182MHZ		6
#define VR_PMA_REF_182P1_200MHZ		7

#define dw_xpcs_is_int_sram_done()					\
	(dw_xpcs_read(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_SRAM) &	\
	 VR_PMA_INIT_DN)
#define dw_xpcs_int_sram_init()						\
	do {								\
		while (!dw_xpcs_is_int_sram_done());			\
	} while (0)
#define dw_xpcs_ext_sram_init()						\
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_SRAM,		\
		      VR_PMA_EXT_LD_DN)

/* ======================================================================
 * Register Addresses
 * ====================================================================== */
#ifdef CONFIG_DW_XPCS_KR
#define VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL2		VR(0x0038)
#define VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL3		VR(0x0039)
#define VR_XS_PMA_Gen5_12G_16G_EQ_INIT_CTRL0		VR(0x003A)
#define VR_XS_PMA_Gen5_12G_16G_EQ_INIT_CTRL1		VR(0x003B)
#endif /* CONFIG_DW_XPCS_KR */
#ifdef CONFIG_DW_XPCS_EEE
#define VR_XS_PMA_Gen5_12G_16G_EEE_CTRL			VR(0x009A)
#endif /* CONFIG_DW_XPCS_EEE */
#ifndef CONFIG_DW_XPCS_SINGLE_LANE
#define VR_XS_PMA_Gen5_12G_16G_DFE_TAP_CTRL1		VR(0x005F)
#endif /* CONFIG_DW_XPCS_SINGLE_LANE */

/* ======================================================================
 * Registers Addresses
 * ====================================================================== */
#define VR_XS_PMA_Gen5_16G_TX_GEN_CTRL3			VR(0x003C)
#define VR_XS_PMA_Gen5_16G_TX_GEN_CTRL4			VR(0x003D)
#define VR_XS_PMA_Gen5_16G_TX_MISC_CTRL0		VR(0x003E)
#define VR_XS_PMA_Gen5_16G_RX_PPM_STS0			VR(0x0061)
#define VR_XS_PMA_Gen5_16G_RX_PPM_CTRL0			VR(0x0065)
#define VR_XS_PMA_Gen5_16G_MPLLA_CTRL1			VR(0x0072)
#define VR_XS_PMA_Gen5_16G_MPLLB_CTRL1			VR(0x0075)
#define VR_XS_PMA_Gen5_16G_MPLLA_CTRL3			VR(0x0077)
#define VR_XS_PMA_Gen5_16G_MPLLB_CTRL3			VR(0x0078)
#define VR_XS_PMA_Gen5_16G_MPLLA_CTRL4			VR(0x0079)
#define VR_XS_PMA_Gen5_16G_MPLLA_CTRL5			VR(0x007A)
#define VR_XS_PMA_Gen5_16G_MPLLB_CTRL4			VR(0x007B)
#define VR_XS_PMA_Gen5_16G_MPLLB_CTRL5			VR(0x007C)
#define VR_XS_PMA_Gen5_16G_MISC_CTRL2			VR(0x009C)

/* ======================================================================
 * Register Addresses
 * ====================================================================== */
#ifndef CONFIG_DW_XPCS_SINGLE_LANE
#define VR_XS_PMA_Gen5_16G_TX_MISC_CTRL1		VR(0x003F)
#define VR_XS_PMA_Gen5_16G_RX_PPM_STS1			VR(0x0062)
#define VR_XS_PMA_Gen5_16G_RX_PPM_CTRL1			VR(0x0066)
#endif /* CONFIG_DW_XPCS_SINGLE_LANE */

/* ======================================================================
 * Register Addresses
 * ====================================================================== */
#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define VR_MII_Gen5_12G_16G_SRAM			VR(0x80BB)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */

/* ======================================================================
 * Register Addresses
 * ====================================================================== */
#ifdef CONFIG_DW_XPCS_VS_MII_MMD
#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define VR_MII_Gen5_12G_16G_TX_GENCTRL0		VR(0x0030)
#define VR_MII_Gen5_12G_16G_TX_GENCTRL1		VR(0x0031)
#define VR_MII_Gen5_12G_16G_TX_GENCTRL2		VR(0x0032)
#define VR_MII_Gen5_12G_16G_TX_BOOST_CTRL	VR(0x0033)
#define VR_MII_Gen5_12G_16G_TX_RATE_CTRL	VR(0x0034)
#define VR_MII_Gen5_12G_16G_TX_POWER_STATE_CTRL	VR(0x0035)
#define VR_MII_Gen5_12G_16G_TX_EQ_CTRL0		VR(0x0036)
#define VR_MII_Gen5_12G_16G_TX_EQ_CTRL1		VR(0x0037)
#define VR_MII_Gen5_12G_16G_TX_STS		VR(0x0040)
#define VR_MII_Gen5_12G_16G_RX_GENCTRL0		VR(0x0050)
#define VR_MII_Gen5_12G_16G_RX_GENCTRL1		VR(0x0051)
#define VR_MII_Gen5_12G_16G_RX_GENCTRL2		VR(0x0052)
#define VR_MII_Gen5_12G_16G_RX_GENCTRL3		VR(0x0053)
#define VR_MII_Gen5_12G_16G_RX_RATE_CTRL	VR(0x0054)
#define VR_MII_Gen5_12G_16G_RX_POWER_STATE_CTRL	VR(0x0055)
#define VR_MII_Gen5_12G_16G_RX_CDR_CTRL		VR(0x0056)
#define VR_MII_Gen5_12G_16G_RX_ATTN_CTRL	VR(0x0057)
#define VR_MII_Gen5_12G_16G_RX_EQ_CTRL4		VR(0x005C)
#define VR_MII_Gen5_12G_16G_MPLL_CMN_CTRL	VR(0x0070)
#define VR_MII_Gen5_12G_16G_MPLLA_CTRL0		VR(0x0071)
#define VR_MII_Gen5_12G_16G_MPLLA_CTRL2		VR(0x0073)
#define VR_MII_Gen5_12G_16G_MPLLB_CTRL0		VR(0x0074)
#define VR_MII_Gen5_12G_16G_MPLLB_CTRL2		VR(0x0076)
#define VR_MII_Gen5_12G_16G_MISC_CTRL0		VR(0x0090)
#define VR_MII_Gen5_12G_16G_REF_CLK_CTRL	VR(0x0091)
#define VR_MII_Gen5_12G_16G_VCO_CAL_LD0		VR(0x0092)
#define VR_MII_Gen5_12G_16G_MISC_STS		VR(0x0098)
#define VR_MII_Gen5_12G_16G_MISC_CTRL1		VR(0x0099)
#define VR_MII_Gen5_12G_16G_EEE_CTRL		VR(0x009A)

/* ======================================================================
 * Register Fields
 * ====================================================================== */
/* 6.446 VR_MII_Gen5_12G_16G_RX_EQ_CTRL4 */
#define VR_MII_RX_AD_REQ			_BV(12)
#define VR_MII_RX_EQ_STRT_CTRL			_BV(11)
#define VR_MII_SELF_MAIN_EN			_BV(10)
#define VR_MII_PING_PONG_EN			_BV(9)
#define VR_MII_SEQ_EQ_EN			_BV(8)
#define VR_MII_CONT_OFF_CAN(l)			_BV((l) + 4)
#define VR_MII_CONT_ADAPT(l)			_BV(l)

#define dw_xpcs_enable_rx_cont_adapt(l)					\
	dw_xpcs_set(VS_MII_MMD, VR_MII_Gen5_12G_16G_RX_EQ_CTRL4,	\
		    VR_MII_CONT_ADAPT(l))
#define dw_xpcs_disable_rx_cont_adapt(l)				\
	dw_xpcs_clear(VS_MII_MMD, VR_MII_Gen5_12G_16G_RX_EQ_CTRL4,	\
		      VR_MII_CONT_ADAPT(l))

/* ======================================================================
 * Registers Addresses
 * ====================================================================== */
#define VR_MII_Gen5_16G_TX_GEN_CTRL3		VR(0x003C)
#define VR_MII_Gen5_16G_TX_GEN_CTRL4		VR(0x003D)
#define VR_MII_Gen5_16G_TX_MISC_CTRL0		VR(0x003E)
#define VR_MII_Gen5_16G_RX_EQ_CTRL0		VR(0x0058)
#define VR_MII_Gen5_16G_RX_EQ_CTRL5		VR(0x005D)
#define VR_MII_Gen5_16G_RX_PPM_STS0		VR(0x0061)
#define VR_MII_Gen5_16G_RX_CDR_CTRL1		VR(0x0064)
#define VR_MII_Gen5_16G_RX_PPM_CTRL0		VR(0x0065)
#define VR_MII_Gen5_16G_RX_GEN_CTRL4		VR(0x0068)
#define VR_MII_Gen5_16G_RX_MISC_CTRL0		VR(0x0069)
#define VR_MII_Gen5_16G_RX_IQ_CTRL0		VR(0x006B)
#define VR_MII_Gen5_16G_MPLLA_CTRL1		VR(0x0072)
#define VR_MII_Gen5_16G_MPLLB_CTRL1		VR(0x0075)
#define VR_MII_Gen5_16G_MPLLA_CTRL3		VR(0x0077)
#define VR_MII_Gen5_16G_MPLLB_CTRL3		VR(0x0078)
#define VR_MII_Gen5_16G_MPLLA_CTRL4		VR(0x0079)
#define VR_MII_Gen5_16G_MPLLA_CTRL5		VR(0x007A)
#define VR_MII_Gen5_16G_MPLLB_CTRL4		VR(0x007B)
#define VR_MII_Gen5_16G_MPLLB_CTRL5		VR(0x007C)
#define VR_MII_Gen5_16G_VCO_CAL_REF0		VR(0x0096)
#define VR_MII_Gen5_16G_MISC_CTRL2		VR(0x009C)

/* ======================================================================
 * Register Fields
 * ====================================================================== */
/* 6.474 VR_MII_Gen5_16G_MPLLA_CTRL3 */
#define VR_MII_MPLLA_BANDWIDTH_OFFSET		0
#define VR_MII_MPLLA_BANDWIDTH_MASK		REG_16BIT_MASK
#define VR_MII_MPLLA_BANDWIDTH(v)		_SET_FV(VR_MII_MPLLA_BANDWIDTH, v)
#define dw_xpcs_config_mplla_bandwidth(w)		\
	dw_xpcs_write(VS_MII_MMD,			\
		      VR_MII_Gen5_16G_MPLLA_CTRL3,	\
		      VR_MII_MPLLA_BANDWIDTH(w))
#else
#define dw_xpcs_enable_rx_cont_adapt(l)					\
	dw_xpcs_set(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_RX_EQ_CTRL4,	\
		    VR_PMA_CONT_ADAPT(l))
#define dw_xpcs_disable_rx_cont_adapt(l)				\
	dw_xpcs_clear(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_RX_EQ_CTRL4,	\
		      VR_PMA_CONT_ADAPT(l))
#define dw_xpcs_config_mplla_bandwidth(w)	do { } while (0)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */
#else
#define dw_xpcs_enable_rx_cont_adapt(l)					\
	dw_xpcs_set(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_RX_EQ_CTRL4,	\
		    VR_PMA_CONT_ADAPT(l))
#define dw_xpcs_disable_rx_cont_adapt(l)				\
	dw_xpcs_clear(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_RX_EQ_CTRL4,	\
		      VR_PMA_CONT_ADAPT(l))
#define dw_xpcs_config_mplla_bandwidth(w)	do { } while (0)
#endif /* CONFIG_DW_XPCS_VS_MII_MMD */

#endif /* __XPCS_PHY_GEN5_16G_H_INCLUDE__ */
