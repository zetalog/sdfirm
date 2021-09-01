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
 * @(#)reg.h: DPU space and register definitions
 * $Id: reg.h,v 1.1 2020-03-02 16:23:00 zhenglv Exp $
 */

#ifndef __REG_DPU_H_INCLUDE__
#define __REG_DPU_H_INCLUDE__

/* CSRs */
#define CSR_UHARTID		0x014
#define CSR_PRIVLV		0xC10

/* Hardware loop extension */
#define CSR_LPSTART(n)		(0x7C0 + (n) << 2) /* LPSTART[0, 1] */
#define CSR_LPEND(n)		(0x7C1 + (n) << 2) /* LPEND[0, 1] */
#define CSR_LPCOUNT(n)		(0x7C2 + (n) << 2) /* LPCOUNT[0, 1] */

#ifdef CONFIG_DPU_GEN2
/* NOCFAB2 */
#define FLASH_BASE		UL(0x00000000)
#define UART_BASE		UL(0x04000000)
#define I2C0_BASE		UL(0x04000100)
#define I2C1_BASE		UL(0x04000200)
#define I2C2_BASE		UL(0x04000300)
#define GPIO_BASE		UL(0x04000400)
#define WDT_BASE		UL(0x04000500)
#define SSI_BASE		UL(0x04000600)
#define TIMER_BASE		UL(0x04001000)
#define TCSR_BASE		UL(0x04100000)
#define TSENSOR_BASE		UL(0x04300000)
#define PLL_REG_BASE		UL(0x04301000)

#define DDR0_PHY_BASE		UL(0x10000000)
#define DDR1_PHY_BASE		UL(0x11000000)
#define DDR0_CTRL_BASE		UL(0x14000000)
#define DDR1_CTRL_BASE		UL(0x14008000)
#define PE_DMA0_BASE		UL(0x14024000)
#define PE_DMA1_BASE		UL(0x14024100)
#define PE_MICC_BASE		UL(0x14024200)
#define PE_DEBUG_BASE		UL(0x14024300)
#define DBG_SLV_BASE		UL(0x14100000)
#define VPU0_BASE		UL(0x14300000)
#define VPU1_BASE		UL(0x14310000)
#define BOOTROM_BASE		UL(0x14400000)
#define SRAM_BASE		UL(0x15000000)
#define PLIC_REG_BASE		UL(0x18000000)
#define PCIEx_SUBSYS_CUST_BASE	UL(0x20000000)
#define PCIE0x16_CUST_BASE	UL(0x20000400)
#define PCIEx_PHY_BASE		UL(0x20000800)
#define PCIEx_PHY1_BASE		UL(0x20000C00)
#define PCIEx_PHY2_BASE		UL(0x20001000)
#define PCIEx_PHY3_BASE		UL(0x20001400)
#define PCIE0x16_CTRL_BASE	UL(0x20800000)
#define PCIEx_DBG_SLV_BASE	UL(0x20800000)

#define PCIE0x16_CDM_BASE       UL(0x20800000)
#define PCIE0x16_SDW_BASE       UL(0x20900000)
#define PCIE0x16_ATU_BASE       UL(0x20b00000)
#define PCIE0x16_DMA_BASE       UL(0x20b80000)
#define PCIE_CTRL_BASE		UL(0x14020000)
#define DDR_DEBUG_BASE		UL(0x04015800)
#define CLUSTER_CFG_BASE        UL(0x04600000)
#if 0
#define PCIE1x8_CTRL_BASE	UL(0x04380000)
#define PCIE2x4_CTRL_BASE	UL(0x04400000)
#define PCIE3x4_CTRL_BASE	UL(0x04480000)
#define PCIE1x8_CUST_BASE	UL(0x04512000)
#define PCIE2x4_CUST_BASE	UL(0x04513000)
#define PCIE3x4_CUST_BASE	UL(0x04514000)
#endif
#define DDR0_DATA_BASE		ULL(0x800000000)
#define DDR1_DATA_BASE		ULL(0x800000000)
#define PCIE0x16_SLV_BASE	ULL(0x100000000)
#else /* CONFIG_DPU_GEN2 */
#define FLASH_BASE		UL(0x00000000)
#define DDR0_PHY_BASE		UL(0x02000000)
#define DDR1_PHY_BASE		UL(0x03000000)
#define DDR0_CTRL_BASE		UL(0x04000000)
#define DDR1_CTRL_BASE		UL(0x04008000)
#define PCIE_CTRL_BASE		UL(0x04010000)
#define PLL_REG_BASE		UL(0x04014000)
#define UART_BASE		UL(0x04015000)
#define I2C0_BASE		UL(0x04015100)
#define I2C1_BASE		UL(0x04015200)
#define I2C2_BASE		UL(0x04015300)
#define PE_DMA0_BASE		UL(0x04015400)
#define PE_DMA1_BASE		UL(0x04015500)
#define PE_MICC_BASE		UL(0x04015600)
#define PE_DEBUG_BASE		UL(0x04015700)
#define DDR_DEBUG_BASE		UL(0x04015800)
#define GPIO_BASE		UL(0x04015900)
#define WDT_BASE		UL(0x04015A00)
#define SSI_BASE		UL(0x04015B00)
#define TIMER_BASE		UL(0x04015C00)
#define TCSR_BASE		UL(0x04100000)
#define DBG_SLV_BASE		UL(0x04200000)
#define PCIE0x16_CTRL_BASE	UL(0x04300000)
#define PCIE1x8_CTRL_BASE	UL(0x04380000)
#define PCIE2x4_CTRL_BASE	UL(0x04400000)
#define PCIE3x4_CTRL_BASE	UL(0x04480000)
#define PCIEx_PHY_BASE		UL(0x04500000)
#define PCIEx_SUBSYS_CUST_BASE	UL(0x04510000)
#define PCIE0x16_CUST_BASE	UL(0x04511000)
#define PCIE1x8_CUST_BASE	UL(0x04512000)
#define PCIE2x4_CUST_BASE	UL(0x04513000)
#define PCIE3x4_CUST_BASE	UL(0x04514000)
#define CLUSTER_CFG_BASE	UL(0x04600000)
#define BOOTROM_BASE		UL(0x06000000)
#define SRAM_BASE		UL(0x08000000)
#define PLIC_REG_BASE		UL(0x10000000)
#define DDR0_DATA_BASE		ULL(0x400000000)
#define DDR1_DATA_BASE		ULL(0x800000000)
#define PCIE0x16_SLV_BASE	ULL(0xC00000000)
#define PCIE1x8_SLV_BASE	ULL(0xD00000000)
#define PCIE2x4_SLV_BASE	ULL(0xE00000000)
#define PCIE3x4_SLV_BASE	ULL(0xF00000000)
#endif /* CONFIG_DPU_GEN2 */

#define SRAM_SIZE		0x100000
#define BOOTROM_SIZE		0x100000
#define DDR0_DATA_SIZE		ULL(0x400000000)
#define DDR1_DATA_SIZE		ULL(0x400000000)

#ifdef CONFIG_DPU_FIRM_DDR
#ifdef CONFIG_DPU_SIM_DDR_BOOT
#define DDR_DATA_BASE		DDR1_DATA_BASE
#define DDR_DATA_SIZE		(512 * 1024 * 1024)
#else
#define DDR_DATA_BASE		DDR0_DATA_BASE
#define DDR_DATA_SIZE		(DDR0_DATA_SIZE + DDR1_DATA_SIZE)
#endif
#ifdef CONFIG_DPU_FIRM_SIZE
#define DPU_FIRM_SIZE		CONFIG_DPU_FIRM_SIZE
#else
#define DPU_FIRM_SIZE		DDR_DATA_SIZE
#endif
#define ROM_BASE		DDR_DATA_BASE
#define ROMEND			(DDR_DATA_BASE + DPU_FIRM_SIZE)
#define RAM_BASE		DDR_DATA_BASE
#define RAMEND			(DDR_DATA_BASE + DDR_DATA_SIZE)
#endif /* CONFIG_DPU_FIRM_DDR */

#ifdef CONFIG_DPU_BOOT_ROM
#define ROM_BASE		BOOTROM_BASE
#define ROMEND			(ROM_BASE + BOOTROM_SIZE)
#define RAM_BASE		SRAM_BASE
#define RAMEND			(SRAM_BASE + SRAM_SIZE)
#endif /* CONFIG_DPU_BOOT_ROM */

#ifdef CONFIG_DPU_BOOT_FLASH
#define ROM_BASE		FLASH_BASE
#define ROMEND			(ROM_BASE + FLASH_SIZE)
#define RAM_BASE		SRAM_BASE
#define RAMEND			(SRAM_BASE + SRAM_SIZE)
#endif /* CONFIG_DPU_BOOT_FLASH */

#ifdef CONFIG_DPU_LOAD_FSBL
#define DDR_DATA_BASE		DDR0_DATA_BASE
/* #define DDR_DATA_SIZE		(DDR0_DATA_SIZE + DDR1_DATA_SIZE) */
#define DDR_DATA_SIZE		DDR0_DATA_SIZE
#ifdef CONFIG_DPU_FIRM_SIZE
#define DPU_FIRM_SIZE		CONFIG_DPU_FIRM_SIZE
#else /* CONFIG_DPU_FIRM_SIZE */
#define DPU_FIRM_SIZE		DDR_DATA_SIZE
#endif /* CONFIG_DPU_FIRM_SIZE */
#define ROM_BASE		BOOTROM_BASE
#define ROMEND			(ROM_BASE + BOOTROM_SIZE)
#define RAM_BASE		SRAM_BASE
#define RAMEND			(SRAM_BASE + SRAM_SIZE)
#endif /* CONFIG_DPU_LOAD_FSBL */

#ifdef CONFIG_DPU_BOOT_RAM
#define ROM_BASE		BOOTROM_BASE
#define ROMEND			(ROM_BASE + BOOTROM_SIZE)
#define RAM_BASE		SRAM_BASE
#define RAMEND			(SRAM_BASE + SRAM_SIZE)
#endif /* CONFIG_DPU_BOOT_RAM */

/* LOVEC */
#ifdef CONFIG_ARCH_HAS_LOVEC
#define VEC_BASE		BOOTROM_BASE
#endif

#endif /* __REG_DPU_H_INCLUDE__ */
