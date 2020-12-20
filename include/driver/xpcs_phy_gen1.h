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
 * @(#)xpcs_phy_gen1.h: Synopsys XAUI phy Gen1 3G interface
 * $Id: xpcs_phy_gen1.h,v 1.0 2020-12-20 13:00:00 zhenglv Exp $
 */

#ifndef __XPCS_PHY_GEN1_H_INCLUDE__
#define __XPCS_PHY_GEN1_H_INCLUDE__

#define VR_XS_PMA_Gen1_TX_BSTCTRL		VR(XS_PMA_MMD, 0x0030)
#define VR_XS_PMA_Gen1_TX_ATTN_CTRL		VR(XS_PMA_MMD, 0x0031)
#define VR_XS_PMA_Gen1_TX_GENCTRL		VR(XS_PMA_MMD, 0x0032)
#define VR_XS_PMA_Gen1_TX_EDGRT_CTRL		VR(XS_PMA_MMD, 0x0033)
#define VR_XS_PMA_Gen1_TXD_STS			VR(XS_PMA_MMD, 0x0040)
#define VR_XS_PMA_Gen1_RXGCTRL			VR(XS_PMA_MMD, 0x0050)
#define VR_XS_PMA_Gen1_RXEQ_CTRL		VR(XS_PMA_MMD, 0x0051)
#define VR_XS_PMA_Gen1_DPLL_MCTRL		VR(XS_PMA_MMD, 0x0052)
#define VR_XS_PMA_Gen1_RDPLL_RST		VR(XS_PMA_MMD, 0x0053)
#define VR_XS_PMA_Gen1_RLOS_CTRL		VR(XS_PMA_MMD, 0x0054)
#define VR_XS_PMA_Gen1_MPLL_CTRL0		VR(XS_PMA_MMD, 0x0070)
#define VR_XS_PMA_Gen1_MPLL_CTRL1		VR(XS_PMA_MMD, 0x0071)
#define VR_XS_PMA_Gen1_MPLL_CTRL2		VR(XS_PMA_MMD, 0x0072)
#define VR_XS_PMA_Gen1_MPLL_STS			VR(XS_PMA_MMD, 0x0080)
#define VR_XS_PMA_Gen1_LVL_CTRL			VR(XS_PMA_MMD, 0x0090)
#define VR_XS_PMA_Gen1_MISC_CTRL		VR(XS_PMA_MMD, 0x0091)

#ifdef CONFIG_DW_XPCS_VS_MII_MMD
#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define VR_MII_Gen1_TX_BSTCTRL			VR(VS_MII_MMD, 0x0030)
#define VR_MII_Gen1_TX_ATTN_CTRL		VR(VS_MII_MMD, 0x0031)
#define VR_MII_Gen1_TX_GENCTRL			VR(VS_MII_MMD, 0x0032)
#define VR_MII_Gen1_TX_EDGRT_CTRL		VR(VS_MII_MMD, 0x0033)
#define VR_MII_Gen1_TXD_STS			VR(VS_MII_MMD, 0x0040)
#define VR_MII_Gen1_RXGCTRL			VR(VS_MII_MMD, 0x0050)
#define VR_MII_Gen1_RXEQ_CTRL			VR(VS_MII_MMD, 0x0051)
#define VR_MII_Gen1_DPLL_MCTRL			VR(VS_MII_MMD, 0x0052)
#define VR_MII_Gen1_RDPLL_RST			VR(VS_MII_MMD, 0x0053)
#define VR_MII_Gen1_RLOS_CTRL			VR(VS_MII_MMD, 0x0054)
#define VR_MII_Gen1_MPLL_CTRL0			VR(VS_MII_MMD, 0x0070)
#define VR_MII_Gen1_MPLL_CTRL1			VR(VS_MII_MMD, 0x0071)
#define VR_MII_Gen1_MPLL_CTRL2			VR(VS_MII_MMD, 0x0072)
#define VR_MII_Gen1_MPLL_STS			VR(VS_MII_MMD, 0x0080)
#define VR_MII_Gen1_LVL_CTRL			VR(VS_MII_MMD, 0x0090)
#define VR_MII_Gen1_MISC_CTRL			VR(VS_MII_MMD, 0x0091)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */
#endif /* CONFIG_DW_XPCS_VS_MII_MMD */

#endif /* __XPCS_PHY_GEN1_H_INCLUDE__ */
