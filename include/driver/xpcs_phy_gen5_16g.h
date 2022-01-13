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

/* VR_XS_PCS_DIG_STS */
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
#define VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0			VR(0x0058)
#define VR_XS_PMA_Gen5_16G_RX_EQ_CTRL1			VR(0x0059)
#define VR_XS_PMA_Gen5_16G_RX_EQ_CTRL2			VR(0x005A)
#define VR_XS_PMA_Gen5_16G_RX_EQ_CTRL3			VR(0x005B)
#define VR_XS_PMA_Gen5_12G_16G_RX_EQ_CTRL4		VR(0x005C)
#define VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5			VR(0x005D)
#define VR_XS_PMA_Gen5_12G_16G_DFE_TAP_CTRL0		VR(0x005E)
#define VR_XS_PMA_Gen5_12G_16G_RX_STS			VR(0x0060)
#define VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1			VR(0x0064)
#define VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4			VR(0x0068)
#define VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0		VR(0x0069)
#define VR_XS_PMA_Gen5_16G_RX_MISC_CTRL1		VR(0x006A)
#define VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0			VR(0x006B)
#define VR_XS_PMA_Gen5_16G_RX_IQ_CTRL1			VR(0x006C)
#define VR_XS_PMA_Gen5_16G_RX_IQ_CTRL2			VR(0x006D)
#define VR_XS_PMA_Gen5_16G_RX_IQ_CTRL3			VR(0x006E)
#define VR_XS_PMA_Gen5_12G_16G_MPLL_CMN_CTRL		VR(0x0070)
#define VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0		VR(0x0071)
#define VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2		VR(0x0073)
#define VR_XS_PMA_Gen5_12G_16G_MPLLB_CTRL0		VR(0x0074)
#define VR_XS_PMA_Gen5_12G_16G_MPLLB_CTRL2		VR(0x0076)
#define VR_XS_PMA_Gen5_12G_16G_MISC_CTRL0		VR(0x0090)
#define VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL		VR(0x0091)
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0		VR(0x0092)
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD1		VR(0x0093)
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD2		VR(0x0094)
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD3		VR(0x0095)
#define VR_XS_PMA_Gen5_16G_VCO_CAL_REF0 		VR(0x0096)
#define VR_XS_PMA_Gen5_16G_VCO_CAL_REF1 		VR(0x0097)
#define VR_XS_PMA_Gen5_12G_16G_MISC_STS			VR(0x0098)
#define VR_XS_PMA_Gen5_12G_16G_MISC_CTRL1		VR(0x0099)
#define VR_XS_PMA_Gen5_12G_16G_SRAM			VR(0x009B)

/* VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL0 */
#define TX_DT_EN_3_1_OFFSET		13
#define TX_DT_EN_3_1_MASK		REG_3BIT_MASK
#define TX_DT_EN_3_1(value)		_SET_FV(TX_DT_EN_3_1, value)
#define TX_DT_EN_0			_BV(12)
#define TX_DT_EN_1			1

/* VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5 */
#define RX3_ADPT_MODE_OFFSET		10
#define RX3_ADPT_MODE_MASK		REG_2BIT_MASK
#define RX3_ADPT_MODE(value)		_SET_FV(RX3_ADPT_MODE, value)
#define RX2_ADPT_MODE_OFFSET		8
#define RX2_ADPT_MODE_MASK		REG_2BIT_MASK
#define RX2_ADPT_MODE(value)		_SET_FV(RX2_ADPT_MODE, value)
#define RX1_ADPT_MODE_OFFSET		6
#define RX1_ADPT_MODE_MASK		REG_2BIT_MASK
#define RX1_ADPT_MODE(value)		_SET_FV(RX1_ADPT_MODE, value)
#define RX0_ADPT_MODE_OFFSET		4
#define RX0_ADPT_MODE_MASK		REG_2BIT_MASK
#define RX0_ADPT_MODE(value)		_SET_FV(RX0_ADPT_MODE, value)

