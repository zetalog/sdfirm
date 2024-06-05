
/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2024
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
 * @(#)crg.h: k1matrix clock/reset generator definitions
 * $Id: crg.h,v 1.1 2024-06-05 17:37:00 zhenglv Exp $
 */

#ifndef __K1MATRIX_CRG_H_INCLUDE__
#define __K1MATRIX_CRG_H_INCLUDE__

#define CRG_BASE		__RMU_SYS_CRG_BASE

#include <driver/sc_pllts12ffclafrac2.h>

#define CRG_REG(offset)		(CRG_BASE + (offset))

#define CPU_CLK_CTL		CRG_REG(0x100)
#define CPU_NIC_CLK_CTL		CRG_REG(0x104)
#define CPU_HAP_CLK_CTL		CRG_REG(0x108)
#define PCIE_TOP_CLK_CTL	CRG_REG(0x110)
#define PCIE_TOP_AUXCLK_CTL	CRG_REG(0x114)
#define PCIE_TOP_CFGCLK_CTL	CRG_REG(0x118)
#define PCIE_BOT_CLK_CTL	CRG_REG(0x120)
#define PCIE_BOT_AUXCLK_CTL	CRG_REG(0x124)
#define PCIE_BOT_CFGCLK_CTL	CRG_REG(0x128)
#define PERI_SUB_CLK_CTL	CRG_REG(0x130)
#define MESH_SUB_CLK_CTL	CRG_REG(0x140)
#define DDR_SUB_CLK_CTL		CRG_REG(0x150)
#define CLUSTER0_CLK_CTL	CRG_REG(0x200)
#define CLUSTER1_CLK_CTL	CRG_REG(0x204)
#define CLUSTER2_CLK_CTL	CRG_REG(0x208)
#define CLUSTER3_CLK_CTL	CRG_REG(0x20C)
#define CLUSTER4_CLK_CTL	CRG_REG(0x210)
#define CLUSTER5_CLK_CTL	CRG_REG(0x214)
#define CLUSTER6_CLK_CTL	CRG_REG(0x218)
#define CLUSTER7_CLK_CTL	CRG_REG(0x21C)
#define SYS_APB_CLK_CTL		CRG_REG(0x220)
#define APIC_CLK_CTL		CRG_REG(0x224)
#define IMSIC_CLK_CTL		CRG_REG(0x228)
#define CPU_CHI_CLK_CTL		CRG_REG(0x22C)
#define RMU_AXI_CLK_DIV		CRG_REG(0x300)
#define RMU_AHB_CLK_DIV		CRG_REG(0x304)
#define RMU_APB_CLK_DIV		CRG_REG(0x308)
#define RMU_LPC_CLK_DIV		CRG_REG(0x30C)
#define RMU_CLK_SEL		CRG_REG(0x310)
#define RMU_eFuse_NS_CLK_SEL	CRG_REG(0x314)
#define RMU_SRAM_CLK_EN		CRG_REG(0x320)
#define RMU_QSPI_CLK_EN		CRG_REG(0x324)
#define RMU_eFuse_NS_CLK_EN	CRG_REG(0x328)
#define RMU_STM_CLK_EN		CRG_REG(0x32C)
#define RMU_LPC_CLK_EN		CRG_REG(0x334)
#define RMU_eSPI_CLK_EN		CRG_REG(0x338)
#define RMU_PVTC_CLK_EN		CRG_REG(0x33C)
#define RMU_UART0_CLK_EN	CRG_REG(0x340)
#define RMU_UART1_CLK_EN	CRG_REG(0x344)
#define RMU_Timer_S_CLK_EN	CRG_REG(0x348)
#define RMU_Spinlock_CLK_EN	CRG_REG(0x34C)
#define RMU_Mailbox_S_CLK_EN	CRG_REG(0x350)
#define RMU_Mailbox_NS_CLK_EN	CRG_REG(0x354)
#define RMU_PIC_CLK_EN		CRG_REG(0x358)
#define RMU_HAP_CLK_EN		CRG_REG(0x35C)
#define PCIE0_BUS_CLK_CTL	CRG_REG(0x400)

/* XXX_CLK_CTL */
#define CRG_CLKEN(n)		_BV(n)
#define CRG_CLKDIV0_OFFSET	4
#define CRG_CLKDIV0_MASK	REG_12BIT_MASK
#define CRG_CLKDIV0(value)	_SET_FV(CRG_CLKDIV0, value)
#define CRG_CLKDIV1_OFFSET	16
#define CRG_CLKDIV1_MASK	REG_8BIT_MASK
#define CRG_CLKDIV1(value)	_SET_FV(CRG_CLKDIV1, value)
#define CRG_CLKSEL_OFFSET	24
#define CRG_CLKSEL_MASK		REG_8BIT_MASK
#define CRG_CLKSEL(value)	_SET_FV(CRG_CLKSEL, value)

#ifdef CONFIG_CRCNTL_TRACE
void crg_trace(bool enabling, const char *name);
#else
#define crg_trace(enabling, name)	do { } while (0)
#endif

#endif /* __K1MATRIX_CRG_H_INCLUDE__ */
