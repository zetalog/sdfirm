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

#ifdef ARCH_HAVE_INT_IRQS
#define NR_INT_IRQS		20
#endif

#define IRQ_CFAB_ECC_CE		EXT_IRQ(1) /* Correctable error */
#define IRQ_CFAB_ECC_UE		EXT_IRQ(2) /* Uncorrectable error */

#define IRQ_PCIE_X16_INTA	EXT_IRQ(3)
#define IRQ_PCIE_X16_INTB	EXT_IRQ(4)
#define IRQ_PCIE_X16_INTC	EXT_IRQ(5)
#define IRQ_PCIE_X16_INTD	EXT_IRQ(6)
#define IRQ_PCIE_X16_AER	EXT_IRQ(7)
#define IRQ_PCIE_X16_MSI	EXT_IRQ(8)
#define IRQ_PCIE_X16_PME	EXT_IRQ(9)
#define IRQ_PCIE_X16_HP		EXT_IRQ(10) /* Hot-plug */
#define IRQ_PCIE_X16_LD		EXT_IRQ(11) /* Link down */
#define IRQ_PCIE_X16_BF		EXT_IRQ(12) /* Bridge flush */
#define IRQ_PCIE_X16_LDMA	EXT_IRQ(13) /* Local DMA */

#define IRQ_PCIE_X8_INTA	EXT_IRQ(14)
#define IRQ_PCIE_X8_INTB	EXT_IRQ(15)
#define IRQ_PCIE_X8_INTC	EXT_IRQ(16)
#define IRQ_PCIE_X8_INTD	EXT_IRQ(17)
#define IRQ_PCIE_X8_AER		EXT_IRQ(18)
#define IRQ_PCIE_X8_MSI		EXT_IRQ(19)
#define IRQ_PCIE_X8_PME		EXT_IRQ(20)
#define IRQ_PCIE_X8_HP		EXT_IRQ(21)
#define IRQ_PCIE_X8_LD		EXT_IRQ(22)
#define IRQ_PCIE_X8_BF		EXT_IRQ(23)
#define IRQ_PCIE_X8_LDMA	EXT_IRQ(24)

#define IRQ_PCIE_X4_0_INTA	EXT_IRQ(25)
#define IRQ_PCIE_X4_0_INTB	EXT_IRQ(26)
#define IRQ_PCIE_X4_0_INTC	EXT_IRQ(27)
#define IRQ_PCIE_X4_0_INTD	EXT_IRQ(28)
#define IRQ_PCIE_X4_0_AER	EXT_IRQ(29)
#define IRQ_PCIE_X4_0_MSI	EXT_IRQ(30)
#define IRQ_PCIE_X4_0_PME	EXT_IRQ(31)
#define IRQ_PCIE_X4_0_HP	EXT_IRQ(32)
#define IRQ_PCIE_X4_0_LD	EXT_IRQ(33)
#define IRQ_PCIE_X4_0_BF	EXT_IRQ(34)
#define IRQ_PCIE_X4_0_LDMA	EXT_IRQ(35)

#define IRQ_PCIE_X4_1_INTA	EXT_IRQ(36)
#define IRQ_PCIE_X4_1_INTB	EXT_IRQ(37)
#define IRQ_PCIE_X4_1_INTC	EXT_IRQ(38)
#define IRQ_PCIE_X4_1_INTD	EXT_IRQ(39)
#define IRQ_PCIE_X4_1_AER	EXT_IRQ(40)
#define IRQ_PCIE_X4_1_MSI	EXT_IRQ(41)
#define IRQ_PCIE_X4_1_PME	EXT_IRQ(42)
#define IRQ_PCIE_X4_1_HP	EXT_IRQ(43)
#define IRQ_PCIE_X4_1_LD	EXT_IRQ(44)
#define IRQ_PCIE_X4_1_BF	EXT_IRQ(45)
#define IRQ_PCIE_X4_1_LDMA	EXT_IRQ(46)

