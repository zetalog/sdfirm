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
/* PLIC contexts
 * +----------+----------+----------+----------+----------+----------+
 * | 0-15     | 16       | 17-32    | 33-48    | 49       | 50-65    |
 * +----------+----------+----------+----------+----------+----------+
 * | S0 APC M | S0 IMC M | S0 APC S | S1 APC M | S1 IMC M | S1 APC S |
 * +----------+----------+----------+----------+----------+----------+
 */
#ifdef CONFIG_DUOWEN_IMC
#ifdef CONFIG_DUOWEN_SOC0
#define plic_hw_m_ctx(cpu)	16
#define plic_hw_s_ctx(cpu)	PLIC_CTX_NONE
#endif /* CONFIG_DUOWEN_SOC0 */
#ifdef CONFIG_DUOWEN_SOC1
#define plic_hw_m_ctx(cpu)	49
#define plic_hw_s_ctx(cpu)	PLIC_CTX_NONE
#endif /* CONFIG_DUOWEN_SOC1 */
#endif
#ifdef CONFIG_DUOWEN_APC
#ifdef CONFIG_DUOWEN_SOCv2
#ifdef CONFIG_DUOWEN_SOC0
#ifdef CONFIG_DUOWEN_SOC_DUAL
/* TODO: support of 2 PLIC controllers */
#define plic_hw_m_ctx(cpu)	(cpu)
#define plic_hw_s_ctx(cpu)	((cpu) + 17)
#else /* CONFIG_DUOWEN_SOC_DUAL */
#define plic_hw_m_ctx(cpu)	(cpu)
#define plic_hw_s_ctx(cpu)	((cpu) + 17)
#endif /* CONFIG_DUOWEN_SOC_DUAL */
#endif /* CONFIG_DUOWEN_SOC0 */
#ifdef CONFIG_DUOWEN_SOC1
#define plic_hw_m_ctx(cpu)	(smp_hw_cpu_hart(cpu) + 33)
#define plic_hw_s_ctx(cpu)	(smp_hw_cpu_hart(cpu) + 50)
#endif /* CONFIG_DUOWEN_SOC1 */
#endif /* CONFIG_DUOWEN_SOCv2 */

#ifdef CONFIG_DUOWEN_SOCv3
#ifdef CONFIG_DUOWEN_SOC_DUAL
/* TODO: support of 2 PLIC controllers */
#define plic_hw_m_ctx(cpu)		\
	(imc_socket_id() == 1 ?		\
	 (smp_hw_cpu_hart(cpu) + 33) : (cpu))
#define plic_hw_s_ctx(cpu)		\
	(imc_socket_id() == 1 ?		\
	 (smp_hw_cpu_hart(cpu) + 50) : ((cpu) + 17))
#else /* CONFIG_DUOWEN_SOC_DUAL */
#define plic_hw_m_ctx(cpu)	(cpu)
#define plic_hw_s_ctx(cpu)	((cpu) + 17)
#endif /* CONFIG_DUOWEN_SOC_DUAL */
#endif /* CONFIG_DUOWEN_SOCv3 */
#endif

#include <asm/ri5cy_firq.h>
#include <asm/plic.h>

#define plic_hw_ctrl_init()		clk_enable(plic_clk)

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
#ifdef CONFIG_MMU
#define plic_hw_mmu_init()		do { } while (0)
#endif

#endif /* __IRQC_DUOWEN_H_INCLUDE__ */
