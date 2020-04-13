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

#include <target/arch.h>

#ifndef __ASSEMBLY__
typedef __unused uint32_t irq_flags_t;

struct pt_regs {
	unsigned long zero;
	unsigned long ra;
	unsigned long sp;
	unsigned long gp;
	unsigned long tp;
	unsigned long t0;
	unsigned long t1;
	unsigned long t2;
	unsigned long s0;
	unsigned long s1;
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
	unsigned long a4;
	unsigned long a5;
	unsigned long a6;
	unsigned long a7;
	unsigned long s2;
	unsigned long s3;
	unsigned long s4;
	unsigned long s5;
	unsigned long s6;
	unsigned long s7;
	unsigned long s8;
	unsigned long s9;
	unsigned long s10;
	unsigned long s11;
	unsigned long t3;
	unsigned long t4;
	unsigned long t5;
	unsigned long t6;
	/* CSRs */
	unsigned long epc;
	unsigned long status;
	unsigned long badaddr;
	unsigned long cause;
	/* a0 value before the syscall */
	unsigned long orig_a0;
};

#define irq_hw_flags_save(x)	((x) = csr_read_clear(CSR_STATUS, SR_IE))
#define irq_hw_flags_restore(x)	csr_set(CSR_STATUS, (x) & SR_IE)
#define irq_hw_flags_enable()	csr_set(CSR_STATUS, SR_IE)
#define irq_hw_flags_disable()	csr_clear(CSR_STATUS, SR_IE)
#define irq_hw_ctrl_init()
#endif /* __ASSEMBLY__ */

#define IRQ_U_SOFT		0
#define IRQ_S_SOFT		1
#define IRQ_H_SOFT		2
#define IRQ_M_SOFT		3
#define IRQ_U_TIMER		4
#define IRQ_S_TIMER		5
#define IRQ_H_TIMER		6
#define IRQ_M_TIMER		7
#define IRQ_U_EXT		8
#define IRQ_S_EXT		9
#define IRQ_H_EXT		10
#define IRQ_M_EXT		11
#ifdef CONFIG_RISCV_EXIT_M
#define IRQ_SOFT		IRQ_M_SOFT
#define IRQ_TIMER		IRQ_M_TIMER
#define IRQ_EXT			IRQ_M_EXT
#endif
#ifdef CONFIG_RISCV_EXIT_S
#define IRQ_SOFT		IRQ_S_SOFT
#define IRQ_TIMER		IRQ_S_TIMER
#define IRQ_EXT			IRQ_S_EXT
#endif

#ifndef ARCH_HAVE_INT_IRQS
#define NR_INT_IRQS		16
#endif
#define IRQ_PLATFORM		NR_INT_IRQS
#define EXT_IRQ(irq)		(IRQ_PLATFORM + (irq))

#define EXC_INSN_MISALIGNED	0
#define EXC_INSN_ACCESS		1
#define EXC_INSN_ILLEGAL	2
#define EXC_BREAKPOINT		3
#define EXC_LOAD_MISALIGNED	4
#define EXC_LOAD_ACCESS		5
#define EXC_STORE_MISALIGNED	6
#define EXC_STORE_ACCESS	7
#define EXC_ECALL_U		8
#define EXC_ECALL_S		9  /* hypervisor call */
#define EXC_ECALL_H		10 /* supervisor call */
#define EXC_ECALL_M		11
#define EXC_INSN_PAGE_FAULT	12
#define EXC_LOAD_PAGE_FAULT	13
#define EXC_STORE_PAGE_FAULT	15

#define EXC_SYSCALL		EXC_ECALL_U

/* xIE (Interrupt Enable) and xIP (Interrupt Pending) flags */

#define IPI_SOFT	0x1
#define IPI_FENCE_I	0x2
#define IPI_SFENCE_VMA	0x4
#define IPI_HALT	0x8

#define IRQ_DIRECT	0x0
#define IRQ_VECTOR	0x1

#ifdef CONFIG_RISCV_VIRQ
#define irq_set_mtvec(vec)		\
	csr_write(CSR_MTVEC, (uintptr_t)(vec) | IRQ_VECTOR)
#define irq_set_stvec(vec)		\
	csr_write(CSR_STVEC, (uintptr_t)(vec) | IRQ_VECTOR)
#define irq_set_utvec(vec)		\
	csr_write(CSR_UTVEC, (uintptr_t)(vec) | IRQ_VECTOR)
#define irq_set_tvec(vec)		\
	csr_write(CSR_TVEC, (uintptr_t)(vec) | IRQ_VECTOR)
#define irq_get_mtvec(vec)		(csr_read(CSR_MTVEC) & ~IRQ_VECTOR)
#define irq_get_stvec(vec)		(csr_read(CSR_STVEC) & ~IRQ_VECTOR)
#define irq_get_utvec(vec)		(csr_read(CSR_UTVEC) & ~IRQ_VECTOR)
#define irq_get_tvec(vec)		(csr_read(CSR_TVEC) & ~IRQ_VECTOR)
#else
#define irq_set_mtvec(vec)		csr_write(CSR_MTVEC, vec)
#define irq_set_stvec(vec)		csr_write(CSR_STVEC, vec)
#define irq_set_utvec(vec)		csr_write(CSR_UTVEC, vec)
#define irq_set_tvec(vec)		csr_write(CSR_TVEC, vec)
#define irq_get_mtvec()			csr_read(CSR_MTVEC)
#define irq_get_stvec()			csr_read(CSR_STVEC)
#define irq_get_utvec()			csr_read(CSR_UTVEC)
#define irq_get_tvec()			csr_read(CSR_TVEC)
#endif

#ifndef __ASSEMBLY__
#include <asm/mach/irq.h>

__noreturn void hart_hang(void);
void show_regs(struct pt_regs *regs);
#endif

#endif /* __IRQ_RISCV_H_INCLUDE__ */
