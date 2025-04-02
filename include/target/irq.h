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
 * @(#)irq.h: interrupt request (IRQ) definitions
 * $Id: irq.h,v 1.0 2020-12-1 23:38:00 zhenglv Exp $
 */

#ifndef __IRQ_H_INCLUDE__
#define __IRQ_H_INCLUDE__

#include <target/bh.h>
#include <asm/irq.h>

#ifndef INVALID_IRQ
#define INVALID_IRQ		NR_IRQS
#endif

#define IRQ_LEVEL_TRIGGERED	0
#define IRQ_EDGE_TRIGGERED	1

#ifndef __ASSEMBLY__
#if NR_IRQS <= 256
typedef uint8_t irq_t;
#elif NR_IRQS <= 65536
typedef uint16_t irq_t;
#else
typedef uint32_t irq_t;
#endif
typedef void (*irq_handler)(irq_t irq);

#ifdef CONFIG_ARCH_HAS_VIC
#include <driver/vic.h>
#else
#include <driver/irqc.h>
#endif

#ifdef CONFIG_SYS_NOIRQ
#define irq_local_enable()		do { } while (0)
#define irq_local_disable()		do { } while (0)
#define irq_local_save(__flags__)	((__flags__) = 0)
#define irq_local_restore(__flags__)	do { } while (0)
#else
#define irq_local_enable()		irq_hw_flags_enable()
#define irq_local_disable()		irq_hw_flags_disable()
#define irq_local_save(__flags__)	irq_hw_flags_save(__flags__)
#define irq_local_restore(__flags__)	irq_hw_flags_restore(__flags__)
#endif

void irq_init(void);
#ifdef CONFIG_SMP
void irq_smp_init(void);
#else /* CONFIG_SMP */
#define irq_smp_init()			do { } while (0)
#endif /* CONFIG_SMP */

#if !defined(CONFIG_CC_ISR_VECTOR) && !defined(CONFIG_SYS_NOIRQ)
void irq_register_vector(irq_t nr, irq_handler isr);
void irq_vectors_init(void);
#else
#define irq_register_vector(nr, isr)
#define irq_vectors_init()
#endif
boolean do_IRQ(irq_t nr);

#ifdef CONFIG_MSI
irq_t irq_mapped_msi(irq_t irq);
cpu_t irq_mapped_cpu(irq_t irq);
#else
#define irq_mapped_msi(irq)		INVALID_IRQ
#define irq_mapped_cpu(irq)		0
#endif

/* irq pollers */
void irq_register_poller(bh_t bh);
boolean irq_poll_bh(bool sync);

#ifndef irq_hw_flags_enable
void irq_hw_flags_enable(void);
#endif
#ifndef irq_hw_flags_disable
void irq_hw_flags_disable(void);
#endif
void irq_hw_handle_irq(void);
#ifndef irq_hw_ctrl_init
void irq_hw_ctrl_init(void);
#endif

typedef void (*trap_handler)(const char *msg);
void trap_handler_vector(trap_handler handler);
#endif /* __ASSEMBLY__ */

#endif /* __IRQ_H_INCLUDE__ */