/* VR_XS_PMA_Gen5_16G_RX_IQ_CTRL3 */
#define RX3_DELTA_IQ_OFFSET		8
#define RX3_DELTA_IQ_MASK		REG_4BIT_MASK
#define RX3_DELTA_IQ(value)		_SET_FV(RX3_DELTA_IQ, value)

/* VR_XS_PMA_Gen5_16G_RX_IQ_CTRL2 */
#define RX2_DELTA_IQ_OFFSET		8
#define RX2_DELTA_IQ_MASK		REG_4BIT_MASK
#define RX2_DELTA_IQ(value)		_SET_FV(RX2_DELTA_IQ, value)

/* VR_XS_PMA_Gen5_16G_RX_IQ_CTRL1 */
#define RX1_DELTA_IQ_OFFSET		8
#define RX1_DELTA_IQ_MASK		REG_4BIT_MASK
#define RX1_DELTA_IQ(value)		_SET_FV(RX1_DELTA_IQ, value)

/* VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0 */
#define RX0_DELTA_IQ_OFFSET		8
#define RX0_DELTA_IQ_MASK		REG_4BIT_MASK
#define RX0_DELTA_IQ(value)		_SET_FV(RX0_DELTA_IQ, value)

/* VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1 */
#define VCO_FRQBAND_3_OFFSET		14
#define VCO_FRQBAND_3_MASK		REG_2BIT_MASK
#define VCO_FRQBAND_3(value)		_SET_FV(VCO_FRQBAND_3, value)
#define VCO_FRQBAND_2_OFFSET		12
#define VCO_FRQBAND_2_MASK		REG_2BIT_MASK
#define VCO_FRQBAND_2(value)		_SET_FV(VCO_FRQBAND_2, value)
#define VCO_FRQBAND_1_OFFSET		10
#define VCO_FRQBAND_1_MASK		REG_2BIT_MASK
#define VCO_FRQBAND_1(value)		_SET_FV(VCO_FRQBAND_1, value)
#define VCO_FRQBAND_0_OFFSET		8
#define VCO_FRQBAND_0_MASK		REG_2BIT_MASK
#define VCO_FRQBAND_0(value)		_SET_FV(VCO_FRQBAND_0, value)
#define VCO_TEMP_COMP_EN_0			_BV(0)
#define VCO_STEP_CTRL_0			_BV(4)

/* VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4 */
#define RX_DFE_BYP_3_1_OFFSET		9
#define RX_DFE_BYP_3_1_MASK		REG_3BIT_MASK
#define RX_DFE_BYP_3_1(value)		_SET_FV(RX_DFE_BYP_3_1, value)
#define RX_DFE_BYP_0			_BV(8)

/* VR_XS_PMA_Gen5_16G_RX_MISC_CTRL1 */
#define RX3_MISC_OFFSET			8
#define RX3_MISC_MASK			REG_8BIT_MASK
#define RX3_MISC(value)			_SET_FV(RX3_MISC, value)

#define RX2_MISC_OFFSET			0
#define RX2_MISC_MASK			REG_8BIT_MASK
#define RX2_MISC(value)			_SET_FV(RX2_MISC, value)

/* VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0 */
#define RX1_MISC_OFFSET			8
#define RX1_MISC_MASK			REG_8BIT_MASK
#define RX1_MISC(value)			_SET_FV(RX1_MISC, value)

#define RX0_MISC_OFFSET			0
#define RX0_MISC_MASK			REG_8BIT_MASK
#define RX0_MISC(value)			_SET_FV(RX0_MISC, value)

/* VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0 */
#define CTLE_BOOST_0_OFFSET		0
#define CTLE_BOOST_0_MASK		REG_5BIT_MASK
#define CTLE_BOOST_0(value)		_SET_FV(CTLE_BOOST_0, value)

