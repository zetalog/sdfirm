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
 * @(#)agera.c: QDF2400 specific agera implementation
 * $Id: agera.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <string.h>
#include <target/cpus.h>
#include <target/delay.h>
#include <target/clk.h>

#define QFPROM_CORR_SPARE_REG27_ROW33_LSB	SEC_CTL_REG(0x00006498)
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_RZVD1_BMSK	0x80000000
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_RZVD1_SHFT	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC02_CLK_CFG_OVR_DCA_VAL_BMSK	0x7c000000
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC02_CLK_CFG_OVR_DCA_VAL_SHFT	0x1a
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC01_CLK_CFG_OVR_DCA_VAL_BMSK	0x3e00000
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC01_CLK_CFG_OVR_DCA_VAL_SHFT	0x15
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC00_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f0000
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC00_CLK_CFG_OVR_DCA_VAL_SHFT	0x10
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_RZVD0_BMSK	0xff00
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_RZVD0_SHFT	0x8
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_MUX_SEL_BMSK	0xf0
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_MUX_SEL_SHFT	0x4
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_DCA_EN_BMSK	0x8
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_DCA_EN_SHFT	0x3
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_INV_EN_BMSK	0x4
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_INV_EN_SHFT	0x2
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_TDL_SEL_BMSK	0x2
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_TDL_SEL_SHFT	0x1
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_EN_BMSK	0x1
#define QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_EN_SHFT	0x0

#define QFPROM_CORR_SPARE_REG27_ROW33_MSB	SEC_CTL_REG(0x0000649c)
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_RZVD1_BMSK	0x80000000
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_RZVD1_SHFT	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_FEC_VALUE_BMSK	0x7f000000
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_FEC_VALUE_SHFT	0x18
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_RZVD0_BMSK	0xf00000
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_RZVD0_SHFT	0x14
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC06_CLK_CFG_OVR_DCA_VAL_BMSK	0xf8000
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC06_CLK_CFG_OVR_DCA_VAL_SHFT	0xf
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC05_CLK_CFG_OVR_DCA_VAL_BMSK	0x7c00
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC05_CLK_CFG_OVR_DCA_VAL_SHFT	0xa
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC04_CLK_CFG_OVR_DCA_VAL_BMSK	0x3e0
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC04_CLK_CFG_OVR_DCA_VAL_SHFT	0x5
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC03_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC03_CLK_CFG_OVR_DCA_VAL_SHFT	0x0

#define QFPROM_CORR_SPARE_REG27_ROW34_LSB	SEC_CTL_REG(0x000064a0)
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_RZVD0_BMSK	0xc0000000
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_RZVD0_SHFT	0x1e
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC12_CLK_CFG_OVR_DCA_VAL_BMSK	0x3e000000
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC12_CLK_CFG_OVR_DCA_VAL_SHFT	0x19
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC11_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f00000
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC11_CLK_CFG_OVR_DCA_VAL_SHFT	0x14
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC10_CLK_CFG_OVR_DCA_VAL_BMSK	0xf8000
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC10_CLK_CFG_OVR_DCA_VAL_SHFT	0xf
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC09_CLK_CFG_OVR_DCA_VAL_BMSK	0x7c00
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC09_CLK_CFG_OVR_DCA_VAL_SHFT	0xa
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC08_CLK_CFG_OVR_DCA_VAL_BMSK	0x3e0
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC08_CLK_CFG_OVR_DCA_VAL_SHFT	0x5
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC07_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC07_CLK_CFG_OVR_DCA_VAL_SHFT	0x0

#define QFPROM_CORR_SPARE_REG27_ROW34_MSB	SEC_CTL_REG(0x000064a4)
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_RZVD1_BMSK	0x80000000
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_RZVD1_SHFT	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_FEC_VALUE_BMSK	0x7f000000
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_FEC_VALUE_SHFT	0x18
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_RZVD0_BMSK	0xf00000
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_RZVD0_SHFT	0x14
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC16_CLK_CFG_OVR_DCA_VAL_BMSK	0xf8000
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC16_CLK_CFG_OVR_DCA_VAL_SHFT	0xf
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC15_CLK_CFG_OVR_DCA_VAL_BMSK	0x7c00
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC15_CLK_CFG_OVR_DCA_VAL_SHFT	0xa
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC14_CLK_CFG_OVR_DCA_VAL_BMSK	0x3e0
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC14_CLK_CFG_OVR_DCA_VAL_SHFT	0x5
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC13_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC13_CLK_CFG_OVR_DCA_VAL_SHFT	0x0

#define QFPROM_CORR_SPARE_REG27_ROW35_LSB	SEC_CTL_REG(0x000064a8)
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_RZVD0_BMSK	0xc0000000
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_RZVD0_SHFT	0x1e
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC22_CLK_CFG_OVR_DCA_VAL_BMSK	0x3e000000
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC22_CLK_CFG_OVR_DCA_VAL_SHFT	0x19
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC21_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f00000
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC21_CLK_CFG_OVR_DCA_VAL_SHFT	0x14
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC20_CLK_CFG_OVR_DCA_VAL_BMSK	0xf8000
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC20_CLK_CFG_OVR_DCA_VAL_SHFT	0xf
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC19_CLK_CFG_OVR_DCA_VAL_BMSK	0x7c00
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC19_CLK_CFG_OVR_DCA_VAL_SHFT	0xa
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC18_CLK_CFG_OVR_DCA_VAL_BMSK	0x3e0
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC18_CLK_CFG_OVR_DCA_VAL_SHFT	0x5
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC17_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC17_CLK_CFG_OVR_DCA_VAL_SHFT	0x0

#define QFPROM_CORR_SPARE_REG27_ROW35_MSB	SEC_CTL_REG(0x000064ac)
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_RZVD1_BMSK	0x80000000
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_RZVD1_SHFT	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_FEC_VALUE_BMSK	0x7f000000
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_FEC_VALUE_SHFT	0x18
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_RZVD0_BMSK	0xffffe0
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_RZVD0_SHFT	0x5
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_APC23_CLK_CFG_OVR_DCA_VAL_BMSK	0x1f
#define QFPROM_CORR_SPARE_REG27_ROW35_MSB_APC23_CLK_CFG_OVR_DCA_VAL_SHFT	0x0

#define APCS_APC00_CLKSEL_ALTPRISRCSEL_1_0_SHFT	0x8

struct agera_apc_fuse {
        unsigned int OvrEn : 1;
        unsigned int OvrInvEn : 1;
        unsigned int OvrDcaEn : 1;
        unsigned int OvrTdlSel : 1;
        unsigned int OvrMuxSel : 4;
        unsigned char OvrDcaVal[24];
};

const uint64_t agera_hw_reg_base[MAX_CPU_CLUSTERS] = {
	AGERA_APC_BASE(00),
	AGERA_APC_BASE(01),
	AGERA_APC_BASE(02),
	AGERA_APC_BASE(03),
	AGERA_APC_BASE(04),
	AGERA_APC_BASE(05),
	AGERA_APC_BASE(06),
	AGERA_APC_BASE(07),
	AGERA_APC_BASE(08),
	AGERA_APC_BASE(09),
	AGERA_APC_BASE(10),
	AGERA_APC_BASE(11),
	AGERA_APC_BASE(12),
	AGERA_APC_BASE(13),
	AGERA_APC_BASE(14),
	AGERA_APC_BASE(15),
	AGERA_APC_BASE(16),
	AGERA_APC_BASE(17),
	AGERA_APC_BASE(18),
	AGERA_APC_BASE(19),
	AGERA_APC_BASE(20),
	AGERA_APC_BASE(21),
	AGERA_APC_BASE(22),
	AGERA_APC_BASE(23),
};

/* A table of the PLLs for each DDR channel */
const uint64_t agera_hw_ddr_base[DDR_MAX_NUM_CHANS] = {
	AGERA_PLL_MODE(GCCMS_MDDR_PLL_BASE(S, 0)),
	AGERA_PLL_MODE(GCCMS_MDDR_PLL_BASE(S, 1)),
	AGERA_PLL_MODE(GCCMS_MDDR_PLL_BASE(N, 0)),
	AGERA_PLL_MODE(GCCMS_MDDR_PLL_BASE(N, 1)),
	AGERA_PLL_MODE(GCCMS_MDDR_PLL_BASE(N, 2)),
	AGERA_PLL_MODE(GCCMS_MDDR_PLL_BASE(S, 2)),
};

/* Workaround RUMI issue where the PCFSM locks up when exiting certain
 * low power states.
 */
#ifdef CONFIG_PRESIL
void agera_hw_enable_pll(uint64_t base)
{
	/* UNLOCK_CNT_2 = LOCK_CNT_5 = 1 */
	__raw_writel(0x10100, AGERA_APC_HW_DCVS_CTL(base));
}
#else
void agera_hw_enable_pll(uint64_t base)
{
}
#endif

#define EXTRACT_FUSE_VAL(val, fuse) \
	(((val) & fuse##_BMSK) >> fuse##_SHFT)

static struct agera_apc_fuse *agera_hw_get_fuse_settings(void)
{
	static bool agera_apc_fuse_init = false;
	static struct agera_apc_fuse fuse;
	uint32_t fuse_val;

	if (agera_apc_fuse_init)
		return &fuse;

	/* Global settings */
	fuse_val = __raw_readl(QFPROM_CORR_SPARE_REG27_ROW33_LSB);
	fuse.OvrEn = EXTRACT_FUSE_VAL(fuse_val,
		QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_EN);
	fuse.OvrInvEn = EXTRACT_FUSE_VAL(fuse_val,
		QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_INV_EN);
	fuse.OvrDcaEn = EXTRACT_FUSE_VAL(fuse_val,
		QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_DCA_EN);
	fuse.OvrTdlSel = EXTRACT_FUSE_VAL(fuse_val,
		QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_TDL_SEL);
	fuse.OvrMuxSel = EXTRACT_FUSE_VAL(fuse_val,
		QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC_CLK_CFG_OVR_MUX_SEL);
	if (fuse.OvrEn && fuse.OvrDcaEn) {
		/* Per APC settings */
		fuse.OvrDcaVal[0] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC00_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[1] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC01_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[2] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW33_LSB_APC02_CLK_CFG_OVR_DCA_VAL);

		fuse_val = __raw_readl(QFPROM_CORR_SPARE_REG27_ROW33_MSB);
		fuse.OvrDcaVal[3] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC03_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[4] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC04_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[5] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC05_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[6] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW33_MSB_APC06_CLK_CFG_OVR_DCA_VAL);
		fuse_val = __raw_readl(QFPROM_CORR_SPARE_REG27_ROW34_LSB);
		fuse.OvrDcaVal[7] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC07_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[8] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC08_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[9] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC09_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[10] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC10_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[11] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC11_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[12] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_LSB_APC12_CLK_CFG_OVR_DCA_VAL);

		fuse_val = __raw_readl(QFPROM_CORR_SPARE_REG27_ROW34_MSB);
		fuse.OvrDcaVal[13] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC13_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[14] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC14_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[15] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC15_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[16] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW34_MSB_APC16_CLK_CFG_OVR_DCA_VAL);

		fuse_val = __raw_readl(QFPROM_CORR_SPARE_REG27_ROW35_LSB);
		fuse.OvrDcaVal[17] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC17_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[18] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC18_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[19] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC19_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[20] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC20_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[21] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC21_CLK_CFG_OVR_DCA_VAL);
		fuse.OvrDcaVal[22] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW35_LSB_APC22_CLK_CFG_OVR_DCA_VAL);

		fuse_val = __raw_readl(QFPROM_CORR_SPARE_REG27_ROW35_MSB);
		fuse.OvrDcaVal[23] = EXTRACT_FUSE_VAL(fuse_val,
			QFPROM_CORR_SPARE_REG27_ROW35_MSB_APC23_CLK_CFG_OVR_DCA_VAL);
	}

	agera_apc_fuse_init = true;
	return &fuse;
}

