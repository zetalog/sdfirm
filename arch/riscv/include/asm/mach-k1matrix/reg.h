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

#define DIE0_BASE		ULL(0x0000000000)
#define DIE1_BASE		ULL(0x8000000000)
#define __DIE_BASE(die)		((uint64_t)(die) << 39)
#define DIE_BASE		k1matrix_die_base

#define __SYS_IO_GLOBAL_BASE	ULL(0x0000000000)
#define __D2D_BASE		ULL(0x000001C000)
#define __PCIE0_SLV_CFG_BASE	ULL(0x0080000000) /* PCIe0 MMIO CFG */
#define __PCIE1_SLV_CFG_BASE	ULL(0x0088000000) /* PCIe1 MMIO CFG */
#define __PCIE0_SLV_MEM_BASE	ULL(0x0100000000) /* PCIe0 MMIO BAR */
#define __PCIE1_SLV_MEM_BASE	ULL(0x0108000000) /* PCIe1 MMIO BAR */
#define __DDR_BASE		ULL(0x0200000000)
#define __UART0_BASE		ULL(0x0000008000)
#define __PIC_BASE		ULL(0x0010000000)
#define __CLINT_BASE		(__PIC_BASE + ULL(0x4000000))

/* SYS_IO global SUB */
#define SYS_IO_GLOBAL_BASE	(DIE_BASE + __SYS_IO_GLOBAL_BASE)
#define TIMER0_BASE		(DIE_BASE + ULL(0x0000000000))
#define TIMER1_BASE		(DIE_BASE + ULL(0x0000004000))
#define UART0_BASE		(DIE_BASE + __UART0_BASE)
#define UART1_BASE		(DIE_BASE + ULL(0x000000C000))
#define GPIO0_BASE		(DIE_BASE + ULL(0x0000010000))
#define GPIO1_BASE		(DIE_BASE + ULL(0x0000014000))
#define WDT_BASE		(DIE_BASE + ULL(0x0000018000))
#define D2D_BASE		(DIE_BASE + __D2D_BASE)
#define DMA_BASE		(DIE_BASE + ULL(0x0000100000))
#define DDR_PHY_BASE		(DIE_BASE + ULL(0x0001000000))
#define DDR_CTRL_BASE		(DIE_BASE + ULL(0x0001800000))
#define DDR_SYSREG_BASE		(DIE_BASE + ULL(0x0001810000))
#define CCIX_DBI_BASE		(DIE_BASE + ULL(0x0002000000))
#define PCIE1_DBI_BASE		(DIE_BASE + ULL(0x0002400000))
#define CCIX_APP_BASE		(DIE_BASE + ULL(0x0002800000))
#define PCIE1_APP_BASE		(DIE_BASE + ULL(0x0002900000))
#define PIC_BASE		(DIE_BASE + ULL(0x0010000000))
#define PLIC_REG_BASE		PIC_BASE
#define CLINT_BASE		(PIC_BASE + ULL(0x4000000))
/* PCIE SUB */
#define PCIE0_SLV_CFG_BASE	(DIE_BASE + __PCIE0_SLV_CFG_BASE)
#define PCIE1_SLV_CFG_BASE	(DIE_BASE + __PCIE1_SLV_CFG_BASE)
#define PCIE0_SLV_MEM_BASE	(DIE_BASE + __PCIE0_SLV_MEM_BASE)
#define PCIE1_SLV_MEM_BASE	(DIE_BASE + __PCIE1_SLV_MEM_BASE)
#define CCIX_SLV_CFG_BASE	PCIE0_SLV_CFG_BASE
#define CCIX_SLV_MEM_BASE	PCIE0_SLV_MEM_BASE
/* DDR SUB */
#define DDR_BASE		(DIE_BASE + __DDR_BASE)
/* SRAM */
#define SRAM0_BASE		ULL(0x0110000000)
#define SRAM1_BASE		ULL(0x0110080000)
/* SYS_IO lobal SUB */
#define SYS_IO_LOCAL_BASE	ULL(0x0F00000000)
/* GMAC SUB */
#define GMAC_BASE		ULL(0x0F00100000)
#define SYSCTL_BASE		ULL(0x0F00110000)
#define N100_CFG_BASE		ULL(0x0F10000000)

#define SRAM01_SIZE		ULL(0x0000080000)
#define __DRAM_SIZE		ULL(0x0020000000)

#ifdef CONFIG_K1MATRIX
#define SMMU_BASE(n)		ULL(0x0000000000)
#define BMU_BASE(n)		ULL(0x0000000000)
#define MBOX_BASE		ULL(0x0000000000)
#endif

#include <asm/mach/cprint.h>

#ifdef CONFIG_K1MATRIX_BOOT_LLP
#define BROM_BASE		SRAM0_BASE
#endif /* CONFIG_K1MATRIX_BOOT_LLP */
#ifdef CONFIG_K1MATRIX_BOOT_SYS
#define BROM_BASE		SRAM1_BASE
#endif /* CONFIG_K1MATRIX_BOOT_SYS */

#ifdef CONFIG_K1MATRIX_ROM
#define BROM_SIZE		SZ_64K
#else /* CONFIG_K1MATRIX_ROM */
#define BROM_SIZE		0
#endif /* CONFIG_K1MATRIX_ROM */

#define SRAM_BASE		(BROM_BASE + BROM_SIZE)
#define SRAM_SIZE		(SRAM01_SIZE - BROM_SIZE)

#ifdef CONFIG_K1MATRIX_DDR_SIZE_CUSTOM
#define DRAM_SIZE               CONFIG_K1MATRIX_MEM_SIZE
#else /* CONFIG_K1MATRIX_DDR_SIZE_CUSTOM */
#define DRAM_SIZE		__DRAM_SIZE
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
#define ROM_BASE		__DDR_BASE
#define ROM_SIZE		DRAM_SIZE
#define RAM_BASE		__DDR_BASE
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
#define ROM_BASE		DDR_BASE
#define ROM_SIZE		DRAM_SIZE
#define RAM_BASE		DDR_BASE
#define RAM_SIZE		DRAM_SIZE
#endif
#endif /* CONFIG_K1MATRIX_TB */

#define ROMEND			(ROM_BASE + ROM_SIZE)
#define RAMEND			(RAM_BASE + RAM_SIZE)

#endif /* __REG_K1MATRIX_H_INCLUDE__ */
