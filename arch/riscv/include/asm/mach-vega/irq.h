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
 * @(#)irq.h: RV32M1 (VEGA) IRQ definitions
 * $Id: irq.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __IRQ_VEGA_H_INCLUDE__
#define __IRQ_VEGA_H_INCLUDE__

/* MCU IRQs */
#define IRQ_DMA0_0	0
#define IRQ_DMA0_1	1
#define IRQ_DMA0_2	2
#define IRQ_DMA0_3	3
#define IRQ_DMA0_E	4
#define IRQ_CMC0	5
#define IRQ_MUA		6
#define IRQ_USB		7
#define IRQ_SDHC	8
#define IRQ_I2S		9
#define IRQ_FLEXIO	10
#define IRQ_EMVSIM	11
#define IRQ_LPIT0	12
#define IRQ_LPSPI0	13
#define IRQ_LPSPI1	14
#define IRQ_LPI2C0	15
#define IRQ_LPI2C1	16
#define IRQ_LPUART0	17
#define IRQ_PTA		18
#define IRQ_TPM0	19
#define IRQ_LPDAC	20
#define IRQ_LPADC	21
#define IRQ_LPCMP0	22
#define IRQ_RTC		23
#define IRQ_INTMUX0_0	24
#define IRQ_INTMUX0_1	25
#define IRQ_INTMUX0_2	26
#define IRQ_INTMUX0_3	27
#define IRQ_INTMUX0_4	28
#define IRQ_INTMUX0_5	29
#define IRQ_INTMUX0_6	30
#define IRQ_INTMUX0_7	31
#define IRQ_EWM		32
#define IRQ_FTFE_CC	33 /* Command complete */
#define IRQ_FTFE_RC	34 /* Read collision */
#define IRQ_LLWU	35
#define IRQ_SPM		36
#define IRQ_WDOG0	37
#define IRQ_SCG		38
#define IRQ_LPTMR0	39
#define IRQ_LPTMR1	40
#define IRQ_TPM1	41
#define IRQ_TPM2	42
#define IRQ_LPI2C2	43
#define IRQ_LPSPI2	44
#define IRQ_LPUART1	45
#define IRQ_LPUART2	46
#define IRQ_PTB		47
#define IRQ_PTC		48
#define IRQ_PTD		49
#define IRQ_CAU3_TC	50 /* Task complete */
#define IRQ_CAU3_SV	51 /* Security violation */
#define IRQ_TRNG	52
#define IRQ_LPIT1	53
#define IRQ_LPTMR2	54
#define IRQ_TPM3	55
#define IRQ_LPI2C3	56
#define IRQ_LPSPI3	57
#define IRQ_LPUART3	58
#define IRQ_PTE		59
#define IRQ_LPCMP1	60
#define IRQ_RF_0	61
#define IRQ_RF_1	62

#define NR_IRQS		64

#endif  /* __IRQ_VEGA_H_INCLUDE__ */
