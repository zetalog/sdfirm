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
 * @(#)pcc.h: RV32M1 (VEGA) PCC (peripheral clock control) driver
 * $Id: pcc.h,v 1.1 2019-08-19 13:49:00 zhenglv Exp $
 */

#ifndef __PCC_VEGA_H_INCLUDE__
#define __PCC_VEGA_H_INCLUDE__

#include <asm/io.h>

#define PCC0_BASE			UL(0x4002B000)
#define PCC1_BASE			UL(0x41027000)
#define PCC_REG(n, offset)		(PCC##n##_BASE + (offset))

/* PCC 0, controls CM4F/RI5CY domain */
#define PCC_MSCM			PCC_REG(0, 0x004)
/* #define PCC_SYSPM			PCC_REG(0, 0x00C) */
#define PCC_AXBS0			PCC_REG(0, 0x010)
#define PCC_DMA0			PCC_REG(0, 0x020)
#define PCC_FLEXBUS			PCC_REG(0, 0x030)
#define PCC_XRDC_MGR			PCC_REG(0, 0x050)
#define PCC_SEMA42_0			PCC_REG(0, 0x06C)
#define PCC_DMAMUX0			PCC_REG(0, 0x084)
#define PCC_EWM				PCC_REG(0, 0x088)
#define PCC_MUA				PCC_REG(0, 0x094)
#define PCC_CRC0			PCC_REG(0, 0x0BC)
#define PCC_LPIT0			PCC_REG(0, 0x0C0)
#define PCC_TPM0			PCC_REG(0, 0x0D4)
#define PCC_TPM1			PCC_REG(0, 0x0D8)
#define PCC_TPM2			PCC_REG(0, 0x0DC)
#define PCC_EMVSIM0			PCC_REG(0, 0x0E0)
#define PCC_FLEXIO0			PCC_REG(0, 0x0E4)
#define PCC_LPI2C0			PCC_REG(0, 0x0E8)
#define PCC_LPI2C1			PCC_REG(0, 0x0EC)
#define PCC_LPI2C2			PCC_REG(0, 0x0F0)
#define PCC_I2S0			PCC_REG(0, 0x0F4)
#define PCC_USDHC0			PCC_REG(0, 0x0F8)
#define PCC_LPSPI0			PCC_REG(0, 0x0FC)
#define PCC_LPSPI1			PCC_REG(0, 0x100)
#define PCC_LPSPI2			PCC_REG(0, 0x104)
#define PCC_LPUART0			PCC_REG(0, 0x108)
#define PCC_LPUART1			PCC_REG(0, 0x10C)
#define PCC_LPUART2			PCC_REG(0, 0x110)
#define PCC_USB0			PCC_REG(0, 0x114)
#define PCC_PORTA			PCC_REG(0, 0x118)
#define PCC_PORTB			PCC_REG(0, 0x11C)
#define PCC_PORTC			PCC_REG(0, 0x120)
#define PCC_PORTD			PCC_REG(0, 0x124)
#define PCC_LPADC0			PCC_REG(0, 0x128)
#define PCC_LPDAC0			PCC_REG(0, 0x130)
#define PCC_VREF			PCC_REG(0, 0x134)
/* #define PCC_ATX			PCC_REG(0, 0x138) */
/* #define PCC_INTMUX0			PCC_REG(0, 0x13C) */
#define PCC_TRACE			PCC_REG(0, 0x200)
/* PCC 1, controls CM0+/ZERO-RISCY domain */
#define PCC_DMA1			PCC_REG(1, 0x020)
#define PCC_GPIOE			PCC_REG(1, 0x03C)
#define PCC_SEMA42_1			PCC_REG(1, 0x06C)
#define PCC_DMAMUX1			PCC_REG(1, 0x084)
#define PCC_INTMUX1			PCC_REG(1, 0x088)
#define PCC_MUB				PCC_REG(1, 0x090)
#define PCC_CAU3			PCC_REG(1, 0x0A0)
#define PCC_TRNG			PCC_REG(1, 0x0A4)
#define PCC_LPIT1			PCC_REG(1, 0x0A8)
#define PCC_TPM3			PCC_REG(1, 0x0B4)
#define PCC_LPI2C3			PCC_REG(1, 0x0B8)
#define PCC_LPSPI3			PCC_REG(1, 0x0D4)
#define PCC_LPUART3			PCC_REG(1, 0x0D8)
#define PCC_PORTE			PCC_REG(1, 0x0DC)
#define PCC_MTB				PCC_REG(1, 0x200)
#define PCC_EXT_CLK			PCC_REG(1, 0x204)

#ifdef CONFIG_VEGA_RI5CY
#define PCC_XRDC_PAC			PCC_REG(0, 0x058)
#define PCC_XRDC_MRC			PCC_REG(0, 0x05C)
#endif
#ifdef CONFIG_VEGA_0RISCY
#define PCC_XRDC_PAC			PCC_REG(1, 0x058)
#define PCC_XRDC_MRC			PCC_REG(1, 0x05C)
#endif

/* The following peripherals support PCD/FRAC fields:
 * 30.5.1.36 PCC LPADC0 Register (PCC_LPADC0)
 * 30.5.1.39 PCC TRACE Register (PCC_TRACE)
 */
#define PCC_PCD_OFFSET			0
#define PCC_PCD_MASK			REG_3BIT_MASK
#define PCC_PCD(value)			_SET_FV(PCC_PCD, value)
#define PCC_PCD_DIV_MAX			8
#define PCC_FRAC			_BV(3)

/* PCS: Peripheral Clock Select
 * The following peripherals support PCS fields:
 * 30.5.1.14 PCC LPIT0 Register (PCC_LPIT0)
 * 30.5.1.15 PCC TPM0 Register (PCC_TPM0)
 * 30.5.1.16 PCC TPM1 Register (PCC_TPM1)
 * 30.5.1.17 PCC TPM2 Register (PCC_TPM2)
 * 30.5.1.18 PCC EMVSIM0 Register (PCC_EMVSIM0)
 * 30.5.1.19 PCC FLEXIO0 Register (PCC_FLEXIO0)
 * 30.5.1.20 PCC LPI2C0 Register (PCC_LPI2C0)
 * 30.5.1.21 PCC LPI2C1 Register (PCC_LPI2C1)
 * 30.5.1.22 PCC LPI2C2 Register (PCC_LPI2C2)
 * 30.5.1.23 PCC I2S0 Register (PCC_I2S0)
 * 30.5.1.24 PCC USDHC0 Register (PCC_USDHC0)
 * 30.5.1.25 PCC LPSPI0 Register (PCC_LPSPI0)
 * 30.5.1.26 PCC LPSPI1 Register (PCC_LPSPI1)
 * 30.5.1.27 PCC LPSPI2 Register (PCC_LPSPI2)
 * 30.5.1.28 PCC LPUART0 Register (PCC_LPUART0)
 * 30.5.1.29 PCC LPUART1 Register (PCC_LPUART1)
 * 30.5.1.30 PCC LPUART2 Register (PCC_LPUART2)
 * 30.5.1.31 PCC USB0 Register (PCC_USB0)
 * 30.5.1.36 PCC LPADC0 Register (PCC_LPADC0)
 * 30.5.1.39 PCC TRACE Register (PCC_TRACE)
 * 30.5.2.12 PCC LPIT1 Register (PCC_LPIT1)
 * 30.5.2.13 PCC TPM3 Register (PCC_TPM3)
 * 30.5.2.14 PCC LPI2C3 Register (PCC_LPI2C3)
 * 30.5.2.15 PCC LPSPI3 Register (PCC_LPSPI3)
 * 30.5.2.16 PCC LPUART3 Register (PCC_LPUART3)
 */
#define PCC_PCS_OFFSET			24
#define PCC_PCS_MASK			REG_3BIT_MASK
#define PCC_PCS(value)			_SET_FV(PCC_PCS, value)
/* 0b000:
 *  Clock is off. Or an external clock can be enabled for this peripheral.
 *  Only LPADC and I2S support external clock:
 *    The external clock to I2S is I2S0_MCLK from PTB5 or PTE19.
 *    The external clock to LPADC is the output of LPADCIRC.
 */
#define PCC_PCS_OFF			0
#define PCC_PCS_EXT			0
#define PCC_PCS_SYS_OSC			1
#define PCC_PCS_SIRC			2
#define PCC_PCS_FIRC			3
#define PCC_PCS_LPFLL			6

#define PCC_INUSE			_BV(29)
#define PCC_CGC				_BV(30)
#define PCC_PR				_BV(31)

/* The count of PCC clocks in one PCC block */
#define PCC_NR_CLKS			130
#define CLK_PCC0_BASE			0
#define CLK_PCC1_BASE			(CLK_PCC0_BASE + PCC_NR_CLKS)
#define CLK_PCC(reg)					\
	((PCC_##reg <= PCC1_BASE) ?			\
	 CLK_PCC0_BASE + ((PCC_##reg - PCC0_BASE) / 4) :\
	 CLK_PCC1_BASE + ((PCC_##reg - PCC1_BASE) / 4))

#define pcc_enable_clk(reg)		__raw_setl(PCC_CGC, reg)
#define pcc_disable_clk(reg)		__raw_clearl(PCC_CGC, reg)
#define pcc_clk_present(reg)		(!!(__raw_readl(reg) & PCC_PR))
#define pcc_clk_inuse(reg)		(!!(__raw_readl(reg) & PCC_INUSE))

void pcc_select_source(caddr_t reg, uint8_t src);
void pcc_config_divider(caddr_t reg,
			uint32_t input_hz, uint32_t output_hz);

#endif /* __PCC_VEGA_H_INCLUDE__ */
