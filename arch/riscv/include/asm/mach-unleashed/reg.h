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
 * @(#)reg.h: FU540 (unleashed) space and register definitions
 * $Id: reg.h,v 1.1 2019-10-16 10:23:00 zhenglv Exp $
 */

#ifndef __REG_UNLEASHED_H_INCLUDE__
#define __REG_UNLEASHED_H_INCLUDE__

#define MSEL_BASE		UL(0x00001000)
#define MSEL_SIZE		UL(0x00001000)
#define MASK_ROM_BASE		UL(0x00010000)
#define MASK_ROM_SIZE		0x8000
#define E51_DTIM_BASE		UL(0x01000000)
#define E51_DTIM_SIZE		0x2000
#define E51_ITIM_BASE		UL(0x01800000)
#define E51_ITIM_SIZE		0x2000
#define U54_ITIM_BASE(n)	UL(0x01808000 + (n) * 0x8000)
#define U54_ITIM_SIZE		0x7000
#define CLINT_BASE		UL(0x02000000)
#define L2_CC_BASE		UL(0x02010000) /* L2 Cache Controller */
#define MSI_BASE		UL(0x02020000)
#define DMAC_BASE		UL(0x03000000) /* DMA Controller */
#define L2_LIM_BASE		UL(0x08000000)
#define L2_LIM_SIZE		UL(0x02000000)
#define L2_0DEV_BASE		UL(0x0A000000)
#define PLIC_REG_BASE		UL(0x0C000000)
#define PRCI_BASE		UL(0x10000000)
#define UART0_BASE		UL(0x10010000)
#define UART1_BASE		UL(0x10011000)
#define PWM0_BASE		UL(0x10020000)
#define PWM1_BASE		UL(0x10021000)
#define I2C_BASE		UL(0x10030000)
#define QSPI0_BASE		UL(0x10040000)
#define QSPI1_BASE		UL(0x10041000)
#define QSPI2_BASE		UL(0x10050000)
#define GPIO_BASE		UL(0x10060000)
#define OTP_BASE		UL(0x10070000)
#define PINC_BASE		UL(0x10080000) /* Pin Controller */
#define EMAC_BASE		UL(0x10090000) /* Ethernet MAC */
#define ETHM_BASE		UL(0x100A0000) /* ETHernet Management */
#define DDRC_BASE		UL(0x100B0000) /* DDR Controller */
#define DDRM_BASE		UL(0x100C0000) /* DDR Management */
#define ERRD_BASE		UL(0x18000000) /* ERRor Device */
#define QSPI0_FLASH_BASE	UL(0x20000000) /* Off-chip flash */
#define QSPI1_FLASH_BASE	UL(0x30000000)
#define QSPI_FLASH_SIZE		UL(0x10000000)
#define DDR_BASE		UL(0x80000000)
#if 0
#define DDR_SIZE		ULL(0x0000001F80000000)
#else
#define DDR_SIZE		UL(0x200000000) /* 8GB */
#endif

#ifdef CONFIG_UNLEASHED_ZSBL
#define ROM_BASE		MASK_ROM_BASE
#define ROMEND			(MASK_ROM_BASE + MASK_ROM_SIZE)
#define RAM_BASE		L2_LIM_BASE
#define RAMEND			(L2_LIM_BASE + L2_LIM_SIZE)
#endif
#ifdef CONFIG_UNLEASHED_FSBL
#define ROM_BASE		L2_LIM_BASE
#define ROMEND			(L2_LIM_BASE + L2_LIM_SIZE)
#define RAM_BASE		L2_LIM_BASE
#define RAMEND			(L2_LIM_BASE + L2_LIM_SIZE)
#endif
#ifdef CONFIG_UNLEASHED_BBL
#define ROM_BASE		DDR_BASE
#define ROMEND			(DDR_BASE + DDR_SIZE)
#define RAM_BASE		DDR_BASE
#define RAMEND			(DDR_BASE + DDR_SIZE)
#endif

#endif /* __REG_UNLEASHED_H_INCLUDE__ */