#define IRQ_DDR0_ECC_CE		EXT_IRQ(47) /* Correctable error */
#define IRQ_DDR0_ECC_CE_FAULT	EXT_IRQ(48)
#define IRQ_DDR0_ECC_UE		EXT_IRQ(49) /* Uncorrectable error */
#define IRQ_DDR0_ECC_UE_FAULT	EXT_IRQ(50)
#define IRQ_DDR0_DFI_ALERT_ERR	EXT_IRQ(51)
#define IRQ_DDR0_SBR_DONE	EXT_IRQ(52)
#define IRQ_DDR0_AWPOISON_0	EXT_IRQ(53)
#define IRQ_DDR0_ARPOISON_0	EXT_IRQ(54)
#define IRQ_DDRP0		EXT_IRQ(55)

#define IRQ_DDR1_ECC_CE		EXT_IRQ(56)
#define IRQ_DDR1_ECC_CE_FAULT	EXT_IRQ(57)
#define IRQ_DDR1_ECC_UE		EXT_IRQ(58)
#define IRQ_DDR1_ECC_UE_FAULT	EXT_IRQ(59)
#define IRQ_DDR1_DFI_ALERT_ERR	EXT_IRQ(60)
#define IRQ_DDR1_SBR_DONE	EXT_IRQ(61)
#define IRQ_DDR1_AWPOISON_0	EXT_IRQ(62)
#define IRQ_DDR1_ARPOISON_0	EXT_IRQ(63)
#define IRQ_DDRP1		EXT_IRQ(64)

#define IRQ_DMAC		EXT_IRQ(65)
#define IRQ_DMAC_CMNREG		EXT_IRQ(66)
#define IRQ_DMAC_CH0		EXT_IRQ(67)
#define IRQ_DMAC_CH1		EXT_IRQ(68)
#define IRQ_DMAC_CH2		EXT_IRQ(69)
#define IRQ_DMAC_CH3		EXT_IRQ(70)
#define IRQ_DMAC_CH4		EXT_IRQ(71)
#define IRQ_DMAC_CH5		EXT_IRQ(72)
#define IRQ_DMAC_CH6		EXT_IRQ(73)
#define IRQ_DMAC_CH7		EXT_IRQ(74)

#define IRQ_PCIE_SMMU_GBL_FLT_NS	EXT_IRQ(75)
#define IRQ_PCIE_SMMU_PERF_TBU0		EXT_IRQ(76)
#define IRQ_PCIE_SMMU_PERF_TBU1		EXT_IRQ(77)
#define IRQ_PCIE_SMMU_PERF_TBU2		EXT_IRQ(78)
#define IRQ_PCIE_SMMU_PERF_TBU3		EXT_IRQ(79)
#define IRQ_PCIE_SMMU_COMB_NS		EXT_IRQ(80) /* combined */

#define IRQ_DMA_SMMU_GBL_FLT_NS		EXT_IRQ(81)
#define IRQ_DMA_SMMU_PERF_TBU		EXT_IRQ(82)
#define IRQ_DMA_SMMU_COMB_NS		EXT_IRQ(83) /* combined */

#define IRQ_UART0		EXT_IRQ(84)
#define IRQ_UART1		EXT_IRQ(85)
#define IRQ_UART2		EXT_IRQ(86)
#define IRQ_UART3		EXT_IRQ(87)

#define IRQ_GPIO0_0		EXT_IRQ(88)
#define IRQ_GPIO0_1		EXT_IRQ(89)
#define IRQ_GPIO0_2		EXT_IRQ(90)
#define IRQ_GPIO0_3		EXT_IRQ(91)
#define IRQ_GPIO0_4		EXT_IRQ(92)
#define IRQ_GPIO0_5		EXT_IRQ(93)
#define IRQ_GPIO0_6		EXT_IRQ(94)
#define IRQ_GPIO0_7		EXT_IRQ(95)

