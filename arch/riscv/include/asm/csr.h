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
 * @(#)csr.h: RISCV architecture extension csr interface
 * $Id: csr.h,v 1.1 2019-08-14 09:53:00 zhenglv Exp $
 */

#ifndef __CSR_RISCV_H_INCLUDE__
#define __CSR_RISCV_H_INCLUDE__

#include <target/config.h>
#include <target/const.h>
#include <asm/assembler.h>

/* This file is intended for being included by architecture specific
 * register header.
 */

/* Status register flags */
#define SR_UIE		_AC(0x00000001, UL) /* User Interrupt Enable */
#define SR_UPIE		_AC(0x00000010, UL) /* User Previous IE */
#define SR_MIE		_AC(0x00000008, UL) /* Machine Interrupt Enable */
#define SR_MPIE		_AC(0x00000080, UL) /* Machine Previous IE */
#define SR_MPP		_AC(0x00001800, UL) /* Previously Machine */
#ifndef CONFIG_ARCH_HAS_NOSEE
#define SR_SPIE		_AC(0x00000020, UL) /* Supervisor Previous IE */
#define SR_SIE		_AC(0x00000002, UL) /* Supervisor Interrupt Enable */
#define SR_SPP		_AC(0x00000100, UL) /* Previously Supervisor */
#define SR_SUM		_AC(0x00040000, UL) /* Supervisor User Memory Access */
#endif
#ifdef CONFIG_CPU_H
#define SR_HIE		_AC(0x00000004, UL) /* Hypervisor Interrupt Enable */
#define SR_HPIE		_AC(0x00000040, UL) /* Hypervisor Previous IE */
#define SR_HPP		_AC(0x00000600, UL) /* Previously Hypervisor */
#endif

#ifdef CONFIG_CPU_F
#define SR_FS		_AC(0x00006000, UL) /* Floating-point Status */
#define SR_FS_OFF	_AC(0x00000000, UL)
#define SR_FS_INITIAL	_AC(0x00002000, UL)
#define SR_FS_CLEAN	_AC(0x00004000, UL)
#define SR_FS_DIRTY	_AC(0x00006000, UL)
#endif

#define SR_XS		_AC(0x00018000, UL) /* Extension Status */
#define SR_XS_OFF	_AC(0x00000000, UL)
#define SR_XS_INITIAL	_AC(0x00008000, UL)
#define SR_XS_CLEAN	_AC(0x00010000, UL)
#define SR_XS_DIRTY	_AC(0x00018000, UL)

#ifndef CONFIG_64BIT
#define SR_SD		_AC(0x80000000, UL) /* FS/XS dirty */
#else
#define SR_SD		_AC(0x8000000000000000, UL) /* FS/XS dirty */
#endif

#define SR_MPRV		_AC(0x00020000, UL)
#define SR_MXR		_AC(0x00080000, UL)
#define SR_TVM		_AC(0x00100000, UL)
#define SR_TW		_AC(0x00200000, UL)
#define SR_TSR		_AC(0x00400000, UL)
#ifdef CONFIG_64BIT
#define SR_UXL		_AC(0x0000000300000000, UL)
#define SR_SXL		_AC(0x0000000C00000000, UL)
#endif

/* SATP flags */
#ifndef CONFIG_64BIT
#define SATP_PPN	_AC(0x003FFFFF, UL)
#define SATP_MODE_32	_AC(0x80000000, UL)
#define SATP_MODE	SATP_MODE_32
#else
#define SATP_PPN	_AC(0x00000FFFFFFFFFFF, UL)
#define SATP_MODE_39	_AC(0x8000000000000000, UL)
#define SATP_MODE	SATP_MODE_39
#endif

/* SCAUSE */
#define SCAUSE_IRQ_FLAG		(_AC(1, UL) << (__riscv_xlen - 1))

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

#define EXC_INST_MISALIGNED	0
#define EXC_INST_ACCESS		1
#define EXC_BREAKPOINT		3
#define EXC_LOAD_ACCESS		5
#define EXC_STORE_ACCESS	7
#define EXC_SYSCALL		8
#define EXC_INST_PAGE_FAULT	12
#define EXC_LOAD_PAGE_FAULT	13
#define EXC_STORE_PAGE_FAULT	15

/* xIE (Interrupt Enable) and xIP (Interrupt Pending) flags */
#define IE_USIE			(_AC(0x1, UL) << IRQ_U_SOFT)
#define IE_UTIE			(_AC(0x1, UL) << IRQ_U_TIMER)
#define IE_UEIE			(_AC(0x1, UL) << IRQ_U_EXT)
#define IE_SSIE			(_AC(0x1, UL) << IRQ_S_SOFT)
#define IE_STIE			(_AC(0x1, UL) << IRQ_S_TIMER)
#define IE_SEIE			(_AC(0x1, UL) << IRQ_S_EXT)
#define IE_HSIE			(_AC(0x1, UL) << IRQ_H_SOFT)
#define IE_HTIE			(_AC(0x1, UL) << IRQ_H_TIMER)
#define IE_HEIE			(_AC(0x1, UL) << IRQ_H_EXT)
#define IE_MSIE			(_AC(0x1, UL) << IRQ_M_SOFT)
#define IE_MTIE			(_AC(0x1, UL) << IRQ_M_TIMER)
#define IE_MEIE			(_AC(0x1, UL) << IRQ_M_EXT)

#ifdef CONFIG_CPU_N
#define CSR_USTATUS		0x000
#define CSR_UIE			0x004
#define CSR_UTVEC		0x005
#define CSR_USCRATCH		0x040
#define CSR_UEPC		0x041
#define CSR_UCAUSE		0x042
#define CSR_UTVAL		0x043
#define CSR_UIP			0x044
#endif
#ifdef CONFIG_CPU_F
#define CSR_FFLAGS		0x001
#define CSR_FRM			0x002
#define CSR_FCSR		0x003
#endif
#define CSR_CYCLE		0xC00
#define CSR_TIME		0xC01
#define CSR_INSTRET		0xC02
#define CSR_HPMCOUNTER(n)	(0xC00+(n)) /* n=3..31 */
#define CSR_CYCLEH		0xC80
#define CSR_TIMEH		0xC81
#define CSR_INSTRETH		0xC82
#define CSR_HPMCOUNTERH(n)	(0xC80+(n)) /* n=3..31 */
#ifndef CONFIG_ARCH_HAS_NOSEE
#define CSR_SSTATUS		0x100
#define CSR_SEDELEG		0x102
#define CSR_SIDELEG		0x103
#define CSR_SIE			0x104
#define CSR_STVEC		0x105
#define CSR_SCOUNTEREN		0x106
#define CSR_SSCRATCH		0x140
#define CSR_SEPC		0x141
#define CSR_SCAUSE		0x142
#define CSR_STVAL		0x143
#define CSR_SIP			0x144
#define CSR_SATP		0x180
#endif
#ifdef CONFIG_CPU_H
#define CSR_HSTATUS		0xA00
#define CSR_HEDELEG		0xA02
#define CSR_HIDELEG		0xA03
#define CSR_HGATP		0xA80
#define CSR_BSSTATUS		0x200
#define CSR_BSIE		0x204
#define CSR_BSTVEC		0x205
#define CSR_BSSCRATCH		0x240
#define CSR_BSEPC		0x241
#define CSR_BSCAUSE		0x242
#define CSR_BSTVAL		0x243
#define CSR_BSIP		0x244
#define CSR_BSATP		0x280
#endif
#define CSR_MVENDORID		0xF11
#define CSR_MARCHID		0xF12
#define CSR_MIMPID		0xF13
#define CSR_MHARTID		0xF14
#define CSR_MSTATUS		0x300
#define CSR_MISA		0x301
#define CSR_MEDELEG		0x302
#define CSR_MIDELEG		0x303
#define CSR_MIE			0x304
#define CSR_MTVEC		0x305
#define CSR_MCOUNTEREN		0x306
#define CSR_MSCRATCH		0x340
#define CSR_MEPC		0x341
#define CSR_MCAUSE		0x342
#define CSR_MTVAL		0x343
#define CSR_MIP			0x344
#ifdef CONFIG_CPU_PMP
#define CSR_PMPCFG(n)		(0x3A0+(n)) /* n=0..3 */
#define CSR_PMPADDR(n)		(0x3B0+(n)) /* n=0..15 */
#endif
#define CSR_MCYCLE		0xB00
#define CSR_MINSTRET		0xB02
#define CSR_MHPMCOUNTER(n)	(0xB03+(n)) /* n=3..31 */
#define CSR_MCYCLEH		0xB80
#define CSR_MINSTRETH		0xB82
#define CSR_MHPMCOUNTERH(n)	(0xB80+(n)) /* n=3..31 */
#define CSR_MCOUNTINHIBIT	0x320
#define CSR_MHPMEVENT(n)	(0x320+(n)) /* n=3..31 */
#define CSR_TSELECT		0x7A0
#define CSR_TDATA1		0x7A1
#define CSR_TDATA2		0x7A2
#define CSR_TDATA3		0x7A3
#define CSR_DCSR		0x7B0
#define CSR_DPC			0x7B1
#define CSR_DSCRATCH0		0x7B2
#define CSR_DSCRATCH1		0x7B3

#ifndef __ASSEMBLY__
#define csr_swap(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	asm volatile("csrrw	%0, " __ASM_STR(csr) ", %1"	\
		     : "=r" (__v) : "rK" (__v)	: "memory");	\
	__v;							\
})
#define csr_read(csr)						\
({								\
	register unsigned long __v;				\
	asm volatile("csrr	%0, " __ASM_STR(csr)		\
		     : "=r" (__v) : : "memory");		\
	__v;							\
})
#define csr_write(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	asm volatile("csrw	" __ASM_STR(csr) ", %0"		\
		     : : "rK" (__v) : "memory");		\
})
#define csr_read_set(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	asm volatile("csrrs	%0, " __ASM_STR(csr) ", %1"	\
		     : "=r" (__v) : "rK" (__v)	: "memory");	\
	__v;							\
})
#define csr_set(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	asm volatile("csrs	" __ASM_STR(csr) ", %0"		\
		     : : "rK" (__v) : "memory");		\
})
#define csr_read_clear(csr, val)				\
({								\
	unsigned long __v = (unsigned long)(val);		\
	asm volatile("csrrc	%0, " __ASM_STR(csr) ", %1"	\
		     : "=r" (__v) : "rK" (__v) : "memory");	\
	__v;							\
})
#define csr_clear(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	asm volatile("csrc	" __ASM_STR(csr) ", %0"		\
		     : : "rK" (__v) : "memory");		\
})
#endif /* __ASSEMBLY__ */

#endif /* __CSR_RISCV_H_INCLUDE__ */
