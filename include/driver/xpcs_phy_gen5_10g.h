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
 * @(#)xpcs_phy_gen5_10g.h: Synopsys XAUI phy Gen5 10G interface
 * $Id: xpcs_phy_gen5_10g.h,v 1.0 2020-12-20 16:00:00 zhenglv Exp $
 */

#ifndef __XPCS_PHY_GEN5_10G_H_INCLUDE__
#define __XPCS_PHY_GEN5_10G_H_INCLUDE__

#define VR_XS_PMA_Gen5_10G_TX_CTRL			VR(XS_PMA_MMD, 0x0034)
#define VR_XS_PMA_Gen5_10G_TX_STS			VR(XS_PMA_MMD, 0x0049)
#define VR_XS_PMA_Gen5_10G_RX_CTRL			VR(XS_PMA_MMD, 0x005C)
#define VR_XS_PMA_Gen5_10G_RX_STS			VR(XS_PMA_MMD, 0x0061)
#define VR_XS_PMA_Gen5_10G_MPLL_CTRL			VR(XS_PMA_MMD, 0x007A)
#define VR_XS_PMA_Gen5_10G_MPLL_STS			VR(XS_PMA_MMD, 0x0089)
#define VR_XS_PMA_Gen5_10G_GEN_CTRL			VR(XS_PMA_MMD, 0x009C)
#define VR_XS_PMA_Gen5_10G_TX_POWER_STATE_CTRL		VR(XS_PMA_MMD, 0x009D)
#define VR_XS_PMA_Gen5_10G_RX_POWER_STATE_CTRL		VR(XS_PMA_MMD, 0x009E)
#define VR_XS_PMA_Gen5_10G_MISC_CTRL			VR(XS_PMA_MMD, 0x009F)

/* Optional registers */
#ifdef CONFIG_DW_XPCS_KR
#define VR_XS_PMA_Gen5_10G_MISC_STS			VR(XS_PMA_MMD, 0x0097)
#endif /* CONFIG_DW_XPCS_KR */
#ifdef CONFIG_DW_XPCS_EEE
#define VR_XS_PMA_Gen5_10G_EEE_CTRL			VR(XS_PMA_MMD, 0x0092)
#endif /* CONFIG_DW_XPCS_EEE */

#ifdef CONFIG_DW_XPCS_VS_MII_MMD
#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define VR_MII_Gen5_10G_TX_CTRL			VR(VS_MII_MMD, 0x0034)
#define VR_MII_Gen5_10G_TX_STS			VR(VS_MII_MMD, 0x0049)
#define VR_MII_Gen5_10G_RX_CTRL			VR(VS_MII_MMD, 0x005C)
#define VR_MII_Gen5_10G_RX_STS			VR(VS_MII_MMD, 0x0061)
#define VR_MII_Gen5_10G_MPLL_CTRL		VR(VS_MII_MMD, 0x007A)
#define VR_MII_Gen5_10G_MPLL_STS		VR(VS_MII_MMD, 0x0089)
#define VR_MII_Gen5_10G_MISC_STS		VR(VS_MII_MMD, 0x0097)
#define VR_MII_Gen5_10G_GEN_CTRL		VR(VS_MII_MMD, 0x009C)
#define VR_MII_Gen5_10G_TX_POWER_STATE_CTRL	VR(VS_MII_MMD, 0x009D)
#define VR_MII_Gen5_10G_RX_POWER_STATE_CTRL	VR(VS_MII_MMD, 0x009E)
#define VR_MII_Gen5_10G_MISC_CTRL		VR(VS_MII_MMD, 0x009F)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */
#endif /* CONFIG_DW_XPCS_VS_MII_MMD */

#endif /* __XPCS_PHY_GEN5_10G_H_INCLUDE__ */