/* VR_XS_PMA_Gen5_16G_RX_EQ_CTRL1 */
#define CTLE_BOOST_1_OFFSET		0
#define CTLE_BOOST_1_MASK		REG_5BIT_MASK
#define CTLE_BOOST_1(value)		_SET_FV(CTLE_BOOST_1, value)
#define CTLE_BOOST_1_LEVEL		6

/* VR_XS_PMA_Gen5_16G_RX_EQ_CTRL2 */
#define CTLE_BOOST_2_OFFSET		0
#define CTLE_BOOST_2_MASK		REG_5BIT_MASK
#define CTLE_BOOST_2(value)		_SET_FV(CTLE_BOOST_2, value)
#define CTLE_BOOST_2_LEVEL		6

/* VR_XS_PMA_Gen5_16G_RX_EQ_CTRL3 */
#define CTLE_BOOST_3_OFFSET		0
#define CTLE_BOOST_3_MASK		REG_5BIT_MASK
#define CTLE_BOOST_3(value)		_SET_FV(CTLE_BOOST_3, value)
#define CTLE_BOOST_3_LEVEL		6

/* VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1 */
#define VBOOST_EN_3_1_OFFSET		5
#define VBOOST_EN_3_1_MASK		REG_3BIT_MASK
#define VBOOST_EN_3_1(value)		_SET_FV(VBOOST_EN_3_1, value)
#define VBOOST_EN_3_1_ENABLE		7
#define VBOOST_EN_0			_BV(4)

/* VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1 */
#define TX_EQ_OVR_RIDE			_BV(6)
#define TX_EQ_POST_OFFSET		0
#define TX_EQ_POST_MASK			REG_6BIT_MASK
#define TX_EQ_POST(value)		_SET_FV(TX_EQ_POST, value)

/* VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0 */
#define TX_EQ_MAIN_OFFSET		8
#define TX_EQ_MAIN_MASK			REG_6BIT_MASK
#define TX_EQ_MAIN(value)		_SET_FV(TX_EQ_MAIN, value)

#define TX_EQ_PRE_OFFSET		0
#define TX_EQ_PRE_MASK			REG_6BIT_MASK
#define TX_EQ_PRE(value)		_SET_FV(TX_EQ_PRE, value)

#define TX_EQ_PRE_LEVEL			0

/* VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2 */
#define MPLLA_DIV_OFFSET		8
#define MPLLA_DIV_MASK			REG_3BIT_MASK
#define MPLLA_DIV(value)		_SET_FV(MPLLA_DIV, value)

/* VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2 */
#define TX3_WIDTH_OFFSET		14
#define TX3_WIDTH_MASK			REG_2BIT_MASK
#define TX3_WIDTH(value)		_SET_FV(TX3_WIDTH, value)

#define TX2_WIDTH_OFFSET		12
#define TX2_WIDTH_MASK			REG_2BIT_MASK
#define TX2_WIDTH(value)		_SET_FV(TX2_WIDTH, value)

#define TX1_WIDTH_OFFSET		10
#define TX1_WIDTH_MASK			REG_2BIT_MASK
#define TX1_WIDTH(value)		_SET_FV(TX1_WIDTH, value)

#define TX0_WIDTH_OFFSET		8
#define TX0_WIDTH_MASK			REG_2BIT_MASK
#define TX0_WIDTH(value)		_SET_FV(TX0_WIDTH, value)
#define WIDTH_10BIT	1
#define WIDTH_20BIT	3

/* VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2 */
#define RX3_WIDTH_OFFSET		14
#define RX3_WIDTH_MASK			REG_2BIT_MASK
#define RX3_WIDTH(value)		_SET_FV(RX3_WIDTH, value)

#define RX2_WIDTH_OFFSET		12
#define RX2_WIDTH_MASK			REG_2BIT_MASK
#define RX2_WIDTH(value)		_SET_FV(RX2_WIDTH, value)

#define RX1_WIDTH_OFFSET		10
#define RX1_WIDTH_MASK			REG_2BIT_MASK
#define RX1_WIDTH(value)		_SET_FV(RX1_WIDTH, value)

