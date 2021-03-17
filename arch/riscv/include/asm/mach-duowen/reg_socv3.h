/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)reg_socv3.h: DUOWEN SoCv3 space and register definitions
 * $Id: reg_socv3.h,v 1.1 2021-01-20 16:20:00 zhenglv Exp $
 */

#ifndef __REG_SOCv3_DUOWEN_H_INCLUDE__
#define __REG_SOCv3_DUOWEN_H_INCLUDE__

#ifndef __ASSEMBLY__
unsigned long duowen_soc_base;
#endif

#define SOC0_BASE		0
#define SOC1_BASE		ULL(0x80000000000)
#define SOC_BASE		duowen_soc_base

/* Local fabric */
#define LCSR_BASE		ULL(0xFF91000000)
#define DEBUG_BASE		ULL(0xFF90000000)
#define SFAB_ROM_BASE		ULL(0xFF8F000000) /* Boot ROM */
#define SFAB_ROM_SIZE		0x100000
#define FLASH_BASE		ULL(0xFF88000000)
#define SFAB_RAM_BASE		ULL(0xFF80000000) /* Sys Mem */
#define SFAB_RAM_SIZE		0x100000

/* System fabric */
#define CRCNTL_BASE		(SOC_BASE + ULL(0xFF60000000))
#define SCSR_BASE		(SOC_BASE + ULL(0xFF60100000))
#define TLMM_BASE		(SOC_BASE + ULL(0xFF60200000))
#define IMC_BASE		(SOC_BASE + ULL(0xFF60600000))
#define GPIO0_BASE		(SOC_BASE + ULL(0xFF61000000))
#define GPIO1_BASE		(SOC_BASE + ULL(0xFF61100000))
#define GPIO2_BASE		(SOC_BASE + ULL(0xFF61200000))
#define SPI0_BASE		(SOC_BASE + ULL(0xFF62000000))
#define SPI1_BASE		(SOC_BASE + ULL(0xFF62100000))
#define SPI2_BASE		(SOC_BASE + ULL(0xFF62200000))
#define SPI3_BASE		(SOC_BASE + ULL(0xFF62300000))
#define SPISLV_BASE		(SOC_BASE + ULL(0xFF62400000))
#define UART0_BASE		(SOC_BASE + ULL(0xFF63000000))
#define UART1_BASE		(SOC_BASE + ULL(0xFF63100000))
#define UART2_BASE		(SOC_BASE + ULL(0xFF63200000))
#define UART3_BASE		(SOC_BASE + ULL(0xFF63300000))
#define I2C0_BASE		(SOC_BASE + ULL(0xFF64000000))
#define I2C1_BASE		(SOC_BASE + ULL(0xFF64100000))
#define I2C2_BASE		(SOC_BASE + ULL(0xFF64200000))
#define I2C3_BASE		(SOC_BASE + ULL(0xFF64300000))
#define I2C4_BASE		(SOC_BASE + ULL(0xFF64400000))
#define I2C5_BASE		(SOC_BASE + ULL(0xFF64500000))
#define I2C6_BASE		(SOC_BASE + ULL(0xFF64600000))
#define I2C7_BASE		(SOC_BASE + ULL(0xFF64700000))
#define I2CSLV0_BASE		(SOC_BASE + ULL(0xFF64800000))
#define I2CSLV1_BASE		(SOC_BASE + ULL(0xFF64900000))
#define I2CSLV2_BASE		(SOC_BASE + ULL(0xFF64A00000))
#define I2CSLV3_BASE		(SOC_BASE + ULL(0xFF64B00000))
#define WDT0_BASE		(SOC_BASE + ULL(0xFF65000000))
#define WDT1_BASE		(SOC_BASE + ULL(0xFF65100000))
#define TIMER0_BASE		(SOC_BASE + ULL(0xFF66000000))
#define TIMER1_BASE		(SOC_BASE + ULL(0xFF66100000))
#define TIMER2_BASE		(SOC_BASE + ULL(0xFF66200000))
#define TIMER3_BASE		(SOC_BASE + ULL(0xFF66300000))
#define SD_BASE			(SOC_BASE + ULL(0xFF67000000))
#define XGMAC_BASE		(SOC_BASE + ULL(0xFF6A000000))
#define ETH_PLL_BASE		(SOC_BASE + ULL(0xFF6A100000))
#define XPCS_BASE		(SOC_BASE + ULL(0xFF6A200000))
#define PLIC_BASE		(SOC_BASE + ULL(0xFF70000000))

