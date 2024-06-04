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
 * @(#)sysreg.h: K1Matrix system registers definitions
 * $Id: sysreg.h,v 1.1 2023-09-06 18:48:00 zhenglv Exp $
 */
#ifndef __SYSREG_K1MATRIX_H_INCLUDE__
#define __SYSREG_K1MATRIX_H_INCLUDE__
#include <asm/mach/reg.h>
#define SYSREG_REG(offset)	(SYS_REG_BASE + (offset))
#define CPU_SUB_SW_RESET	SYSREG_REG(0x2100)
#define PCIE_TOP_SW_RESET	SYSREG_REG(0x2104)
#define PCIE_BOT_SW_RESET	SYSREG_REG(0x2108)
#define PERI_SUB_SW_RESET	SYSREG_REG(0x210C)
#define MESH_SUB_SW_RESET	SYSREG_REG(0x2110)
#define DDR_SUB_SW_RESET	SYSREG_REG(0x2114)
#define CLUSTER0_SW_RESET	SYSREG_REG(0x2200)
#define CLUSTER1_SW_RESET	SYSREG_REG(0x2204)
#define CLUSTER2_SW_RESET	SYSREG_REG(0x2208)
#define CLUSTER3_SW_RESET	SYSREG_REG(0x220C)
#define CLUSTER4_SW_RESET	SYSREG_REG(0x2210)
#define CLUSTER5_SW_RESET	SYSREG_REG(0x2214)
#define CLUSTER6_SW_RESET	SYSREG_REG(0x2218)
#define CLUSTER7_SW_RESET	SYSREG_REG(0x221C)
#define CPU_SW_RESET		SYSREG_REG(0x2220)
#define RMU_SRAM_SW_RESET	SYSREG_REG(0x2320)
#define RMU_QSPI_SW_RESET	SYSREG_REG(0x2324)
#define RMU_EFUSE_NS_SW_RESET	SYSREG_REG(0x2328)
#define RMU_STM_SW_RESET	SYSREG_REG(0x232C)
#define RMU_SYSREG_SW_RESET	SYSREG_REG(0x2330)
#define RMU_LPC_SW_RESET	SYSREG_REG(0x2334)
#define RMU_ESPI_SW_RESET	SYSREG_REG(0x2338)
#define RMU_PVTC_SW_RESET	SYSREG_REG(0x233C)
#define RMU_UART0_SW_RESET	SYSREG_REG(0x2340)
#define RMU_UART1_SW_RESET	SYSREG_REG(0x2344)
#define RMU_TIMER_S_SW_RESET	SYSREG_REG(0x2348)
#define RMU_SPINLOCK_SW_RESET	SYSREG_REG(0x234C)
#define RMU_MAILBOX_S_SW_RESET	SYSREG_REG(0x2350)
#define RMU_MAILBOX_NS_SW_RESET	SYSREG_REG(0x2354)
#define RMU_PIC_SW_RESET	SYSREG_REG(0x2358)
#define PCIE0_SW_RESET		SYSREG_REG(0x2400)
#define PCIE1_SW_RESET		SYSREG_REG(0x2404)
#define PCIE2_SW_RESET		SYSREG_REG(0x2408)
#define PCIE3_SW_RESET		SYSREG_REG(0x240C)
#define MPHY0_SW_RESET		SYSREG_REG(0x2420)
#define MPHY1_SW_RESET		SYSREG_REG(0x2424)
#define MPHY2_SW_RESET		SYSREG_REG(0x2428)
#define PCIE4_SW_RESET		SYSREG_REG(0x2600)
#define PCIE5_SW_RESET		SYSREG_REG(0x2604)
#define PCIE6_SW_RESET		SYSREG_REG(0x2608)
#define PCIE7_SW_RESET		SYSREG_REG(0x260C)
#define PCIE8_SW_RESET		SYSREG_REG(0x2610)
#define PCIE9_SW_RESET		SYSREG_REG(0x2614)
#define PCIE_TCU2_SW_RESET	SYSREG_REG(0x2634)
#define PCIE_SUBLINK_SW_RESET	SYSREG_REG(0x2638)
#define PCIE_SYS_SW_RESET	SYSREG_REG(0x263C)
#define COMBPHY3_APB_SW_RESET	SYSREG_REG(0x2640)
#define COMBPHY4_APB_SW_RESET	SYSREG_REG(0x2644)
#define COMBPHY5_APB_SW_RESET	SYSREG_REG(0x2648)
#define COMBPHY3_PHYRESET_SW_RESET	SYSREG_REG(0x264C)
#define COMBPHY4_PHYRESET_SW_RESET	SYSREG_REG(0x2650)
#define COMBPHY5_PHYRESET_SW_RESET	SYSREG_REG(0x2654)
#define PERI_SPI0_SW_RESET	SYSREG_REG(0x2820)
#define PERI_SPI1_SW_RESET	SYSREG_REG(0x2824)
#define PERI_SPI2_SW_RESET	SYSREG_REG(0x2828)
#define PERI_SPI3_SW_RESET	SYSREG_REG(0x282C)
#define PERI_SMBUS0_SW_RESET	SYSREG_REG(0x2840)
#define PERI_SMBUS1_SW_RESET	SYSREG_REG(0x2844)
#define PERI_I3C0_SW_RESET	SYSREG_REG(0x2860)
#define PERI_I3C1_SW_RESET	SYSREG_REG(0x2864)
#define PERI_I2C0_SW_RESET	SYSREG_REG(0x2880)
#define PERI_I2C1_SW_RESET	SYSREG_REG(0x2884)
#define PERI_I2C2_SW_RESET	SYSREG_REG(0x2888)
#define PERI_I2C3_SW_RESET	SYSREG_REG(0x288C)
#define PERI_I2C4_SW_RESET	SYSREG_REG(0x2890)
#define PERI_I2C5_SW_RESET	SYSREG_REG(0x2894)
#define PERI_UART0_SW_RESET	SYSREG_REG(0x28A0)
#define PERI_UART1_SW_RESET	SYSREG_REG(0x28A4)
#define PERI_UART2_SW_RESET	SYSREG_REG(0x28A8)
#define PERI_UART3_SW_RESET	SYSREG_REG(0x28AC)
#define PERI_GPIO0_SW_RESET	SYSREG_REG(0x28C0)
#define PERI_GPIO1_SW_RESET	SYSREG_REG(0x28C4)
#define PERI_GPIO2_SW_RESET	SYSREG_REG(0x28C8)
#define PERI_GPIO3_SW_RESET	SYSREG_REG(0x28CC)
#define PERI_GPIO4_SW_RESET	SYSREG_REG(0x28D0)
#define PERI_WDT0_SW_RESET	SYSREG_REG(0x28E0)
#define PERI_WDT1_SW_RESET	SYSREG_REG(0x28E4)
#define PERI_WDT2_SW_RESET	SYSREG_REG(0x28E8)
#define PERI_WDT3_SW_RESET	SYSREG_REG(0x28EC)
#define PERI_WDT4_SW_RESET	SYSREG_REG(0x28F0)
#define PERI_WDT5_SW_RESET	SYSREG_REG(0x28F4)
#define PERI_WDT6_SW_RESET	SYSREG_REG(0x28F8)
#define PERI_WDT7_SW_RESET	SYSREG_REG(0x28FC)
#define PERI_GTIMER0_SW_RESET	SYSREG_REG(0x2900)
#define PERI_GTIMER1_SW_RESET	SYSREG_REG(0x2920)
#define PERI_GTIMER2_SW_RESET	SYSREG_REG(0x2940)
#define PERI_GTIMER3_SW_RESET	SYSREG_REG(0x2960)
#define PERI_DMAC_SW_RESET	SYSREG_REG(0x29C0)
#define PERI_GMAC_SW_RESET	SYSREG_REG(0x29E0)
/* CPU_SUB_SW_RESET */
#define CPU_SUB_RESET_BIT	_BV(0)
/* PCIE_BOT/TOP_SW_RESET */
#define PCIE_SUB_RESET_BIT	_BV(0)
/* PERI_SUB_SW_RESET */
#define PERI_SUB_RESET_BIT	_BV(0)
/* MESH_SUB_SW_RESET */
#define MESH_SUB_RESET_BIT	_BV(0)
/* DDR_SUB_SW_RESET */
#define DDR_SUB_RESET_BIT	_BV(0)
/* CLUSTER0/1/2/3/4/5/6/7_SW_RESET */
#define CPU_RESET_BIT(cpu)	_BV(cpu)
/* CPU_SW_RESET */
#define CPU_SUB_SRESET_BIT	_BV(0)
/* RMU_LPC_SW_RESET */
#define RMU_LPC_RESET_BIT	_BV(0)
#define RMU_LPC_LRESET_BIT	_BV(1)
#define RMU_LPC_IO_RESET_BIT	_BV(2)
/* RMU_ESPI_SW_RESET */
#define RMU_ESPI_RESET_BIT	_BV(0)
/* RMU_PVTC_SW_RESET */
#define RMU_PVTC_RESET_BIT	_BV(0)
/* RMU_UART0/1_SW_RESET */
#define RMU_UART_RESET_BIT	_BV(0)
/* RMU_TIMER_S_SW_RESET */
#define RMU_TIMER_RESET_BIT	_BV(0)
/* RMU_MAILBOX_S/NS_SW_RESET */
#define RMU_MAILBOX_RESET_BIT	_BV(0)
/* RMU_PIC_SW_RESET */
#define RMU_PIC_RESET_BIT	_BV(0)
/* PCIE0~9_SW_RESET */
#define PCIE_RESET_BIT		_BV(0)
#define PCIE_RC_PRESET_BIT	_BV(1)
#define PCIE_EP_PRESET_BIT	_BV(2) // only for PCIE2/3/4
/* MPHY0/1/2_SW+RESET */
#define MPHY_RESET_BIT		_BV(0)
/* PCIE_TCU2_SW_RESET */
#define PCIE_TCU2_RESET_BIT	_BV(0)
/* PCIE_SUBLINK_SW_RESET */
#define PCIE_TCU2_RESET_BIT	_BV(0)
/* PCIE_SUBLINK_SW_RESET */
#define PCIE_NIC400LINK_RESET_BIT	_BV(0)
/* PCIE_SYS_SW_RESET */
#define PCIE_NIC400CFG_RESET_BIT	_BV(0)
/* COMPHY3/4/5_APB_SW_RESET */
#define PCIE_COMBPHY_PRESET_BIT	_BV(0)
/* PERI_SPI0/1/2/3_SW_RESET */
#define PERI_SPI_RESET_BIT	_BV(0)
/* PERI_SMBUS0/1_SW_RESET */
#define PERI_SMBUS_RESET_BIT	_BV(0)
/* PERI_I3C0/1_SW_RESET */
#define PERI_I3C_RESET_BIT	_BV(0)
/* PERI_I2C0/1/2/3/4/5_SW_RESET */
#define PERI_I2C_RESET_BIT	_BV(0)
/* PERI_UART0/1/2/3_SW_RESET */
#define PERI_UART_RESET_BIT	_BV(0)
/* PERI_GPIO0/1/2/3_SW_RESET */
#define PERI_GPIO_RESET_BIT	_BV(0)
/* PERI_WDT0/1/2/3/4/5/6/7/8_SW_RESET */
#define PERI_WDT_RESET_BIT	_BV(0)
/* PERI_GTIMER0/1/2/3_SW_RESET */
#define PERI_GTIMER_RESET_BIT	_BV(31)
#define PERI_TIMER_RESET_BIT(timer)	_BV(timer)
/* PERI_DMAC_SW_RESET */
#define PERI_DMAC_RESET_BIT	_BV(0)
/* PERI_GMAC_SW_RESET */
#define PERI_GMAC_RESET_BIT	_BV(0)
/* EFUSE_INFO_REG0 */
#define CPU_CORE_VLD(n)		_BV(n)
/* EFUSE_INFO_REG1 */
#define DIE_ID			_BV(5)
#define BOOTHART_OFFSET		0
#define BOOTHART_MASK		REG_5BIT_MASK
#define BOOTHART(value)		_GET_FV(BOOTHART, value)
/* PAD_IN_STATUS */
#define PAD_BOOT_SEL_OFFSET	1
#define PAD_BOOT_SEL_MASK	REG_1BIT_MASK
#define PAD_BOOT_SEL(value)	_GET_FV(PAD_BOOT_SEL, value)
#define __PAD_BOOT_SEL		_BV(PAD_BOOT_SEL_OFFSET)
#define PAD_DIE_ID_OFFSET	0
#define PAD_DIE_ID_MASK		REG_1BIT_MASK
#define PAD_DIE_ID(value)	_GET_FV(PAD_DIE_ID, value)
#define __PAD_DIE_ID		_BV(PAD_DIE_ID_OFFSET)
#define sysreg_cpu_sub_reset() \
	__raw_setl(CPU_SUB_RESET_BIT, CPU_SUB_SW_RESET)
#define sysreg_pcie_top_sub_reset() \
	__raw_setl(PCIE_SUB_RESET_BIT, PCIE_TOP_SW_RESET)
#define sysreg_pcie_bot_sub_reset() \
	__raw_setl(PCIE_SUB_RESET_BIT, PCIE_BOT_SW_RESET)
#define sysreg_peri_sub_reset() \
	__raw_setl(PERI_SUB_RESET_BIT, PERI_SUB_SW_RESET)
#define sysreg_mesh_sub_reset() \
	__raw_setl(MESH_SUB_RESET_BIT, MESH_SUB_SW_RESET)
#define sysreg_ddr_sub_reset() \
	__raw_setl(DDR_SUB_RESET_BIT, DDR_SUB_SW_RESET)
#define sysreg_cluster0_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER0_SW_RESET)
#define sysreg_cluster1_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER1_SW_RESET)
#define sysreg_cluster2_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER2_SW_RESET)
#define sysreg_cluster3_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER3_SW_RESET)
#define sysreg_cluster4_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER4_SW_RESET)
#define sysreg_cluster5_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER5_SW_RESET)
#define sysreg_cluster6_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER6_SW_RESET)
#define sysreg_cluster7_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET_BIT(cpu), CLUSTER7_SW_RESET)
#define sysreg_cpu_sub_soft_reset()		\
	__raw_setl(CPU_SUB_SRESET_BIT, CPU_SW_RESET)
#define sysreg_rmu_uart0_reset() \
	__raw_setl(RMU_UART_RESET_BIT, RMU_UART0_SW_RESET)
#define sysreg_peri_spi0_reset() \
	__raw_setl(PERI_SPI_RESET_BIT, PERI_SPI0_SW_RESET)
#define sysreg_peri_spi1_reset() \
	__raw_setl(PERI_SPI_RESET_BIT, PERI_SPI1_SW_RESET)
#define sysreg_peri_spi2_reset() \
	__raw_setl(PERI_SPI_RESET_BIT, PERI_SPI2_SW_RESET)
#define sysreg_peri_spi3_reset() \
	__raw_setl(PERI_SPI_RESET_BIT, PERI_SPI3_SW_RESET)
#define sysreg_peri_smbus0_reset() \
	__raw_setl(PERI_SMBUS_RESET_BIT, PERI_SMBUS0_SW_RESET)
#define sysreg_peri_smbus1_reset() \
	__raw_setl(PERI_SMBUS_RESET_BIT, PERI_SMBUS1_SW_RESET)
#define sysreg_peri_i3c0_reset() \
	__raw_setl(PERI_I3C_RESET_BIT, PERI_I3C0_SW_RESET)
#define sysreg_peri_i3c1_reset() \
	__raw_setl(PERI_I3C_RESET_BIT, PERI_I3C1_SW_RESET)
#define sysreg_peri_i2c0_reset() \
	__raw_setl(PERI_I2C_RESET_BIT, PERI_I2C0_SW_RESET)
#define sysreg_peri_i2c1_reset() \
	__raw_setl(PERI_I2C_RESET_BIT, PERI_I2C1_SW_RESET)
#define sysreg_peri_i2c2_reset() \
	__raw_setl(PERI_I2C_RESET_BIT, PERI_I2C2_SW_RESET)
#define sysreg_peri_i2c3_reset() \
	__raw_setl(PERI_I2C_RESET_BIT, PERI_I2C3_SW_RESET)
#define sysreg_peri_i2c4_reset() \
	__raw_setl(PERI_I2C_RESET_BIT, PERI_I2C4_SW_RESET)
#define sysreg_peri_i2c5_reset() \
	__raw_setl(PERI_I2C_RESET_BIT, PERI_I2C5_SW_RESET)
#define sysreg_peri_uart0_reset() \
	__raw_setl(PERI_UART_RESET_BIT, PERI_UART0_SW_RESET)
#define sysreg_peri_uart1_reset() \
	__raw_setl(PERI_UART_RESET_BIT, PERI_UART1_SW_RESET)
#define sysreg_peri_uart2_reset() \
	__raw_setl(PERI_UART_RESET_BIT, PERI_UART2_SW_RESET)
#define sysreg_peri_uart3_reset() \
	__raw_setl(PERI_UART_RESET_BIT, PERI_UART3_SW_RESET)
#define sysreg_peri_gpio0_reset() \
	__raw_setl(PERI_GPIO_RESET_BIT, PERI_GPIO0_SW_RESET)
#define sysreg_peri_gpio1_reset() \
	__raw_setl(PERI_GPIO_RESET_BIT, PERI_GPIO1_SW_RESET)
#define sysreg_peri_gpio2_reset() \
	__raw_setl(PERI_GPIO_RESET_BIT, PERI_GPIO2_SW_RESET)
#define sysreg_peri_gpio3_reset() \
	__raw_setl(PERI_GPIO_RESET_BIT, PERI_GPIO3_SW_RESET)
#define sysreg_peri_gpio4_reset() \
	__raw_setl(PERI_GPIO_RESET_BIT, PERI_GPIO4_SW_RESET)
#define sysreg_peri_wdt0_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT0_SW_RESET)
#define sysreg_peri_wdt1_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT1_SW_RESET)
#define sysreg_peri_wdt2_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT2_SW_RESET)
#define sysreg_peri_wdt3_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT3_SW_RESET)
#define sysreg_peri_wdt4_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT4_SW_RESET)
#define sysreg_peri_wdt5_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT5_SW_RESET)
#define sysreg_peri_wdt6_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT6_SW_RESET)
#define sysreg_peri_wdt7_reset() \
	__raw_setl(PERI_WDT_RESET_BIT, PERI_WDT7_SW_RESET)
#define sysreg_peri_gtimer0_reset() \
	__raw_setl(PERI_GTIMER_RESET_BIT, PERI_GTIMER0_SW_RESET)
#define sysreg_peri_gtimer0_reset_timer(timer) \
	__raw_setl(PERI_TIMER_RESET_BIT(timer), PERI_GTIMER0_SW_RESET)
#define sysreg_peri_gtimer1_reset() \
	__raw_setl(PERI_GTIMER_RESET_BIT, PERI_GTIMER1_SW_RESET)
#define sysreg_peri_gtimer1_reset_timer(timer) \
	__raw_setl(PERI_TIMER_RESET_BIT(timer), PERI_GTIMER1_SW_RESET)
#define sysreg_peri_gtimer2_reset() \
	__raw_setl(PERI_GTIMER_RESET_BIT, PERI_GTIMER2_SW_RESET)
#define sysreg_peri_gtimer2_reset_timer(timer) \
	__raw_setl(PERI_TIMER_RESET_BIT(timer), PERI_GTIMER2_SW_RESET)
#define sysreg_peri_gtimer3_reset() \
	__raw_setl(PERI_GTIMER_RESET_BIT, PERI_GTIMER3_SW_RESET)
#define sysreg_peri_gtimer3_reset_timer(timer) \
	__raw_setl(PERI_TIMER_RESET_BIT(timer), PERI_GTIMER3_SW_RESET)
#define sysreg_peri_dmac_reset() \
	__raw_setl(PERI_DMAC_RESET_BIT, PERI_DMAC_SW_RESET)
#define sysreg_peri_gmac_reset() \
	__raw_setl(PERI_GMAC_RESET_BIT, PERI_GMAC_SW_RESET)
#define EFUSE_INFO_REG0		SYSREG_REG(0x1000) /* CPU mask */
#define EFUSE_INFO_REG1		SYSREG_REG(0x1004) /* boot CPU */
#define PAD_IN_STATUS		SYSREG_REG(0x2010)
#define sysreg_boot_sel()		\
	PAD_BOOT_SEL(__raw_readl(PAD_IN_STATUS))
#define sysreg_die_id()			\
	PAD_DIE_ID(__raw_readl(PAD_IN_STATUS))
#define sysreg_boot_cpu()		\
	BOOTHART(__raw_readl(EFUSE_INFO_REG1))
#define sysreg_cpu_mask()		\
	__raw_readl(EFUSE_INFO_REG0)
#ifndef __ASSEMBLY__
void k1matrix_cpu_reset(void);
uint32_t acpu_get_cpu_map(void);
uint8_t acpu_get_cluster_map(void);
#endif
#endif /* __SYSREG_K1MATRIX_H_INCLUDE__ */