static void agera_hw_apply_clksel_fuse(uint32_t apc,
				       uint32_t *p_clk_sel)
{
	struct agera_apc_fuse *fuse = agera_hw_get_fuse_settings();

	if (fuse->OvrEn) {
		(*p_clk_sel) = fuse->OvrMuxSel |
			(fuse->OvrMuxSel <<
			 APCS_APC00_CLKSEL_ALTPRISRCSEL_1_0_SHFT);
	}
}

/* Apply fuse overrides:
 * Refer to cm_pll_agera datasheet (80-P4246-2) for more details
 */
static void agera_hw_apply_config_fuse(uint32_t apc,
				       struct agera_pll_regs *regs)
{
	struct agera_apc_fuse *fuse = agera_hw_get_fuse_settings();

	if (fuse->OvrEn) {
		/*
		 * TDL enable for other PLL outputs - pll_config_ctl[55]
		 *  0 - TDL output is not routed to main and aux outputs
		 *  1 - TDL output is output to other PLL outputs
		 */
		regs->config_ctl_hi &= ~(1 << 23);
		if (fuse->OvrTdlSel)
			regs->config_ctl_hi |= (1 << 23);
		/*
		 * Output clock polarity - pll_user_ctl[7]
		 *  0 - Do not invert output
		 *  1 - Invert output
		 */
		regs->user_ctl &= ~(1 << 7);
		if (fuse->OvrInvEn)
			regs->user_ctl |= (1 << 7);
		/* Duty cycle adjust */
		if (fuse->OvrDcaEn) {
			/*
			 * Output duty cycle control enable on lv_early
			 * output - pll_config_ctl[37]
			 *  0 - Duty cycle adjustment is disabled
			 *  1 - Duty cycle adjustment is enabled
			 */
			regs->config_ctl_hi |= 1 << 5;
			/*
			 * Duty cycle control override - pll_config_ctl[44]
			 *  0 - Uses internal duty cycle measurement to
			 *      change duty cycle
			 *  1 - Uses duty cycle control from
			 *       pll_config_ctl[42:38] to adjust duty cycle
			 */
			regs->config_ctl_hi |= 1 << 12;
			/*
			 * Duty cycle adjustment - pll_config_ctl[42:38]
			 */
			regs->config_ctl_hi &= ~0x7C0;
			regs->config_ctl_hi |=
				(fuse->OvrDcaVal[apc] & 0x1F) << 6;
		}
	}
}

