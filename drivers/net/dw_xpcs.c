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
 * @(#)dw_xpcs.c: Synopsys DesignWare ethernet PCS implementation
 * $Id: dw_xpcs.c,v 1.0 2020-12-20 17:20:00 zhenglv Exp $
 */

#include <target/eth.h>

#ifdef CONFIG_ARCH_IS_DW_XPCS_APB
#ifdef CONFIG_ARCH_IS_DW_XPCS_APB_INDIRECT
#define DWCXS_APBI_MMD			0xFF
#define DWCXS_APBI_BASE(addr)		((addr) >> 8)
#define DWCXS_APBI_ADDR(addr)		((addr) & 0xff)
#define DWCXS_APBI_REG(offset)		\
	DWCXS_REG((offset) << DW_XPCS_APBI_OFFSET)

uint16_t xpcs_base;

uint16_t dw_xpcs_read(uint16_t mmd, uint16_t addr)
{
	caddr_t reg = (caddr_t)mmd << 16 | addr;
	uint16_t base = DWCXS_APBI_BASE(reg);

	if (xpcs_base != base) {
		xpcs_base = base;
		__raw_writew(base, DWCXS_APBI_REG(DWCXS_APBI_MMD));
	}
	return __raw_readw(DWCXS_APBI_REG(DWCXS_APBI_ADDR(reg)));
}

void dw_xpcs_write(uint16_t mmd, uint16_t addr, uint16_t value)
{
	caddr_t reg = (caddr_t)mmd << 16 | addr;
	uint16_t base = DWCXS_APBI_BASE(reg);

	if (xpcs_base != base) {
		xpcs_base = base;
		__raw_writew(base, DWCXS_APBI_REG(DWCXS_APBI_MMD));
	}
	__raw_writew(value, DWCXS_APBI_REG(DWCXS_APBI_ADDR(reg)));
}
#else /* CONFIG_ARCH_IS_DW_XPCS_APB_INDIRECT */
uint16_t dw_xpcs_read(uint16_t mmd, uint16_t addr)
{
	caddr_t reg = (caddr_t)mmd << 16 | addr;

	return (uint16_t)__raw_readl(reg);
}

void dw_xpcs_write(uint16_t mmd, uint16_t addr, uint16_t value)
{
	caddr_t reg = (caddr_t)mmd << 16 | addr;

	__raw_writel((uint32_t)value, reg);
}
#endif /* CONFIG_ARCH_IS_DW_XPCS_APB_INDIRECT */
#endif /* CONFIG_ARCH_IS_DW_XPCS_APB */

#ifdef CONFIG_ARCH_IS_DW_XPCS_MDIO
uint16_t dw_xpcs_read(uint16_t mmd, uint16_t addr)
{
	uint16_t value = 0;
	uint32_t reg = MII_ADDR_C45 | (uint32_t)mmd << 16 | addr;

	(void)mdio_read(DW_XPCS_PHY_ADDR, reg, &value);
	return value;
}

void dw_xpcs_write(uint16_t mmd, uint16_t addr, uint16_t value)
{
	uint32_t reg = MII_ADDR_C45 | (uint32_t)mmd << 16 | addr;

	(void)mdio_write(DW_XPCS_PHY_ADDR, reg, value);
}
#endif /* CONFIG_ARCH_IS_DW_XPCS_MDIO */

void dw_xpcs_switch_mode(void)
{
	uint32_t val = 0;
	uint32_t val_mask = 0;

#ifdef CONFIG_DUOWEN_ETH_USXGMII_AN37
	val = dw_xpcs_read(PCS_MMD, VR_XS_PCS_DIG_CTRL1);
	val |= USXG_EN;
	dw_xpcs_write(PCS_MMD, VR_XS_PCS_DIG_CTRL1, val);

	val = dw_xpcs_read(PCS_MMD, VR_XS_PCS_KR_CTRL);
	val_mask = (USXG_MODE_MASK << USXG_MODE_OFFSET);
	val &= (~val_mask);
	val |= USXG_MODE_10G_SXGMII;
	dw_xpcs_write(PCS_MMD, VR_XS_PCS_KR_CTRL, val);
#endif
#ifdef CONFIG_DUOWEN_ETH_SGMII_1000M
	dw_xpcs_write(PMA_MMD, SR_PMA_CTRL1, SR_PMA_SS_1G);
#endif

#ifdef CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	dw_xpcs_write(PCS_MMD, VR_XS_PCS_XAUI_CTRL, RXAUI_MODE);
#endif

#if defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI || \
	defined CONFIG_DUOWEN_ETH_SGMII_1000M
	val = dw_xpcs_read(PCS_MMD, SR_XS_PCS_CTRL1);
	val_mask = SR_PCS_SS13;
	val &= (~val_mask);
	val |= PCS_SPEED_SELECTION;
	dw_xpcs_write(PCS_MMD, SR_XS_PCS_CTRL1, val);
#endif

#if defined CONFIG_DUOWEN_ETH_SGMII_1000M || \
	defined CONFIG_DUOWEN_ETH_USXGMII_AN37
	val = dw_xpcs_read(VS_MII_MMD, SR_MII_CTRL);
#ifdef CONFIG_DUOWEN_ETH_SGMII_1000M
	val_mask = SR_MII_SS13 | SR_MII_AN_ENABLE | SR_MII_SS6;
#else
	val_mask = SR_MII_SS13 | SR_MII_SS6 | SR_MII_SS5;
#endif
	val &= (~val_mask);
#ifdef CONFIG_DUOWEN_ETH_SGMII_1000M
	val |= SR_MII_SS13;
#else
	val |= SR_MII_SS13 | SR_MII_SS6;
#endif
	dw_xpcs_write(VS_MII_MMD, SR_MII_CTRL, val);
#endif

	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL0,
		      VR_PMA_MPLLA_MULTIPLIER(MPLLA_MULTIPLIER));

#ifdef CONFIG_DUOWEN_ETH_10000BASE_KX4
	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL);
	val_mask = REF_MPLLA_DIV2 | (REF_RANGE_MASK << REF_RANGE_OFFSET) |
		REF_CLK_DIV2;
	val &= (~val_mask);
	val |= (REF_MPLLA_DIV2 | REF_RANGE(CLK_156P1_182MHZ) | REF_CLK_DIV2);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_REF_CLK_CTRL, val);