#define RX0_WIDTH_OFFSET		8
#define RX0_WIDTH_MASK			REG_2BIT_MASK
#define RX0_WIDTH(value)		_SET_FV(RX0_WIDTH, value)

/* VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL */
#define TX0_RATE_OFFSET			0
#define TX0_RATE_MASK			REG_3BIT_MASK
#define TX0_RATE(value)			_SET_FV(TX0_RATE, value)

#define TX1_RATE_OFFSET			4
#define TX1_RATE_MASK			REG_3BIT_MASK
#define TX1_RATE(value)			_SET_FV(TX1_RATE, value)

#define TX2_RATE_OFFSET			8
#define TX2_RATE_MASK			REG_3BIT_MASK
#define TX2_RATE(value)			_SET_FV(TX2_RATE, value)

#define TX3_RATE_OFFSET			12
#define TX3_RATE_MASK			REG_3BIT_MASK
#define TX3_RATE(value)			_SET_FV(TX3_RATE, value)

/* VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL */
#define RX0_RATE_OFFSET			0
#define RX0_RATE_MASK			REG_2BIT_MASK
#define RX0_RATE(value)			_SET_FV(RX0_RATE, value)

#define RX1_RATE_OFFSET			4
#define RX1_RATE_MASK			REG_2BIT_MASK
#define RX1_RATE(value)			_SET_FV(RX1_RATE, value)

#define RX2_RATE_OFFSET			8
#define RX2_RATE_MASK			REG_2BIT_MASK
#define RX2_RATE(value)			_SET_FV(RX2_RATE, value)

#define RX3_RATE_OFFSET			12
#define RX3_RATE_MASK			REG_2BIT_MASK
#define RX3_RATE(value)			_SET_FV(RX3_RATE, value)

/* VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0 */
#define VR_PMA_MPLLA_MULTIPLIER_OFFSET		0
#define VR_PMA_MPLLA_MULTIPLIER_MASK		REG_8BIT_MASK
#define VR_PMA_MPLLA_MULTIPLIER(value)		\
	_SET_FV(VR_PMA_MPLLA_MULTIPLIER, value)

/* VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL */
#define REF_MPLLA_DIV2		_BV(6)
#define REF_RANGE_OFFSET	3
#define REF_RANGE_MASK		REG_3BIT_MASK
#define REF_RANGE(value)	_SET_FV(REF_RANGE, value)
#define REF_CLK_DIV2		_BV(2)
#define CLK_156P1_182MHZ	6

#define VCO_REF_LD_1_OFFSET	8
#define VCO_REF_LD_1_MASK	REG_7BIT_MASK
#define VCO_REF_LD_1(value)	_SET_FV(VCO_REF_LD_1, value)

#define VCO_REF_LD_0_OFFSET	0
#define VCO_REF_LD_0_MASK	REG_7BIT_MASK
#define VCO_REF_LD_0(value)	_SET_FV(VCO_REF_LD_0, value)

/* VR_XS_PMA_Gen5_16G_VCO_CAL_REF1 */
#define VCO_REF_LD_3_OFFSET	8
#define VCO_REF_LD_3_MASK	REG_7BIT_MASK
#define VCO_REF_LD_3(value)	_SET_FV(VCO_REF_LD_3, value)

#define VCO_REF_LD_2_OFFSET	0
#define VCO_REF_LD_2_MASK	REG_7BIT_MASK
#define VCO_REF_LD_2(value)	_SET_FV(VCO_REF_LD_2, value)

/* VR_XS_PMA_Gen5_12G_16G_SRAM */
#define VR_PMA_EXT_LD_DN	_BV(1)
#define VR_PMA_INIT_DN		_BV(0)

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

/* Optional registers */
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
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD1		VR(0x0093)
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD2		VR(0x0094)
#define VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD3		VR(0x0095)
#endif /* CONFIG_DW_XPCS_SINGLE_LANE */

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

