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
 * @(#)irqc.h: RISCV IRQ controller interfaces
 * $Id: irqc.h,v 1.1 2019-08-18 21:39:00 zhenglv Exp $
 */

#ifndef __IRQC_RISCV_H_INCLUDE__
#define __IRQC_RISCV_H_INCLUDE__

#include <target/arch.h>

#ifndef __ASSEMBLY__
#define IRQ2IE(irq)		(_AC(0x1, UL) << (irq))
#define riscv_enable_irq(irq)	csr_set(CSR_IE, IRQ2IE(irq))
#define riscv_disable_irq(irq)	csr_clear(CSR_IE, IRQ2IE(irq))
#define riscv_clear_irq(irq)	csr_clear(CSR_IP, IRQ2IE(irq))
#define riscv_trigger_irq(irq)	csr_set(CSR_IP, IRQ2IE(irq))
#define riscv_irq_raised(irq)	(csr_read(CSR_IP) & IRQ2IE(irq))

/* Interrupt filtering and virtual interrupts:
 * When supervisor mode is implemented, the Advanced Interrupt Architecture
 * adds a facility for software filtering of interrupts and for virtual
 * interrupts, making use of new CSRs mvien and mvip.
 * Interrupt filtering permits a supervisor-level interrupt (SEI or SSI) or
 * local or custom interrupt to trap to M-mode and then be selectively
 * delegated by software to supervisor level, even while the corresponding
 * bit in mideleg remains zero. The same hardware may also, under the right
 * circumstances, allow machine level to assert virtual interrupts to
 * supervisor level that have no connection to any real interrupt events.
 *                 *          *
 *  +------------+----------+---------+---------+--------------+
 *  | mideleg[n] | mvien[n] | mvip[n] | sip[n]  | sie[n]       |
 *  +------------+----------+---------+---------+--------------+
 *  | 0          | 0        | RO 0    | RO 0    | RO 0         |
 * *| 0          | 1        | RW      | mvip[n] | RW (mvie[n]) |
 *  | 1          | -        | mip[n]  | mip[n]  | mie[n]       |
 *  +------------+----------+---------+---------+--------------+
 *  NOTE:
 *   Normally AIA RW sie[n] is invisible to software and is implemented as
 *   mvie[n] register.
 */
#ifdef CONFIG_RISCV_EXIT_S
#ifdef CONFIG_RISCV_AIA
#define riscv_enable_aia(irq)	csr_set(CSR_MVIEN, IRQ2IE(irq))
#define riscv_delegate_irq(irq)	csr_set(CSR_MVIP, IRQ2IE(irq))
#else /* CONFIG_RISCV_AIA */
#define riscv_enable_aia(irq)	do { } while (0)
#define riscv_delegate_irq(irq)	csr_set(CSR_MIDELEG, IRQ2IE(irq))
#endif /* CONFIG_RISCV_AIA */
#endif /* CONFIG_RISCV_EXIT_S */

#ifdef CONFIG_RISCV_EXIT_VS
#ifdef CONFIG_RISCV_AIA
#define riscv_enable_aia(irq)	csr_set(CSR_HVIEN, IRQ2IE(irq))
#define riscv_delegate_irq(irq)	csr_set(CSR_HVIP, IRQ2IE(irq))
#else /* CONFIG_RISCV_AIA */
#define riscv_enable_aia(irq)	do { } while (0)
#define riscv_delegate_irq(irq)	csr_set(CSR_HIDELEG, IRQ2IE(irq))
#endif /* CONFIG_RISCV_AIA */
#endif /* CONFIG_RISCV_EXIT_VS */

/* RISCV allows embedded IRQ controllers */
#include <asm/mach/irqc.h>
#endif

#endif /* __IRQC_RISCV_H_INCLUDE__ */
