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
 * @(#)irq.h: DPU specific IRQ assignments
 * $Id: irq.h,v 1.1 2020-03-02 16:58:00 zhenglv Exp $
 */

#ifndef __IRQ_DPU_H_INCLUDE__
#define __IRQ_DPU_H_INCLUDE__

#define IRQ_PCIE_X16_INTA	EXT_IRQ(3)
#define IRQ_PCIE_X16_INTB	EXT_IRQ(4)
#define IRQ_PCIE_X16_INTC	EXT_IRQ(5)
#define IRQ_PCIE_X16_INTD	EXT_IRQ(6)
#define IRQ_PCIE_X16_MSI	EXT_IRQ(8)
#define IRQ_PCIE_X16_HP		EXT_IRQ(10) /* Hot-plug */
#define IRQ_PCIE_X16_LD		EXT_IRQ(11) /* Link down */
#define IRQ_PCIE_X16_BF		EXT_IRQ(12) /* Bridge flush */
#define IRQ_PCIE_X16_LDMA	EXT_IRQ(13) /* Local DMA */

#define IRQ_PCIE_X8_INTA	EXT_IRQ(14)
#define IRQ_PCIE_X8_INTB	EXT_IRQ(15)
#define IRQ_PCIE_X8_INTC	EXT_IRQ(16)
#define IRQ_PCIE_X8_INTD	EXT_IRQ(17)
#define IRQ_PCIE_X8_MSI		EXT_IRQ(19)
#define IRQ_PCIE_X8_HP		EXT_IRQ(21)
#define IRQ_PCIE_X8_LD		EXT_IRQ(22)
#define IRQ_PCIE_X8_BF		EXT_IRQ(23)
#define IRQ_PCIE_X8_LDMA	EXT_IRQ(24)

#define IRQ_PCIE_X4_0_INTA	EXT_IRQ(25)
#define IRQ_PCIE_X4_0_INTB	EXT_IRQ(26)
#define IRQ_PCIE_X4_0_INTC	EXT_IRQ(27)
#define IRQ_PCIE_X4_0_INTD	EXT_IRQ(28)
#define IRQ_PCIE_X4_0_MSI	EXT_IRQ(30)
#define IRQ_PCIE_X4_0_HP	EXT_IRQ(32)
#define IRQ_PCIE_X4_0_LD	EXT_IRQ(33)
#define IRQ_PCIE_X4_0_BF	EXT_IRQ(34)
#define IRQ_PCIE_X4_0_LDMA	EXT_IRQ(35)

#define IRQ_PCIE_X4_1_INTA	EXT_IRQ(36)
#define IRQ_PCIE_X4_1_INTB	EXT_IRQ(37)
#define IRQ_PCIE_X4_1_INTC	EXT_IRQ(38)
#define IRQ_PCIE_X4_1_INTD	EXT_IRQ(39)
#define IRQ_PCIE_X4_1_MSI	EXT_IRQ(41)
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
#define IRQ_AWPOISON_0		EXT_IRQ(53)
#define IRQ_ARPOISON_0		EXT_IRQ(54)
#define IRQ_DDR1_ECC_CE		EXT_IRQ(55)
#define IRQ_DDR1_ECC_CE_FAULT	EXT_IRQ(56)
#define IRQ_DDR1_ECC_UE		EXT_IRQ(57)
#define IRQ_DDR1_ECC_UE_FAULT	EXT_IRQ(58)
#define IRQ_DDR1_DFI_ALERT_ERR	EXT_IRQ(59)
#define IRQ_DDR1_SBR_DONE	EXT_IRQ(60)
#define IRQ_PE16_DMAC0		EXT_IRQ(63)
#define IRQ_PE16_DMAC1		EXT_IRQ(64)
#define IRQ_PE16_MICC0		EXT_IRQ(65)
#define IRQ_PE16_MICC1		EXT_IRQ(66)
#define IRQ_PE16_DEBUG0		EXT_IRQ(67)
#define IRQ_APB_PIRQ		EXT_IRQ(68)
#define IRQ_UART		EXT_IRQ(69)
#define IRQ_GPIO0		EXT_IRQ(70)
#define IRQ_GPIO1		EXT_IRQ(71)
#define IRQ_GPIO2		EXT_IRQ(72)
#define IRQ_GPIO3		EXT_IRQ(73)
#define IRQ_GPIO4		EXT_IRQ(74)
#define IRQ_GPIO5		EXT_IRQ(75)
#define IRQ_GPIO6		EXT_IRQ(76)
#define IRQ_GPIO7		EXT_IRQ(77)
#define IRQ_I2C0		EXT_IRQ(78)
#define IRQ_I2C1		EXT_IRQ(79)
#define IRQ_I2C2		EXT_IRQ(80)
#define IRQ_SPI			EXT_IRQ(81)
#define IRQ_WDT			EXT_IRQ(82)
#define IRQ_TMR0		EXT_IRQ(83)
#define IRQ_TMR1		EXT_IRQ(84)
#define IRQ_TMR2		EXT_IRQ(85)
#define IRQ_TMR3		EXT_IRQ(86)
#define IRQ_TMR4		EXT_IRQ(87)
#define IRQ_TMR5		EXT_IRQ(88)
#define IRQ_TMR6		EXT_IRQ(89)
#define IRQ_TMR7		EXT_IRQ(90)
#define IRQ_TMR8		EXT_IRQ(91)
#define IRQ_TMR9		EXT_IRQ(92)
#define IRQ_TMR10		EXT_IRQ(93)
#define IRQ_TMR11		EXT_IRQ(94)
#define IRQ_TMR12		EXT_IRQ(95)
#define IRQ_TMR13		EXT_IRQ(96)
#define IRQ_TMR14		EXT_IRQ(97)
#define IRQ_TMR15		EXT_IRQ(98)
#define IRQ_TMR16		EXT_IRQ(99)
#define IRQ_TMR17		EXT_IRQ(100)
#define IRQ_TMR18		EXT_IRQ(101)
#define IRQ_TMR19		EXT_IRQ(102)
#define IRQ_TMR20		EXT_IRQ(103)
#define IRQ_TMR21		EXT_IRQ(104)
#define IRQ_TMR22		EXT_IRQ(105)
#define IRQ_TMR23		EXT_IRQ(106)
#define IRQ_TMR24		EXT_IRQ(107)
#define IRQ_TMR25		EXT_IRQ(108)
#define IRQ_TMR26		EXT_IRQ(109)
#define IRQ_TMR27		EXT_IRQ(110)
#define IRQ_TMR28		EXT_IRQ(111)
#define IRQ_TMR29		EXT_IRQ(112)
#define IRQ_TMR30		EXT_IRQ(113)
#define IRQ_TMR31		EXT_IRQ(114)
#define IRQ_TMR			EXT_IRQ(115)
#define	IRQ_FLASH		EXT_IRQ(116)

#define NR_EXT_IRQS		128
#define NR_IRQS			(NR_INT_IRQS + NR_EXT_IRQS)

#endif  /* __IRQ_DPU_H_INCLUDE__ */