#endif

	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD0, VCO_LD_VAL_0);

#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD1, VCO_LD_VAL_1);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD2, VCO_LD_VAL_2);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_VCO_CAL_LD3, VCO_LD_VAL_3);
#endif

	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF0,
		      VCO_REF_LD_1(VCO_REF_LD_1_VAL) |
		      VCO_REF_LD_0(VCO_REF_LD_0_VAL));

#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_VCO_CAL_REF1,
		      VCO_REF_LD_3(VCO_REF_LD_3_VAL) |
		      VCO_REF_LD_2(VCO_REF_LD_2_VAL));
#endif
	val = dw_xpcs_read(VS_MII_MMD, VR_MII_Gen5_12G_16G_RX_EQ_CTRL4);
#ifndef CONFIG_DUOWEN_ETH_USXGMII_AN37
	val_mask = (CONT_ADAPT_3_1_MASK << CONT_ADAPT_3_1_OFFSET) |
		CONT_ADAPT_0;
#else
	val_mask = CONT_ADAPT_0;
#endif
	val &= (~val_mask);

#ifndef CONFIG_DUOWEN_ETH_USXGMII_AN37
	val |= CONT_ADAPT_DISABLE;
#else
	val |= CONT_ADAPT_0_ENABLE;
#endif
	dw_xpcs_write(VS_MII_MMD,
		      VR_MII_Gen5_12G_16G_RX_EQ_CTRL4, val);

	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_RATE_CTRL,
		      TX0_RATE(BAUD_DIV4) | TX1_RATE(BAUD_DIV4) | TX2_RATE(BAUD_DIV4)
		      | TX3_RATE(BAUD_DIV4));

	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_RATE_CTRL,
		      RX0_RATE(BAUD_DIV4) | RX1_RATE(BAUD_DIV4) | RX2_RATE(BAUD_DIV4)
		      | RX3_RATE(BAUD_DIV4));

	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL2,
		      TX3_WIDTH(WIDTH_BITS) | TX2_WIDTH(WIDTH_BITS) |
		      TX1_WIDTH(WIDTH_BITS)
		      | TX0_WIDTH(WIDTH_BITS));

	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_RX_GENCTRL2,
		      RX3_WIDTH(WIDTH_BITS) | RX2_WIDTH(WIDTH_BITS) |
		      RX1_WIDTH(WIDTH_BITS)
		      | RX0_WIDTH(WIDTH_BITS));

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2);
	val_mask = (MPLLA_DIV_MASK << MPLLA_DIV_OFFSET);
	val &= (~val_mask);
	val |= MPLLA_DIV(MPLLA_DIV_CLK_EN);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_MPLLA_CTRL2, val);

