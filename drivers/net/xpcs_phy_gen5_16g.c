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
 * @(#)xpcs_phy_gen5_16g.c: Synopsys DesignWare GEN5-16G phy
 * $Id: xpcs_phy_gen5_16g.c,v 1.0 2022-01-13 17:20:00 zhenglv Exp $
 */

#include <target/eth.h>

/* 7.2 Switching to 10G XAUI Mode or KX4 Mode */
void dw_xpcs_switch_xaui_kx4(void)
{
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL,
			   REF_MPLLA_DIV2 | REF_RANGE(CLK_156P1_182MHZ) |
			   REF_CLK_DIV2,
			   REF_MPLLA_DIV2 | REF_RANGE(REF_RANGE_MASK) |
			   REF_CLK_DIV2);
	dw_xpcs_enable_rx_cont_adapt(1);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL,
			   REF_MPLLA_DIV2 | REF_RANGE(CLK_156P1_182MHZ) |
			   REF_CLK_DIV2,
			   REF_MPLLA_DIV2 | REF_RANGE(REF_RANGE_MASK) |
			   REF_CLK_DIV2);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD1,
		      VCO_LD_VAL_1);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD2,
		      VCO_LD_VAL_2);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD3,
		      VCO_LD_VAL_3);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF1,
		      VCO_REF_LD_3(VCO_REF_LD_3_VAL) |
		      VCO_REF_LD_2(VCO_REF_LD_2_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL1,
			   CTLE_BOOST_1(CTLE_BOOST_1_LEVEL),
			   CTLE_BOOST_1(CTLE_BOOST_1_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL2,
			   CTLE_BOOST_2(CTLE_BOOST_2_LEVEL),
			   CTLE_BOOST_2(CTLE_BOOST_2_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL3,
			   CTLE_BOOST_3(CTLE_BOOST_3_LEVEL),
			   CTLE_BOOST_3(CTLE_BOOST_3_MASK));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL1,
		      RX3_MISC(RX3_MISC_VAL) | RX2_MISC(RX2_MISC_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_3(VCO_FRQBAND_VAL) |
		      VCO_FRQBAND_2(VCO_FRQBAND_VAL) |
		      VCO_FRQBAND_1(VCO_FRQBAND_VAL) |
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL1,
			   RX1_DELTA_IQ(RX1_DELTA_IQ_VAL),
			   RX1_DELTA_IQ(RX1_DELTA_IQ_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL2,
			   RX2_DELTA_IQ(RX2_DELTA_IQ_VAL),
			   RX2_DELTA_IQ(RX2_DELTA_IQ_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL3,
			   RX3_DELTA_IQ(RX3_DELTA_IQ_VAL),
			   RX3_DELTA_IQ(RX3_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_rxaui(void)
{
	/* KX4 mode 10G */
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, USXG_EN);
	dw_xpcs_write_mask(PCS_MMD, VR_XS_PCS_KR_CTRL,
			   USXG_MODE(USXG_MODE_10G_SXGMII),
			   USXG_MODE(USXG_MODE_MASK));
	dw_xpcs_write(PCS_MMD, VR_XS_PCS_XAUI_CTRL, RXAUI_MODE);
	dw_xpcs_set(PCS_MMD, SR_XS_PCS_CTRL1, SR_PCS_SS13);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD1,
		      VCO_LD_VAL_1);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD2,
		      VCO_LD_VAL_2);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD3,
		      VCO_LD_VAL_3);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF1,
		      VCO_REF_LD_3(VCO_REF_LD_3_VAL) |
		      VCO_REF_LD_2(VCO_REF_LD_2_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL1,
			   CTLE_BOOST_1(CTLE_BOOST_1_LEVEL),
			   CTLE_BOOST_1(CTLE_BOOST_1_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL2,
			   CTLE_BOOST_2(CTLE_BOOST_2_LEVEL),
			   CTLE_BOOST_2(CTLE_BOOST_2_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL3,
			   CTLE_BOOST_3(CTLE_BOOST_3_LEVEL),
			   CTLE_BOOST_3(CTLE_BOOST_3_MASK));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL1,
		      RX3_MISC(RX3_MISC_VAL) | RX2_MISC(RX2_MISC_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_3(VCO_FRQBAND_VAL) |
		      VCO_FRQBAND_2(VCO_FRQBAND_VAL) |
		      VCO_FRQBAND_1(VCO_FRQBAND_VAL) |
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL1,
			   RX1_DELTA_IQ(RX1_DELTA_IQ_VAL),
			   RX1_DELTA_IQ(RX1_DELTA_IQ_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL2,
			   RX2_DELTA_IQ(RX2_DELTA_IQ_VAL),
			   RX2_DELTA_IQ(RX2_DELTA_IQ_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL3,
			   RX3_DELTA_IQ(RX3_DELTA_IQ_VAL),
			   RX3_DELTA_IQ(RX3_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL0,
			   TX_DT_EN_3_1(TX_DT_EN_1) | TX_DT_EN_0,
			   TX_DT_EN_3_1(TX_DT_EN_3_1_MASK) | TX_DT_EN_0);
}

void dw_xpcs_switch_kx(void)
{
	/* KX mode 1G */
	dw_xpcs_write(PMA_MMD, SR_PMA_CTRL1, SR_PMA_SS_1G);
	dw_xpcs_clear(PCS_MMD, SR_XS_PCS_CTRL1, SR_PCS_SS13);
	dw_xpcs_mii_select_speed(DW_XPCS_SGMII_1000);
	dw_xpcs_mii_disable_an();
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
			   RX0_MISC(RX0_MISC_VAL), RX0_MISC(RX0_MISC_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_0_VAL, RX_DFE_BYP_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(1, RX_ADPT_MODE_VAL);
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_kr(void)
{
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_usxgmii(void)
{
	dw_xpcs_mii_select_speed(DW_XPCS_USXGMII_10000);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_enable_rx_cont_adapt(1);
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_qsgmii(void)
{
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_gmii(void)
{
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_config_cl37_an(void)
{
}

void dw_xpcs_config_cl72_training(void)
{
}

void dw_xpcs_config_cl73_an(void)
{
}

void dw_xpcs_switch_mode(uint8_t mode)
{
	switch (mode) {
	case PHY_INTFC_USXGMII:
		dw_xpcs_switch_usxgmii();
		break;
	case PHY_INTFC_GMII:
		dw_xpcs_switch_gmii();
		break;
	case PHY_INTFC_RXAUI:
		dw_xpcs_switch_rxaui();
		break;
	case PHY_INTFC_SGMII:
	case PHY_INTFC_1000BASE_KX:
		dw_xpcs_switch_kx();
		break;
	case PHY_INTFC_XAUI:
	case PHY_INTFC_10000BASE_KX4:
		dw_xpcs_switch_xaui_kx4();
		break;
	case PHY_INTFC_10000BASE_KR:
		dw_xpcs_switch_kr();
		break;
	}
}
