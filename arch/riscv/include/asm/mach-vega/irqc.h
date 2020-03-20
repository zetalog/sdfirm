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
 * @(#)irqc.h: RV32M1 (VEGA) specific interrupt controller driver
 * $Id: irqc.h,v 1.1 2019-08-18 12:12:00 zhenglv Exp $
 */

#ifndef __IRQC_VEGA_H_INCLUDE__
#define __IRQC_VEGA_H_INCLUDE__

#ifndef ARCH_HAVE_IRQC
#define ARCH_HAVE_IRQC
#else
#error "Multiple IRQ controller defined"
#endif

#include <asm/mach/event.h>

#define irqc_hw_ctrl_init()		event_init_ctrl()
#define irqc_hw_enable_irq(irq)		event_enable_irq(irq)
#define irqc_hw_disable_irq(irq)	event_disable_irq(irq)
#define irqc_hw_clear_irq(irq)		event_clear_irq(irq)
#define irqc_hw_trigger_irq(irq)	event_set_irq(irq)
#define irqc_hw_configure_irq(irq, priority, trigger)	\
	event_configure_irq(irq, priority, trigger)

#define irqc_hw_handle_irq()		event_handle_irq()

#endif /* __IRQC_VEGA_H_INCLUDE__ */
