/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
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
 * @(#)vic_nvic.h: nested vectored interrupt controller interfaces
 * $Id: vic_nvic.h,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#ifndef __VIC_NVIC_H_INCLUDE__
#define __VIC_NVIC_H_INCLUDE__

#include <asm/nvic.h>
#include <asm/mach/mem.h>
#include <asm/mach/irq.h>
#include <asm/mach/scb.h>

#ifndef ARCH_HAVE_VIC
#define ARCH_HAVE_VIC			1
#else
#error "Multiple VIC controller defined"
#endif

#define __VIC_HW_PRIO_MAX	NVIC_PRIO_MAX
#define __VIC_HW_PRIO_TRAP_MIN	NVIC_PRIO_TRAP_MIN

typedef void (*irq_handler)(void);

#define vic_hw_irq_enable(irq)		nvic_irq_enable_irq(irq)
#define vic_hw_irq_disable(irq)		nvic_irq_disable_irq(irq)
#define vic_hw_irq_trigger(irq)		nvic_irq_trigger_irq(irq)
void vic_hw_irq_priority(uint8_t irq, uint8_t prio);
void vic_hw_trap_priority(uint8_t trap, uint8_t prio);
void vic_hw_register_irq(uint8_t nr, irq_handler h);
void vic_hw_register_trap(uint8_t nr, irq_handler h);
void vic_hw_vectors_init(void);

#endif /* __VIC_NVIC_H_INCLUDE__ */
