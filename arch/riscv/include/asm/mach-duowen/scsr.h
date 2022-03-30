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
 * @(#)scsr.h: DUOWEN SysCSR register definitions
 * $Id: scsr.h,v 1.1 2019-09-02 11:12:00 zhenglv Exp $
 */

#ifndef __SCSR_DUOWEN_H_INCLUDE__
#define __SCSR_DUOWEN_H_INCLUDE__

#define AXI_AXSIZE_BYTES		32

#include <target/amba.h>

/*===========================================================================
 * SCSR registers
 *===========================================================================*/
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define __DUOWEN_SCSR_BASE(soc)		(__SOC_BASE(soc) + __SCSR_BASE)
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define __DUOWEN_SCSR_BASE(soc)		SCSR_BASE
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#ifdef CONFIG_MMU
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define DUOWEN_SCSR_BASE(soc)		duowen_scsr_reg_base[soc]
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define DUOWEN_SCSR_BASE(soc)		duowen_scsr_reg_base
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#else
#define DUOWEN_SCSR_BASE(soc)		__DUOWEN_SCSR_BASE(soc)
#endif
#define __SCSR_REG(soc, offset)		(DUOWEN_SCSR_BASE(soc) + (offset))
#define SCSR_REG(offset)		\
	(DUOWEN_SCSR_BASE(imc_socket_id()) + (offset))
#define SCSR_HW_VERSION			SCSR_REG(0x00)
#define SCSR_HART_ID_LO			SCSR_REG(0x18)
#define SCSR_HART_ID_HI			SCSR_REG(0x1C)
#define SCSR_IMC_IPI			SCSR_REG(0x20)
#define SCSR_UART_STATUS		SCSR_REG(0x90)
#define SCSR_TIMER_PAUSE_EN0		SCSR_REG(0x94)
#define SCSR_TIMER_PAUSE_EN1		SCSR_REG(0x98)
#define SCSR_SD_STABLE			SCSR_REG(0x9C)
#define SCSR_SD_STATUS			SCSR_REG(0xA0)
#define SCSR_WDT_PAUSE_EN		SCSR_REG(0xA4)
#define SCSR_CLINT_CFG			SCSR_REG(0xC0)
#define SCSR_PLIC_CFG			SCSR_REG(0xC4)
#define SCSR_AMU_CFG			SCSR_REG(0xC8)
#define ____SCSR_SW_MSG(n)		(0xD0 + ((n) << 2))
#define __SCSR_SW_MSG(soc, n)		__SCSR_REG(soc, ____SCSR_SW_MSG(n))
#define SCSR_SW_MSG(n)			SCSR_REG(____SCSR_SW_MSG(n))
#define SCSR_PHASE_FLAG			SCSR_REG(0xFC)
#define SCSR_PMA_CFG_LO(n)		SCSR_REG(0x100 + ((n) << 3))
#define SCSR_PMA_CFG_HI(n)		SCSR_REG(0x104 + ((n) << 3))
#define SCSR_PMA_ADDR_LO(n)		SCSR_REG(0x140 + ((n) << 3))
#define SCSR_PMA_ADDR_HI(n)		SCSR_REG(0x144 + ((n) << 3))

/* SOC_HW_VERSION */
#define SCSR_MINOR_OFFSET		0
#define SCSR_MINOR_MASK			REG_8BIT_MASK
#define SCSR_MINOR(value)		_GET_FV(SCSR_MINOR, value)
#define SCSR_MAJOR_OFFSET		8
#define SCSR_MAJOR_MASK			REG_8BIT_MASK
#define SCSR_MAJOR(value)		_GET_FV(SCSR_MAJOR, value)
/* BOOT_MODE */
/* Boot modes:
 * SIM type is encoded in bit-3:
 *  SIM=0(asic): real chip
 *  SIM=1(sim): simulators
 * Boot CPU is encoded in bit-2:
 *  CPU=0(imc): boot from IMC boot addresses
 *  CPU=1(apc): boot from APC boot addresses
 * Boot rom (boot from) is encoded in bit-1, but in different SIM type, it
 * encodes different usages:
 *  SIM type is asic:
 *   MODE=0(ROM): internal ROM:
 *   MODE=1(SPI): external ROM, i.e., SPI flash
 *  SIM type is sim:
 *   MODE=0(RAM): internal SRAM
 *   MODE=1(DDR): external DDR
 * Boot storage (load from) is enocded in bit-0:
 *  FLASH=0(SD): SDHC is used for locating next loaders
 *  FLASH=1(SSI): SSI0 flash is used for locating next loaders
 */
/* SIM types */
#define IMC_BOOT_ASIC			0x00
#define IMC_BOOT_SIM			0x08
/* Boot CPUs */
#define IMC_BOOT_IMC			0x00
#define IMC_BOOT_APC			0x04
/* Boot roms (boot from) */
#define IMC_BOOT_ROM			0x00 /* IMC_BOOT_ASIC */
#define IMC_BOOT_SPI			0x02 /* IMC_BOOT_ASIC */
#define IMC_BOOT_RAM			0x00 /* IMC_BOOT_SIM */
#define IMC_BOOT_DDR			0x02 /* IMC_BOOT_SIM */
/* Boot storages (load from) */
#define IMC_BOOT_SD			0x00
#define IMC_BOOT_SSI			0x01

/* UART_STATUS */
#define IMC_UART3_LP_REQ_SCLK		7
#define IMC_UART3_LP_REQ_PCLK		6
#define IMC_UART2_LP_REQ_SCLK		5
#define IMC_UART2_LP_REQ_PCLK		4
#define IMC_UART1_LP_REQ_SCLK		3
#define IMC_UART1_LP_REQ_PCLK		2
#define IMC_UART0_LP_REQ_SCLK		1
#define IMC_UART0_LP_REQ_PCLK		0
#define IMC_MAX_APB_PERIPHS		8

/* SD_STABLE */
#define IMC_SD_HOST_REG_VOL_STABLE	_BV(0)

/* SD_STATUS */
#define IMC_SD_DATXFER_WIDTH_OFFSET	7
#define IMC_SD_DATXFER_WIDTH_MASK	REG_2BIT_MASK
#define IMC_SD_DATXFER_WIDTH(value)	_GET_FV(IMC_SD_DATXFER_WIDTH, value)
#define IMC_SD_VDD1_SEL_OFFSET		4
#define IMC_SD_VDD1_SEL_MASK		REG_3BIT_MASK
#define IMC_SD_VDD1_SEL(value)		_GET_FV(IMC_SD_VDD1_SEL, value)
#define IMC_SD_UHSI_DRV_STH_OFFSET	2
#define IMC_SD_UHSI_DRV_STH_MASK	REG_2BIT_MASK
#define IMC_SD_UHSI_DRV_STH(value)	_GET_FV(IMC_SD_UHSI_DRV_STH, value)
#define IMC_SD_VDD1_ON			_BV(1)
#define IMC_SD_UHSI_SWVOLT_EN		_BV(0)

/* CLINT_CFG */
#define IMC_CLINT_BYPASS		_BV(0) /* bypass address translation */

/* PLIC_CFG */
#define IMC_PLIC_DUAL			_BV(0)

/* AMU_CFG */
#define IMC_AMU_BYPASS			_BV(0)

#ifndef __ASSEMBLY__
#define imc_plic_enable_dual()		\
	__raw_setl(IMC_PLIC_DUAL, SCSR_PLIC_CFG)
#define imc_plic_disable_dual()		\
	__raw_clearl(IMC_PLIC_DUAL, SCSR_PLIC_CFG)
#define imc_pma_read_cfg(n)						\
	MAKELLONG(__raw_readl(SCSR_PMA_CFG_LO(n)),			\
		  __raw_readl(SCSR_PMA_CFG_HI(n)))
#define __imc_pma_write_cfg(n, v)					\
	do {								\
		__raw_writel(LODWORD(v), SCSR_PMA_CFG_LO(n));		\
		__raw_writel(HIDWORD(v), SCSR_PMA_CFG_HI(n));		\
	} while (0)
#define imc_pma_read_addr(n)						\
	MAKELLONG(__raw_readl(SCSR_PMA_ADDR_LO(n)),			\
		  __raw_readl(SCSR_PMA_ADDR_HI(n)))
#define __imc_pma_write_addr(n, a)					\
	do {								\
		__raw_writel(LODWORD(a), SCSR_PMA_ADDR_LO(n));		\
		__raw_writel(HIDWORD(a), SCSR_PMA_ADDR_HI(n));		\
	} while (0)
int imc_pma_set(int n, unsigned long attr,
		phys_addr_t addr, unsigned long log2len);
#endif /* __ASSEMBLY__ */

/*===========================================================================
 * LCSR registers
 *===========================================================================*/
#define __DUOWEN_LCSR_BASE		LCSR_BASE
#ifdef CONFIG_MMU
#define __LCSR_BASE			duowen_lcsr_reg_base
#else /* CONFIG_MMU */
#define __LCSR_BASE			__DUOWEN_LCSR_BASE
#endif /* CONFIG_MMU */
#define LCSR_REG(offset)		(__LCSR_BASE + (offset))

/* Boot code used definitions */
#define ____SCSR_BOOT_MODE		0x00
#define ____SCSR_SOCKET_ID		0x04

#define __SCSR_BOOT_MODE		(LCSR_BASE + ____SCSR_BOOT_MODE)
#define __SCSR_SOCKET_ID		(LCSR_BASE + ____SCSR_SOCKET_ID)

#define SCSR_BOOT_MODE			LCSR_REG(____SCSR_BOOT_MODE)
#define SCSR_SOCKET_ID			LCSR_REG(____SCSR_SOCKET_ID)
#define SCSR_IMC_BOOT_ADDR_LO		LCSR_REG(0x08)
#define SCSR_IMC_BOOT_ADDR_HI		LCSR_REG(0x0C)
#define SCSR_IMC_BOOT_ADDR_CFG_LO	SCSR_IMC_BOOT_ADDR_LO
#define SCSR_IMC_BOOT_ADDR_CFG_HI	SCSR_IMC_BOOT_ADDR_HI
#define SCSR_APC_JUMP_ADDR_LO(apc)	LCSR_REG(0x10 + ((apc) << 3))
#define SCSR_APC_JUMP_ADDR_HI(apc)	LCSR_REG(0x14 + ((apc) << 3))
#define SCSR_PARTIAL_GOOD		LCSR_REG(0x50)
#define SCSR_APC_BOOT_ADDR_LO		LCSR_REG(0x60)
#define SCSR_APC_BOOT_ADDR_HI		LCSR_REG(0x64)

/* SOCKET_ID */
#define IMC_SOCKET_ID			_BV(1)
#define IMC_CHIP_LINK			_BV(0)

/* PARTIAL_GOOD */
#define APC_CLUSTER0_CLAMP		_BV(0)
#define APC_CLUSTER0_APC0_CLAMP		_BV(1)
#define APC_CLUSTER0_APC0_CPU0_CLAMP	_BV(2)
#define APC_CLUSTER0_APC0_CPU1_CLAMP	_BV(3)
#define APC_CLUSTER0_APC1_CLAMP		_BV(4)
#define APC_CLUSTER0_APC1_CPU0_CLAMP	_BV(5)
#define APC_CLUSTER0_APC1_CPU1_CLAMP	_BV(6)
#define APC_CLUSTER0_CACHE_DISABLE	_BV(7)
#define APC_CLUSTER1_CLAMP		_BV(8)
#define APC_CLUSTER1_APC0_CLAMP		_BV(9)
#define APC_CLUSTER1_APC0_CPU0_CLAMP	_BV(10)
#define APC_CLUSTER1_APC0_CPU1_CLAMP	_BV(11)
#define APC_CLUSTER1_APC1_CLAMP		_BV(12)
#define APC_CLUSTER1_APC1_CPU0_CLAMP	_BV(13)
#define APC_CLUSTER1_APC1_CPU1_CLAMP	_BV(14)
#define APC_CLUSTER1_CACHE_DISABLE	_BV(15)
#define APC_CLUSTER2_CLAMP		_BV(16)
#define APC_CLUSTER2_APC0_CLAMP		_BV(17)
#define APC_CLUSTER2_APC0_CPU0_CLAMP	_BV(18)
#define APC_CLUSTER2_APC0_CPU1_CLAMP	_BV(19)
#define APC_CLUSTER2_APC1_CLAMP		_BV(20)
#define APC_CLUSTER2_APC1_CPU0_CLAMP	_BV(21)
#define APC_CLUSTER2_APC1_CPU1_CLAMP	_BV(22)
#define APC_CLUSTER2_CACHE_DISABLE	_BV(23)
#define APC_CLUSTER3_CLAMP		_BV(24)
#define APC_CLUSTER3_APC0_CLAMP		_BV(25)
#define APC_CLUSTER3_APC0_CPU0_CLAMP	_BV(26)
#define APC_CLUSTER3_APC0_CPU1_CLAMP	_BV(27)
#define APC_CLUSTER3_APC1_CLAMP		_BV(28)
#define APC_CLUSTER3_APC1_CPU0_CLAMP	_BV(29)
#define APC_CLUSTER3_APC1_CPU1_CLAMP	_BV(30)
#define APC_CLUSTER3_CACHE_DISABLE	_BV(31)

#define APC_CLUSTER_OFFSET		0
#define APC_CLUSTER_MASK						\
	(APC_CLUSTER0_CLAMP | APC_CLUSTER1_CLAMP |			\
	 APC_CLUSTER2_CLAMP | APC_CLUSTER3_CLAMP)
#define APC_GET_CLUSTER(value)		_GET_FV(APC_CLUSTER, value)
#define APC_SET_CLUSTER(value)		_SET_FV(APC_CLUSTER, value)
#define APC_CLUSTER_SCU_OFFSET		7
#define APC_CLUSTER_SCU_MASK						\
	((APC_CLUSTER0_CACHE_DISABLE | APC_CLUSTER1_CACHE_DISABLE |	\
	  APC_CLUSTER2_CACHE_DISABLE | APC_CLUSTER3_CACHE_DISABLE)	\
	 >> APC_CLUSTER_SCU_OFFSET)
#define APC_GET_CLUSTER_SCU(value)	_GET_FV(APC_CLUSTER_SCU, value)
#define APC_SET_CLUSTER_SCU(value)	_SET_FV(APC_CLUSTER_SCU, value)
#define APC_CLUSTER_APC_OFFSET		1
#define APC_CLUSTER_APC_MASK						\
	((APC_CLUSTER0_APC0_CLAMP | APC_CLUSTER0_APC1_CLAMP |		\
	  APC_CLUSTER1_APC0_CLAMP | APC_CLUSTER1_APC1_CLAMP |		\
	  APC_CLUSTER2_APC0_CLAMP | APC_CLUSTER2_APC1_CLAMP |		\
	  APC_CLUSTER3_APC0_CLAMP | APC_CLUSTER3_APC1_CLAMP)		\
	 >> APC_CLUSTER_APC_OFFSET)
#define APC_GET_CLUSTER_APC(value)	_GET_FV(APC_CLUSTER_APC, value)
#define APC_SET_CLUSTER_APC(value)	_SET_FV(APC_CLUSTER_APC, value)
#define APC_CLUSTER_CPU_OFFSET		2
#define APC_CLUSTER_CPU_MASK						\
	((APC_CLUSTER0_APC0_CPU0_CLAMP | APC_CLUSTER0_APC0_CPU1_CLAMP |	\
	  APC_CLUSTER0_APC1_CPU0_CLAMP | APC_CLUSTER0_APC1_CPU1_CLAMP |	\
	  APC_CLUSTER1_APC0_CPU0_CLAMP | APC_CLUSTER1_APC0_CPU1_CLAMP |	\
	  APC_CLUSTER1_APC1_CPU0_CLAMP | APC_CLUSTER1_APC1_CPU1_CLAMP |	\
	  APC_CLUSTER2_APC0_CPU0_CLAMP | APC_CLUSTER2_APC0_CPU1_CLAMP |	\
	  APC_CLUSTER2_APC1_CPU0_CLAMP | APC_CLUSTER2_APC1_CPU1_CLAMP |	\
	  APC_CLUSTER3_APC0_CPU0_CLAMP | APC_CLUSTER3_APC0_CPU1_CLAMP |	\
	  APC_CLUSTER3_APC1_CPU0_CLAMP | APC_CLUSTER3_APC1_CPU1_CLAMP)	\
	 >> APC_CLUSTER_CPU_OFFSET)
#define APC_GET_CLUSTER_CPU(value)	_GET_FV(APC_CLUSTER_CPU, value)
#define APC_SET_CLUSTER_CPU(value)	_SET_FV(APC_CLUSTER_CPU, value)

#ifndef __ASSEMBLY__
#define imc_get_boot_addr()				\
	MAKELLONG(__raw_readl(SCSR_IMC_BOOT_ADDR_LO),	\
		  __raw_readl(SCSR_IMC_BOOT_ADDR_HI))
#define imc_set_boot_addr(addr)				\
	do {						\
		__raw_writel(LODWORD(addr),		\
			     SCSR_IMC_BOOT_ADDR_CFG_LO);\
		__raw_writel(HIDWORD(addr),		\
			     SCSR_IMC_BOOT_ADDR_CFG_HI);\
	} while (0)
#define imc_get_hart_id()				\
	MAKELLONG(__raw_readl(SCSR_HART_ID_LO),		\
		  __raw_readl(SCSR_HART_ID_HI))
#define imc_set_hart_id(hart)				\
	do {						\
		__raw_writel(LODWORD(hart),		\
			     SCSR_HART_ID_LO);		\
		__raw_writel(HIDWORD(hart),		\
			     SCSR_HART_ID_HI);		\
	} while (0)
#define imc_sim_mode()		(__raw_readl(SCSR_BOOT_MODE) & IMC_BOOT_SIM)
#define imc_boot_cpu()		(__raw_readl(SCSR_BOOT_MODE) & IMC_BOOT_APC)
#define imc_boot_from()		(__raw_readl(SCSR_BOOT_MODE) & IMC_BOOT_SPI)
#define imc_sim_boot_from()	(__raw_readl(SCSR_BOOT_MODE) & IMC_BOOT_DDR)
#define imc_load_from()		(__raw_readl(SCSR_BOOT_MODE) & IMC_BOOT_SSI)
#define imc_socket_id()					\
	(__raw_readl(SCSR_SOCKET_ID) & IMC_SOCKET_ID ? 1 : 0)
#define imc_chip_link()					\
	(!!(__raw_readl(SCSR_SOCKET_ID) & IMC_CHIP_LINK))
#define apc_get_boot_addr()				\
	MAKELLONG(__raw_readl(SCSR_APC_BOOT_ADDR_LO),	\
		  __raw_readl(SCSR_APC_BOOT_ADDR_HI))
#define apc_set_boot_addr(addr)				\
	do {						\
		__raw_writel(LODWORD(addr),		\
			     SCSR_APC_BOOT_ADDR_CFG_LO);\
		__raw_writel(HIDWORD(addr),		\
			     SCSR_APC_BOOT_ADDR_CFG_HI);\
	} while (0)
#define apc_get_partial_good()	(~(__raw_readl(SCSR_PARTIAL_GOOD)))
#define apc_get_cluster_mask()	APC_GET_CLUSTER(apc_get_partial_good())
#define apc_get_scu_mask()	APC_GET_CLUSTER_SCU(apc_get_partial_good())
#define apc_get_apc_mask()	APC_GET_CLUSTER_APC(apc_get_partial_good())
#define apc_get_cpu_mask()	APC_GET_CLUSTER_CPU(apc_get_partial_good())
#define apc_set_cluster_mask(mask)					\
	__raw_writel_mask(APC_SET_CLUSTER(~(mask)),			\
			  APC_SET_CLUSTER(APC_CLUSTER_MASK),		\
			  SCSR_PARTIAL_GOOD)
#define apc_set_scu_mask(mask)						\
	__raw_writel_mask(APC_SET_CLUSTER_SCU(~(mask)),			\
			  APC_SET_CLUSTER_SCU(APC_CLUSTER_SCU_MASK),	\
			  SCSR_PARTIAL_GOOD)
#define apc_set_apc_mask(mask)						\
	__raw_writel_mask(APC_SET_CLUSTER_APC(~(mask)),			\
			  APC_SET_CLUSTER_APC(APC_CLUSTER_APC_MASK),	\
			  SCSR_PARTIAL_GOOD)
#define apc_set_cpu_mask(mask)						\
	__raw_writel_mask(APC_SET_CLUSTER_CPU(~(mask)),			\
			  APC_SET_CLUSTER_CPU(APC_CLUSTER_CPU_MASK),	\
			  SCSR_PARTIAL_GOOD)
uint16_t apc_get_cpu_map(void);
void apc_set_cpu_map(uint16_t map);
uint8_t apc_get_apc_map(void);
void apc_set_apc_map(uint8_t map);
uint8_t apc_get_cluster_map(void);
void apc_set_cluster_map(uint8_t map);
void apc_set_jump_addr(caddr_t addr);
#define __apc_set_jump_addr(apc, addr)				\
	do {							\
		__raw_writel(LODWORD(addr),			\
			     SCSR_APC_JUMP_ADDR_LO(apc));	\
		__raw_writel(HIDWORD(addr),			\
			     SCSR_APC_JUMP_ADDR_HI(apc));	\
	} while (0)
#define apc_get_jump_addr()					\
	MAKELLONG(__raw_readl(SCSR_APC_JUMP_ADDR_LO(0)),	\
		  __raw_readl(SCSR_APC_JUMP_ADDR_HI(0)))
#endif /* __ASSEMBLY__ */

#include <asm/mach/rom.h>

#endif /* __SCSR_DUOWEN_H_INCLUDE__ */