#ifndef CONFIG_DUOWEN_ETH_USXGMII_AN37
	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL0,
		      TX_EQ_PRE(TX_EQ_PRE_LEVEL) |
		      TX_EQ_MAIN(TX_EQ_MAIN_AMPLITUDE));

	dw_xpcs_write(PMA_MMD,
		      VR_XS_PMA_Gen5_12G_16G_TX_EQ_CTRL1,
		      TX_EQ_OVR_RIDE |
		      TX_EQ_POST(TX_EQ_POST_LEVEL));
#endif
	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1);
	val_mask = (VBOOST_EN_3_1_MASK << VBOOST_EN_3_1_OFFSET) |
		VBOOST_EN_0;
	val &= (~val_mask);
	val |= (VBOOST_EN_3_1(VBOOST_EN_3_1_ENABLE) | VBOOST_EN_0);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL1, val);

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0);
	val_mask = (CTLE_BOOST_0_MASK << CTLE_BOOST_0_OFFSET);
	val &= (~val_mask);
	val |= CTLE_BOOST_0(CTLE_BOOST_0_LEVEL);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL0, val);

#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL1);
	val_mask = (CTLE_BOOST_1_MASK << CTLE_BOOST_1_OFFSET);
	val &= (~val_mask);
	val |= CTLE_BOOST_1(CTLE_BOOST_1_LEVEL);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL1, val);

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL2);
	val_mask = (CTLE_BOOST_2_MASK << CTLE_BOOST_2_OFFSET);
	val &= (~val_mask);
	val |= CTLE_BOOST_2(CTLE_BOOST_2_LEVEL);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL2, val);

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL3);
	val_mask = (CTLE_BOOST_3_MASK << CTLE_BOOST_3_OFFSET);
	val &= (~val_mask);
	val |= CTLE_BOOST_3(CTLE_BOOST_3_LEVEL);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL3, val);
#endif

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0);
#ifdef CONFIG_DUOWEN_ETH_SGMII_1000M
	val_mask = (RX0_MISC_MASK <<  RX0_MISC_OFFSET);
	val &= (~val_mask);
	val |= RX0_MISC_VAL;
#else
	val = 0;
	val |= (RX1_MISC(RX1_MISC_VAL) | RX0_MISC(RX0_MISC_VAL));
#endif
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL0, val);

#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_MISC_CTRL1,
		      (RX3_MISC(RX3_MISC_VAL) | RX2_MISC(RX2_MISC_VAL)));

#endif

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4);
#ifdef CONFIG_DUOWEN_ETH_SGMII_1000M
	val_mask = RX_DFE_BYP_0;
	val &= (~val_mask);
	val |= RX_DFE_BYP_0_VAL;
#else
	val_mask = (RX_DFE_BYP_3_1_MASK <<  RX_DFE_BYP_3_1_OFFSET) |
		RX_DFE_BYP_0;
	val &= (~val_mask);
	val |= (RX_DFE_BYP_3_1(RX_DFE_BYP_3_1_VAL) | RX_DFE_BYP_0_VAL);
#endif
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_GEN_CTRL4, val);

#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI

	val = VCO_FRQBAND_3(VCO_FRQBAND_VAL)
		| VCO_FRQBAND_2(VCO_FRQBAND_VAL)
		| VCO_FRQBAND_1(VCO_FRQBAND_VAL)
		| VCO_FRQBAND_0(VCO_FRQBAND_VAL);
