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
 * @(#)xpcs_phy_gen2.h: Synopsys XAUI phy Gen2 3G/6G interface
 * $Id: xpcs_phy_gen2.h,v 1.0 2020-12-20 14:00:00 zhenglv Exp $
 */

#ifndef __XPCS_PHY_GEN2_INCLUDE__
#define __XPCS_PHY_GEN2_INCLUDE__

#define VR_XS_PMA_Gen2_Gen4_TX_BSTCTRL0		VR(0x0038)
#define VR_XS_PMA_Gen2_Gen4_TX_BSTCTRL1		VR(0x0039)
#define VR_XS_PMA_Gen2_Gen4_TX_LVLCTRL0		VR(0x003A)
#define VR_XS_PMA_Gen2_Gen4_TX_LVLCTRL1		VR(0x003B)
#define VR_XS_PMA_Gen2_Gen4_TXGENCTRL0		VR(0x003C)
#define VR_XS_PMA_Gen2_Gen4_TXGENCTRL1		VR(0x003D)
#define VR_XS_PMA_Gen2_Gen4_TX_STS		VR(0x0048)
#define VR_XS_PMA_Gen2_Gen4_RXGENCTRL0		VR(0x0058)
#define VR_XS_PMA_Gen2_Gen4_RXGENCTRL1		VR(0x0059)
#define VR_XS_PMA_Gen2_Gen4_MPLL_CTRL0		VR(0x0078)
#define VR_XS_PMA_Gen2_Gen4_MPLL_CTRL1		VR(0x0079)
#define VR_XS_PMA_Gen2_Gen4_MPLL_STS		VR(0x0088)
#define VR_XS_PMA_Gen2_Gen4_LVL_CTRL		VR(0x0098)
#define VR_XS_PMA_Gen2_Gen4_MISC_CTRL0		VR(0x0099)
#define VR_XS_PMA_Gen2_Gen4_MISC_CTRL1		VR(0x009A)

#ifdef CONFIG_DW_XPCS_VS_MII_MMD
#ifdef CONFIG_ARCH_IS_DW_XPCS_1000BASE_X
#define VR_MII_Gen2_Gen4_TX_BSTCTRL0		VR(0x0038)
#define VR_MII_Gen2_Gen4_TX_LVLCTRL0		VR(0x003A)
#define VR_MII_Gen2_Gen4_TXGENCTRL0		VR(0x003C)
#define VR_MII_Gen2_Gen4_TXGENCTRL1		VR(0x003D)
#define VR_MII_Gen2_Gen4_TX_STS			VR(0x0048)
#define VR_MII_Gen2_Gen4_RXGENCTRL0		VR(0x0058)
#define VR_MII_Gen2_Gen4_RXGENCTRL1		VR(0x0059)
#define VR_MII_Gen2_Gen4_MPLL_CTRL0		VR(0x0078)
#define VR_MII_Gen2_Gen4_MPLL_CTRL1		VR(0x0079)
#define VR_MII_Gen2_Gen4_MPLL_STS		VR(0x0088)
#define VR_MII_Gen2_Gen4_LVL_CTRL		VR(0x0098)
#define VR_MII_Gen2_Gen4_MISC_CTRL0		VR(0x0099)
#define VR_MII_Gen2_Gen4_MISC_CTRL1		VR(0x009A)

#define VR_MII_Gen2_RXLOS_CTRL			VR(0x005A)
#define VR_MII_Gen2_MISC_STS			VR(0x009B)

#define VR_MII_Gen4_TXGENCTRL2			VR(0x003E)
#define VR_MII_Gen4_TXGENCTRL3			VR(0x003F)
#define VR_MII_Gen4_RXEQ_CTRL			VR(0x005B)
#define VR_MII_Gen4_RXLOS_CTRL0			VR(0x005D)
#define VR_MII_Gen4_EEE_CTRL			VR(0x0092)
#define VR_MII_Gen4_MISC_CTRL2			VR(0x0093)
#endif /* CONFIG_ARCH_IS_DW_XPCS_1000BASE_X */
#endif /* CONFIG_DW_XPCS_VS_MII_MMD */

#endif /* __XPCS_PHY_GEN2_INCLUDE__ */
