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
 * @(#)irq.h: kendryte K210 specific IRQ assignments
 * $Id: irq.h,v 1.0 2020-13-28 13:41:00 zhenglv Exp $
 */

#ifndef __IRQ_K210_H_INCLUDE__
#define __IRQ_K210_H_INCLUDE__

#define IRQ_SPI0	1
#define IRQ_SPI1	2
#define IRQ_SPI_SLAVE	3
#define IRQ_SPI3	4
#define IRQ_I2S0	5
#define IRQ_I2S1	6
#define IRQ_I2S2	7
#define IRQ_I2C0	8
#define IRQ_I2C1	9
#define IRQ_I2C2	10
#define IRQ_UART1	11
#define IRQ_UART2	12
#define IRQ_UART3	13
#define IRQ_TIMER0A	14
#define IRQ_TIMER0B	15
#define IRQ_TIMER1A	16
#define IRQ_TIMER1B	17
#define IRQ_TIMER2A	18
#define IRQ_TIMER2B	19
#define IRQ_RTC		20
#define IRQ_WDT0	21
#define IRQ_WDT1	22
#define IRQ_APB_GPIO	23
#define IRQ_DVP		24
#define IRQ_AI		25
#define IRQ_FFT		26
#define IRQ_DMA0	27
#define IRQ_DMA1	28
#define IRQ_DMA2	29
#define IRQ_DMA3	30
#define IRQ_DMA4	31
#define IRQ_DMA5	32
#define IRQ_UARTHS	33
#define IRQ_GPIOHS0	34
#define IRQ_GPIOHS1	35
#define IRQ_GPIOHS2	36
#define IRQ_GPIOHS3	37
#define IRQ_GPIOHS4	38
#define IRQ_GPIOHS5	39
#define IRQ_GPIOHS6	40
#define IRQ_GPIOHS7	41
#define IRQ_GPIOHS8	42
#define IRQ_GPIOHS9	43
#define IRQ_GPIOHS10	44
#define IRQ_GPIOHS11	45
#define IRQ_GPIOHS12	46
#define IRQ_GPIOHS13	47
#define IRQ_GPIOHS14	48
#define IRQ_GPIOHS15	49
#define IRQ_GPIOHS16	50
#define IRQ_GPIOHS17	51
#define IRQ_GPIOHS18	52
#define IRQ_GPIOHS19	53
#define IRQ_GPIOHS20	54
#define IRQ_GPIOHS21	55
#define IRQ_GPIOHS22	56
#define IRQ_GPIOHS23	57
#define IRQ_GPIOHS24	58
#define IRQ_GPIOHS25	59
#define IRQ_GPIOHS26	60
#define IRQ_GPIOHS27	61
#define IRQ_GPIOHS28	62
#define IRQ_GPIOHS29	63
#define IRQ_GPIOHS30	64
#define IRQ_GPIOHS31	65

#define NR_IRQS		128

#endif /* __IRQ_K210_H_INCLUDE__ */