/* Config fabric */
#define CLINT_BASE		(SOC_BASE + ULL(0xFF010F0000))
#define CLUSTER0_BASE		(SOC_BASE + ULL(0xFF01000000))
#define CLUSTER1_BASE		(SOC_BASE + ULL(0xFF01100000))
#define CLUSTER2_BASE		(SOC_BASE + ULL(0xFF01200000))
#define CLUSTER3_BASE		(SOC_BASE + ULL(0xFF01300000))
#define COHFAB_BASE		(SOC_BASE + ULL(0xFF02000000))
#define COHFAB_PLL_BASE		(SOC_BASE + ULL(0xFF02100000))
#define DMA_BASE		(SOC_BASE + ULL(0xFF03000000))
#define DDR0_CTRL_BASE		(SOC_BASE + ULL(0xFF04000000))
#define DDR0_PHY_BASE		(SOC_BASE + ULL(0xFF05000000))
#define DDR1_CTRL_BASE		(SOC_BASE + ULL(0xFF06000000))
#define DDR1_PHY_BASE		(SOC_BASE + ULL(0xFF07000000))
#define DMA_SMMU_BASE		(SOC_BASE + ULL(0xFF08000000))
#define PCIE_SMMU_BASE		(SOC_BASE + ULL(0xFF08400000))
#define PCIE_SUB_CUST_BASE	(SOC_BASE + ULL(0xFF09000000))
#define PCIE0_CUST_BASE		(SOC_BASE + ULL(0xFF09001000))
#define PCIE1_CUST_BASE		(SOC_BASE + ULL(0xFF09002000))
#define PCIE2_CUST_BASE		(SOC_BASE + ULL(0xFF09003000))
#define PCIE3_CUST_BASE		(SOC_BASE + ULL(0xFF09004000))
#define PCIE0_CTRL_BASE		(SOC_BASE + ULL(0xFF09100000))
#define PCIE0_PHY_BASE		(SOC_BASE + ULL(0xFF09180000))
#define PCIE1_CTRL_BASE		(SOC_BASE + ULL(0xFF09200000))
#define PCIE1_PHY_BASE		(SOC_BASE + ULL(0xFF09280000))
#define PCIE2_CTRL_BASE		(SOC_BASE + ULL(0xFF09300000))
#define PCIE2_PHY_BASE		(SOC_BASE + ULL(0xFF09380000))
#define PCIE3_CTRL_BASE		(SOC_BASE + ULL(0xFF09400000))
#define PCIE3_PHY_BASE		(SOC_BASE + ULL(0xFF09480000))
#define TSENSOR_BASE		(SOC_BASE + ULL(0xFF0A000000))

#define MSG_BASE		(SOC_BASE + ULL(0xFF601FFF00))

#define DEV_BASE		(SOC_BASE + ULL(0xFF00000000))
#define DEV_SIZE		ULL(0x0100000000)

/* DDR memory region */
#define __DDR_BASE		ULL(0x0000000000)
#define DDR_BASE		(SOC_BASE + __DDR_BASE)

/* PCIe memory region */
#define PCIE_BASE		(SOC_BASE + ULL(0x40000000000))
#define PCIE_SIZE		SZ_2T

#endif /* __REG_SOCv3_DUOWEN_H_INCLUDE__ */
