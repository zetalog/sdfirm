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
 * @(#)reg.h: kendryte K210 space and register definitions
 * $Id: reg.h,v 1.1 2020-01-26 20:25:00 zhenglv Exp $
 */

#ifndef __REG_K210_H_INCLUDE__
#define __REG_K210_H_INCLUDE__

/* Under Coreplex */
#define CLINT_BASE		UL(0x02000000)
#define PLIC_BASE		UL(0x0C000000)

/* Under TileLink */
#define UARTHS_BASE		UL(0x38000000)
#define GPIOHS_BASE		UL(0x38001000)

/* Under AXI 64 bit */
#define RAM_BASE		UL(0x80000000)
#define RAM_SIZE		UL(0x600000)

#define IO_BASE			UL(0x40000000)
#define IO_SIZE			UL(0x600000)

#define AI_RAM_BASE		UL(0x80600000)
#define AI_RAM_SIZE		UL(0x200000)

#define AI_IO_BASE		UL(0x40600000)
#define AI_IO_SIZE		UL(0x200000)

#define AI_BASE			UL(0x40800000)
#define AI_SIZE			UL(0xC00000)

#define FFT_BASE		UL(0x42000000)
#define FFT_SIZE		UL(0x400000)

#define ROM_BASE		UL(0x88000000)
#define ROM_SIZE		UL(0x20000)

/* Under AHB 32 bit */
#define DMAC_BASE		UL(0x50000000)

/* Under APB1 32 bit */
#define GPIO_BASE		UL(0x50200000)
#define UART1_BASE		UL(0x50210000)
#define UART2_BASE		UL(0x50220000)
#define UART3_BASE		UL(0x50230000)
#define SPI_SLAVE_BASE		UL(0x50240000)
#define I2S0_BASE		UL(0x50250000)
#define I2S1_BASE		UL(0x50260000)
#define I2S2_BASE		UL(0x50270000)
#define I2C0_BASE		UL(0x50280000)
#define I2C1_BASE		UL(0x50290000)
#define I2C2_BASE		UL(0x502A0000)
#define FPIOA_BASE		UL(0x502B0000)
#define SHA256_BASE		UL(0x502C0000)
#define TIMER0_BASE		UL(0x502D0000)
#define TIMER1_BASE		UL(0x502E0000)
#define TIMER2_BASE		UL(0x502F0000)

/* Under APB2 32 bit */
#define WDT0_BASE		UL(0x50400000)
#define WDT1_BASE		UL(0x50410000)
#define OTP_BASE		UL(0x50420000)
#define DVP_BASE		UL(0x50430000)
#define SYSCTL_BASE		UL(0x50440000)
#define AES_BASE		UL(0x50450000)
#define RTC_BASE		UL(0x50460000)

/* Under APB3 32 bit */
#define SPI0_BASE		UL(0x52000000)
#define SPI1_BASE		UL(0x53000000)
#define SPI3_BASE		UL(0x54000000)

#define ROMEND			(ROM_BASE + ROM_SIZE)
#define RAMEND			(RAM_BASE + RAM_SIZE)

#endif /* __REG_K210_H_INCLUDE__ */
