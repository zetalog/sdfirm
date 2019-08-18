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
 * @(#)event.h: RV32M1 (VEGA) event unit (IRQC) driver
 * $Id: event.h,v 1.1 2019-08-18 12:09:00 zhenglv Exp $
 */

#ifndef __EVENT_VEGA_H_INCLUDE__
#define __EVENT_VEGA_H_INCLUDE__

#ifndef ARCH_HAVE_IRQC
#define ARCH_HAVE_IRQC
#else
#error "Multiple IRQ controller defined"
#endif

/* RI5CY and ZERO-RISCY core both have an event unit to support vectorized
 * interrupts of up to 32 lines and event triggering of up to 32 input
 * lines. The interrupt and event lines are separately masked and buffered.
 */
#ifdef CONFIG_VEGA_RI5CY
#define EVENT_BASE		UL(0xE0041000)
#endif
#ifdef CONFIG_VEGA_0RISCY
#define EVENT_BASE		UL(0x4101F000)
#endif
#define EVENT_REG(off)		(EVENT_BASE + (off))
#define EVENT_1BIT_REG(off, n)	REG_1BIT_ADDR(EVENT_BASE+(off), n)
#define EVENT_4BIT_REG(off, n)	REG_4BIT_ADDR(EVENT_BASE+(off), n)

#define EVENT_INTPTEN(n)	EVENT_1BIT_REG(0x00, n)
#define EVENT_INTPTPEND		EVENT_REG(0x04)
#define EVENT_INTPTPENDSET	EVENT_REG(0x08)
#define EVENT_INTPTPENDCLEAR	EVENT_REG(0x0C)
#define EVENT_INTPTSECURE	EVENT_REG(0x10)
#define EVENT_INTPTPRI(n)	EVENT_4BIT_REG(0x14, n)
#define EVENT_INTPRIBASE	EVENT_REG(0x24)
#define EVENT_INTPTENACTIVE	EVENT_REG(0x28)
#define EVENT_INTACTPRI(n)	EVENT_4BIT_REG(0x2C, n)
#define EVENT_EVENTEN		EVENT_REG(0x40)
#define EVENT_EVENTPEND		EVENT_REG(0x44)
#define EVENT_EVTPENDSET	EVENT_REG(0x48)
#define EVENT_EVTPENDCLEAR	EVENT_REG(0x4C)
#define EVENT_SLPCTRL		EVENT_REG(0x80)
#define EVENT_SLPSTATUS		EVENT_REG(0x84)

/* EVENT_IRQ: applies to the following 1BIT registers
 * 3.4.2.1 Interrupt Enable Register (EVENTx_INTPTEN)
 * 3.4.2.2 Interrupt Pending Register (EVENTx_INTPTPEND)
 * 3.4.2.3 Set Interrupt Pending Register (EVENTx_INTPTPENDSET)
 * 3.4.2.4 Clear Interrupt Pending Register (EVENTx_INTPTPENDCLEAR)
 * 3.4.2.5 Interrupt Secure Register (EVENTx_INTPTSECURE)
 * 3.4.2.11 Interrupt Active Register (EVENTx_INTPTENACTIVE)
 */
#define EVENT_IRQ(n)			_BV(REG_1BIT_OFFSET(n))

/* EVENT_EVT: applies to the following 1BIT registers
 * 3.4.2.16 Event Enable Register (EVENTx_EVENTEN)
 * 3.4.2.17 Event Pending Register (EVENTx_EVENTPEND)
 * 3.4.2.18 Set Event Pending Register (EVENTx_EVTPENDSET)
 * 3.4.2.19 Clear Event Pending Register (EVENTx_EVTPENDCLEAR)
 */
#define EVENT_EVT(n)			_BV(REG_1BIT_OFFSET(n))

/* EVENT_PRI: applies to the following 4BIT registers
 * 3.4.2.6-9 Interrupt Priority 0-3 Register (EVENTx_INTPTPRI0-3)
 * 3.4.2.12-15 Interrupt Active Priority 0-3 Register (EVENTx_INTACTPRI0-3)
 */
#define EVENT_PRI_MAX			7
#define EVENT_PRI_MASK			REG_4BIT_MASK
#define EVENT_PRI_OFFSET(n)		REG_4BIT_OFFSET(n)
#define EVENT_PRI(n, value)		\
	_SET_FVn(n, EVENT_PRI, (value) & EVENT_PRI_MAX)

/* 3.4.2.10 Interrupt Priority Base (EVENTx_INTPRIBASE) */
#define EVENT_IPBASE_OFFSET		0
#define EVENT_IPBASE_MASK		REG_4BIT_MASK
#define EVENT_IPBASE(value)		_SET_FV(EVENT_IPBASE, value)

/* 3.4.2.20 Sleep Control Register (EVENTx_SLPCTRL) */
#define SLPCTRL_SLPCTRL_OFFSET		0
#define SLPCTRL_SLPCTRL_MASK		REG_2BIT_MASK
#define SLPCTRL_SLPCTRL(value)		_SET_FV(SLPCTRL_SLPCTRL, value)
#define SLPCTRL_SYSRSTREQST		_BV(31)

/* 3.4.2.21 Sleep Status Register (EVENTx_SLPSTATUS) */
#define SLPSTATUS_SLPSTAT_OFFSET	0
#define SLPSTATUS_SLPSTAT_MASK		REG_2BIT_MASK
#define SLPSTATUS_SLPSTAT(value)	_GET_FV(SLPSTATUS_SLPSTAT, value)

/* The sleep ctrl/stat value */
#define EVENT_SLP_SLEEP_ENABLE		0x1
#define EVENT_SLP_DEEP_SLEEP_ENABLE	0x2

#define event_enable_irq(irq)	\
	__raw_setl(EVENT_IRQ(irq), EVENT_INTPTEN(irq))
#define event_disable_irq(irq)	\
	__raw_clearl(EVENT_IRQ(irq), EVENT_IRQPTENz(irq))
#define event_clear_irq(irq)	\
	__raw_setl(EVENT_IRQ(irq), EVENT_INTPTPENDCLEAR(irq))
#define event_set_irq(irq)	\
	__raw_setl(EVENT_IRQ(ira), EVENT_INTPTPENDSET(irq))
#define event_irq_pending(irq)				\
	((__raw_readl(EVENT_INTPTPEND(irq)) >>		\
	  EVENT_IRQ_OFFSET(irq)) & 0x1)
#define event_irq_active(irq)				\
	((__raw_readl(EVENT_INTPTENACTIVE(irq)) >>	\
	  EVENT_IRQ_OFFSET(irq)) & 0x1)

void event_init_ctrl(void);
void event_configure_irq(irq_t irq, uint8_t pri, uint8_t trigger);

#endif /* __EVENT_VEGA_H_INCLUDE__ */
