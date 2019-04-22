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
 * @(#)imccc.h: IMC_CC (IMC Clock Controller) definitions
 * $Id: imccc.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __IMCCC_QDF2400_H_INCLUDE__
#define __IMCCC_QDF2400_H_INCLUDE__

/* Accepts the clock outputs from GCCMW, generates the clocks
 * and the resets for the IMC hard macro.
 * It maintains 4 clock groups:
 * 1. 500MHz clock group for IMC core.
 * 2. 333MHz clock group for IMC SoC.
 * 3. 100MHz clock group for west cfg SFPB.
 * 4. 250MHz clock group for debug AJU IMC.
 */
#define IMC_CC_BASE			ULL(0x0FF6FC40000)
#define IMC_CC_BLOCK_SHIFT		16
#define IMC_CC_REG(blk, off)		\
	((caddr_t)IMC_CC_BASE + ((blk) << IMC_CC_BLOCK_SHIFT) + (off))

/* SYS */
#define IMC_CC_SYS			0
#define IMC_CC_IMC_POSTDIV_CDIVR	IMC_CC_REG(IMC_CC_SYS, 0x0000)
#define IMC_CC_IMC_CMD_RCGR		IMC_CC_REG(IMC_CC_SYS, 0x0004)
#define IMC_CC_IMC_PROC_CMD_RCGR	IMC_CC_REG(IMC_CC_SYS, 0x000C)
#define IMC_CC_IMC_AXI_BRDG_PROC_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0014)
#define IMC_CC_IMC_DBG_BRDG_PROC_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0018)
#define IMC_CC_IMC_PROC_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x001C)
#define IMC_CC_IMC_S0_AHB_CBCR		IMC_CC_REG(IMC_CC_SYS, 0x0020)
#define IMC_CC_IMC_S1_AHB_CBCR		IMC_CC_REG(IMC_CC_SYS, 0x0024)
#define IMC_CC_IMCFAB_SFAB_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0028)
#define IMC_CC_IMCFAB_RAM0_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x002C)
#define IMC_CC_IMCFAB_RAM1_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0030)
#define IMC_CC_IMCFAB_CORE_CBCR		IMC_CC_REG(IMC_CC_SYS, 0x0034)
#define IMC_CC_IMCFAB_RAM2_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0038)
#define IMC_CC_IMCFAB_S0_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x003C)
#define IMC_CC_IMCFAB_S1_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0040)
#define IMC_CC_SFVBU_IMC_DVM_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0044)
#define IMC_CC_IMC_RAM0_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0048)
#define IMC_CC_IMC_RAM1_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x004C)
#define IMC_CC_IMC_RAM2_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0050)
#define IMC_CC_IMC_IOMMU_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0054)
#define IMC_CC_IMC_PROC_CBCR		IMC_CC_REG(IMC_CC_SYS, 0x0058)
#define IMC_CC_IMCFAB_RAM3_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x005C)
#define IMC_CC_IMCFAB_RAM4_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0060)
#define IMC_CC_IMCFAB_RAM5_FCLK_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0064)
#define IMC_CC_IMC_RAM3_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0068)
#define IMC_CC_IMC_RAM4_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x006C)
#define IMC_CC_IMC_RAM5_AXI_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0070)
#define IMC_CC_IMC_QSMMUV3_CLIENT_CBCR	IMC_CC_REG(IMC_CC_SYS, 0x0074)
#define IMC_CC_IMC_WARM_RESET_STATUS	IMC_CC_REG(IMC_CC_SYS, 0x0200)

/* MISC */
#define IMC_CC_MISC			1
#define IMC_CC_IM_SLEEP_CBCR			IMC_CC_REG(IMC_CC_MISC, 0x0000)
#define IMC_CC_IMC_SLEEP_CBCR			IMC_CC_REG(IMC_CC_MISC, 0x0004)
#define IMC_CC_IMC_XO_CBCR			IMC_CC_REG(IMC_CC_MISC, 0x0008)
#define IMC_CC_IMCFAB_BOOT_ROM_FCLK_CBCR	IMC_CC_REG(IMC_CC_MISC, 0x000C)
#define IMC_CC_BOOT_ROM_AHB_CBCR		IMC_CC_REG(IMC_CC_MISC, 0x0010)
#define IMC_CC_IMC_RESETR			IMC_CC_REG(IMC_CC_MISC, 0x0100)
#define IMC_CC_USE_SLEEP_RET_EN			IMC_CC_REG(IMC_CC_MISC, 0x0200)

/* DEBUG */
#define IMC_CC_DEBUG			2
#define IMC_CC_DEBUG_CLK_CTL		IMC_CC_REG(IMC_CC_DEBUG, 0x0000)
#define IMC_CC_DEBUG_CDIVR		IMC_CC_REG(IMC_CC_DEBUG, 0x0004)

#endif /* __IMCCC_QDF2400_H_INCLUDE__ */
