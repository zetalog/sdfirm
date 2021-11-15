/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)irq.h: DPU-LP specific IRQ assignments
 * $Id: irq.h,v 1.1 2021-11-01 16:42:00 zhenglv Exp $
 */

#ifndef __IRQ_DPULP_H_INCLUDE__
#define __IRQ_DPULP_H_INCLUDE__

#define IRQ_GPIO0		EXT_IRQ(9)
#define IRQ_GPIO1		EXT_IRQ(10)
#define IRQ_GPIO2		EXT_IRQ(11)
#define IRQ_GPIO3		EXT_IRQ(12)
#define IRQ_GPIO4		EXT_IRQ(13)
#define IRQ_GPIO5		EXT_IRQ(14)
#define IRQ_GPIO6		EXT_IRQ(15)
#define IRQ_GPIO7		EXT_IRQ(16)
#define IRQ_FLASH		EXT_IRQ(17)
#define IRQ_SD			EXT_IRQ(18)
#define IRQ_SD_WAKEUP		EXT_IRQ(19)
#define IRQ_UART0		EXT_IRQ(20)
#define IRQ_UART1		EXT_IRQ(21)
#define IRQ_UART2		EXT_IRQ(22)
#define IRQ_UART3		EXT_IRQ(23)
#define IRQ_I2C0		EXT_IRQ(24)
#define IRQ_I2C1		EXT_IRQ(25)
#define IRQ_I2C2		EXT_IRQ(26)
#define IRQ_I2C3		EXT_IRQ(27)
#define IRQ_SSI			EXT_IRQ(28)
#define IRQ_WDT			EXT_IRQ(29)
#define IRQ_TSENS0		EXT_IRQ(30)
#define IRQ_TSENS1		EXT_IRQ(31)
#define IRQ_TSENS2		EXT_IRQ(32)
#define IRQ_TSENS3		EXT_IRQ(33)
#define IRQ_DDR0_ARPOISON_0	EXT_IRQ(41)
#define IRQ_DDR0_AWPOISON_0	EXT_IRQ(42)
#define IRQ_DDR0_ARPOISON_1	EXT_IRQ(43)
#define IRQ_DDR0_AWPOISON_1	EXT_IRQ(44)
#define IRQ_DDR0_PHY		EXT_IRQ(45)
#define IRQ_DDR0_ECC_CE		EXT_IRQ(46)
#define IRQ_DDR0_ECC_CE_FAULT	EXT_IRQ(47)
#define IRQ_DDR0_ECC_UE		EXT_IRQ(48)
#define IRQ_DDR0_ECC_UE_FAULT	EXT_IRQ(49)
#define IRQ_DDR0_SBR_DONE	EXT_IRQ(50)
#define IRQ_DDR1_ARPOISON_0	EXT_IRQ(57)
#define IRQ_DDR1_AWPOISON_0	EXT_IRQ(58)
#define IRQ_DDR1_ARPOISON_1	EXT_IRQ(59)
#define IRQ_DDR1_AWPOISON_1	EXT_IRQ(60)
#define IRQ_DDR1_PHY		EXT_IRQ(61)
#define IRQ_DDR1_ECC_CE		EXT_IRQ(62)
#define IRQ_DDR1_ECC_CE_FAULT	EXT_IRQ(63)
#define IRQ_DDR1_ECC_UE		EXT_IRQ(64)
#define IRQ_DDR1_ECC_UE_FAULT	EXT_IRQ(65)
#define IRQ_DDR1_SBR_DONE	EXT_IRQ(66)
#define IRQ_ETH_CH0_RX		EXT_IRQ(97)
#define IRQ_ETH_CH1_RX		EXT_IRQ(98)
#define IRQ_ETH_CH2_RX		EXT_IRQ(99)
#define IRQ_ETH_CH3_RX		EXT_IRQ(100)
#define IRQ_ETH_CH4_RX		EXT_IRQ(101)
#define IRQ_ETH_CH5_RX		EXT_IRQ(102)
#define IRQ_ETH_CH6_RX		EXT_IRQ(103)
#define IRQ_ETH_CH7_RX		EXT_IRQ(104)
#define IRQ_ETH_CH0_TX		EXT_IRQ(105)
#define IRQ_ETH_CH1_TX		EXT_IRQ(106)
#define IRQ_ETH_CH2_TX		EXT_IRQ(107)
#define IRQ_ETH_CH3_TX		EXT_IRQ(108)
#define IRQ_ETH_CH4_TX		EXT_IRQ(109)
#define IRQ_ETH_CH5_TX		EXT_IRQ(110)
#define IRQ_ETH_CH6_TX		EXT_IRQ(111)
#define IRQ_ETH_CH7_TX		EXT_IRQ(112)
#define IRQ_ETH_LPI		EXT_IRQ(113)
#define IRQ_ETH_PMT		EXT_IRQ(114)
#define IRQ_ETH_XGMAC		EXT_IRQ(115)
#define IRQ_ETH_XPCS		EXT_IRQ(116)
#define IRQ_PE16_DEBUG		EXT_IRQ(121)
#define IRQ_PE16_DMA0		EXT_IRQ(122)
#define IRQ_PE16_DMA1		EXT_IRQ(123)
#define IRQ_PE16_MICC0		EXT_IRQ(124)
#define IRQ_PE16_MICC1		EXT_IRQ(125)
#define IRQ_PCIE_AER		EXT_IRQ(129)
#define IRQ_PCIE_BRF		EXT_IRQ(130) /* Bridge flush */
#define IRQ_PCIE_HP		EXT_IRQ(131) /* Hot-plug */
#define IRQ_PCIE_INTA		EXT_IRQ(132)
#define IRQ_PCIE_INTB		EXT_IRQ(133)
#define IRQ_PCIE_INTC		EXT_IRQ(134)
#define IRQ_PCIE_INTD		EXT_IRQ(135)
#define IRQ_PCIE_LD		EXT_IRQ(136) /* Link down */
#define IRQ_PCIE_LDMA		EXT_IRQ(137) /* Local DMA */
#define IRQ_PCIE_MSI		EXT_IRQ(138)
#define IRQ_PCIE_PME		EXT_IRQ(139)
#define IRQ_VPU0		EXT_IRQ(145)
#define IRQ_VPU1		EXT_IRQ(149)
#define IRQ_RAB0_PHY0		EXT_IRQ(153)
#define IRQ_RAB0_PHY1		EXT_IRQ(154)
#define IRQ_RAB0_PHY2		EXT_IRQ(155)
#define IRQ_RAB0_PHY3		EXT_IRQ(156)
#define IRQ_RAB0_PHY4		EXT_IRQ(157)
#define IRQ_RAB0_PHY5		EXT_IRQ(158)
#define IRQ_RAB0_PHY6		EXT_IRQ(159)
#define IRQ_RAB0_PHY7		EXT_IRQ(160)
#define IRQ_RAB0_PHY8		EXT_IRQ(161)
#define IRQ_RAB0_PHY9		EXT_IRQ(162)
#define IRQ_RAB0_PHY10		EXT_IRQ(163)
#define IRQ_RAB0_PHY11		EXT_IRQ(164)
#define IRQ_RAB0_PHY12		EXT_IRQ(165)
#define IRQ_RAB0_PHY13		EXT_IRQ(166)
#define IRQ_RAB0_PHY14		EXT_IRQ(167)
#define IRQ_RAB0_PHY15		EXT_IRQ(168)
#define IRQ_RAB0		EXT_IRQ(169)
#define IRQ_RAB1_PHY0		EXT_IRQ(177)
#define IRQ_RAB1_PHY1		EXT_IRQ(178)
#define IRQ_RAB1_PHY2		EXT_IRQ(179)
#define IRQ_RAB1_PHY3		EXT_IRQ(180)
#define IRQ_RAB1_PHY4		EXT_IRQ(181)
#define IRQ_RAB1_PHY5		EXT_IRQ(182)
#define IRQ_RAB1_PHY6		EXT_IRQ(183)
#define IRQ_RAB1_PHY7		EXT_IRQ(184)
#define IRQ_RAB1_PHY8		EXT_IRQ(185)
#define IRQ_RAB1_PHY9		EXT_IRQ(186)
#define IRQ_RAB1_PHY10		EXT_IRQ(187)
#define IRQ_RAB1_PHY11		EXT_IRQ(188)
#define IRQ_RAB1_PHY12		EXT_IRQ(189)
#define IRQ_RAB1_PHY13		EXT_IRQ(190)
#define IRQ_RAB1_PHY14		EXT_IRQ(191)
#define IRQ_RAB1_PHY15		EXT_IRQ(192)
#define IRQ_RAB1		EXT_IRQ(193)

#define NR_EXT_IRQS		256
#define NR_IRQS			(NR_INT_IRQS + NR_EXT_IRQS)

#endif  /* __IRQ_DPULP_H_INCLUDE__ */
