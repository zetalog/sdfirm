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

#define IRQ_CFAB_ECC_CE		1 /* Correctable error */
#define IRQ_CFAB_ECC_UE		2 /* Uncorrectable error */

#define IRQ_PCIE_X16_INTA	3
#define IRQ_PCIE_X16_INTB	4
#define IRQ_PCIE_X16_INTC	5
#define IRQ_PCIE_X16_INTD	6
#define IRQ_PCIE_X16_MSI	8
#define IRQ_PCIE_X16_HP		10 /* Hot-plug */
#define IRQ_PCIE_X16_LD		11 /* Link down */
#define IRQ_PCIE_X16_BF		12 /* Bridge flush */
#define IRQ_PCIE_X16_LDMA	13 /* Local DMA */

#define IRQ_PCIE_X8_INTA	14
#define IRQ_PCIE_X8_INTB	15
#define IRQ_PCIE_X8_INTC	16
#define IRQ_PCIE_X8_INTD	17
#define IRQ_PCIE_X8_MSI		19
#define IRQ_PCIE_X8_HP		21
#define IRQ_PCIE_X8_LD		22
#define IRQ_PCIE_X8_BF		23
#define IRQ_PCIE_X8_LDMA	24

#define IRQ_PCIE_X4_0_INTA	25
#define IRQ_PCIE_X4_0_INTB	26
#define IRQ_PCIE_X4_0_INTC	27
#define IRQ_PCIE_X4_0_INTD	28
#define IRQ_PCIE_X4_0_MSI	30
#define IRQ_PCIE_X4_0_HP	32
#define IRQ_PCIE_X4_0_LD	33
#define IRQ_PCIE_X4_0_BF	34
#define IRQ_PCIE_X4_0_LDMA	35

#define IRQ_PCIE_X4_1_INTA	36
#define IRQ_PCIE_X4_1_INTB	37
#define IRQ_PCIE_X4_1_INTC	38
#define IRQ_PCIE_X4_1_INTD	39
#define IRQ_PCIE_X4_1_MSI	41
#define IRQ_PCIE_X4_1_HP	43
#define IRQ_PCIE_X4_1_LD	44
#define IRQ_PCIE_X4_1_BF	45
#define IRQ_PCIE_X4_1_LDMA	46

#define IRQ_DDR0_ECC_CE		47 /* Correctable error */
#define IRQ_DDR0_ECC_CE_FAULT	48
#define IRQ_DDR0_ECC_UE		49 /* Uncorrectable error */
#define IRQ_DDR0_ECC_UE_FAULT	50
#define IRQ_DDR0_DFI_ALERT_ERR	51
#define IRQ_DDR0_SBR_DONE	52
#define IRQ_AWPOISON_0		53
#define IRQ_ARPOISON_0		54

#define IRQ_DDR1_ECC_CE		55
#define IRQ_DDR1_ECC_CE_FAULT	56
#define IRQ_DDR1_ECC_UE		57
#define IRQ_DDR1_ECC_UE_FAULT	58
#define IRQ_DDR1_DFI_ALERT_ERR	59
#define IRQ_DDR1_SBR_DONE	60

#define IRQ_DMAC		63
#define IRQ_DMAC_CMNREG		64
#define IRQ_DMAC_CH0		65
#define IRQ_DMAC_CH1		66
#define IRQ_DMAC_CH2		67
#define IRQ_DMAC_CH3		68
#define IRQ_DMAC_CH4		69
#define IRQ_DMAC_CH5		70
#define IRQ_DMAC_CH6		71
#define IRQ_DMAC_CH7		72

#define IRQ_PCIE_SMMU_GBL_FLT_NS	73
#define IRQ_PCIE_SMMU_PERF_TBU0		74
#define IRQ_PCIE_SMMU_PERF_TBU1		75
#define IRQ_PCIE_SMMU_PERF_TBU2		76
#define IRQ_PCIE_SMMU_PERF_TBU3		77
#define IRQ_PCIE_SMMU_COMB_NS		78

#define IRQ_DMA_SMMU_GBL_FLT_NS		79
#define IRQ_DMA_SMMU_PERF_TBU_DMA	80
#define IRQ_DMA_SMMU_COMB_NS		81

#define IRQ_UART0		82
#define IRQ_UART1		83
#define IRQ_UART2		84
#define IRQ_UART3		85

#define IRQ_GPIO0_0		86
#define IRQ_GPIO0_1		87
#define IRQ_GPIO0_2		88
#define IRQ_GPIO0_3		89
#define IRQ_GPIO0_4		90
#define IRQ_GPIO0_5		91
#define IRQ_GPIO0_6		92
#define IRQ_GPIO0_7		93

#define IRQ_GPIO1_0		94
#define IRQ_GPIO1_1		95
#define IRQ_GPIO1_2		96
#define IRQ_GPIO1_3		97
#define IRQ_GPIO1_4		98
#define IRQ_GPIO1_5		99
#define IRQ_GPIO1_6		100
#define IRQ_GPIO1_7		101
#define IRQ_GPIO1_8		102
#define IRQ_GPIO1_9		103
#define IRQ_GPIO1_10		104
#define IRQ_GPIO1_11		105
#define IRQ_GPIO1_12		106
#define IRQ_GPIO1_13		107
#define IRQ_GPIO1_14		108
#define IRQ_GPIO1_15		109

