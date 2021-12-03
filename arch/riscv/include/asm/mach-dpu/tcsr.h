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
 * @(#)tcsr.h: DPU top control/status register definitions
 * $Id: tcsr.h,v 1.1 2020-03-09 17:29:00 zhenglv Exp $
 */

#ifndef __TCSR_DPU_H_INCLUDE__
#define __TCSR_DPU_H_INCLUDE__

#define AXI_AXSIZE_BYTES		32

#include <target/amba.h>

#define __DPU_TCSR_BASE			TCSR_BASE
#ifdef CONFIG_MMU
#define DPU_TCSR_BASE			dpu_tcsr_reg_base
#ifndef __ASSEMBLY__
extern caddr_t dpu_tcsr_reg_base;
#endif
#else
#define DPU_TCSR_BASE			__DPU_TCSR_BASE
#endif
#define TCSR_REG(offset)		(DPU_TCSR_BASE + (offset))
#define TCSR_MSG_REG(offset)		TCSR_REG(0xF00 + (offset))
#define TCSR_SOC_HW_VERSION		TCSR_REG(0x00)
#define TCSR_BOOT_MODE			TCSR_REG(0x04)
#define TCSR_BOOT_ADDR_LO		TCSR_REG(0x10)
#define TCSR_BOOT_ADDR_HI		TCSR_REG(0x14)
#define TCSR_HART_ID_LO			TCSR_REG(0x18)
#define TCSR_HART_ID_HI			TCSR_REG(0x1C)
#define TCSR_DDR_INTERLEAVE		TCSR_REG(0x20)
#define TCSR_VPU_HIGH_ADDR		TCSR_REG(0x70)
#define TCSR_SHUTDN_REQ			TCSR_REG(0x80)
#define TCSR_BRINGUP_REQ		TCSR_REG(0x84)
#define TCSR_SHUTDN_ACK			TCSR_REG(0x88)
#define TCSR_BRINGUP_ACK		TCSR_REG(0x8C)
#define TCSR_LP_STATUS			TCSR_REG(0x90)
#define TCSR_SIM_FINISH			TCSR_MSG_REG(0xFC)

/* SOC_HW_VERSION */
#define IMC_MAJOR_OFFSET		8
#define IMC_MAJOR_MASK			REG_8BIT_MASK
#define IMC_MAJOR(value)		_GET_FV(IMC_MAJOR, value)
#define IMC_MINOR_OFFSET		0
#define IMC_MINOR_MASK			REG_8BIT_MASK
#define IMC_MINOR(value)		_GET_FV(IMC_MINOR, value)
/* BOOT_MODE */
#define IMC_BOOT_MODE_OFFSET		0
#define IMC_BOOT_MODE_MASK		REG_2BIT_MASK
#define IMC_BOOT_MODE(value)		_GET_FV(IMC_BOOT_MODE, value)
#define IMC_BOOT_ROM			0x00
#define IMC_BOOT_FLASH			0x01
#define IMC_BOOT_USE_BOOT_ADDR		0x02
/* FLASH_SEL */
#define IMC_FLASH_SEL_OFFSET		4
#define IMC_FLASH_SEL_MASK		REG_1BIT_MASK
#define IMC_FLASH_SEL(value)		_GET_FV(IMC_FLASH_SEL, value)
#define IMC_FLASH_SPI			0x00
#define IMC_FLASH_SSI			0x01
/* CPU_SEL */
#define IMC_CPU_SEL_OFFSET		5
#define IMC_CPU_SEL_MASK		REG_1BIT_MASK
#define IMC_CPU_SEL(value)		_GET_FV(IMC_CPU_SEL, value)
#define IMC_CPU_IMC			0x00
#define IMC_CPU_APC			0x01
/* SIM MSG SRC */
#ifdef CONFIG_DPU_TCSR_SIM_FINISH
#define IMC_SIM_PASS			_BV(31)
#define IMC_SIM_FAIL			_BV(30)
#endif

#define imc_soc_major()					\
	IMC_MAJOR(__raw_readl(TCSR_SOC_HW_VERSION))
#define imc_soc_minor()					\
	IMC_MINOR(__raw_readl(TCSR_SOC_HW_VERSION))
#define imc_hart_id()					\
	MAKELLONG(__raw_readl(TCSR_HART_ID_LO), __raw_readl(TCSR_HART_ID_HI))
#define imc_boot_mode()					\
	IMC_BOOT_MODE(__raw_readl(TCSR_BOOT_MODE))
#define imc_boot_flash()				\
	IMC_FLASH_SEL(__raw_readl(TCSR_BOOT_MODE))
#define imc_boot_cpu()					\
	IMC_CPU_SEL(__raw_readl(TCSR_BOOT_MODE))
#define imc_boot_addr()					\
	 MAKELLONG(__raw_readl(TCSR_BOOT_ADDR_LO),	\
		   __raw_readl(TCSR_BOOT_ADDR_HI))
#ifdef CONFIG_DPU_TCSR_SIM_FINISH
#define imc_sim_finish(pass)				\
	__raw_setl((pass) ? IMC_SIM_PASS : IMC_SIM_FAIL, TCSR_SIM_FINISH)
#else
#define imc_sim_finish(pass)		do { } while (0)
#endif

#ifdef CONFIG_DPU_TCSR_LOW_POWER
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
/* LP_STATUS */
#define IMC_I2C2_EN			6
#define IMC_I2C1_EN			5
#define IMC_I2C0_EN			4
#define IMC_UART_LP_REQ_SCLK		3
#define IMC_UART_LP_REQ_PCLK		2
#define IMC_SSI_SLEEP			1
#define IMC_SSI_BUSY			0
#define IMC_MAX_APB_PERIPHS		7

#define IMC_AXI_REQ(periph)		_BV(periph)
#define IMC_AXI_ACTIVE(periph)		_BV((periph) + 16)
#define IMC_AXI_ACK(periph)		_BV(periph)

#define imc_axi_enter_low_power(periph)				\
	do {							\
		__raw_clearl(IMC_AXI_REQ(periph),		\
			     TCSR_SHUTDN_REQ);			\
		while (__raw_readl(TCSR_SHUTDN_ACK) &		\
		       IMC_AXI_ACK(periph));			\
	} while (0)
#define imc_axi_exit_low_power(periph)				\
	do {							\
		__raw_setl(IMC_AXI_REQ(periph),			\
			   TCSR_BRINGUP_REQ);			\
		while (!(__raw_readl(TCSR_BRINGUP_ACK) &	\
		         IMC_AXI_ACK(periph)));			\
	} while (0)
#define imc_axi_is_low_power(periph)				\
	(!(__raw_readl(TCSR_SHUTDN_ACK) & IMC_AXI_ACTIVE(periph)))
#define imc_apb_is_low_power(periph)				\
	(__raw_readl(TCSR_LP_STATUS) & _BV(periph))
#ifndef __ASSEMBLY__
void imc_axi_register_periphs(uint16_t periphs);
void imc_axi_unregister_periphs(uint16_t periphs);
#endif
#else
#define imc_axi_enter_low_power(periph)		do { } while (0)
#define imc_axi_exit_low_power(periph)		do { } while (0)
#define imc_axi_is_low_power(periph)		false
#define imc_axi_register_periphs(periphs)	do { } while (0)
#define imc_axi_unregister_periphs(periphs)	do { } while (0)
#define imc_apb_is_low_power(periph)		false
#endif

#ifdef CONFIG_DPU_DDR_INTLV
#define IMC_DDR_INTLV				1
#else /* CONFIG_DPU_DDR_INTLV */
#define IMC_DDR_INTLV				0
#endif /* CONFIG_DPU_DDR_INTLV */

#define imc_config_ddr_intlv()			\
	__raw_writel(IMC_DDR_INTLV, TCSR_DDR_INTERLEAVE)

#endif /* __TCSR_DPU_H_INCLUDE__ */