#define IRQ_GPIO1_0		EXT_IRQ(96)
#define IRQ_GPIO1_1		EXT_IRQ(97)
#define IRQ_GPIO1_2		EXT_IRQ(98)
#define IRQ_GPIO1_3		EXT_IRQ(99)
#define IRQ_GPIO1_4		EXT_IRQ(100)
#define IRQ_GPIO1_5		EXT_IRQ(101)
#define IRQ_GPIO1_6		EXT_IRQ(102)
#define IRQ_GPIO1_7		EXT_IRQ(103)
#define IRQ_GPIO1_8		EXT_IRQ(104)
#define IRQ_GPIO1_9		EXT_IRQ(105)
#define IRQ_GPIO1_10		EXT_IRQ(106)
#define IRQ_GPIO1_11		EXT_IRQ(107)
#define IRQ_GPIO1_12		EXT_IRQ(108)
#define IRQ_GPIO1_13		EXT_IRQ(109)
#define IRQ_GPIO1_14		EXT_IRQ(110)
#define IRQ_GPIO1_15		EXT_IRQ(111)

#define IRQ_GPIO2_0		EXT_IRQ(112)
#define IRQ_GPIO2_1		EXT_IRQ(113)
#define IRQ_GPIO2_2		EXT_IRQ(114)
#define IRQ_GPIO2_3		EXT_IRQ(115)
#define IRQ_GPIO2_4		EXT_IRQ(116)
#define IRQ_GPIO2_5		EXT_IRQ(117)
#define IRQ_GPIO2_6		EXT_IRQ(118)
#define IRQ_GPIO2_7		EXT_IRQ(119)
#define IRQ_GPIO2_8		EXT_IRQ(120)
#define IRQ_GPIO2_9		EXT_IRQ(121)
#define IRQ_GPIO2_10		EXT_IRQ(122)
#define IRQ_GPIO2_11		EXT_IRQ(123)
#define IRQ_GPIO2_12		EXT_IRQ(124)
#define IRQ_GPIO2_13		EXT_IRQ(125)
#define IRQ_GPIO2_14		EXT_IRQ(126)
#define IRQ_GPIO2_15		EXT_IRQ(127)
#define IRQ_GPIO2_16		EXT_IRQ(128)
#define IRQ_GPIO2_17		EXT_IRQ(129)
#define IRQ_GPIO2_18		EXT_IRQ(130)
#define IRQ_GPIO2_19		EXT_IRQ(131)
#define IRQ_GPIO2_20		EXT_IRQ(132)
#define IRQ_GPIO2_21		EXT_IRQ(133)
#define IRQ_GPIO2_22		EXT_IRQ(134)
#define IRQ_GPIO2_23		EXT_IRQ(135)
#define IRQ_GPIO2_24		EXT_IRQ(136)
#define IRQ_GPIO2_25		EXT_IRQ(137)
#define IRQ_GPIO2_26		EXT_IRQ(138)
#define IRQ_GPIO2_27		EXT_IRQ(139)
#define IRQ_GPIO2_28		EXT_IRQ(140)
#define IRQ_GPIO2_29		EXT_IRQ(141)
#define IRQ_GPIO2_30		EXT_IRQ(142)
#define IRQ_GPIO2_31		EXT_IRQ(143)

#define IRQ_SD			EXT_IRQ(144)
#define IRQ_SD_WAKEUP		EXT_IRQ(145)

#define IRQ_I2C_MST0		EXT_IRQ(146)
#define IRQ_I2C_MST1		EXT_IRQ(147)
#define IRQ_I2C_MST2		EXT_IRQ(148)
#define IRQ_I2C_MST3		EXT_IRQ(149)
#define IRQ_I2C_MST4		EXT_IRQ(150)
#define IRQ_I2C_MST5		EXT_IRQ(151)
#define IRQ_I2C_MST6		EXT_IRQ(152)
#define IRQ_I2C_MST7		EXT_IRQ(153)

#define IRQ_I2C_SLV0		EXT_IRQ(154)
#define IRQ_I2C_SLV1		EXT_IRQ(155)
#define IRQ_I2C_SLV2		EXT_IRQ(156)
#define IRQ_I2C_SLV3		EXT_IRQ(157)

