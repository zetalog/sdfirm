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
#include <target/clk.h>

/* 7.2.4 Programming Sequence for Synopsys Enterprise Gen5 12G/16G PHY
 * Table 7-7 Synopsys Enterprise Gen5 16G PHY: 10G XAUI Mode Swiching
 */
void dw_xpcs_switch_xaui_kx4(int nr_lanes, clk_freq_t ref_freq)
{
	int l;

	dw_xpcs_config_xaui();
	dw_xpcs_write(XS_PCS_MMD, SR_XS_PCS_CTRL2,
		      SR_PCS_TYPE(SR_PCS_10000BASE_X));
#ifdef CONFIG_DW_XPCS_BACKPLANE
	dw_xpcs_set(XS_PMA_MMD, SR_PMA_CTRL1, SR_PMA_SS13);
#endif
	dw_xpcs_set(XS_PCS_MMD, SR_XS_PCS_CTRL1, SR_PCS_SS13);
	dw_xpcs_write(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		VR_PMA_MPLLA_MULTIPLIER(40));
	dw_xpcs_config_mplla_bandwidth(41022);
	for (l = 0; l < nr_lanes; l++) {
		dw_xpcs_write(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD(l),
			VR_PMA_VCO_LD_VAL(1360));
		if (ref_freq == 78125000)
			dw_xpcs_write_mask(XS_PMA_MMD,
				VR_XS_PMA_Gen5_16G_VCO_CAL_REF(l),
				VR_PMA_VCO_REF_LD(l, 17),
				VR_PMA_VCO_REF_LD(l, VR_PMA_VCO_REF_LD_MASK));
		else
			dw_xpcs_write_mask(XS_PMA_MMD,
				VR_XS_PMA_Gen5_16G_VCO_CAL_REF(l),
				VR_PMA_VCO_REF_LD(l, 34),
				VR_PMA_VCO_REF_LD(l, VR_PMA_VCO_REF_LD_MASK));
		dw_xpcs_disable_rx_cont_adapt(l);
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
			VR_PMA_TX_RATE(l, VR_PMA_RATE_BAUD_DIV4),
			VR_PMA_TX_RATE(l, VR_PMA_TX_RATE_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
			VR_PMA_RX_RATE(l, VR_PMA_RATE_BAUD_DIV4),
			VR_PMA_RX_RATE(l, VR_PMA_RX_RATE_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
			VR_PMA_TX_WIDTH(l, VR_PMA_WIDTH_10BIT),
			VR_PMA_TX_WIDTH(l, VR_PMA_TX_WIDTH_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
			VR_PMA_RX_WIDTH(l, VR_PMA_WIDTH_10BIT),
			VR_PMA_RX_WIDTH(l, VR_PMA_RX_WIDTH_MASK));
	}
	dw_xpcs_write_mask(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
		VR_PMA_MPLLA_DIV_CLK_EN(VR_PMA_MPLLA_DIV10_CLK),
		VR_PMA_MPLLA_DIV_CLK_EN(VR_PMA_MPLLA_DIV_CLK_EN_MASK));
#if 0
	dw_xpcs_write_mask(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL,
		VR_PMA_REF_MPLLA_DIV2 | VR_PMA_REF_CLK_DIV2 |
		VR_PMA_REF_RANGE(VR_PMA_REF_156P1_182MHZ),
		VR_PMA_REF_MPLLA_DIV2 | VR_PMA_REF_CLK_DIV2 |
		VR_PMA_REF_RANGE(VR_PMA_REF_RANGE_MASK));
#endif
	dw_xpcs_write(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		VR_PMA_TX_EQ_MAIN(40) | VR_PMA_TX_EQ_PRE(0));
	dw_xpcs_write_mask(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		VR_PMA_TX_EQ_POST(0),
		VR_PMA_TX_EQ_POST(VR_PMA_TX_EQ_POST_MASK));
	dw_xpcs_set(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		VR_PMA_TX_EQ_OVR_RIDE);
	for (l = 0; l < nr_lanes; l++) {
		dw_xpcs_set(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			VR_PMA_VBOOST_EN(l));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_EQ_CTRL(l),
			VR_PMA_CTLE_BOOST(6),
			VR_PMA_CTLE_BOOST(VR_PMA_CTLE_BOOST_MASK));
		dw_xpcs_clear(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
			VR_PMA_VCO_STEP_CTRL(l));
		dw_xpcs_clear(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
			VR_PMA_VCO_TEMP_COMP_EN(l));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_MISC_CTRL(l),
			VR_PMA_RX_MISC(l, 7),
			VR_PMA_RX_MISC(l, VR_PMA_RX_MISC_MASK));
		dw_xpcs_set(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			VR_PMA_RX_DFE_BYP(l));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_IQ_CTRL(l),
			VR_PMA_VCO_FRQBAND(l, 2),
			VR_PMA_VCO_FRQBAND(l, VR_PMA_VCO_FRQBAND_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_IQ_CTRL(l),
			VR_PMA_RX_DELTA_IQ(0),
			VR_PMA_RX_DELTA_IQ(VR_PMA_RX_DELTA_IQ_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5,
			VR_PMA_RX_ADPT_MODE(l, VR_PMA_RX_ADPT_PCS),
			VR_PMA_RX_ADPT_MODE(l, VR_PMA_RX_ADPT_MODE_MASK));
	}
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
	dw_xpcs_hw_sram_init();
	while (dw_xpcs_read(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1) & VR_RST);
}

/* 7.3.3 Programming Sequence for Synopsys Enterprise Gen5 12G/16G PHY
 * Table 7-13 Synopsys Enterprise Gen5 16G PHY: 10G RXAUI Mode Swiching
 */
void dw_xpcs_switch_rxaui(int nr_lanes, clk_freq_t ref_freq)
{
	int l;

	dw_xpcs_config_rxaui();
	dw_xpcs_write(XS_PCS_MMD, SR_XS_PCS_CTRL2,
		      SR_PCS_TYPE(SR_PCS_10000BASE_X));
#ifdef CONFIG_DW_XPCS_BACKPLANE
	dw_xpcs_set(XS_PMA_MMD, SR_PMA_CTRL1, SR_PMA_SS13);
#endif
	dw_xpcs_set(XS_PCS_MMD, SR_XS_PCS_CTRL1, SR_PCS_SS13);
	dw_xpcs_write(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		VR_PMA_MPLLA_MULTIPLIER(40));
	dw_xpcs_config_mplla_bandwidth(41022);
	for (l = 0; l < nr_lanes; l++) {
		dw_xpcs_write(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD(l),
			VR_PMA_VCO_LD_VAL(1360));
		if (ref_freq == 78125000)
			dw_xpcs_write_mask(XS_PMA_MMD,
				VR_XS_PMA_Gen5_16G_VCO_CAL_REF(l),
				VR_PMA_VCO_REF_LD(l, 17),
				VR_PMA_VCO_REF_LD(l, VR_PMA_VCO_REF_LD_MASK));
		else
			dw_xpcs_write_mask(XS_PMA_MMD,
				VR_XS_PMA_Gen5_16G_VCO_CAL_REF(l),
				VR_PMA_VCO_REF_LD(l, 34),
				VR_PMA_VCO_REF_LD(l, VR_PMA_VCO_REF_LD_MASK));
		dw_xpcs_disable_rx_cont_adapt(l);
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
			VR_PMA_TX_RATE(l, VR_PMA_RATE_BAUD_DIV4),
			VR_PMA_TX_RATE(l, VR_PMA_TX_RATE_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
			VR_PMA_RX_RATE(l, VR_PMA_RATE_BAUD_DIV4),
			VR_PMA_RX_RATE(l, VR_PMA_RX_RATE_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
			VR_PMA_TX_WIDTH(l, VR_PMA_WIDTH_20BIT),
			VR_PMA_TX_WIDTH(l, VR_PMA_TX_WIDTH_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
			VR_PMA_RX_WIDTH(l, VR_PMA_WIDTH_20BIT),
			VR_PMA_RX_WIDTH(l, VR_PMA_RX_WIDTH_MASK));
	}
	dw_xpcs_write_mask(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
		VR_PMA_MPLLA_DIV_CLK_EN(VR_PMA_MPLLA_DIV10_CLK),
		VR_PMA_MPLLA_DIV_CLK_EN(VR_PMA_MPLLA_DIV_CLK_EN_MASK));
#if 0
	dw_xpcs_write_mask(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL,
		VR_PMA_REF_MPLLA_DIV2 | VR_PMA_REF_CLK_DIV2 |
		VR_PMA_REF_RANGE(VR_PMA_REF_156P1_182MHZ),
		VR_PMA_REF_MPLLA_DIV2 | VR_PMA_REF_CLK_DIV2 |
		VR_PMA_REF_RANGE(VR_PMA_REF_RANGE_MASK));
#endif
	dw_xpcs_write(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		VR_PMA_TX_EQ_MAIN(30) | VR_PMA_TX_EQ_PRE(0));
	dw_xpcs_write_mask(XS_PMA_MMD,
		VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		VR_PMA_TX_EQ_POST(40),
		VR_PMA_TX_EQ_POST(VR_PMA_TX_EQ_POST_MASK));
	dw_xpcs_set(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		VR_PMA_TX_EQ_OVR_RIDE);
	for (l = 0; l < nr_lanes; l++) {
		dw_xpcs_set(XS_PMA_MMD,
			VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			VR_PMA_VBOOST_EN(l));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_EQ_CTRL(l),
			VR_PMA_CTLE_BOOST(6),
			VR_PMA_CTLE_BOOST(VR_PMA_CTLE_BOOST_MASK));
		dw_xpcs_clear(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
			VR_PMA_VCO_STEP_CTRL(l));
		dw_xpcs_clear(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
			VR_PMA_VCO_TEMP_COMP_EN(l));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_MISC_CTRL(l),
			VR_PMA_RX_MISC(l, 5),
			VR_PMA_RX_MISC(l, VR_PMA_RX_MISC_MASK));
		dw_xpcs_clear(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			VR_PMA_RX_DFE_BYP(l));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_IQ_CTRL(l),
			VR_PMA_VCO_FRQBAND(l, 2),
			VR_PMA_VCO_FRQBAND(l, VR_PMA_VCO_FRQBAND_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_IQ_CTRL(l),
			VR_PMA_RX_DELTA_IQ(0),
			VR_PMA_RX_DELTA_IQ(VR_PMA_RX_DELTA_IQ_MASK));
		dw_xpcs_write_mask(XS_PMA_MMD,
			VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5,
			VR_PMA_RX_ADPT_MODE(l, VR_PMA_RX_ADPT_RPCS),
			VR_PMA_RX_ADPT_MODE(l, VR_PMA_RX_ADPT_MODE_MASK));
	}
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
	dw_xpcs_hw_sram_init();
	while (dw_xpcs_read(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1) & VR_RST);
}

void dw_xpcs_switch_kx(void)
{
	/* KX mode 1G */
	dw_xpcs_write(XS_PMA_MMD, SR_PMA_CTRL1, SR_PMA_SS_1G);
	dw_xpcs_clear(XS_PCS_MMD, SR_XS_PCS_CTRL1, SR_PCS_SS13);
	dw_xpcs_mii_select_speed(DW_XPCS_SGMII_1000);
	dw_xpcs_mii_disable_an();
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
			   RX0_MISC(RX0_MISC_VAL), RX0_MISC(RX0_MISC_MASK));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_0_VAL, RX_DFE_BYP_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(1, RX_ADPT_MODE_VAL);
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_kr(void)
{
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_usxgmii(void)
{
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, USXG_EN);
	dw_xpcs_write_mask(XS_PCS_MMD, VR_XS_PCS_KR_CTRL,
			   USXG_MODE(USXG_MODE_10G_SXGMII),
			   USXG_MODE(USXG_MODE_MASK));
	dw_xpcs_mii_select_speed(DW_XPCS_USXGMII_10000);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_enable_rx_cont_adapt(1);
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_qsgmii(void)
{
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
}

void dw_xpcs_switch_gmii(void)
{
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0,
		      VCO_LD_VAL_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));
	dw_xpcs_disable_rx_cont_adapt(4);
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) |
		      TX2_RATE(BAUD_DIV4) | TX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) |
		      RX2_RATE(BAUD_DIV4) | RX3_RATE(BAUD_DIV4));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS) | TX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS) | RX0_WIDTH(WIDTH_BITS));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2,
			   MPLLA_DIV(MPLLA_DIV_CLK_EN),
			   MPLLA_DIV(MPLLA_DIV_MASK));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));
	dw_xpcs_write(XS_PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0,
			   VBOOST_EN_3_1(VBOOST_EN_3_1_MASK) | VBOOST_EN_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0,
			   CTLE_BOOST_0(CTLE_BOOST_0_LEVEL),
			   CTLE_BOOST_0(CTLE_BOOST_0_MASK));
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0,
		      RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) |
			   RX_DFE_BYP_0_VAL,
			   RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_MASK) |
			   RX_DFE_BYP_0);
	dw_xpcs_write(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1,
		      VCO_FRQBAND_0(VCO_FRQBAND_VAL) |
		      VCO_TEMP_COMP_EN_0 | VCO_STEP_CTRL_0);
	dw_xpcs_write_mask(XS_PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0,
			   RX0_DELTA_IQ(RX0_DELTA_IQ_VAL),
			   RX0_DELTA_IQ(RX0_DELTA_IQ_MASK));
	dw_xpcs_config_rx_adapt_mode(4, RX_ADPT_MODE_VAL);
	dw_xpcs_set(XS_PCS_MMD, VR_XS_PCS_DIG_CTRL1, VR_RST);
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