#define IRQ_GPIO2_0		110
#define IRQ_GPIO2_1		111
#define IRQ_GPIO2_2		112
#define IRQ_GPIO2_3		113
#define IRQ_GPIO2_4		114
#define IRQ_GPIO2_5		115
#define IRQ_GPIO2_6		116
#define IRQ_GPIO2_7		117
#define IRQ_GPIO2_8		118
#define IRQ_GPIO2_9		119
#define IRQ_GPIO2_10		120
#define IRQ_GPIO2_11		121
#define IRQ_GPIO2_12		122
#define IRQ_GPIO2_13		123
#define IRQ_GPIO2_14		124
#define IRQ_GPIO2_15		125
#define IRQ_GPIO2_16		126
#define IRQ_GPIO2_17		127
#define IRQ_GPIO2_18		128
#define IRQ_GPIO2_19		129
#define IRQ_GPIO2_20		130
#define IRQ_GPIO2_21		131
#define IRQ_GPIO2_22		132
#define IRQ_GPIO2_23		133
#define IRQ_GPIO2_24		134
#define IRQ_GPIO2_25		135
#define IRQ_GPIO2_26		136
#define IRQ_GPIO2_27		137
#define IRQ_GPIO2_28		138
#define IRQ_GPIO2_29		139
#define IRQ_GPIO2_30		140
#define IRQ_GPIO2_31		141

#define IRQ_SD			142
#define IRQ_SD_WAKEUP		143

#define IRQ_I2C_MST0		144
#define IRQ_I2C_MST1		145
#define IRQ_I2C_MST2		146
#define IRQ_I2C_MST3		147
#define IRQ_I2C_MST4		148
#define IRQ_I2C_MST5		149
#define IRQ_I2C_MST6		150
#define IRQ_I2C_MST7		151

#define IRQ_I2C_SLV0		152
#define IRQ_I2C_SLV1		153
#define IRQ_I2C_SLV2		154
#define IRQ_I2C_SLV3		155

#define IRQ_SPI_MST0		156
#define IRQ_SPI_MST1		157
#define IRQ_SPI_MST2		158
#define IRQ_SPI_MST3		159

#define IRQ_SPI_SLV		160

#define IRQ_WDT0		161
#define IRQ_WDT1		162

#define IRQ_TIMER0_0		163
#define IRQ_TIMER0_1		164
#define IRQ_TIMER0_2		165
#define IRQ_TIMER0_3		166
#define IRQ_TIMER0_4		167
#define IRQ_TIMER0_5		168
#define IRQ_TIMER0_6		169
#define IRQ_TIMER0_7		170

#define IRQ_TIMER1_0		171
#define IRQ_TIMER1_1		172
#define IRQ_TIMER1_2		173
#define IRQ_TIMER1_3		174
#define IRQ_TIMER1_4		175
#define IRQ_TIMER1_5		176
#define IRQ_TIMER1_6		177
#define IRQ_TIMER1_7		178

#define IRQ_TIMER2_0		179
#define IRQ_TIMER2_1		180
#define IRQ_TIMER2_2		181
#define IRQ_TIMER2_3		182
#define IRQ_TIMER2_4		183
#define IRQ_TIMER2_5		184
#define IRQ_TIMER2_6		185
#define IRQ_TIMER2_7		186

#define IRQ_TIMER3_0		187
#define IRQ_TIMER3_1		188
#define IRQ_TIMER3_2		189
#define IRQ_TIMER3_3		190
#define IRQ_TIMER3_4		191
#define IRQ_TIMER3_5		192
#define IRQ_TIMER3_6		193
#define IRQ_TIMER3_7		194
#define IRQ_TIMER3_8		195
#define IRQ_TIMER3_9		196
#define IRQ_TIMER3_10		197
#define IRQ_TIMER3_11		198
#define IRQ_TIMER3_12		199
#define IRQ_TIMER3_13		200
#define IRQ_TIMER3_14		201
#define IRQ_TIMER3_15		202
#define IRQ_TIMER3_16		203
#define IRQ_TIMER3_17		204
#define IRQ_TIMER3_18		205
#define IRQ_TIMER3_19		206
#define IRQ_TIMER3_20		207
#define IRQ_TIMER3_21		208
#define IRQ_TIMER3_22		209
#define IRQ_TIMER3_23		210
#define IRQ_TIMER3_24		211
#define IRQ_TIMER3_25		212
#define IRQ_TIMER3_26		213
#define IRQ_TIMER3_27		214
#define IRQ_TIMER3_28		215
#define IRQ_TIMER3_29		216
#define IRQ_TIMER3_30		217
#define IRQ_TIMER3_31		218

#define IRQ_TSENS0		219
#define IRQ_TSENS1		220
#define IRQ_TSENS2		221
#define IRQ_TSENS3		222

#ifdef CONFIG_DUOWEN_IMC
#define NR_IRQS		64
#endif /* CONFIG_DUOWEN_IMC */
#ifdef CONFIG_DUOWEN_APC
#define NR_IRQS		64
#endif /* CONFIG_DUOWEN_APC */

#endif  /* __IRQ_DUOWEN_H_INCLUDE__ */