#define IRQ_SPI_MST0		EXT_IRQ(158)
#define IRQ_SPI_MST1		EXT_IRQ(159)
#define IRQ_SPI_MST2		EXT_IRQ(160)
#define IRQ_SPI_MST3		EXT_IRQ(161)

#define IRQ_SPI_SLV		EXT_IRQ(162)

#define IRQ_WDT0		EXT_IRQ(163)
#define IRQ_WDT1		EXT_IRQ(164)

#define IRQ_TIMER0_0		EXT_IRQ(165)
#define IRQ_TIMER0_1		EXT_IRQ(166)
#define IRQ_TIMER0_2		EXT_IRQ(167)
#define IRQ_TIMER0_3		EXT_IRQ(168)
#define IRQ_TIMER0_4		EXT_IRQ(169)
#define IRQ_TIMER0_5		EXT_IRQ(170)
#define IRQ_TIMER0_6		EXT_IRQ(171)
#define IRQ_TIMER0_7		EXT_IRQ(172)

#define IRQ_TIMER1_0		EXT_IRQ(173)
#define IRQ_TIMER1_1		EXT_IRQ(174)
#define IRQ_TIMER1_2		EXT_IRQ(175)
#define IRQ_TIMER1_3		EXT_IRQ(176)
#define IRQ_TIMER1_4		EXT_IRQ(177)
#define IRQ_TIMER1_5		EXT_IRQ(178)
#define IRQ_TIMER1_6		EXT_IRQ(179)
#define IRQ_TIMER1_7		EXT_IRQ(180)

#define IRQ_TIMER2_0		EXT_IRQ(181)
#define IRQ_TIMER2_1		EXT_IRQ(182)
#define IRQ_TIMER2_2		EXT_IRQ(183)
#define IRQ_TIMER2_3		EXT_IRQ(184)
#define IRQ_TIMER2_4		EXT_IRQ(185)
#define IRQ_TIMER2_5		EXT_IRQ(186)
#define IRQ_TIMER2_6		EXT_IRQ(187)
#define IRQ_TIMER2_7		EXT_IRQ(188)

#define IRQ_WDT0_0		EXT_IRQ(189)
#define IRQ_WDT0_1		EXT_IRQ(190)
#define IRQ_WDT1_0		EXT_IRQ(191)
#define IRQ_WDT1_1		EXT_IRQ(192)

#define IRQ_XGMAC_LPI		EXT_IRQ(194) /* XGMAC low power */
#define IRQ_XGMAC_PMT		EXT_IRQ(195) /* XGMAC power management */
#define IRQ_XGMAC_SBD		EXT_IRQ(196) /* XGMAC sideband */
#define IRQ_XPCS_SBD		EXT_IRQ(197) /* XGMAC physical coding sublayer */
#define IRQ_SBD_RX(ch)		EXT_IRQ(198 + (ch)) /* channel sideband RX */
#define IRQ_SBD_TX(ch)		EXT_IRQ(206 + (ch)) /* channel sideband TX */

#define IRQ_DDR0_AWPOISON_1	EXT_IRQ(214)
#define IRQ_DDR0_ARPOISON_1	EXT_IRQ(215)
#define IRQ_DDR1_AWPOISON_1	EXT_IRQ(216)
#define IRQ_DDR1_ARPOISON_1	EXT_IRQ(217)

#define IRQ_TSENS0		EXT_IRQ(220)
#define IRQ_TSENS1		EXT_IRQ(221)
#define IRQ_TSENS2		EXT_IRQ(222)
#define IRQ_TSENS3		EXT_IRQ(223)

#define IRQ_FLASH		EXT_IRQ(224)

#define NR_EXT_IRQS		256
#define NR_IRQS			(NR_INT_IRQS + NR_EXT_IRQS)

#endif  /* __IRQ_DUOWEN_H_INCLUDE__ */
