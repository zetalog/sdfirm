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

#define SYS_CTL_BADDR           ULL(0x0F00110000)
#define PCIE_APP_ADDR           ULL(0x2800000)
#define PCIE_DBI_ADDR           ULL(0x2000000)

void pcie_linkup(void)
{
    uint32_t reg;
    int32_t timeout;

    reg = __raw_readl(SYS_CTL_BADDR + 0x300);
    reg |= (0x1 << 1);
    __raw_writel(reg, SYS_CTL_BADDR + 0x300);          //1

    reg = __raw_readl(SYS_CTL_BADDR + 0x300);
    reg |= (0x1 << 2);
    __raw_writel(reg, SYS_CTL_BADDR + 0x300);          //2

    __raw_writel(0x4, PCIE_APP_ADDR);                  //3

    reg = __raw_readl(SYS_CTL_BADDR + 0x300);
    reg |= (0x1 << 3);
    __raw_writel(reg, SYS_CTL_BADDR + 0x300);          //4

    __raw_writel(0x701a0, PCIE_DBI_ADDR + 0x710);      //5

    __raw_writel(0x1, PCIE_APP_ADDR + 4);              //6

    timeout = 10000000;
    while (__raw_readl(PCIE_APP_ADDR + 0x104) != 3) {  //7
        if (timeout-- <= 0) {
            sbi_printf("wait linkup timeout\n");
            break;
        }
    }
}