uint32_t agera_hw_config_pll(uint32_t apc, struct agera_pll_regs *regs)
{
	uint32_t clk_sel;
	uint8_t major, minor;

	soc_get_version(major, minor);
	if (major == 1) {
		regs->user_ctl = AGERA_USR_CTL_V1;
		clk_sel = AGERA_APC_CLKSEL_EARLY;
	} else {
		regs->user_ctl = AGERA_USR_CTL_V2;
		clk_sel = AGERA_APC_CLKSEL_MAIN;
	}
	agera_hw_apply_config_fuse(apc, regs);
	agera_hw_apply_clksel_fuse(apc, &clk_sel);
	return clk_sel;
}

#define AGERA_PLL_LOCK_TIMEOUT_US	300
#define MHZ_TO_LVAL(x)			((x) / 20)

static bool agera_config_apc(uint32_t apc,
			     const struct agera_pll_regs *regs)
{
	uint32_t timeout = 0;
	uint64_t base;

	if (regs == NULL || apc >= MAX_CPU_CLUSTERS)
		return false;

	base = agera_hw_reg_base[apc];
	/* The PLL is already enabled, it cannot be reconfigured,
	 * noted that this function won't set this bit, but use
	 * FSM_EN bit instead.
	 */
	if (agera_pll_is_enabled(base))
		return false;

	/* Configure AUX clock */
	__raw_writel(0x20002, AGERA_APC_AUX_CBCR(base));

	/* Enable ACS and SCS
	 * HPSS HPG Section 7.2.2
	 */

	/* Enable Soft Stop/Start */
	__raw_writel(0x0007, AGERA_APC_SSSCTL(base));
	/* Clock Halt Exit Control */
	__raw_writel(0x01F1, AGERA_APC_CLKHALT_EXIT_CTL(base));
	/* Pulse Swallower Config */
	__raw_writel(0x20005, AGERA_APC_PSCTL(base));

	/* Configure the PLL. */

	/* Updates from HSR  */
	__raw_writel(regs->config_ctl_hi, AGERA_APC_CONFIG_CTL_HI(base));
	__raw_writel(regs->config_ctl_lo, AGERA_APC_CONFIG_CTL_LO(base));
	__raw_writel(regs->user_ctl, AGERA_APC_USER_CTL(base));
	__raw_writel(regs->test_ctl_hi, AGERA_APC_TEST_CTL_HI(base));
	__raw_writel(regs->test_ctl_lo, AGERA_APC_TEST_CTL_LO(base));

	/* Program frequency */
	__raw_writel(regs->l, AGERA_APC_L_VAL(base));
	__raw_writel(regs->alpha, AGERA_APC_ALPHA_VAL(base));
	while (__raw_readl(AGERA_APC_L_VAL_STATUS(base)) != regs->l) {
		udelay(1);
		timeout++;
		if (timeout > 300)
			return false;
	}

	/* Bring PLL up in HW mode, BIAS_CNT=7; LOCK_CNT=0; FSM_EN=1 */
	__raw_writel(0x0011C000, AGERA_PLL_MODE(base));
	agera_hw_enable_pll(base);
	return true;
}