/* Optional registers */
#ifndef CONFIG_DW_XPCS_SINGLE_LANE
#define VR_XS_PMA_Gen5_16G_TX_MISC_CTRL1		VR(0x003F)
#define VR_XS_PMA_Gen5_16G_RX_PPM_STS1			VR(0x0062)
#define VR_XS_PMA_Gen5_16G_RX_PPM_CTRL1			VR(0x0066)
#endif /* CONFIG_DW_XPCS_SINGLE_LANE */

#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define VR_MII_Gen5_12G_16G_SRAM			VR(0x80BB)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */

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

/* VR_MII_Gen5_12G_16G_RX_EQ_CTRL4 */
#define CONT_ADAPT_3_OFFSET			0
#define CONT_ADAPT_3_MASK			REG_4BIT_MASK
#define CONT_ADAPT_3(value)			_SET_FV(CONT_ADAPT_3, value)
#define CONT_ADAPT_0				_BV(0)

#define dw_xpcs_enable_rx_cont_adapt(l)					\
	do {								\
		if ((l) == 4) {						\
			dw_xpcs_write_mask(VS_MII_MMD,			\
				VR_MII_Gen5_12G_16G_RX_EQ_CTRL4,	\
				CONT_ADAPT_3(CONT_ADAPT_MASK),		\
				CONT_ADAPT_3(CONT_ADAPT_MASK));		\
		} else {						\
			dw_xpcs_set(VS_MII_MMD,				\
				VR_MII_Gen5_12G_16G_RX_EQ_CTRL4,	\
				CONT_ADAPT_0);				\
		}							\
	} while (0)
#define dw_xpcs_disable_rx_cont_adapt(l)				\
	do {								\
		if ((l) == 4) {						\
			dw_xpcs_write_mask(VS_MII_MMD,			\
				VR_MII_Gen5_12G_16G_RX_EQ_CTRL4,	\
				CONT_ADAPT_3(0),			\
				CONT_ADAPT_3(CONT_ADAPT_MASK));		\
		} else {						\
			dw_xpcs_clear(VS_MII_MMD,			\
				VR_MII_Gen5_12G_16G_RX_EQ_CTRL4,	\
				CONT_ADAPT_0);				\
		}							\
	} while (0)
#define dw_xpcs_config_rx_adapt_mode(l, v)				\
	do {								\
		if ((l) == 4) {						\
			dw_xpcs_write_mask(PMA_MMD,			\
				VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5,		\
				RX3_ADPT_MODE(v) | RX2_ADPT_MODE(v) |	\
				RX1_ADPT_MODE(v) | RX0_ADPT_MODE(v),	\
				RX3_ADPT_MODE(RX3_ADPT_MODE_MASK) |	\
				RX2_ADPT_MODE(RX2_ADPT_MODE_MASK) |	\
				RX1_ADPT_MODE(RX1_ADPT_MODE_MASK) |	\
				RX0_ADPT_MODE(RX0_ADPT_MODE_MASK));	\
		} else {						\
			dw_xpcs_write_mask(PMA_MMD,			\
				VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5,		\
				RX0_ADPT_MODE(v),			\
				RX0_ADPT_MODE(RX0_ADPT_MODE_MASK));	\
		}							\
	} while (0)

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
#else
#define dw_xpcs_enable_rx_cont_adapt(l)		do { } while (0)
#define dw_xpcs_disable_rx_cont_adapt(l)	do { } while (0)
#define dw_xpcs_config_rx_adapt_mode(l, v)	do { } while (0)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */
#else
#define dw_xpcs_enable_rx_cont_adapt(l)		do { } while (0)
#define dw_xpcs_disable_rx_cont_adapt(l)	do { } while (0)
#define dw_xpcs_config_rx_adapt_mode(l, v)	do { } while (0)
#endif /* CONFIG_DW_XPCS_VS_MII_MMD */

#endif /* __XPCS_PHY_GEN5_16G_H_INCLUDE__ */
