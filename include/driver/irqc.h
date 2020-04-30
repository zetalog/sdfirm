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
 * @(#)irqc.h: IRQ controller driver abstraction
 * $Id: irqc.h,v 1.1 2019-08-18 22:03:00 zhenglv Exp $
 */

#ifndef __IRQC_DRIVER_H_INCLUDE__
#define __IRQC_DRIVER_H_INCLUDE__

/* Most architectures contain architecture level IRQ controller, so
 * allow architecture to split it down to SoC level.
 */
#ifdef CONFIG_ARCH_HAS_IRQC
#include <asm/irqc.h>
#endif

#ifdef ARCH_HAVE_IRQC
#define irqc_enable_irq(irq)		irqc_hw_enable_irq(irq)
#define irqc_disable_irq(irq)		irqc_hw_disable_irq(irq)
#define irqc_clear_irq(irq)		irqc_hw_clear_irq(irq)
#define irqc_trigger_irq(irq)		irqc_hw_trigger_irq(irq)
#define irqc_configure_irq(irq, prio, trigger)	\
	irqc_hw_configure_irq(irq, prio, trigger)
#ifdef CONFIG_ARCH_HAS_IRQC_ACK
/* Defer acknowledgement */
#define irqc_ack_irq(irq)		irqc_hw_ack_irq(irq)
#else
#define irqc_ack_irq(irq)		irqc_hw_clear_irq(irq)
#endif
#ifdef CONFIG_ARCH_HAS_IRQC_MASK
#define irqc_mask_irq(irq)		irqc_hw_mask_irq(irq)
#define irqc_unmask_irq(irq)		irqc_hw_unmask_irq(irq)
#else
#define irqc_mask_irq(irq)		irqc_hw_disable_irq(irq)
#define irqc_unmask_irq(irq)		irqc_hw_enable_irq(irq)
#endif
#ifndef CONFIG_SMP
#define irqc_hw_smp_init()		do { } while (0)
#endif
#ifndef CONFIG_MMU
#define irqc_hw_mmu_init()		do { } while (0)
#endif
#else
#define irqc_hw_ctrl_init()		do { } while (0)
#define irqc_hw_smp_init()		do { } while (0)
#define irqc_hw_mmu_init()		do { } while (0)
#define irqc_enable_irq(irq)		do { } while (0)
#define irqc_disable_irq(irq)		do { } while (0)
#define irqc_clear_irq(irq)		do { } while (0)
#define irqc_trigger_irq(irq)		do { } while (0)
#define irqc_configure_irq(irq, prio, trigger)	\
	do { } while (0)
#define irqc_mask_irq(irq)		do { } while (0)
#define irqc_unmask_irq(irq)		do { } while (0)
#define irqc_ack_irq(irq)		do { } while (0)
#endif

#endif /* __IRQC_DRIVER_H_INCLUDE__ */
