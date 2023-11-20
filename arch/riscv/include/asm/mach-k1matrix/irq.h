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

#define IRQ_TIMER0		EXT_IRQ(18)
#define IRQ_TIMER1		EXT_IRQ(19)
#define IRQ_UART0		EXT_IRQ(20)
#define IRQ_UART1		EXT_IRQ(21)
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

#define IRQ_SMMU_EVTQ		EXT_IRQ(48)
#define IRQ_SMMU_PRIQ		EXT_IRQ(49)
#define IRQ_SMMU_CMDQ		EXT_IRQ(50)
#define IRQ_SMMU_GERR		EXT_IRQ(51)

#define NR_EXT_IRQS		64
#define NR_IRQS			(NR_INT_IRQS + NR_EXT_IRQS)

#endif /* __IRQ_K1MATRIX_H_INCLUDE__ */
