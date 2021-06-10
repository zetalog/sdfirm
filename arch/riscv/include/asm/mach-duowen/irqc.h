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

#ifdef CONFIG_DUOWEN_SBI_DUAL
#define PLIC_BASE(soc)			(__SOC_BASE(soc) + __PLIC_BASE)
#define PLIC_HW_MAX_CHIPS		2
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define PLIC_BASE			IRQC_BASE
#define PLIC_HW_MAX_CHIPS		1
#endif /* CONFIG_DUOWEN_SBI_DUAL */

#define PLIC_GATEWAY_BASE		0x4001000
#define PLIC_SOCKET_CNTL_BASE		0x4002000

#define PLIC_GATEWAYR_BASE(offset)	(PLIC_GATEWAY_BASE + (offset))

#define PLIC_ISPENDR_BASE		PLIC_GATEWAY_BASE(0x100)
#define PLIC_ICPENDR_BASE		PLIC_GATEWAY_BASE(0x180)
#define PLIC_ISCLAIMR_BASE		PLIC_GATEWAY_BASE(0x200)
#define PLIC_ICCLAIMR_BASE		PLIC_GATEWAY_BASE(0x280)
#define PLIC_ICFGR_BASE			PLIC_GATEWAY_BASE(0x300)

#define PLIC_ISPENDR(soc, irq)		\
	PLIC_1BIT_REG(soc, PLIC_ISPENDR_BASE, irq)
#define PLIC_ICPENDR(soc, irq)		\
	PLIC_1BIT_REG(PLIC_ICPENDR_BASE, irq)
#define PLIC_ISCLAIMR(soc, irq)		\
	PLIC_1BIT_REG(PLIC_ISCLAIMR_BASE, irq)
#define PLIC_ICCLAIMR(soc, irq)		\
	PLIC_1BIT_REG(PLIC_ICCLAIMR_BASE, irq)
#define PLIC_ICFGR(soc, irq)		\
	PLIC_1BIT_REG(PLIC_ICFGR_BASE, irq)

#define PLIC_SOCKET_CNTL(soc)			\
	PLIC_REG(imc_socket_id(), PLIC_SOCKET_CNTL_BASE + ((soc) << 2))

/* PLIC_SOCKET_CNTL */
#define PLIC_SOCKET_CONN_DISCONN		_BV(0)
#define PLIC_SOCKET_CONN_DISCONN_RETRY		_BV(1)
#define PLIC_SOCKET_CONN_DISCONN_OUTSTANDING	_BV(31)

#define plic_socket_outstanding(soc)		\
	(__raw_readl(PLIC_SOCKET_CNTL(soc)) &	\
	 PLIC_SOCKET_CONN_DISCONN_OUTSTANDING)
#define plic_socket_connect(soc)			\
	do {						\
		__raw_setl(PLIC_SOCKET_CONN_DISCONN,	\
			   PLIC_SOCKET_CNTL(soc));	\
		while (plic_socket_outstanding(soc));	\
	} while (0)
#define plic_socket_disconnect(soc)			\
	do {						\
		__raw_clearl(PLIC_SOCKET_CONN_DISCONN,	\
			     PLIC_SOCKET_CNTL(soc));	\
		while (plic_socket_outstanding(soc));	\
	} while (0)

#define PLIC_HW_PRI_MAX		31
/* PLIC contexts
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * | 0-15   | 16     | 17-32  | 33     | 34-49  | 50     | 51-66  | 67     |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * | S0APCM | S0IMCM | S0APCS | S0IMCS | S1APCM | S1IMCM | S1APCS | S1IMCS |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 */
#ifdef CONFIG_DUOWEN_IMC
#define plic_hw_m_ctx(cpu)	(imc_socket_id() == 1 ? 50 : 16)
#define plic_hw_s_ctx(cpu)	(imc_socket_id() == 1 ? 67 : 33)
#endif /* CONFIG_DUOWEN_IMC */

#ifdef CONFIG_DUOWEN_APC
/* TODO: support of 2 PLIC controllers */
#define plic_hw_m_ctx(cpu)		\
	(imc_socket_id() == 1 ?		\
	 (smp_hw_cpu_hart(cpu) + 18) : (cpu))
#define plic_hw_s_ctx(cpu)		\
	(imc_socket_id() == 1 ?		\
	 (smp_hw_cpu_hart(cpu) + 35) : ((cpu) + 17))
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define plic_hw_cpu_soc(cpu)		\
	(smp_hw_cpu_hart(cpu) < SOC1_HART ? 0 : 1)
#define plic_hw_irq_soc(irq)		((irq) / __NR_EXT_IRQS)
#define plic_hw_irq_irq(irq)		((irq) & (__NR_EXT_IRQS - 1))
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#endif /* CONFIG_DUOWEN_APC */

#include <asm/ri5cy_firq.h>
#include <asm/plic.h>

#define plic_hw_ctrl_init()		clk_enable(plic_clk)

/* Internal IRQs */
#ifdef CONFIG_DUOWEN_IMC
#define plic_hw_enable_int(irq)		riscv_enable_firq(irq)
#define plic_hw_disable_int(irq)	riscv_disable_firq(irq)
#define plic_hw_clear_int(irq)		riscv_clear_firq(irq)
#define plic_hw_trigger_int(irq)	riscv_trigger_firq(irq)
#else /* CONFIG_DUOWEN_IMC */
#define plic_hw_enable_int(irq)		riscv_enable_irq(irq)
#define plic_hw_disable_int(irq)	riscv_disable_irq(irq)
#define plic_hw_clear_int(irq)		riscv_clear_irq(irq)
#define plic_hw_trigger_int(irq)	riscv_trigger_irq(irq)
#endif /* CONFIG_DUOWEN_IMC */
#ifdef CONFIG_MMU
#define plic_hw_mmu_init()		do { } while (0)
#endif /* CONFIG_MMU */

#endif /* __IRQC_DUOWEN_H_INCLUDE__ */