static uint32_t agera_get_apc_settings(uint32_t apc, uint32_t freq_mhz,
				       struct agera_pll_regs *regs)
{
	/* ASSERT(!regs || apc >= MAX_CPU_CLUSTERS); */

	memset(regs , 0, sizeof(*regs));
	regs->l = MHZ_TO_LVAL(freq_mhz);

	/* Default configuration */
	regs->config_ctl_hi = AGERA_CFG_CTL_HI;
	regs->config_ctl_lo = AGERA_CFG_CTL_LO;
	regs->test_ctl_hi = AGERA_TST_CTL_HI;
	regs->test_ctl_lo = AGERA_TST_CTL_LO;
	return agera_hw_config_pll(apc, regs);
}

bool agera_enable_apc(uint8_t apc, uint32_t freq_mhz)
{
	uint64_t base;
	struct agera_pll_regs regs = {0};
	uint32_t clksel;

	if (apc >= MAX_CPU_CLUSTERS)
		return false;

	clksel = agera_get_apc_settings(apc, freq_mhz, &regs);

	/* Apply config configurations */
	if (!agera_config_apc(apc, &regs))
		return false;

	/* Select the high speed PLL and program AON source */
	base = agera_hw_reg_base[apc];
	__raw_writel_mask(clksel | AGERA_APC_AONCLK_PROG,
			  AGERA_APC_CLKSEL_MASK | AGERA_APC_AONCLK_MASK,
			  AGERA_APC_CLKSEL(base));
	return true;
}

