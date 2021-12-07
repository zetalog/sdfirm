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
 * @(#)reg.h: DPU-LP space and register definitions
 * $Id: reg.h,v 1.1 2021-11-01 14:03:00 zhenglv Exp $
 */

#ifndef __REG_DPULP_H_INCLUDE__
#define __REG_DPULP_H_INCLUDE__

#define FLASH_BASE		ULL(0x00000000000)
#define UART0_BASE		ULL(0x00004000000)
#define UART1_BASE		ULL(0x00004000100)
#define UART2_BASE		ULL(0x00004000200)
#define UART3_BASE		ULL(0x00004000300)
#define I2C0_BASE		ULL(0x00004000400)
#define I2C1_BASE		ULL(0x00004000500)
#define I2C2_BASE		ULL(0x00004000600)
#define I2C3_BASE		ULL(0x00004000700)
#define GPIO_BASE		ULL(0x00004000800)
#define WDT_BASE		ULL(0x00004000900)
#define SSI_BASE		ULL(0x00004000A00)
#define TIMER_BASE		ULL(0x00004001000)
#define TCSR_BASE		ULL(0x00004100000)
#define TSENS_BASE		ULL(0x00004300000)
#define CRU_BASE		ULL(0x00004302000)
#define SD_BASE			ULL(0x00004303000)
#define DDR0_PHY_BASE		ULL(0x00010000000)
#define DDR1_PHY_BASE		ULL(0x00011000000)
#define DDR0_CTRL_BASE		ULL(0x00014000000)
#define DDR1_CTRL_BASE		ULL(0x00014008000)
#define PE_DMA0_BASE		ULL(0x00014024000)
#define PE_DMA1_BASE		ULL(0x00014024100)
#define PE_MICC_BASE		ULL(0x00014024200)
#define PE_DEBUG_BASE		ULL(0x00014024300)
#define VPU0_BASE		ULL(0x00014300000)
#define VPU1_BASE		ULL(0x00014310000)
#define BROM_BASE		ULL(0x00014400000)
#define SRAM_BASE		ULL(0x00015000000)
#define XGMAC0_BASE		ULL(0x00016000000)
#define ETH0_BASE		ULL(0x00016100000)
#define XGMAC1_BASE		ULL(0x00016400000)
#define ETH1_BASE		ULL(0x00016500000)
#define CLINT_BASE		ULL(0x00016800000)
#define PLIC_BASE		ULL(0x00018000000)
#define PCIE_SUB_BASE		ULL(0x00020000000)
#define PCIE_X16_BASE		ULL(0x00020000400)
#define PCIE_P0_BASE		ULL(0x00020020000)
#define PCIE_P1_BASE		ULL(0x00020040000)
#define PCIE_P2_BASE		ULL(0x00020060000)
#define PCIE_P3_BASE		ULL(0x00020080000)
#define PCIE_DBI_SLV_BASE	ULL(0x00020800000)
#define RAB0_CTRL_BASE		ULL(0x00021000000)
#define RAB1_CTRL_BASE		ULL(0x00022000000)
#define RAB0_PHY_BASE		ULL(0x00023000000)
#define RAB1_PHY_BASE		ULL(0x00024000000)
#define PCIE_SLV_BASE		ULL(0x00100000000)
#define DDR0_DATA_BASE		ULL(0x01000000000)
#define DDR0C0_DATA_BASE	DDR0_DATA_BASE
#define DDR0C1_DATA_BASE	(DDR0_DATA_BASE + DDRC_DATA_SIZE)
#define DDR1_DATA_BASE		ULL(0x01800000000)
#define DDR1C0_DATA_BASE	DDR1_DATA_BASE
#define DDR1C1_DATA_BASE	(DDR1_DATA_BASE + DDRC_DATA_SIZE)
#define RAB0_SLV_BASE		ULL(0x02000000000)
#define RAB1_SLV_BASE		ULL(0x02400000000)

#define SRAM_SIZE		0x100000
#define BROM_SIZE		0x100000
#define DDRC_DATA_SIZE		ULL(0x400000000) /* 16GB */
#define RAB_SLV_SIZE		ULL(0x100000000) /* 4GB */

#ifdef CONFIG_DPULP_SIM_DDR_BOOT
#define DDR_DATA_BASE		DDR1_DATA_BASE
#define DDR_DATA_SIZE		(512 * 1024 * 1024)
#else
#define DDR_DATA_BASE		DDR0_DATA_BASE
#define DDR_DATA_SIZE		(2 * DDR_CH_DATA_SIZE) /* CH0 + CH1 */
#endif
#ifdef CONFIG_DPULP_FIRM_SIZE
#define DPULP_FIRM_SIZE		CONFIG_DPULP_FIRM_SIZE
#else /* CONFIG_DPULP_FIRM_SIZE */
#define DPULP_FIRM_SIZE		DDR_DATA_SIZE
#endif /* CONFIG_DPULP_FIRM_SIZE */

#ifdef CONFIG_DPULP_BOOT_ROM
#define ROM_BASE		BROM_BASE
#define ROMEND			(ROM_BASE + BROM_SIZE)
#endif /* CONFIG_DPULP_BOOT_ROM */

#ifdef CONFIG_DPULP_BOOT_FLASH
#define ROM_BASE		FLASH_BASE
#define ROMEND			(ROM_BASE + FLASH_SIZE)
#endif /* CONFIG_DPULP_BOOT_FLASH */

#ifdef CONFIG_DPULP_LOAD_FSBL
#define ROM_BASE		BROM_BASE
#define ROMEND			(ROM_BASE + BROM_SIZE)
#endif /* CONFIG_DPULP_LOAD_FSBL */

#ifdef CONFIG_DPULP_TB
#define ROM_BASE		BOOTROM_BASE
#define ROMEND			(ROM_BASE + BOOTROM_SIZE)
#endif /* CONFIG_DPULP_TB */

#ifdef CONFIG_DPULP_FIRM_DDR
#define RAM_BASE		DDR_DATA_BASE
#define RAMEND			(DDR_DATA_BASE + DDR_DATA_SIZE)
#else /* CONFIG_DPULP_FIRM_DDR */
#define RAM_BASE		SRAM_BASE
#define RAMEND			(RAM_BASE + SRAM_SIZE)
#endif /* CONFIG_DPULP_FIRM_DDR */

#endif /* __REG_DPULP_H_INCLUDE__ */
