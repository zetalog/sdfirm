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
 * @(#)irq.h: K1Matrix specific IRQ assignments
 * $Id: irq.h,v 1.1 2023-09-06 18:38:00 zhenglv Exp $
 */

#ifndef __IRQ_K1MATRIX_H_INCLUDE__
#define __IRQ_K1MATRIX_H_INCLUDE__

#ifdef CONFIG_K1MATRIX_RMU
#define IRQ_X60_ERR		EXT_IRQ(1)
#define IRQ_RMU_TIMER0		EXT_IRQ(2)
#define IRQ_RMU_TIMER1		EXT_IRQ(3)
#define IRQ_RMU_TIMER2		EXT_IRQ(4)
#define IRQ_RMU_TIMER3		EXT_IRQ(5)
#define IRQ_RMU_WDT		EXT_IRQ(8)
#define IRQ_APU_WDT		EXT_IRQ(9)
#define IRQ_RMU_STM		EXT_IRQ(10)
#define IRQ_C0_C0_WAKEUP	EXT_IRQ(12)
#define IRQ_C0_C1_WAKEUP	EXT_IRQ(13)
#define IRQ_C0_C2_WAKEUP	EXT_IRQ(14)
#define IRQ_C0_C3_WAKEUP	EXT_IRQ(15)
#define IRQ_C1_C0_WAKEUP	EXT_IRQ(16)
#define IRQ_C1_C1_WAKEUP	EXT_IRQ(17)
#define IRQ_C1_C2_WAKEUP	EXT_IRQ(18)
#define IRQ_C1_C3_WAKEUP	EXT_IRQ(19)
#define IRQ_C2_C0_WAKEUP	EXT_IRQ(20)
#define IRQ_C2_C1_WAKEUP	EXT_IRQ(21)
#define IRQ_C2_C2_WAKEUP	EXT_IRQ(22)
#define IRQ_C2_C3_WAKEUP	EXT_IRQ(23)
#define IRQ_C3_C0_WAKEUP	EXT_IRQ(24)
#define IRQ_C3_C1_WAKEUP	EXT_IRQ(25)
#define IRQ_C3_C2_WAKEUP	EXT_IRQ(26)
#define IRQ_C3_C3_WAKEUP	EXT_IRQ(27)
#define IRQ_C4_C0_WAKEUP	EXT_IRQ(28)
#define IRQ_C4_C1_WAKEUP	EXT_IRQ(29)
#define IRQ_C4_C2_WAKEUP	EXT_IRQ(30)
#define IRQ_C4_C3_WAKEUP	EXT_IRQ(31)
#define IRQ_C5_C0_WAKEUP	EXT_IRQ(32)
#define IRQ_C5_C1_WAKEUP	EXT_IRQ(33)
#define IRQ_C5_C2_WAKEUP	EXT_IRQ(34)
#define IRQ_C5_C3_WAKEUP	EXT_IRQ(35)
#define IRQ_C6_C0_WAKEUP	EXT_IRQ(36)
#define IRQ_C6_C1_WAKEUP	EXT_IRQ(37)
#define IRQ_C6_C2_WAKEUP	EXT_IRQ(38)
#define IRQ_C6_C3_WAKEUP	EXT_IRQ(39)
#define IRQ_C7_C0_WAKEUP	EXT_IRQ(40)
#define IRQ_C7_C1_WAKEUP	EXT_IRQ(41)
#define IRQ_C7_C2_WAKEUP	EXT_IRQ(42)
#define IRQ_C7_C3_WAKEUP	EXT_IRQ(43)
#define IRQ_MBOX_S0		EXT_IRQ(44)
#define IRQ_MBOX_S1		EXT_IRQ(45)
#define IRQ_MBOX_S2		EXT_IRQ(46)
#define IRQ_MBOX_S3		EXT_IRQ(47)
#define IRQ_MBOX_S4		EXT_IRQ(48)
#define IRQ_MBOX_S5		EXT_IRQ(49)
#define IRQ_MBOX_S6		EXT_IRQ(50)
#define IRQ_MBOX_S7		EXT_IRQ(51)
#define IRQ_MBOX_S8		EXT_IRQ(52)
#define IRQ_MBOX_S9		EXT_IRQ(53)
#define IRQ_MBOX_S10		EXT_IRQ(54)
#define IRQ_MBOX_S11		EXT_IRQ(55)
#define IRQ_MBOX_S12		EXT_IRQ(56)
#define IRQ_MBOX_S13		EXT_IRQ(57)
#define IRQ_MBOX_S14		EXT_IRQ(58)
#define IRQ_MBOX_S15		EXT_IRQ(59)
#define IRQ_MBOX_NS0		EXT_IRQ(60)
#define IRQ_MBOX_NS1		EXT_IRQ(61)
#define IRQ_MBOX_NS2		EXT_IRQ(62)
#define IRQ_MBOX_NS3		EXT_IRQ(63)
#define IRQ_MBOX_NS4		EXT_IRQ(64)
#define IRQ_MBOX_NS5		EXT_IRQ(65)
#define IRQ_MBOX_NS6		EXT_IRQ(66)
#define IRQ_MBOX_NS7		EXT_IRQ(67)
#define IRQ_MBOX_NS8		EXT_IRQ(68)
#define IRQ_MBOX_NS9		EXT_IRQ(69)
#define IRQ_MBOX_NS10		EXT_IRQ(70)
#define IRQ_MBOX_NS11		EXT_IRQ(71)
#define IRQ_MBOX_NS12		EXT_IRQ(72)
#define IRQ_MBOX_NS13		EXT_IRQ(73)
#define IRQ_MBOX_NS14		EXT_IRQ(74)
#define IRQ_MBOX_NS15		EXT_IRQ(75)
#define IRQ_MBOX_NS16		EXT_IRQ(76)
#define IRQ_MBOX_NS17		EXT_IRQ(77)
#define IRQ_MBOX_NS18		EXT_IRQ(78)
#define IRQ_MBOX_NS19		EXT_IRQ(79)
#define IRQ_MBOX_NS20		EXT_IRQ(80)
#define IRQ_MBOX_NS21		EXT_IRQ(81)
#define IRQ_MBOX_NS22		EXT_IRQ(82)
#define IRQ_MBOX_NS23		EXT_IRQ(83)
#define IRQ_MBOX_NS24		EXT_IRQ(84)
#define IRQ_MBOX_NS25		EXT_IRQ(85)
#define IRQ_MBOX_NS26		EXT_IRQ(86)
#define IRQ_MBOX_NS27		EXT_IRQ(87)
#define IRQ_MBOX_NS28		EXT_IRQ(88)
#define IRQ_MBOX_NS29		EXT_IRQ(89)
#define IRQ_MBOX_NS30		EXT_IRQ(90)
#define IRQ_MBOX_NS31		EXT_IRQ(91)
#define IRQ_SPINLOCK		EXT_IRQ(92)
#define IRQ_EHSM		EXT_IRQ(98)
#define IRQ_PVT			EXT_IRQ(100)
#define IRQ_PVT_1		EXT_IRQ(101)
#define IRQ_DIE1_MEIP		EXT_IRQ(104)
#define IRQ_DIE2_MEIP		EXT_IRQ(105)
#define IRQ_DIE3_MEIP		EXT_IRQ(106)
#define NIC400_IMISC_SLAVEIF0_BMU_INTR	EXT_IRQ(107)
#define PCIE1_SLV_BMU_INTR	EXT_IRQ(108)
#define PCIE1_TS_MSTR_BMU_INTR	EXT_IRQ(109)
#define PCIE1_TMMSTR_BMU_INTR	EXT_IRQ(110)
#define IRQ_DMAC		EXT_IRQ(112)
#define IRQ_GPIOA		EXT_IRQ(114)
#define IRQ_GPIOB		EXT_IRQ(115)
#define IRQ_GPIOC		EXT_IRQ(116)
#define IRQ_ESPI_CON		EXT_IRQ(120)
#define IRQ_ESPI_VW		EXT_IRQ(121)
#define IRQ_QSPI		EXT_IRQ(123)
#define IRQ_SMBUS0		EXT_IRQ(124)
#define IRQ_SMBUS1		EXT_IRQ(125)
#define IRQ_I2C0		EXT_IRQ(126)
#define IRQ_I2C1		EXT_IRQ(127)
#define IRQ_I2C2		EXT_IRQ(128)
#define IRQ_I2C3		EXT_IRQ(129)
#define IRQ_UART0		EXT_IRQ(134)
#define IRQ_UART1		EXT_IRQ(135)
#define IRQ_SPS_S		EXT_IRQ(140)
#define IRQ_SPS_M		EXT_IRQ(141)
#define IRQ_N100_REQERRS_NID0	EXT_IRQ(144)
#define IRQ_N100_REQERRNS_NID0	EXT_IRQ(145)
#define IRQ_N100_REQFLTS_NID0	EXT_IRQ(146)
#define IRQ_N100_REQFLTNS_NID0	EXT_IRQ(147)
#define PCIE0_AER_CE		EXT_IRQ(148)
#endif
#ifdef CONFIG_K1MATRIX_APU
#define IRQ_TIMER0		EXT_IRQ(18)
#define IRQ_TIMER1		EXT_IRQ(19)

#define IRQ_GPIO0		EXT_IRQ(22)
#define IRQ_GPIO1		EXT_IRQ(23)
#define IRQ_WDT			EXT_IRQ(24)
#define IRQ_SPINLOCK		EXT_IRQ(25)
#define IRQ_DMA			EXT_IRQ(26)
#define IRQ_DDR_BMU		EXT_IRQ(27)
#define IRQ_DDR_PHY		EXT_IRQ(28)
#define IRQ_CCIX_SII		EXT_IRQ(29)
#define IRQ_CCIX_MSI		EXT_IRQ(30)
#define IRQ_CCIX_EDMA		EXT_IRQ(31)
#define IRQ_PCIE1_SII		EXT_IRQ(32)
#define IRQ_PCIE1_MSI		EXT_IRQ(33)
#define IRQ_PCIE1_EDMA		EXT_IRQ(34)
#define IRQ_GMAC		EXT_IRQ(35)
#define IRQ_GMAC_RX		EXT_IRQ(36)
#define IRQ_GMAC_TX0		EXT_IRQ(37)
#define IRQ_GMAC_TX1		EXT_IRQ(38)
#define IRQ_UART0		EXT_IRQ(134)
#define IRQ_UART1		EXT_IRQ(135)
#define IRQ_UART2		EXT_IRQ(136)
#define IRQ_UART3		EXT_IRQ(137)
#endif

/* Simulation pseudo IRQs */
#define IRQ_SMMU_EVTQ		EXT_IRQ(48)
#define IRQ_SMMU_PRIQ		EXT_IRQ(49)
#define IRQ_SMMU_CMDQ		EXT_IRQ(50)
#define IRQ_SMMU_GERR		EXT_IRQ(51)
#define IRQ_LPC			EXT_IRQ(56)
#define IRQ_KCS			EXT_IRQ(57)
#define IRQ_BMU			EXT_IRQ(58)

#define NR_EXT_IRQS		256
#define NR_IRQS			(NR_INT_IRQS + NR_EXT_IRQS)

#endif /* __IRQ_K1MATRIX_H_INCLUDE__ */
