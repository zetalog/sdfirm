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

#define IRQ_SPI0	EXT_IRQ(1)
#define IRQ_SPI1	EXT_IRQ(2)
#define IRQ_SPI_SLAVE	EXT_IRQ(3)
#define IRQ_SPI3	EXT_IRQ(4)
#define IRQ_I2S0	EXT_IRQ(5)
#define IRQ_I2S1	EXT_IRQ(6)
#define IRQ_I2S2	EXT_IRQ(7)
#define IRQ_I2C0	EXT_IRQ(8)
#define IRQ_I2C1	EXT_IRQ(9)
#define IRQ_I2C2	EXT_IRQ(10)
#define IRQ_UART1	EXT_IRQ(11)
#define IRQ_UART2	EXT_IRQ(12)
#define IRQ_UART3	EXT_IRQ(13)
#define IRQ_TIMER0A	EXT_IRQ(14)
#define IRQ_TIMER0B	EXT_IRQ(15)
#define IRQ_TIMER1A	EXT_IRQ(16)
#define IRQ_TIMER1B	EXT_IRQ(17)
#define IRQ_TIMER2A	EXT_IRQ(18)
#define IRQ_TIMER2B	EXT_IRQ(19)
#define IRQ_RTC		EXT_IRQ(20)
#define IRQ_WDT0	EXT_IRQ(21)
#define IRQ_WDT1	EXT_IRQ(22)
#define IRQ_APB_GPIO	EXT_IRQ(23)
#define IRQ_DVP		EXT_IRQ(24)
#define IRQ_AI		EXT_IRQ(25)
#define IRQ_FFT		EXT_IRQ(26)
#define IRQ_DMA0	EXT_IRQ(27)
#define IRQ_DMA1	EXT_IRQ(28)
#define IRQ_DMA2	EXT_IRQ(29)
#define IRQ_DMA3	EXT_IRQ(30)
#define IRQ_DMA4	EXT_IRQ(31)
#define IRQ_DMA5	EXT_IRQ(32)
#define IRQ_UARTHS	EXT_IRQ(33)
#define IRQ_GPIOHS0	EXT_IRQ(34)
#define IRQ_GPIOHS1	EXT_IRQ(35)
#define IRQ_GPIOHS2	EXT_IRQ(36)
#define IRQ_GPIOHS3	EXT_IRQ(37)
#define IRQ_GPIOHS4	EXT_IRQ(38)
#define IRQ_GPIOHS5	EXT_IRQ(39)
#define IRQ_GPIOHS6	EXT_IRQ(40)
#define IRQ_GPIOHS7	EXT_IRQ(41)
#define IRQ_GPIOHS8	EXT_IRQ(42)
#define IRQ_GPIOHS9	EXT_IRQ(43)
#define IRQ_GPIOHS10	EXT_IRQ(44)
#define IRQ_GPIOHS11	EXT_IRQ(45)
#define IRQ_GPIOHS12	EXT_IRQ(46)
#define IRQ_GPIOHS13	EXT_IRQ(47)
#define IRQ_GPIOHS14	EXT_IRQ(48)
#define IRQ_GPIOHS15	EXT_IRQ(49)
#define IRQ_GPIOHS16	EXT_IRQ(50)
#define IRQ_GPIOHS17	EXT_IRQ(51)
#define IRQ_GPIOHS18	EXT_IRQ(52)
#define IRQ_GPIOHS19	EXT_IRQ(53)
#define IRQ_GPIOHS20	EXT_IRQ(54)
#define IRQ_GPIOHS21	EXT_IRQ(55)
#define IRQ_GPIOHS22	EXT_IRQ(56)
#define IRQ_GPIOHS23	EXT_IRQ(57)
#define IRQ_GPIOHS24	EXT_IRQ(58)
#define IRQ_GPIOHS25	EXT_IRQ(59)
#define IRQ_GPIOHS26	EXT_IRQ(60)
#define IRQ_GPIOHS27	EXT_IRQ(61)
#define IRQ_GPIOHS28	EXT_IRQ(62)
#define IRQ_GPIOHS29	EXT_IRQ(63)
#define IRQ_GPIOHS30	EXT_IRQ(64)
#define IRQ_GPIOHS31	EXT_IRQ(65)

#define NR_EXT_IRQS	128
#define NR_IRQS		(NR_INT_IRQS + NR_EXT_IRQS)

#endif /* __IRQ_K210_H_INCLUDE__ */
