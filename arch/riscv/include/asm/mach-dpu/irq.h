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

#define IRQ_PCIE_X16_INTA	2
#define IRQ_PCIE_X16_INTB	3
#define IRQ_PCIE_X16_INTC	4
#define IRQ_PCIE_X16_INTD	5
#define IRQ_PCIE_X16_MSI	7
#define IRQ_PCIE_X16_HP		9  /* Hot-plug */
#define IRQ_PCIE_X16_LD		10 /* Link down */
#define IRQ_PCIE_X16_BF		11 /* Bridge flush */
#define IRQ_PCIE_X16_LDMA	12 /* Local DMA */

#define IRQ_PCIE_X8_INTA	13
#define IRQ_PCIE_X8_INTB	14
#define IRQ_PCIE_X8_INTC	15
#define IRQ_PCIE_X8_INTD	16
#define IRQ_PCIE_X8_MSI		18
#define IRQ_PCIE_X8_HP		20
#define IRQ_PCIE_X8_LD		21
#define IRQ_PCIE_X8_BF		22
#define IRQ_PCIE_X8_LDMA	23

#define IRQ_PCIE_X4_0_INTA	24
#define IRQ_PCIE_X4_0_INTB	25
#define IRQ_PCIE_X4_0_INTC	26
#define IRQ_PCIE_X4_0_INTD	27
#define IRQ_PCIE_X4_0_MSI	29
#define IRQ_PCIE_X4_0_HP	31
#define IRQ_PCIE_X4_0_LD	32
#define IRQ_PCIE_X4_0_BF	33
#define IRQ_PCIE_X4_0_LDMA	34

#define IRQ_PCIE_X4_1_INTA	35
#define IRQ_PCIE_X4_1_INTB	36
#define IRQ_PCIE_X4_1_INTC	37
#define IRQ_PCIE_X4_1_INTD	38
#define IRQ_PCIE_X4_1_MSI	40
#define IRQ_PCIE_X4_1_HP	42
#define IRQ_PCIE_X4_1_LD	43
#define IRQ_PCIE_X4_1_BF	44
#define IRQ_PCIE_X4_1_LDMA	45
#define IRQ_DDR0_ECC_CE		46 /* Correctable error */
#define IRQ_DDR0_ECC_CE_FAULT	47
#define IRQ_DDR0_ECC_UE		48 /* Uncorrectable error */
#define IRQ_DDR0_ECC_UE_FAULT	49
#define IRQ_DDR0_DFI_ALERT_ERR	50
#define IRQ_DDR0_SBR_DONE	51
#define IRQ_AWPOISON_0		52
#define IRQ_ARPOISON_0		53
#define IRQ_DDR1_ECC_CE		54
#define IRQ_DDR1_ECC_CE_FAULT	55
#define IRQ_DDR1_ECC_UE		56
#define IRQ_DDR1_ECC_UE_FAULT	57
#define IRQ_DDR1_DFI_ALERT_ERR	58
#define IRQ_DDR1_SBR_DONE	59
#define IRQ_PE16_DMAC0		62
#define IRQ_PE16_DMAC1		63
#define IRQ_PE16_MICC0		64
#define IRQ_PE16_MICC1		65
#define IRQ_PE16_DEBUG0		66
#define IRQ_APB_PIRQ		67
#define IRQ_UART		68
#define IRQ_GPIO0		69
#define IRQ_GPIO1		70
#define IRQ_GPIO2		71
#define IRQ_GPIO3		72
#define IRQ_GPIO4		73
#define IRQ_GPIO5		74
#define IRQ_GPIO6		75
#define IRQ_GPIO7		76
#define IRQ_I2C0		77
#define IRQ_I2C1		78
#define IRQ_I2C2		79
#define IRQ_SPI			80
#define IRQ_WDT			81
#define IRQ_TMR0		82
#define IRQ_TMR1		83
#define IRQ_TMR2		84
#define IRQ_TMR3		85
#define IRQ_TMR4		86
#define IRQ_TMR5		87
#define IRQ_TMR6		88
#define IRQ_TMR7		89
#define IRQ_TMR8		90
#define IRQ_TMR9		91
#define IRQ_TMR10		92
#define IRQ_TMR11		93
#define IRQ_TMR12		94
#define IRQ_TMR13		95
#define IRQ_TMR14		96
#define IRQ_TMR15		97
#define IRQ_TMR16		98
#define IRQ_TMR17		99
#define IRQ_TMR18		100
#define IRQ_TMR19		101
#define IRQ_TMR20		102
#define IRQ_TMR21		103
#define IRQ_TMR22		104
#define IRQ_TMR23		105
#define IRQ_TMR24		106
#define IRQ_TMR25		107
#define IRQ_TMR26		108
#define IRQ_TMR27		109
#define IRQ_TMR28		110
#define IRQ_TMR29		111
#define IRQ_TMR30		112
#define IRQ_TMR31		113
#define IRQ_TMR			114
#define	IRQ_FLASH		115

#define NR_IRQS			128

#endif  /* __IRQ_DPU_H_INCLUDE__ */
