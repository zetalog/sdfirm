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
 * @(#)irq.h: DUOWEN specific IRQ assignments
 * $Id: irq.h,v 1.1 2019-09-02 14:14:00 zhenglv Exp $
 */

#ifndef __IRQ_DUOWEN_H_INCLUDE__
#define __IRQ_DUOWEN_H_INCLUDE__

#define IRQ_CFAB_ECC_CE		IRQ_EXT(1) /* Correctable error */
#define IRQ_CFAB_ECC_UE		IRQ_EXT(2) /* Uncorrectable error */

#define IRQ_PCIE_X16_INTA	IRQ_EXT(3)
#define IRQ_PCIE_X16_INTB	IRQ_EXT(4)
#define IRQ_PCIE_X16_INTC	IRQ_EXT(5)
#define IRQ_PCIE_X16_INTD	IRQ_EXT(6)
#define IRQ_PCIE_X16_MSI	IRQ_EXT(8)
#define IRQ_PCIE_X16_HP		IRQ_EXT(10) /* Hot-plug */
#define IRQ_PCIE_X16_LD		IRQ_EXT(11) /* Link down */
#define IRQ_PCIE_X16_BF		IRQ_EXT(12) /* Bridge flush */
#define IRQ_PCIE_X16_LDMA	IRQ_EXT(13) /* Local DMA */

#define IRQ_PCIE_X8_INTA	IRQ_EXT(14)
#define IRQ_PCIE_X8_INTB	IRQ_EXT(15)
#define IRQ_PCIE_X8_INTC	IRQ_EXT(16)
#define IRQ_PCIE_X8_INTD	IRQ_EXT(17)
#define IRQ_PCIE_X8_MSI		IRQ_EXT(19)
#define IRQ_PCIE_X8_HP		IRQ_EXT(21)
#define IRQ_PCIE_X8_LD		IRQ_EXT(22)
#define IRQ_PCIE_X8_BF		IRQ_EXT(23)
#define IRQ_PCIE_X8_LDMA	IRQ_EXT(24)

#define IRQ_PCIE_X4_0_INTA	IRQ_EXT(25)
#define IRQ_PCIE_X4_0_INTB	IRQ_EXT(26)
#define IRQ_PCIE_X4_0_INTC	IRQ_EXT(27)
#define IRQ_PCIE_X4_0_INTD	IRQ_EXT(28)
#define IRQ_PCIE_X4_0_MSI	IRQ_EXT(30)
#define IRQ_PCIE_X4_0_HP	IRQ_EXT(32)
#define IRQ_PCIE_X4_0_LD	IRQ_EXT(33)
#define IRQ_PCIE_X4_0_BF	IRQ_EXT(34)
#define IRQ_PCIE_X4_0_LDMA	IRQ_EXT(35)

#define IRQ_PCIE_X4_1_INTA	IRQ_EXT(36)
#define IRQ_PCIE_X4_1_INTB	IRQ_EXT(37)
#define IRQ_PCIE_X4_1_INTC	IRQ_EXT(38)
#define IRQ_PCIE_X4_1_INTD	IRQ_EXT(39)
#define IRQ_PCIE_X4_1_MSI	IRQ_EXT(41)
#define IRQ_PCIE_X4_1_HP	IRQ_EXT(43)
#define IRQ_PCIE_X4_1_LD	IRQ_EXT(44)
#define IRQ_PCIE_X4_1_BF	IRQ_EXT(45)
#define IRQ_PCIE_X4_1_LDMA	IRQ_EXT(46)

#define IRQ_DDR0_ECC_CE		IRQ_EXT(47) /* Correctable error */
#define IRQ_DDR0_ECC_CE_FAULT	IRQ_EXT(48)
#define IRQ_DDR0_ECC_UE		IRQ_EXT(49) /* Uncorrectable error */
#define IRQ_DDR0_ECC_UE_FAULT	IRQ_EXT(50)
#define IRQ_DDR0_DFI_ALERT_ERR	IRQ_EXT(51)
#define IRQ_DDR0_SBR_DONE	IRQ_EXT(52)
#define IRQ_AWPOISON_0		IRQ_EXT(53)
#define IRQ_ARPOISON_0		IRQ_EXT(54)

#define IRQ_DDR1_ECC_CE		IRQ_EXT(55)
#define IRQ_DDR1_ECC_CE_FAULT	IRQ_EXT(56)
#define IRQ_DDR1_ECC_UE		IRQ_EXT(57)
#define IRQ_DDR1_ECC_UE_FAULT	IRQ_EXT(58)
#define IRQ_DDR1_DFI_ALERT_ERR	IRQ_EXT(59)
#define IRQ_DDR1_SBR_DONE	IRQ_EXT(60)

#define IRQ_DMAC		IRQ_EXT(63)
#define IRQ_DMAC_CMNREG		IRQ_EXT(64)
#define IRQ_DMAC_CH0		IRQ_EXT(65)
#define IRQ_DMAC_CH1		IRQ_EXT(66)
#define IRQ_DMAC_CH2		IRQ_EXT(67)
#define IRQ_DMAC_CH3		IRQ_EXT(68)
#define IRQ_DMAC_CH4		IRQ_EXT(69)
#define IRQ_DMAC_CH5		IRQ_EXT(70)
#define IRQ_DMAC_CH6		IRQ_EXT(71)
#define IRQ_DMAC_CH7		IRQ_EXT(72)

#define IRQ_PCIE_SMMU_GBL_FLT_NS	IRQ_EXT(73)
#define IRQ_PCIE_SMMU_PERF_TBU0		IRQ_EXT(74)
#define IRQ_PCIE_SMMU_PERF_TBU1		IRQ_EXT(75)
#define IRQ_PCIE_SMMU_PERF_TBU2		IRQ_EXT(76)
#define IRQ_PCIE_SMMU_PERF_TBU3		IRQ_EXT(77)
#define IRQ_PCIE_SMMU_COMB_NS		IRQ_EXT(78)

#define IRQ_DMA_SMMU_GBL_FLT_NS		IRQ_EXT(79)
#define IRQ_DMA_SMMU_PERF_TBU_DMA	IRQ_EXT(80)
#define IRQ_DMA_SMMU_COMB_NS		IRQ_EXT(81)

#define IRQ_UART0		IRQ_EXT(82)
#define IRQ_UART1		IRQ_EXT(83)
#define IRQ_UART2		IRQ_EXT(84)
#define IRQ_UART3		IRQ_EXT(85)

#define IRQ_GPIO0_0		IRQ_EXT(86)
#define IRQ_GPIO0_1		IRQ_EXT(87)
#define IRQ_GPIO0_2		IRQ_EXT(88)
#define IRQ_GPIO0_3		IRQ_EXT(89)
#define IRQ_GPIO0_4		IRQ_EXT(90)
#define IRQ_GPIO0_5		IRQ_EXT(91)
#define IRQ_GPIO0_6		IRQ_EXT(92)
#define IRQ_GPIO0_7		IRQ_EXT(93)

#define IRQ_GPIO1_0		IRQ_EXT(94)
#define IRQ_GPIO1_1		IRQ_EXT(95)
#define IRQ_GPIO1_2		IRQ_EXT(96)
#define IRQ_GPIO1_3		IRQ_EXT(97)
#define IRQ_GPIO1_4		IRQ_EXT(98)
#define IRQ_GPIO1_5		IRQ_EXT(99)
#define IRQ_GPIO1_6		IRQ_EXT(100)
#define IRQ_GPIO1_7		IRQ_EXT(101)
#define IRQ_GPIO1_8		IRQ_EXT(102)
#define IRQ_GPIO1_9		IRQ_EXT(103)
#define IRQ_GPIO1_10		IRQ_EXT(104)
#define IRQ_GPIO1_11		IRQ_EXT(105)
#define IRQ_GPIO1_12		IRQ_EXT(106)
#define IRQ_GPIO1_13		IRQ_EXT(107)
#define IRQ_GPIO1_14		IRQ_EXT(108)
#define IRQ_GPIO1_15		IRQ_EXT(109)

#define IRQ_GPIO2_0		IRQ_EXT(110)
#define IRQ_GPIO2_1		IRQ_EXT(111)
#define IRQ_GPIO2_2		IRQ_EXT(112)
#define IRQ_GPIO2_3		IRQ_EXT(113)
#define IRQ_GPIO2_4		IRQ_EXT(114)
#define IRQ_GPIO2_5		IRQ_EXT(115)
#define IRQ_GPIO2_6		IRQ_EXT(116)
#define IRQ_GPIO2_7		IRQ_EXT(117)
#define IRQ_GPIO2_8		IRQ_EXT(118)
#define IRQ_GPIO2_9		IRQ_EXT(119)
#define IRQ_GPIO2_10		IRQ_EXT(120)
#define IRQ_GPIO2_11		IRQ_EXT(121)
#define IRQ_GPIO2_12		IRQ_EXT(122)
#define IRQ_GPIO2_13		IRQ_EXT(123)
#define IRQ_GPIO2_14		IRQ_EXT(124)
#define IRQ_GPIO2_15		IRQ_EXT(125)
#define IRQ_GPIO2_16		IRQ_EXT(126)
#define IRQ_GPIO2_17		IRQ_EXT(127)
#define IRQ_GPIO2_18		IRQ_EXT(128)
#define IRQ_GPIO2_19		IRQ_EXT(129)
#define IRQ_GPIO2_20		IRQ_EXT(130)
#define IRQ_GPIO2_21		IRQ_EXT(131)
#define IRQ_GPIO2_22		IRQ_EXT(132)
#define IRQ_GPIO2_23		IRQ_EXT(133)
#define IRQ_GPIO2_24		IRQ_EXT(134)
#define IRQ_GPIO2_25		IRQ_EXT(135)
#define IRQ_GPIO2_26		IRQ_EXT(136)
#define IRQ_GPIO2_27		IRQ_EXT(137)
#define IRQ_GPIO2_28		IRQ_EXT(138)
#define IRQ_GPIO2_29		IRQ_EXT(139)
#define IRQ_GPIO2_30		IRQ_EXT(140)
#define IRQ_GPIO2_31		IRQ_EXT(141)

#define IRQ_SD			IRQ_EXT(142)
#define IRQ_SD_WAKEUP		IRQ_EXT(143)

#define IRQ_I2C_MST0		IRQ_EXT(144)
#define IRQ_I2C_MST1		IRQ_EXT(145)
#define IRQ_I2C_MST2		IRQ_EXT(146)
#define IRQ_I2C_MST3		IRQ_EXT(147)
#define IRQ_I2C_MST4		IRQ_EXT(148)
#define IRQ_I2C_MST5		IRQ_EXT(149)
#define IRQ_I2C_MST6		IRQ_EXT(150)
#define IRQ_I2C_MST7		IRQ_EXT(151)

#define IRQ_I2C_SLV0		IRQ_EXT(152)
#define IRQ_I2C_SLV1		IRQ_EXT(153)
#define IRQ_I2C_SLV2		IRQ_EXT(154)
#define IRQ_I2C_SLV3		IRQ_EXT(155)

#define IRQ_SPI_MST0		IRQ_EXT(156)
#define IRQ_SPI_MST1		IRQ_EXT(157)
#define IRQ_SPI_MST2		IRQ_EXT(158)
#define IRQ_SPI_MST3		IRQ_EXT(159)

#define IRQ_SPI_SLV		IRQ_EXT(160)

#define IRQ_WDT0		IRQ_EXT(161)
#define IRQ_WDT1		IRQ_EXT(162)

#define IRQ_TIMER0_0		IRQ_EXT(163)
#define IRQ_TIMER0_1		IRQ_EXT(164)
#define IRQ_TIMER0_2		IRQ_EXT(165)
#define IRQ_TIMER0_3		IRQ_EXT(166)
#define IRQ_TIMER0_4		IRQ_EXT(167)
#define IRQ_TIMER0_5		IRQ_EXT(168)
#define IRQ_TIMER0_6		IRQ_EXT(169)
#define IRQ_TIMER0_7		IRQ_EXT(170)

#define IRQ_TIMER1_0		IRQ_EXT(171)
#define IRQ_TIMER1_1		IRQ_EXT(172)
#define IRQ_TIMER1_2		IRQ_EXT(173)
#define IRQ_TIMER1_3		IRQ_EXT(174)
#define IRQ_TIMER1_4		IRQ_EXT(175)
#define IRQ_TIMER1_5		IRQ_EXT(176)
#define IRQ_TIMER1_6		IRQ_EXT(177)
#define IRQ_TIMER1_7		IRQ_EXT(178)

#define IRQ_TIMER2_0		IRQ_EXT(179)
#define IRQ_TIMER2_1		IRQ_EXT(180)
#define IRQ_TIMER2_2		IRQ_EXT(181)
#define IRQ_TIMER2_3		IRQ_EXT(182)
#define IRQ_TIMER2_4		IRQ_EXT(183)
#define IRQ_TIMER2_5		IRQ_EXT(184)
#define IRQ_TIMER2_6		IRQ_EXT(185)
#define IRQ_TIMER2_7		IRQ_EXT(186)

#define IRQ_TIMER3_0		IRQ_EXT(187)
#define IRQ_TIMER3_1		IRQ_EXT(188)
#define IRQ_TIMER3_2		IRQ_EXT(189)
#define IRQ_TIMER3_3		IRQ_EXT(190)
#define IRQ_TIMER3_4		IRQ_EXT(191)
#define IRQ_TIMER3_5		IRQ_EXT(192)
#define IRQ_TIMER3_6		IRQ_EXT(193)
#define IRQ_TIMER3_7		IRQ_EXT(194)
#define IRQ_TIMER3_8		IRQ_EXT(195)
#define IRQ_TIMER3_9		IRQ_EXT(196)
#define IRQ_TIMER3_10		IRQ_EXT(197)
#define IRQ_TIMER3_11		IRQ_EXT(198)
#define IRQ_TIMER3_12		IRQ_EXT(199)
#define IRQ_TIMER3_13		IRQ_EXT(200)
#define IRQ_TIMER3_14		IRQ_EXT(201)
#define IRQ_TIMER3_15		IRQ_EXT(202)
#define IRQ_TIMER3_16		IRQ_EXT(203)
#define IRQ_TIMER3_17		IRQ_EXT(204)
#define IRQ_TIMER3_18		IRQ_EXT(205)
#define IRQ_TIMER3_19		IRQ_EXT(206)
#define IRQ_TIMER3_20		IRQ_EXT(207)
#define IRQ_TIMER3_21		IRQ_EXT(208)
#define IRQ_TIMER3_22		IRQ_EXT(209)
#define IRQ_TIMER3_23		IRQ_EXT(210)
#define IRQ_TIMER3_24		IRQ_EXT(211)
#define IRQ_TIMER3_25		IRQ_EXT(212)
#define IRQ_TIMER3_26		IRQ_EXT(213)
#define IRQ_TIMER3_27		IRQ_EXT(214)
#define IRQ_TIMER3_28		IRQ_EXT(215)
#define IRQ_TIMER3_29		IRQ_EXT(216)
#define IRQ_TIMER3_30		IRQ_EXT(217)
#define IRQ_TIMER3_31		IRQ_EXT(218)

#define IRQ_TSENS0		IRQ_EXT(219)
#define IRQ_TSENS1		IRQ_EXT(220)
#define IRQ_TSENS2		IRQ_EXT(221)
#define IRQ_TSENS3		IRQ_EXT(222)

#define NR_EXT_IRQS		256
#define NR_IRQS			(NR_INT_IRQS + NR_EXT_IRQS)

#endif  /* __IRQ_DUOWEN_H_INCLUDE__ */
