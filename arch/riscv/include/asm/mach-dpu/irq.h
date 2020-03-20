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
#define IRQ_PE16_DMAC0		IRQ_EXT(63)
#define IRQ_PE16_DMAC1		IRQ_EXT(64)
#define IRQ_PE16_MICC0		IRQ_EXT(65)
#define IRQ_PE16_MICC1		IRQ_EXT(66)
#define IRQ_PE16_DEBUG0		IRQ_EXT(67)
#define IRQ_APB_PIRQ		IRQ_EXT(68)
#define IRQ_UART		IRQ_EXT(69)
#define IRQ_GPIO0		IRQ_EXT(70)
#define IRQ_GPIO1		IRQ_EXT(71)
#define IRQ_GPIO2		IRQ_EXT(72)
#define IRQ_GPIO3		IRQ_EXT(73)
#define IRQ_GPIO4		IRQ_EXT(74)
#define IRQ_GPIO5		IRQ_EXT(75)
#define IRQ_GPIO6		IRQ_EXT(76)
#define IRQ_GPIO7		IRQ_EXT(77)
#define IRQ_I2C0		IRQ_EXT(78)
#define IRQ_I2C1		IRQ_EXT(79)
#define IRQ_I2C2		IRQ_EXT(80)
#define IRQ_SPI			IRQ_EXT(81)
#define IRQ_WDT			IRQ_EXT(82)
#define IRQ_TMR0		IRQ_EXT(83)
#define IRQ_TMR1		IRQ_EXT(84)
#define IRQ_TMR2		IRQ_EXT(85)
#define IRQ_TMR3		IRQ_EXT(86)
#define IRQ_TMR4		IRQ_EXT(87)
#define IRQ_TMR5		IRQ_EXT(88)
#define IRQ_TMR6		IRQ_EXT(89)
#define IRQ_TMR7		IRQ_EXT(90)
#define IRQ_TMR8		IRQ_EXT(91)
#define IRQ_TMR9		IRQ_EXT(92)
#define IRQ_TMR10		IRQ_EXT(93)
#define IRQ_TMR11		IRQ_EXT(94)
#define IRQ_TMR12		IRQ_EXT(95)
#define IRQ_TMR13		IRQ_EXT(96)
#define IRQ_TMR14		IRQ_EXT(97)
#define IRQ_TMR15		IRQ_EXT(98)
#define IRQ_TMR16		IRQ_EXT(99)
#define IRQ_TMR17		IRQ_EXT(100)
#define IRQ_TMR18		IRQ_EXT(101)
#define IRQ_TMR19		IRQ_EXT(102)
#define IRQ_TMR20		IRQ_EXT(103)
#define IRQ_TMR21		IRQ_EXT(104)
#define IRQ_TMR22		IRQ_EXT(105)
#define IRQ_TMR23		IRQ_EXT(106)
#define IRQ_TMR24		IRQ_EXT(107)
#define IRQ_TMR25		IRQ_EXT(108)
#define IRQ_TMR26		IRQ_EXT(109)
#define IRQ_TMR27		IRQ_EXT(110)
#define IRQ_TMR28		IRQ_EXT(111)
#define IRQ_TMR29		IRQ_EXT(112)
#define IRQ_TMR30		IRQ_EXT(113)
#define IRQ_TMR31		IRQ_EXT(114)
#define IRQ_TMR			IRQ_EXT(115)
#define	IRQ_FLASH		IRQ_EXT(116)

#define NR_EXT_IRQS		128
#define NR_IRQS			(NR_INT_IRQS + NR_EXT_IRQS)

#endif  /* __IRQ_DPU_H_INCLUDE__ */
