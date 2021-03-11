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

#define __DUOWEN_SCSR_BASE		SCSR_BASE
#ifdef CONFIG_MMU
#define __SCSR_BASE			duowen_scsr_reg_base
#else
#define __SCSR_BASE			__DUOWEN_SCSR_BASE
#endif
#define SCSR_REG(offset)		(__SCSR_BASE + (offset))
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
#define SCSR_PHASE_FLAG			SCSR_REG(0xFC)
#define SCSR_PMA_CFG_LO(n)		SCSR_REG(0x100 + ((n) << 3))
#define SCSR_PMA_CFG_HI(n)		SCSR_REG(0x104 + ((n) << 3))
#define SCSR_PMA_ADDR_LO(n)		SCSR_REG(0x140 + ((n) << 3))
#define SCSR_PMA_ADDR_HI(n)		SCSR_REG(0x144 + ((n) << 3))

#ifdef CONFIG_DUOWEN_SOCv2
#define SCSR_BOOT_MODE			SCSR_REG(0x04)
#define SCSR_IMC_BOOT_ADDR_LO		SCSR_REG(0x08)
#define SCSR_IMC_BOOT_ADDR_HI		SCSR_REG(0x0C)
#define SCSR_IMC_BOOT_ADDR_CFG_LO	SCSR_REG(0x10)
#define SCSR_IMC_BOOT_ADDR_CFG_HI	SCSR_REG(0x14)
#define SCSR_CHIP_LINK_CFG		SCSR_REG(0xC4)
#define SCSR_CLAMP_CFG			SCSR_REG(0xD0)
#define SCSR_CHIP_CFG			SCSR_REG(0xF0)
#endif /* CONFIG_DUOWEN_SOCv2 */

#ifdef CONFIG_DUOWEN_SOCv3
#define LCSR_REG(offset)		(LCSR_BASE + (offset))

#define SCSR_BOOT_MODE			LCSR_REG(0x00)
#define SCSR_SOCKET_ID			LCSR_REG(0x04)
#define SCSR_IMC_BOOT_ADDR_LO		LCSR_REG(0x08)
#define SCSR_IMC_BOOT_ADDR_HI		LCSR_REG(0x0C)
#define SCSR_IMC_BOOT_ADDR_CFG_LO	SCSR_IMC_BOOT_ADDR_LO
#define SCSR_IMC_BOOT_ADDR_CFG_HI	SCSR_IMC_BOOT_ADDR_HI
#define SCSR_APC_JUMP_ADDR_LO(apc)	LCSR_REG(0x10 + ((apc) << 3))
#define SCSR_APC_JUMP_ADDR_HI(apc)	LCSR_REG(0x14 + ((apc) << 3))
#define SCSR_PARTIAL_GOOD		LCSR_REG(0x50)
#define SCSR_APC_BOOT_ADDR_LO		LCSR_REG(0x60)
#define SCSR_APC_BOOT_ADDR_HI		LCSR_REG(0x64)
#endif /* CONFIG_DUOWEN_SOCv3 */

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

/* SHUTDN_REQ/ACK */
#define IMC_DDR1_CTRL			15
#define IMC_DDR1			14 /* DDR AXI */
#define IMC_DDR0_CTRL			13
#define IMC_DDR0			12 /* DDR AXI */
#define IMC_PCIE_X4_1_DBI		11
#define IMC_PCIE_X4_0_DBI		10
#define IMC_PCIE_X8_DBI			9
#define IMC_PCIE_X16_DBI		8
#define IMC_PCIE_X4_1_SLV		7
#define IMC_PCIE_X4_1_MST		6
#define IMC_PCIE_X4_0_SLV		5
#define IMC_PCIE_X4_0_MST		4
#define IMC_PCIE_X8_SLV			3
#define IMC_PCIE_X8_MST			2
#define IMC_PCIE_X16_SLV		1
#define IMC_PCIE_X16_MST		0
#define IMC_MAX_AXI_PERIPHS		16

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

#define imc_pma_read_cfg(n)						\
	MAKELLONG(__raw_readl(SCSR_PMA_CFG_LO(n)),			\
		  __raw_readl(SCSR_PMA_CFG_HI(n)))
#define imc_pma_write_cfg(n, v)						\
	do {								\
		__raw_writel(LOWORD(v), SCSR_PMA_CFG_LO(n));		\
		__raw_writel(HIWORD(v), SCSR_PMA_CFG_HI(n));		\
	} while (0)
#define imc_pma_write_addr(n, a)					\
	do {								\
		__raw_writel(LOWORD(a), SCSR_PMA_ADDR_LO(n));		\
		__raw_writel(HIWORD(a), SCSR_PMA_ADDR_HI(n));		\
	} while (0)

#ifndef __ASSEMBLY__
#ifdef CONFIG_DUOWEN_SOCv3
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
#else
#define apc_set_boot_addr(addr)		imc_set_boot_addr(addr)
#define apc_get_boot_addr()		imc_get_boot_addr()
#define apc_set_jump_addr(addr)		imc_set_boot_addr(addr)
#define apc_get_jump_addr()		imc_get_boot_addr()
#endif
int imc_pma_set(int n, unsigned long attr,
		phys_addr_t addr, unsigned long log2len);
#endif

#endif /* __SCSR_DUOWEN_H_INCLUDE__ */
