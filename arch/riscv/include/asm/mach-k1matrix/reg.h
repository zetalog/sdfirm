/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)reg.h: K1Matrix specific register layout definitions
 * $Id: reg.h,v 1.1 2023-09-06 18:10:00 zhenglv Exp $
 */
#ifndef __REG_K1MATRIX_H_INCLUDE__
#define __REG_K1MATRIX_H_INCLUDE__
#ifndef __ASSEMBLY__
extern unsigned long k1matrix_die_base;
#endif
#define DIE_BASE			k1matrix_die_base
#define DIE0_BASE			ULL(0x00000000000)
#define DIE0_IO_BASE			ULL(0x00400000000)
#define DIE1_IO_BASE			ULL(0x00480000000)
#define DIE2_IO_BASE			ULL(0x00500000000)
#define DIE3_IO_BASE			ULL(0x00580000000)
#define DIE0_PCIE_IO_BASE		ULL(0x04000000000)
#define DIE1_PCIE_IO_BASE		ULL(0x05000000000)
#define DIE2_PCIE_IO_BASE		ULL(0x06000000000)
#define DIE3_PCIE_IO_BASE		ULL(0x07000000000)
#define DIE0_PCIE_SLV_MEM_BASE		ULL(0x40000000000)
#define DIE1_PCIE_SLV_MEM_BASE		ULL(0x50000000000)
#define DIE2_PCIE_SLV_MEM_BASE		ULL(0x60000000000)
#define DIE3_PCIE_SLV_MEM_BASE		ULL(0x70000000000)
#define DIE0_DRAM_REGION0_BASE		ULL(0x00000000000)
#define DIE0_DRAM_REGION1_BASE		ULL(0xC0100000000)
#define DIE1_DRAM_BASE			ULL(0xD0000000000)
#define DIE2_DRAM_BASE			ULL(0xE0000000000)
#define DIE3_DRAM_BASE			ULL(0xF0000000000)
#define __DDR_BASE			ULL(0x00000000000)
#define __DDR_RESION0_BASE	 	ULL(0x00000000000)
#define __RMU_SPI_XIP_BASE		ULL(0x00400000000)
#define __RMU_ROM_BASE			ULL(0x00408000000)
#define __RMU_RAM_BASE			ULL(0x00408100000)
#define __RMU_MAILBOX_EHSM_BASE		ULL(0x0040C000000)
#define __RMU_EFUSE_S_BASE		ULL(0x0040C010000)
#define __RMU_LS_DMA_BASE		ULL(0x0040C020000)
#define __RMU_SPI_M_0_BASE		ULL(0x0040C400000)
#define __RMU_SPI_S_0_BASE		ULL(0x0040C410000)
#define __RMU_QSPI_BASE			ULL(0x0040C420000)
#define __RMU_SMBUS0_BASE		ULL(0x0040C500000)
#define __RMU_SMBUS1_BASE		ULL(0x0040C510000)
#define __RMU_I2C0_BASE			ULL(0x0040C600000)
#define __RMU_I2C1_BASE			ULL(0x0040C610000)
#define __RMU_I2C2_BASE			ULL(0x0040C620000)
#define __RMU_I2C3_BASE			ULL(0x0040C630000)
#define __RMU_GPIOA_BASE		ULL(0x0040C700000)
#define __RMU_WDT_BASE			ULL(0x0040C710000)
#define __RMU_TIMER0_3_BASE		ULL(0x0040C720000)
#define __RMU_REG_BASE			ULL(0x0040C730000)
#define __RMU_PLIC_BASE			ULL(0x00410000000)
#define __RMU_CLINT_BASE		ULL(0x00414000000)
#define __RMU_UART0_BASE		ULL(0x00418000000)
#define __RMU_UART1_BASE		ULL(0x00418010000)
#define __RMU_TIMER_S_BASE		ULL(0x00418020000)
#define __RMU_MAILBOX_S_BASE		ULL(0x00418030000)
#define __RMU_PIC_EX_BASE		ULL(0x00418040000)
#define __RMU_PVT_CRTL_BASE		ULL(0x00418050000)
#define __RMU_STM_BASE			ULL(0x00418060000)
#define __RMU_SYS_REG_BASE		ULL(0x00418070000)
#define __RMU_SYS_CRG_BASE		ULL(0x00418080000)
#define __RMU_LPC_CFG_BASE		ULL(0x0041A000000)
#define __RMU_ESPI_CFG_BASE		ULL(0x0041A010000)
#define __RMU_SPINLOCK_BASE		ULL(0x0041A200000)
#define __RMU_MAILBOX_NS_BASE		ULL(0x0041A210000)
#define __RMU_EMUSE_NS_BASE		ULL(0x0041A220000)
#define __RMU_LPC_IO_BASE		ULL(0x0041BE00000)
#define __RMU_ESPI_IO_BASE		ULL(0x0041BE10000)
#define __RMU_LPC_FLASH_BASE		ULL(0x0041C000000)
#define __RMU_ESPI_FLASH_BASE		ULL(0x0041E000000)
#define __SPI0_BASE			ULL(0x00422000000)
#define __SPI1_BASE			ULL(0x00422010000)
#define __SPI2_BASE			ULL(0x00422020000)
#define __SPI3_BASE			ULL(0x00422030000)
#define __SMBUS1_BASE			ULL(0x00422100000)
#define __SMBUS2_BASE			ULL(0x00422110000)
#define __I2C0_BASE			ULL(0x00422200000)
#define __I2C1_BASE			ULL(0x00422210000)
#define __I2C2_BASE			ULL(0x00422220000)
#define __I2C3_BASE			ULL(0x00422230000)
#define __I2C4_BASE			ULL(0x00422240000)
#define __I2C5_BASE			ULL(0x00422250000)
#define __I2C6_BASE			ULL(0x00422260000)
#define __I2C7_BASE			ULL(0x00422270000)
#define __UART0_BASE			ULL(0x00422300000)
#define __UART1_BASE			ULL(0x00422310000)
#define __UART2_BASE			ULL(0x00422320000)
#define __UART3_BASE			ULL(0x00422330000)
#define __GPIOA_BASE			ULL(0x00422400000)
#define __GPIOB_BASE			ULL(0x00422410000)
#define __GPIOC_BASE			ULL(0x00422420000)
#define __GPIOD_BASE			ULL(0x00422430000)
#define __GPIOE_BASE			ULL(0x00422440000)
#define __WDT0_BASE			ULL(0x00422500000)
#define __WDT1_BASE			ULL(0x00422510000)
#define __WDT2_BASE			ULL(0x00422520000)
#define __WDT3_BASE			ULL(0x00422530000)
#define __WDT4_BASE			ULL(0x00422540000)
#define __WDT5_BASE			ULL(0x00422550000)
#define __WDT6_BASE			ULL(0x00422560000)
#define __WDT7_BASE			ULL(0x00422570000)
#define __TIMER_NS_0_7_BASE		ULL(0x00422700000)
#define __TIMER_NS_8_15_BASE		ULL(0x00422710000)
#define __TIMER_NS_16_23_BASE		ULL(0x00422720000)
#define __TIMER_NS_24_31_BASE		ULL(0x00422730000)
#define __HS_DMA_BASE			ULL(0x00424C00000)
#define __GMAC_BASE			ULL(0x00424C20000)
#define __S_ACLINT_HARTX_SSIP_BASE	ULL(0x00430400000)
#define __S_APLIC_S_DOMAIN_CFG_BASE	ULL(0x00430500000)
#define __S_MODE_MSI_BASE		ULL(0x00430600000)
#define __M_ACLINT_HARTX_MSIP_BASE	ULL(0x00432000000)
#define __M_ACLINT_HARTX_MTIP_BASE	ULL(0x00432100000)
#define __M_APLIC_M_DOMAIN_CFG_BASE	ULL(0x00432200000)
#define __M_MODE_MSI_BASE		ULL(0x00432300000)
#define __NIC400_PIC_GPV_BASE		ULL(0x00433B00000)
#define __IOPMP_CTRL_BASE		ULL(0x00433C00000)
#define __DDR_CHA_PHY_BASE		ULL(0x00434000000)
#define __DDR_CHA_CRTL_BASE		ULL(0x00435000000)
#define __DDR_CHA_LSC_BASE		ULL(0x00435800000)
#define __DDR_CHB_PHY_BASE		ULL(0x00436000000)
#define __DDR_CHB_CRTL_BASE		ULL(0x00437000000)
#define __DDR_CHB_LSC_BASE		ULL(0x00437800000)
#define __DDR_CHC_PHY_BASE		ULL(0x00438000000)
#define __DDR_CHC_CRTL_BASE		ULL(0x00439000000)
#define __DDR_CHC_LSC_BASE		ULL(0x00439800000)
#define __DDR_CHD_PHY_BASE		ULL(0x0043A000000)
#define __DDR_CHD_CRTL_BASE		ULL(0x0043B000000)
#define __DDR_CHD_LSC_BASE		ULL(0x0043B800000)
#define __N100_CFG_BASE			ULL(0x00440000000)
#define __N100_OCM_BASE			ULL(0x00444000000)
#define __ACPU_RAS_SPACE_BASE		ULL(0x00448000000)
#define __PCIE0_SLV_BASE		ULL(0x04000000000)
#define __PCIE1_SLV_BASE		ULL(0x04100000000)
#define __PCIE2_SLV_BASE		ULL(0x04200000000)
#define __PCIE3_SLV_BASE		ULL(0x04300000000)
#define __PCIE0_DBI_BASE		ULL(0x04700000000)
#define __PCIE0_APP_BASE		ULL(0x04700400000)
#define __PCIE_4L_RC_0_PHY_BASE		ULL(0x04700600000)
#define __PCIE1_DBI_BASE		ULL(0x04700700000)
#define __PCIE1_APP_BASE		ULL(0x04700B00000)
#define __PCIE_4L_RC_1_PHY_BASE		ULL(0x04700C00000)
#define __PCIE2_DBI_BASE		ULL(0x04700D00000)
#define __PCIE2_APP_BASE		ULL(0x04701100000)
#define __PCIE3_DBI_BASE		ULL(0x04701200000)
#define __PCIE3_APP_BASE		ULL(0x04701600000)
#define __PCIE_16L88L_DM_CCIX_0_1_PHY_BASE	ULL(0x04701700000)
#define __IOMMU_CRTL0_CFG_BASE		ULL(0x04780000000)
#define __IOMMU_CRTL1_CFG_BASE		ULL(0x04780200000)
#define __PCIE4_SLV_BASE		ULL(0x04800000000)
#define __PCIE5_SLV_BASE		ULL(0x04900000000)
#define __PCIE6_SLV_BASE		ULL(0x04A00000000)
#define __PCIE7_SLV_BASE		ULL(0x04B00000000)
#define __PCIE8_SLV_BASE		ULL(0x04C00000000)
#define __PCIE9_SLV_BASE		ULL(0x04D00000000)
#define __PCIE4_DBI_BASE		ULL(0x04F00000000)
#define __PCIE4_APP_BASE		ULL(0x04F00400000)
#define __PCIE5_DBI_BASE		ULL(0x04F00500000)
#define __PCIE5_APP_BASE		ULL(0x04F00900000)
#define __PCIE_8L44L_DM_CCIX_2_PHY_BASE	ULL(0x04F00A00000)
#define __PCIE6_DBI_BASE		ULL(0x04F00B00000)
#define __PCIE6_APP_BASE		ULL(0x04F00F00000)
#define __PCIE7_DBI_BASE		ULL(0x04F01000000)
#define __PCIE7_APP_BASE		ULL(0x04F01400000)
#define __PCIE8_DBI_BASE		ULL(0x04F01500000)
#define __PCIE8_APP_BASE		ULL(0x04F01900000)
#define __PCIE_4L211L_RC_PHY_BASE	ULL(0x04F01A00000)
#define __PCIE_9_DBI_BASE		ULL(0x04F01B00000)
#define __PCIE_9_APP_BASE		ULL(0x04F01F00000)
#define __PCIE_16L_RC_PHY_BASE		ULL(0x04F02000000)
#define __IOMMU_CRTL2_CFG_BASE		ULL(0x04F80000000)
#define __IOMMU_CRTL3_CFG_BASE		ULL(0x04F80200000)
#define __PCIE0_MEM_BASE		ULL(0x40000000000)
#define __PCIE1_MEM_BASE		ULL(0x40100000000)
#define __PCIE2_MEM_BASE		ULL(0x40200000000)
#define __PCIE3_MEM_BASE		ULL(0x43800000000)
#define __PCIE4_MEM_BASE		ULL(0x44000000000)
#define __PCIE5_MEM_BASE		ULL(0x44800000000)
#define __PCIE6_MEM_BASE		ULL(0x44900000000)
#define __PCIE7_MEM_BASE		ULL(0x44A00000000)
#define __PCIE8_MEM_BASE		ULL(0x44B00000000)
#define __PCIE9_MEM_BASE		ULL(0x48000000000)
#define __DRAM_REGION1_BASE		ULL(0xC0100000000)
#define SPACEMIT_ESPI_BASE		__RMU_ESPI_CFG_BASE
#define SPACEMIT_ESPI_IO_BASE		__RMU_ESPI_IO_BASE
#define SPACEMIT_ESPI_FLASH_BASE	__RMU_ESPI_FLASH_BASE
#define SPACEMIT_ESPI_PR_MEM0		(0x22000000U)
#define SPACEMIT_ESPI_PR_MEM1		(0x23000000U)
#if defined(CONFIG_K1MATRIX_ZSBL) || defined(CONFIG_K1MATRIX_FSBL)
#define CLINT_BASE			__RMU_CLINT_BASE
#define UART0_BASE			__RMU_UART0_BASE
#define SPACEMIT_LPC_BASE		__RMU_LPC_CFG_BASE
#define CORELPC_BASE			__RMU_LPC_IO_BASE
#define KCS_BASE			__RMU_LPC_IO_BASE
#else
#endif
#define N100_CFG_BASE			__N100_CFG_BASE
#define SYS_REG_BASE			__RMU_SYS_CRG_BASE
#define SRAM0_BASE			__RMU_RAM_BASE
#define SRAM01_SIZE			SZ_512K
#define DDR_REGION0_SIZE		SZ_4G
#include <asm/mach/cprint.h>
#ifdef CONFIG_K1MATRIX_BOOT_LLP
#define BROM_BASE		__RMU_ROM_BASE
#endif /* CONFIG_K1MATRIX_BOOT_LLP */
#ifdef CONFIG_K1MATRIX_BOOT_SYS
#define BROM_BASE		SRAM1_BASE
#endif /* CONFIG_K1MATRIX_BOOT_SYS */
#ifdef CONFIG_K1MATRIX_ROM
#define BROM_SIZE		SZ_64K
#else /* CONFIG_K1MATRIX_ROM */
#define BROM_SIZE		0
#endif /* CONFIG_K1MATRIX_ROM */
#define SRAM_BASE		(SRAM0_BASE)
#define SRAM_SIZE		(SRAM01_SIZE)
#ifdef CONFIG_K1MATRIX_DDR_SIZE_CUSTOM
#define DRAM_SIZE		CONFIG_K1MATRIX_MEM_SIZE
#else /* CONFIG_K1MATRIX_DDR_SIZE_CUSTOM */
#define DRAM_SIZE		DDR_REGION0_SIZE
#endif /* CONFIG_K1MATRIX_DDR_SIZE_CUSTOM */
#ifdef CONFIG_K1MATRIX_ZSBL
#define ROM_BASE		BROM_BASE
#define ROM_SIZE		BROM_SIZE
#define RAM_BASE		SRAM_BASE
#define RAM_SIZE		SRAM_SIZE
#endif /* CONFIG_K1MATRIX_ZSBL */
#ifdef CONFIG_K1MATRIX_FSBL
#define ROM_BASE		SRAM_BASE
#define ROM_SIZE		SRAM_SIZE
#define RAM_BASE		SRAM_BASE
#define RAM_SIZE		SRAM_SIZE
#endif /* CONFIG_K1MATRIX_FSBL */
#ifdef CONFIG_K1MATRIX_BBL
#define ROM_BASE		__DDR_RESION0_BASE
#define ROM_SIZE		DRAM_SIZE
#define RAM_BASE		__DDR_RESION0_BASE
#define RAM_SIZE		DRAM_SIZE
#endif /* CONFIG_K1MATRIX_BBL */
#ifdef CONFIG_K1MATRIX_TB
#ifdef CONFIG_K1MATRIX_TB_ROM
#define ROM_BASE		BROM_BASE
#define ROM_SIZE		BROM_SIZE
#define RAM_BASE		SRAM_BASE
#define RAM_SIZE		SRAM_SIZE
#endif
#ifdef CONFIG_K1MATRIX_TB_SRAM
#define ROM_BASE		SRAM_BASE
#define ROM_SIZE		SRAM_SIZE
#define RAM_BASE		SRAM_BASE
#define RAM_SIZE		SRAM_SIZE
#endif
#ifdef CONFIG_K1MATRIX_TB_DDR
#define ROM_BASE		__DDR_RESION0_BASE
#define ROM_SIZE		DRAM_SIZE
#define RAM_BASE		__DDR_RESION0_BASE
#define RAM_SIZE		DRAM_SIZE
#endif
#endif /* CONFIG_K1MATRIX_TB */
#define ROMEND			(ROM_BASE + ROM_SIZE)
#define RAMEND			(RAM_BASE + RAM_SIZE)
#endif /* __REG_K1MATRIX_H_INCLUDE__ */