static bool agera_config_cbf(uint32_t freq_mhz)
{
	uint32_t L = MHZ_TO_LVAL(freq_mhz);

	/* Configure PLL frequency. */
	__raw_writel(L, AGERA_CBF_L_VAL);

	if (agera_pll_is_enabled(AGERA_CBF_BASE))
		return true;

	/* Enable the PLL output if it hasn't already been. Also assume
	 * that PLL config registers need to be programmed here as well.
	 */
	/* CBF settings match APCS v1 settings */
	__raw_writel(AGERA_CFG_CTL_HI, AGERA_CBF_CONFIG_CTL_HI);
	__raw_writel(AGERA_CFG_CTL_LO, AGERA_CBF_CONFIG_CTL_LO);
	__raw_writel(AGERA_TST_CTL_HI, AGERA_CBF_TEST_CTL_HI);
	__raw_writel(AGERA_TST_CTL_LO, AGERA_CBF_TEST_CTL_LO);
	__raw_writel(0x2, AGERA_PLL_MODE(AGERA_CBF_BASE));
	udelay(5);
	__raw_writel(0x6, AGERA_PLL_MODE(AGERA_CBF_BASE));
	udelay(50);
	__raw_writel(0x7, AGERA_PLL_MODE(AGERA_CBF_BASE));
	return true;
}

bool agera_enable_cbf(uint32_t freq_mhz)
{
	/* Configure the pulse swallower as per HPG 7.5.
	 * The PS is used to to ramp the CBF clock on and off when
	 * switching the GFMUX and during idle entry/exit.
	 *   APCS_HPLL_PSCTL 0x30446  0011 0000 0100 0100 0110
	 */
	__raw_writel(0x30446, AGERA_CBF_PSCTL);

	/* First configure the CBF PLL to the max frequency that the PSC
	 * can safely swallow.
	 */
	if (!agera_config_cbf(AGERA_CBF_PS_SAFE_FREQ_MHZ))
		return false;

	/* Enable SSWEN */
	__raw_setl(AGERA_CBF_SSWEN, AGERA_CBF_SSS_CTL);

	/* Select the high speed PLL and wait 50us for PSC_OFF to set */
	__raw_writel_mask(AGERA_CBF_CLKSEL_VAL, AGERA_CBF_CLKSEL_MASK,
			  AGERA_CBF_GFMUX_CTL);
	udelay(50);

	/* Slew to the final frequency. */
	return agera_config_cbf(freq_mhz);
}

#ifdef CONFIG_DDR
#ifdef CONFIG_AGERA_PLL_LOCK
static bool __agera_pll_wait_lock(uint64_t base)
{
	uint32_t tout = 0;

	while (!(__raw_readl(AGERA_PLL_MODE(base)) &
	         AGERA_PLL_MODE_LOCK_DET)) {
		if (tout++ >= AGERA_PLL_LOCK_TIMEOUT_US)
			return false;
		busywait(1);
	}
	return true;
}
#else
static bool __agera_pll_wait_lock(uint64_t base)
{
	busywait(50);
	return true;
}
#endif

