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
 * @(#)reg.h: DUOWEN space and register definitions
 * $Id: reg.h,v 1.1 2019-09-02 11:12:00 zhenglv Exp $
 */

#ifndef __REG_DUOWEN_H_INCLUDE__
#define __REG_DUOWEN_H_INCLUDE__

#ifndef __ASSEMBLY__
extern unsigned long duowen_soc_base;
#endif

#define SOC0_BASE		0
#define SOC1_BASE		ULL(0x80000000000)
#define __SOC_BASE(soc)		((uint64_t)(soc) << 43)
#define SOC_BASE		duowen_soc_base

/* Local fabric */
#define LCSR_BASE		ULL(0xFF91000000)
#define DEBUG_BASE		ULL(0xFF90000000)
#define SFAB_ROM_BASE		ULL(0xFF8F000000) /* Boot ROM */
#define SFAB_ROM_SIZE		0x100000
#define FLASH_BASE		ULL(0xFF88000000)
/* The SPINOR model uses 24-bit addressing, however some of the addresses
 * are overlapped with controller own registers, so it can only access
 * <16MB address space, say 15M here
 */
#define FLASH_SIZE		0x1E00000
#define SFAB_RAM_BASE		ULL(0xFF80000000) /* Sys Mem */
#define SFAB_RAM_SIZE		0x100000

/* System fabric */
#define __CRCNTL_BASE		ULL(0xFF60000000)
#define __SCSR_BASE		ULL(0xFF60100000)
#define __TLMM_BASE		ULL(0xFF60200000)
#define __GPIO0_BASE		ULL(0xFF61000000)
#define __GPIO1_BASE		ULL(0xFF61100000)
#define __GPIO2_BASE		ULL(0xFF61200000)
#define __XGMAC_BASE		ULL(0xFF6A000000)
#define __ETH_PLL_BASE		ULL(0xFF6A100000)
#define __XPCS_BASE		ULL(0xFF6A200000)
#define __PLIC_BASE		ULL(0xFF70000000)

#define CRCNTL_BASE		(SOC_BASE + __CRCNTL_BASE)
#define SCSR_BASE		(SOC_BASE + __SCSR_BASE)
#define TLMM_BASE		(SOC_BASE + __TLMM_BASE)
#define IMC_BASE		(SOC_BASE + ULL(0xFF60600000))
#define GPIO0_BASE		(SOC_BASE + __GPIO0_BASE)
#define GPIO1_BASE		(SOC_BASE + __GPIO1_BASE)
#define GPIO2_BASE		(SOC_BASE + __GPIO2_BASE)
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
#define XGMAC_BASE		(SOC_BASE + __XGMAC_BASE)
#define ETH_PLL_BASE		(SOC_BASE + __ETH_PLL_BASE)
#define XPCS_BASE		(SOC_BASE + __XPCS_BASE)
#define IRQC_BASE		(SOC_BASE + __PLIC_BASE)

/* Config fabric */
#define __CLUSTER0_BASE		ULL(0xFF01000000)
#define __CLUSTER1_BASE		ULL(0xFF01100000)
#define __CLUSTER2_BASE		ULL(0xFF01200000)
#define __CLUSTER3_BASE		ULL(0xFF01300000)
#define __COHFAB_PLL_BASE	ULL(0xFF02100000)
#define __DMA_SMMU_BASE		ULL(0xFF08000000)

#define CLINT_BASE		(SOC_BASE + ULL(0xFF010F0000))
#define CLUSTER0_BASE		(SOC_BASE + __CLUSTER0_BASE)
#define CLUSTER1_BASE		(SOC_BASE + __CLUSTER1_BASE)
#define CLUSTER2_BASE		(SOC_BASE + __CLUSTER2_BASE)
#define CLUSTER3_BASE		(SOC_BASE + __CLUSTER3_BASE)
#define COHFAB_BASE		(SOC_BASE + ULL(0xFF02000000))
#define COHFAB_PLL_BASE		(SOC_BASE + __COHFAB_PLL_BASE)
#define DMA_BASE		(SOC_BASE + ULL(0xFF03000000))
#define DDR0_CTRL_BASE		(SOC_BASE + ULL(0xFF04000000))
#define DDR0_PHY_BASE		(SOC_BASE + ULL(0xFF05000000))
#define DDR1_CTRL_BASE		(SOC_BASE + ULL(0xFF06000000))
#define DDR1_PHY_BASE		(SOC_BASE + ULL(0xFF07000000))
#define DMA_SMMU_BASE		(SOC_BASE + __DMA_SMMU_BASE)
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

#define __DEV_BASE		ULL(0xFF00000000)
#define DEV_BASE		(SOC_BASE + __DEV_BASE)
#define DEV_SIZE		ULL(0x0100000000)

/* DDR memory region */
#define __DDR_BASE		ULL(0x0000000000)
#define DDR_BASE		(SOC_BASE + __DDR_BASE)
#define __DDR_SIZE		SZ_512G

/* PCIe memory region */
#define __PCIE_BASE		ULL(0x40000000000)
#define PCIE_BASE		(SOC_BASE + __PCIE_BASE)
#define PCIE_SIZE		SZ_2T

/* DDR memory region */
#ifdef CONFIG_DUOWEN_DDR_32M
#define DDR_SIZE		SZ_32M
#endif
#ifdef CONFIG_DUOWEN_DDR_64M
#define DDR_SIZE		SZ_64M
#endif
#ifdef CONFIG_DUOWEN_DDR_128M
#define DDR_SIZE		SZ_128M
#endif
#ifdef CONFIG_DUOWEN_DDR_256M
#define DDR_SIZE		SZ_256M
#endif
#ifdef CONFIG_DUOWEN_DDR_512M
#define DDR_SIZE		SZ_512M
#endif
#ifdef CONFIG_DUOWEN_DDR_1G
#define DDR_SIZE		SZ_1G
#endif
#ifdef CONFIG_DUOWEN_DDR_2G
#define DDR_SIZE		SZ_2G
#endif
#ifdef CONFIG_DUOWEN_DDR_4G
#define DDR_SIZE		SZ_4G
#endif
#ifdef CONFIG_DUOWEN_DDR_8G
#define DDR_SIZE		SZ_8G
#endif
#ifdef CONFIG_DUOWEN_DDR_16G
#define DDR_SIZE		SZ_16G
#endif
#ifdef CONFIG_DUOWEN_DDR_32G
#define DDR_SIZE		SZ_32G
#endif
#ifdef CONFIG_DUOWEN_DDR_64G
#define DDR_SIZE		SZ_64G
#endif
#ifdef CONFIG_DUOWEN_DDR_128G
#define DDR_SIZE		SZ_128G
#endif
#ifdef CONFIG_DUOWEN_DDR_256G
#define DDR_SIZE		SZ_256G
#endif
#ifdef CONFIG_DUOWEN_DDR_512G
#define DDR_SIZE		SZ_512G
#endif

/* Reserved APC self-bootloader RAM */
#define ZSBL_ROM_SIZE		0xE0000
#define ASBL_ROM_SIZE		(SFAB_ROM_SIZE - ZSBL_ROM_SIZE)
#define ASBL_RAM_SIZE		0x8000
#define FSBL_RAM_SIZE		(SFAB_RAM_SIZE - ASBL_RAM_SIZE)

#define IMC_ROM_BASE		SFAB_ROM_BASE
#define IMC_ROM_SIZE		ZSBL_ROM_SIZE
#define APC_ROM_BASE		(SFAB_ROM_BASE + ZSBL_ROM_SIZE)
#define APC_ROM_SIZE		ASBL_ROM_SIZE

#define IMC_RAM_BASE		SFAB_RAM_BASE
#ifdef CONFIG_DUOWEN_FSBL
#define IMC_RAM_SIZE		FSBL_RAM_SIZE
#else /* CONFIG_DUOWEN_FSBL */
#define IMC_RAM_SIZE		SFAB_RAM_SIZE
#endif /* CONFIG_DUOWEN_FSBL */

#ifdef CONFIG_DUOWEN_ASBL
#define APC_RAM_BASE		(SFAB_RAM_BASE + FSBL_RAM_SIZE)
#define APC_RAM_SIZE		ASBL_RAM_SIZE
#else /* CONFIG_DUOWEN_ASBL */
#define APC_RAM_BASE		SFAB_RAM_BASE
#define APC_RAM_SIZE		SFAB_RAM_SIZE
#endif /* CONFIG_DUOWEN_ASBL */

#ifdef CONFIG_DUOWEN_ASBL
#define ROM_BASE		APC_ROM_BASE
#define ROM_SIZE		APC_ROM_SIZE
#endif /* CONFIG_DUOWEN_ASBL */

#ifdef CONFIG_DUOWEN_ZSBL
#ifdef CONFIG_DUOWEN_BOOT_APC
#define ROM_BASE		APC_ROM_BASE
#define ROM_SIZE		APC_ROM_SIZE
#else /* CONFIG_DUOWEN_BOOT_APC */
#define ROM_BASE		IMC_ROM_BASE
#define ROM_SIZE		IMC_ROM_SIZE
#endif /* CONFIG_DUOWEN_BOOT_APC */
#endif /* CONFIG_DUOWEN_ZSBL */

#ifdef CONFIG_DUOWEN_FSBL
#ifdef CONFIG_DUOWEN_FSBL_SPI
#define ROM_BASE		FLASH_BASE
#define ROM_SIZE		FLASH_SIZE
#else /* CONFIG_DUOWEN_FSBL_SPI */
#ifdef CONFIG_DUOWEN_BOOT_APC
#define ROM_BASE		APC_RAM_BASE
#define ROM_SIZE		APC_RAM_SIZE
#else /* CONFIG_DUOWEN_BOOT_APC */
#define ROM_BASE		IMC_RAM_BASE
#define ROM_SIZE		IMC_RAM_SIZE
#endif /* CONFIG_DUOWEN_BOOT_APC */
#endif /* CONFIG_DUOWEN_FSBL_SPI */
#endif /* CONFIG_DUOWEN_FSBL */

#ifdef CONFIG_DUOWEN_BBL
#define ROM_BASE		__DDR_BASE
#define ROM_SIZE		__DDR_SIZE
#endif /* CONFIG_DUOWEN_BBL */

#ifdef CONFIG_DUOWEN_TB
#ifdef CONFIG_DUOWEN_TB_DDR
#define ROM_BASE		__DDR_BASE
#define ROM_SIZE		__DDR_SIZE
#endif /* CONFIG_DUOWEN_TB_DDR */
#ifdef CONFIG_DUOWEN_TB_RAM
#define ROM_BASE		SFAB_RAM_BASE
#define ROM_SIZE		SFAB_RAM_SIZE
#endif /* CONFIG_DUOWEN_TB_RAM */
#ifdef CONFIG_DUOWEN_TB_SPI
#define ROM_BASE		FLASH_BASE
#define ROM_SIZE		FLASH_SIZE
#endif /* CONFIG_DUOWEN_TB_SPI */
#endif /* CONFIG_DUOWEN_TB */

#ifdef CONFIG_DUOWEN_BOOT_APC
#define RAM_BASE		APC_RAM_BASE
#define RAM_SIZE		APC_RAM_SIZE
#else /* CONFIG_DUOWEN_BOOT_APC */
#if defined(CONFIG_DUOWEN_BBL) || defined(CONFIG_DUOWEN_TB_DDR)
#define RAM_BASE		__DDR_BASE
#define RAM_SIZE		DDR_SIZE
#else /* CONFIG_DUOWEN_BBL || CONFIG_DUOWEN_TB_DDR */
#define RAM_BASE		IMC_RAM_BASE
#define RAM_SIZE		IMC_RAM_SIZE
#endif /* CONFIG_DUOWEN_BBL || CONFIG_DUOWEN_TB_DDR */
#endif /* CONFIG_DUOWEN_BOOT_APC */

#define ROMEND			(ROM_BASE + ROM_SIZE)
#define RAMEND			(RAM_BASE + RAM_SIZE)

#endif /* __REG_DUOWEN_H_INCLUDE__ */
