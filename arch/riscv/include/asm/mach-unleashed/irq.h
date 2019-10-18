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
 * @(#)irq.h: FU540 (unleashed) specific IRQ assignments
 * $Id: irq.h,v 1.1 2019-10-16 11:18:00 zhenglv Exp $
 */

#ifndef __IRQ_UNLEASHED_H_INCLUDE__
#define __IRQ_UNLEASHED_H_INCLUDE__

#define IRQ_L2_DIRE	1 /* L2 Cache ECC DirError */
#define IRQ_L2_DATAE	2 /* L2 Cache ECC DataError */
#define IRQ_L2_DATAF	3 /* L2 Cache ECC DataFail */
#define IRQ_UART0	4
#define IRQ_UART1	5
#define IRQ_QSPI2	6
#define IRQ_GPIO(n)	(7 + (n)) /* GPIO0-15 */
#define IRQ_DMA(n)	(23 + (n)) /* DMA0-7 */
#define IRQ_MSI		(32 + (n))
#define IRQ_PWM0CMP(n)	(42 + (n)) /* PWMCMP0-3IP */
#define IRQ_PWM1CMP(n)	(46 + (n)) /* PWMCMP0-3IP */
#define IRQ_I2C		50
#define IRQ_QSPI0	51
#define IRQ_QSPI1	52
#define IRQ_GBE		53 /* GigaBit Ethernet */
#define NR_IRQS		53

#endif  /* __IRQ_UNLEASHED_H_INCLUDE__ */