/* Macros for initializing the HF (High Frequency) PLLs */
#define PLL_HF_CONFIG_CTL_HI	0x000003D2
#define PLL_HF_CONFIG_CTL_LO	0x20000AA8
#define PLL_HF_TEST_CTL_HI	0x00004000
#define PLL_HF_TEST_CTL_LO	0x04000400

/* This function is specially created to work on PLLs with the non-standard
 * register layout where PLL_L_VAL = PLL_MODE + 8 (instead of the usual 4).
 * This function does not deal with fractional (alpha) modes.  It is only
 * for the high speed PLLs used by DDR and CBF.
 */
static bool agera_config_ddr(uint64_t base, uint32_t l,
			     uint32_t alpha, uint32_t post_div)
{
	uint32_t mode;

	/* The PLL is already enabled, it cannot be reconfigured */
	if (agera_pll_is_enabled(base))
		return false;

	/* Take PLL out of power down. */
	__raw_clearl(AGERA_APC_MODE_APC_PDN, AGERA_PLL_MODE(base));

	/* Enable fractional mode and set alpha to 2s compliment mode */
	if (alpha != 0) {
		__raw_clearl(AGERA_DDR_ALPHA_MODE,
			     AGERA_MDDR_PLL_USER_CTL(base));
		__raw_setl(AGERA_DDR_MN_EN, AGERA_MDDR_PLL_USER_CTL(base));
	}

	/* Set frequency */
	__raw_writel(l, AGERA_MDDR_PLL_L_VAL(base));
	__raw_writel(alpha, AGERA_MDDR_PLL_ALPHA_VAL(base));

	/* Set up CONFIG_CTL and TEST_CTL */
	__raw_writel(PLL_HF_CONFIG_CTL_LO, AGERA_MDDR_PLL_CONFIG_CTL_LO(base));
	__raw_writel(PLL_HF_CONFIG_CTL_HI, AGERA_MDDR_PLL_CONFIG_CTL_HI(base));
	__raw_writel(PLL_HF_TEST_CTL_LO, AGERA_MDDR_PLL_TEST_CTL_LO(base));
	__raw_writel(PLL_HF_TEST_CTL_HI, AGERA_MDDR_PLL_TEST_CTL_HI(base));

	/* Set the PLL PostDiv */
	if (post_div == 0) {
		/* prevent integer roll-under */
		post_div = 1;
	}
	__raw_writel_mask(AGERA_DDR_POSTDIV_CTL(post_div - 1),
			  AGERA_DDR_POSTDIV_CTL(AGERA_DDR_POSTDIV_CTL_MASK),
			  AGERA_MDDR_PLL_USER_CTL(base));
	/* Set the PLL Main out */
	__raw_setl(AGERA_DDR_LVMAIN_EN, AGERA_MDDR_PLL_USER_CTL(base));

	/* Enable the PLL */
	mode = __raw_readl(AGERA_PLL_MODE(base));
	mode &= ~(AGERA_PLL_MODE_RESET_N |
		  AGERA_PLL_MODE_BYPASSNL |
		  AGERA_PLL_MODE_OUTCTRL);
	__raw_writel(mode | AGERA_PLL_MODE_BYPASSNL, AGERA_PLL_MODE(base));
	busywait(50);
	__raw_setl(AGERA_PLL_MODE_RESET_N, AGERA_PLL_MODE(base));
	if (!__agera_pll_wait_lock(base))
		return false;
	__raw_setl(AGERA_PLL_MODE_OUTCTRL, AGERA_PLL_MODE(base));
	return true;
}

bool agera_enable_ddr(uint8_t ddr)
{
	ClockDDRPlanType *plan = NULL;

	plan = Clock_GetDDRPLLConfig(ddr);
	CORE_VERIFY(plan != NULL);
	return agera_config_ddr(agera_hw_ddr_base[ddr],
				plan->nPLL_L, plan->nPLL_Alpha,
				plan->nPLL_PostDivCtl);
}
#endif

bool Clock_HMSSInit(void)
{
	int apc;
	uint64_t partial_goods = get_cpu_mask();

	for (apc = 0; apc < MAX_CPU_CLUSTERS; apc++) {
		if (partial_goods & CLUSTER_TO_CPU_MASK(apc)) {
			if (!agera_enable_apc(apc, CPU_BOOT_FREQ_MHZ))
				return false;
		}
	}

	return agera_enable_cbf(CBF_BOOT_FREQ_MHZ);
}

uint32_t Clock_APSSCpuSpeed(uint32_t nCpu)
{
	return CPU_BOOT_FREQ_MHZ;
}
