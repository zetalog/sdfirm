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
 * @(#)irq.h: RISCV IRQ flags interface
 * $Id: irq.h,v 1.1 2019-08-14 13:27:00 zhenglv Exp $
 */

#ifndef __IRQ_RISCV_H_INCLUDE__
#define __IRQ_RISCV_H_INCLUDE__

#include <asm/reg.h>

#ifndef __ASSEMBLY__
typedef uint32_t irq_flags_t;
struct pt_regs {
	unsigned long regs[31];
};

#ifdef CONFIG_SYS_MONITOR
#define irq_hw_flags_save(x)	((x) = csr_read_clear(CSR_MSTATUS, SR_MIE))
#define irq_hw_flags_restore(x)	csr_set(CSR_MSTATUS, (x) & SR_MIE)
#define irq_hw_flags_enable()	csr_set(CSR_MSTATUS, SR_MIE)
#define irq_hw_flags_disable()	csr_clear(CSR_MSTATUS, SR_MIE)
#else
#define irq_hw_flags_save(x)	((x) = csr_read_clear(CSR_SSTATUS, SR_SIE))
#define irq_hw_flags_restore(x)	csr_set(CSR_SSTATUS, (x) & SR_SIE)
#define irq_hw_flags_enable()	csr_set(CSR_SSTATUS, SR_SIE)
#define irq_hw_flags_disable()	csr_clear(CSR_SSTATUS, SR_SIE)
#endif
#define irq_hw_ctrl_init()
#endif /* __ASSEMBLY__ */

#define IPI_SOFT	0x1
#define IPI_FENCE_I	0x2
#define IPI_SFENCE_VMA	0x4
#define IPI_HALT	0x8

#include <asm/mach/irq.h>

#endif /* __IRQ_RISCV_H_INCLUDE__ */
