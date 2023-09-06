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
 * @(#)pcie.c: K1Matrix specific PCIe implementation
 * $Id: pcie.c,v 1.1 2023-08-01 11:41:01 zhenglv Exp $
 */

#include <stdint.h>
#include <asm/io.h>
#include <target/delay.h>

#define SYS_RSTGEN_R_BADDR      ULL(0x002F024000)
#define SYS_CLKGEN_R_BADDR      ULL(0x002F020000)
#define SYS_XXX                 ULL(0x0040000000)
#define SYS_XXX2                ULL(0x0044000000)
#define SYS_XXX3                ULL(0x0045080000)

#define SERDES_SW_RESET_OFFSET  ULL(0x300)
#define SERDES_CLK_EN_OFFSET    ULL(0x404)
#define SERDES_SER2MBUS_OFFSET  ULL(0x408)
#define PCIE_SW_RESET_OFFSET    ULL(0x304)
#define PCIE_CLK_EN_OFFSET      ULL(0x40C)
#define PCIE_PHY0_CLKEN_OFFSET  ULL(0x42C)
#define PCIE_PHY0_RESETN_OFFSET ULL(0x314)
#define PCIE0_PERSTN_OFFSET     ULL(0x318)
#define SERDES_LANE_OFFSET      ULL(0xC)
#define IP_RXSTANDBY_OFFSET     ULL(0x8C4)
#define PCIE_PHY0_RSTN_OFFSET   ULL(0x0)
#define IP_FAST_LINK_OFFSET     ULL(0x710)
#define LTSSM_ENABLE_OFFSET     ULL(0x4)
#define WAIT_PL_UP_DL_UP        ULL(0x104)

void pcie_linkup(void)
{
    uint32_t reg;
    int32_t timeout;

    __raw_writel(0x1, SYS_RSTGEN_R_BADDR + SERDES_SW_RESET_OFFSET);  //1

    __raw_writel(0x1, SYS_CLKGEN_R_BADDR + SERDES_CLK_EN_OFFSET);    //2

    __raw_writel(0x1, SYS_CLKGEN_R_BADDR + SERDES_SER2MBUS_OFFSET);  //3

    __raw_writel(0x1, SYS_RSTGEN_R_BADDR + PCIE_SW_RESET_OFFSET);    //4

    __raw_writel(0x1, SYS_CLKGEN_R_BADDR + PCIE_CLK_EN_OFFSET);      //5

    reg = __raw_readl(SYS_CLKGEN_R_BADDR + PCIE_PHY0_CLKEN_OFFSET);  //6
    reg |= 0x1;
    __raw_writel(reg, SYS_CLKGEN_R_BADDR + PCIE_PHY0_CLKEN_OFFSET);

    reg = __raw_readl(SYS_RSTGEN_R_BADDR + PCIE_PHY0_RESETN_OFFSET); //7
    reg |= 0x1;
    __raw_writel(reg, SYS_RSTGEN_R_BADDR + PCIE_PHY0_RESETN_OFFSET);

    reg = __raw_readl(SYS_RSTGEN_R_BADDR + PCIE0_PERSTN_OFFSET);     //8
    reg |= 0x1;
    __raw_writel(reg, SYS_RSTGEN_R_BADDR + PCIE0_PERSTN_OFFSET);

    __raw_writel(0x3210, SYS_XXX3 + SERDES_LANE_OFFSET);             //9

    reg = __raw_readl(SYS_XXX + IP_RXSTANDBY_OFFSET);                //10
    reg &= 0xFFFFFFF0U;
    __raw_writel(reg, SYS_XXX + IP_RXSTANDBY_OFFSET);

    __raw_writel(0x1, SYS_XXX3 + PCIE_PHY0_RSTN_OFFSET);             //11

    udelay(2);

    __raw_writel(0x701A0, SYS_XXX + IP_FAST_LINK_OFFSET);            //12

    __raw_writel(0xF0001, SYS_XXX3 + PCIE_PHY0_RSTN_OFFSET);         //13

    __raw_writel(0x1, SYS_XXX2 + LTSSM_ENABLE_OFFSET);               //14

    timeout = 10000000;
    while (__raw_readl(SYS_XXX2 + WAIT_PL_UP_DL_UP) != 3) {          //15
        if (timeout-- <= 0) {
            printf("wait linkup timeout\n");
            break;
        }
    }
}