#else
	val = VCO_FRQBAND_0(VCO_FRQBAND_VAL)
		| VCO_TEMP_COMP_EN_0
		| VCO_STEP_CTRL_0;
#endif
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_CDR_CTRL1, val);

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0);
	val_mask = (RX0_DELTA_IQ_MASK <<  RX0_DELTA_IQ_OFFSET);
	val &= (~val_mask);
	val |= RX0_DELTA_IQ(RX0_DELTA_IQ_VAL);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL0, val);

#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL1);
	val_mask = (RX1_DELTA_IQ_MASK <<  RX1_DELTA_IQ_OFFSET);
	val &= (~val_mask);
	val |= RX1_DELTA_IQ_VAL;
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL1, val);
#endif
#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL2);
	val_mask = (RX2_DELTA_IQ_MASK <<  RX2_DELTA_IQ_OFFSET);
	val &= (~val_mask);
	val |= RX2_DELTA_IQ_VAL;
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL2, val);
#endif
#if defined CONFIG_DUOWEN_ETH_10000BASE_KX4 || \
	defined CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL3);
	val_mask = (RX3_DELTA_IQ_MASK <<  RX3_DELTA_IQ_OFFSET);
	val &= (~val_mask);
	val |= RX3_DELTA_IQ_VAL;
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_IQ_CTRL3, val);
#endif

	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5);
#ifndef CONFIG_DUOWEN_ETH_SGMII_1000M
	val_mask = (RX3_ADPT_MODE_MASK <<  RX3_ADPT_MODE_OFFSET) |
		(RX2_ADPT_MODE_MASK <<  RX2_ADPT_MODE_OFFSET) |
		(RX1_ADPT_MODE_MASK <<  RX1_ADPT_MODE_OFFSET);
	val &= (~val_mask);
#endif
	val_mask = (RX0_ADPT_MODE_MASK <<  RX0_ADPT_MODE_OFFSET);
	val &= (~val_mask);
	val |= RX3_ADPT_MODE(RX_ADPT_MODE_VAL)
		| RX2_ADPT_MODE(RX_ADPT_MODE_VAL)
		| RX1_ADPT_MODE(RX_ADPT_MODE_VAL)
		| RX0_ADPT_MODE(RX_ADPT_MODE_VAL);
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_16G_RX_EQ_CTRL5, val);

	val = dw_xpcs_read(PCS_MMD, VR_XS_PCS_DIG_CTRL1);
	val &= ~VR_RST;
	val |= VR_RST;
	dw_xpcs_write(PCS_MMD, VR_XS_PCS_DIG_CTRL1, val);

#ifdef CONFIG_DUOWEN_ETH_10000BASE_RXAUI
	val = dw_xpcs_read(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL0);
	val_mask = (TX_DT_EN_3_1_MASK <<  TX_DT_EN_3_1_OFFSET) | TX_DT_EN_0;
	val &= (~val_mask);
	val |= TX_DT_EN_3_1(TX_DT_EN_1);
	val |= TX_DT_EN_0;
	dw_xpcs_write(PMA_MMD, VR_XS_PMA_Gen5_12G_16G_TX_GENCTRL0, val);
#endif
}

void dw_xpcs_link_up(void)
{
	dw_xpcs_write(PMA_MMD, SR_PMA_CTRL1,
		      SR_PMA_RST | SR_PMA_SS_10G);
	dw_xpcs_hw_sram_init();
	dw_xpcs_power_init();

	dw_xpcs_an_init();

	dw_xpcs_hw_ctrl_init();

	dw_xpcs_write(PMA_MMD, SR_PMA_CTRL2,
		      SR_PMA_TYPE(DW_XPCS_PMA_TYPE));
	dw_xpcs_hw_sram_init();
	dw_xpcs_power_init();

	if (DW_XPCS_PCS_TYPE !=
	    sr_pcs_type(dw_xpcs_read(XS_PCS_MMD, SR_XS_PCS_CTRL2))) {
		dw_xpcs_write(XS_PCS_MMD, SR_XS_PCS_CTRL2,
			      SR_PCS_TYPE(DW_XPCS_PCS_TYPE));
	}
	while (!(dw_xpcs_read(XS_PCS_MMD, SR_XS_PCS_STS1) & SR_PCS_RLU));
}
