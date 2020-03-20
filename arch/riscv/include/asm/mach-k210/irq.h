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

#define IRQ_SPI0	IRQ_EXT(1)
#define IRQ_SPI1	IRQ_EXT(2)
#define IRQ_SPI_SLAVE	IRQ_EXT(3)
#define IRQ_SPI3	IRQ_EXT(4)
#define IRQ_I2S0	IRQ_EXT(5)
#define IRQ_I2S1	IRQ_EXT(6)
#define IRQ_I2S2	IRQ_EXT(7)
#define IRQ_I2C0	IRQ_EXT(8)
#define IRQ_I2C1	IRQ_EXT(9)
#define IRQ_I2C2	IRQ_EXT(10)
#define IRQ_UART1	IRQ_EXT(11)
#define IRQ_UART2	IRQ_EXT(12)
#define IRQ_UART3	IRQ_EXT(13)
#define IRQ_TIMER0A	IRQ_EXT(14)
#define IRQ_TIMER0B	IRQ_EXT(15)
#define IRQ_TIMER1A	IRQ_EXT(16)
#define IRQ_TIMER1B	IRQ_EXT(17)
#define IRQ_TIMER2A	IRQ_EXT(18)
#define IRQ_TIMER2B	IRQ_EXT(19)
#define IRQ_RTC		IRQ_EXT(20)
#define IRQ_WDT0	IRQ_EXT(21)
#define IRQ_WDT1	IRQ_EXT(22)
#define IRQ_APB_GPIO	IRQ_EXT(23)
#define IRQ_DVP		IRQ_EXT(24)
#define IRQ_AI		IRQ_EXT(25)
#define IRQ_FFT		IRQ_EXT(26)
#define IRQ_DMA0	IRQ_EXT(27)
#define IRQ_DMA1	IRQ_EXT(28)
#define IRQ_DMA2	IRQ_EXT(29)
#define IRQ_DMA3	IRQ_EXT(30)
#define IRQ_DMA4	IRQ_EXT(31)
#define IRQ_DMA5	IRQ_EXT(32)
#define IRQ_UARTHS	IRQ_EXT(33)
#define IRQ_GPIOHS0	IRQ_EXT(34)
#define IRQ_GPIOHS1	IRQ_EXT(35)
#define IRQ_GPIOHS2	IRQ_EXT(36)
#define IRQ_GPIOHS3	IRQ_EXT(37)
#define IRQ_GPIOHS4	IRQ_EXT(38)
#define IRQ_GPIOHS5	IRQ_EXT(39)
#define IRQ_GPIOHS6	IRQ_EXT(40)
#define IRQ_GPIOHS7	IRQ_EXT(41)
#define IRQ_GPIOHS8	IRQ_EXT(42)
#define IRQ_GPIOHS9	IRQ_EXT(43)
#define IRQ_GPIOHS10	IRQ_EXT(44)
#define IRQ_GPIOHS11	IRQ_EXT(45)
#define IRQ_GPIOHS12	IRQ_EXT(46)
#define IRQ_GPIOHS13	IRQ_EXT(47)
#define IRQ_GPIOHS14	IRQ_EXT(48)
#define IRQ_GPIOHS15	IRQ_EXT(49)
#define IRQ_GPIOHS16	IRQ_EXT(50)
#define IRQ_GPIOHS17	IRQ_EXT(51)
#define IRQ_GPIOHS18	IRQ_EXT(52)
#define IRQ_GPIOHS19	IRQ_EXT(53)
#define IRQ_GPIOHS20	IRQ_EXT(54)
#define IRQ_GPIOHS21	IRQ_EXT(55)
#define IRQ_GPIOHS22	IRQ_EXT(56)
#define IRQ_GPIOHS23	IRQ_EXT(57)
#define IRQ_GPIOHS24	IRQ_EXT(58)
#define IRQ_GPIOHS25	IRQ_EXT(59)
#define IRQ_GPIOHS26	IRQ_EXT(60)
#define IRQ_GPIOHS27	IRQ_EXT(61)
#define IRQ_GPIOHS28	IRQ_EXT(62)
#define IRQ_GPIOHS29	IRQ_EXT(63)
#define IRQ_GPIOHS30	IRQ_EXT(64)
#define IRQ_GPIOHS31	IRQ_EXT(65)

#define NR_EXT_IRQS	128
#define NR_IRQS		(NR_INT_IRQS + NR_EXT_IRQS)

#endif /* __IRQ_K210_H_INCLUDE__ */
