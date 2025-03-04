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
 * @(#)dw_timers.h: Synopsys DesignWare APB timers interface
 * $Id: dw_timers.h,v 1.1 2019-09-25 14:52:00 zhenglv Exp $
 */

#ifndef __DW_TIMERS_H_INCLUDE__
#define __DW_TIMERS_H_INCLUDE__

/* Required implementation specific definitions:
 *
 * DW_TIMERS_BASE: base address of the timer registers
 * DW_TIMERS_SIZE: register block size of timres
 * DW_TIMERS_WIDTH: counter bit width
 */
#define DW_TIMERS_REG(x)	(DW_TIMERS_BASE + (x))
#define DW_TIMERSn_REG(n, x)	DW_TIMERS_REG((n) * DW_TIMERS_SIZE + (x))

#define TIMERLOADCOUNT(n)	DW_TIMERSn_REG(n, 0x00)
#define TIMERCURRENTVAL(n)	DW_TIMERSn_REG(n, 0x04)
#define TIMERCONTROLREG(n)	DW_TIMERSn_REG(n, 0x08)
#define TIMEREOI(n)		DW_TIMERSn_REG(n, 0x0C)
#define TIMERINTSTAT(n)		DW_TIMERSn_REG(n, 0x10)

#define TIMERSINTSTATUS		DW_TIMERS_REG(0xA0)
#define TIMERSEOI		DW_TIMERS_REG(0xA4)
#define TIMERSRAWINTSTATUS	DW_TIMERS_REG(0xA8)
#define TIMERS_COMP_VERSION	DW_TIMERS_REG(0xAC)

#define TIMER_ENABLE		_BV(0)
#define TIMER_MODE_OFFSET	1
#define TIMER_MODE_MASK		REG_1BIT_MASK
#define TIMER_MODE(value)	_SET_FV(TIMER_MODE, value)
#define TIMER_USER_DEFINED	1
#define TIMER_FREE_RUNNING	0
#define TIMER_INTSTAT		_BV(0)
#define TIMER_INTERRUPT_MASK	_BV(2)
#define TIMER_PWM		_BV(3)
#define TIMER_PWM_EN		_BV(4)

#if DW_TIMERS_WIDTH == 32
#define DW_TIMERS_MAX		(UL(0) - 1)
#else
#define DW_TIMERS_MAX		(_BV_UL(DW_TIMERS_WIDTH) - 1)
#endif

#define dw_timers_disable(n)				\
	__raw_clearl(TIMER_ENABLE, TIMERCONTROLREG(n))
#define dw_timers_enable(n)				\
	__raw_setl(TIMER_ENABLE, TIMERCONTROLREG(n))
#define dw_timers_set_mode(n, mode)			\
	__raw_writel_mask(TIMER_MODE(mode),		\
			  TIMER_MODE(TIMER_MODE_MASK),	\
			  TIMERCONTROLREG(n))
#define dw_timers_mask_irq(n)				\
	__raw_setl(TIMER_INTERRUPT_MASK,		\
		   TIMERCONTROLREG(n))
#define dw_timers_unmask_irq(n)				\
	__raw_clearl(TIMER_INTERRUPT_MASK,		\
		     TIMERCONTROLREG(n))
#define dw_timers_set_counter(n, counter)		\
	__raw_writel(counter, TIMERLOADCOUNT(n))
#define dw_timers_get_counter(n)			\
	(DW_TIMERS_MAX - __raw_readl(TIMERCURRENTVAL(n)))
#define dw_timers_irq_raised(n)				\
	(__raw_readl(TIMERINTSTAT(n)) & TIMER_INTSTAT)
#define dw_timers_eoi(n)				\
	__raw_readl(TIMEREOI(n))

#ifndef __ASSEMBLY__
void dw_timers_tsc_init(int n);
void dw_timers_gpt_init(int n);
#endif

#endif /* __DW_TIMERS_H_INCLUDE__ */
