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
 * @(#)irqc.h: DUOWEN specific IRQ controller interfaces
 * $Id: irqc.h,v 1.1 2019-10-12 11:20:00 zhenglv Exp $
 */

#ifndef __IRQC_DUOWEN_H_INCLUDE__
#define __IRQC_DUOWEN_H_INCLUDE__

#include <target/clk.h>

#ifndef ARCH_HAVE_IRQC
#define ARCH_HAVE_IRQC		1
#else
#error "Multiple IRQ controller defined"
#endif

#define PLIC_HW_PRI_MAX		31
#define plic_hw_m_ctx(hartid)	(2 * (hartid))
#define plic_hw_s_ctx(hartid)	((hartid) < 16 ? (2 * (hartid) + 1) : PLIC_CTX_NONE)

#include <asm/ri5cy_firq.h>
#include <asm/plic.h>

#define plic_hw_ctrl_init()		clk_enable(plic_hclk)

/* Internal IRQs */
#ifdef CONFIG_DUOWEN_IMC
#define plic_hw_enable_int(irq)		riscv_enable_firq(irq)
#define plic_hw_disable_int(irq)	riscv_disable_firq(irq)
#define plic_hw_clear_int(irq)		riscv_clear_firq(irq)
#define plic_hw_trigger_int(irq)	riscv_trigger_firq(irq)
#else
#define plic_hw_enable_int(irq)		riscv_enable_irq(irq)
#define plic_hw_disable_int(irq)	riscv_disable_irq(irq)
#define plic_hw_clear_int(irq)		riscv_clear_irq(irq)
#define plic_hw_trigger_int(irq)	riscv_trigger_irq(irq)
#endif

#endif /* __IRQC_DUOWEN_H_INCLUDE__ */
