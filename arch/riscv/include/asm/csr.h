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
 * @(#)csr.h: RISCV specific control register interface
 * $Id: csr.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __CSR_RISCV_H_INCLUDE__
#define __CSR_RISCV_H_INCLUDE__

#include <target/const.h>
#include <asm/assembler.h>

/* Status register flags */
#define SR_SIE		_AC(0x00000002, UL) /* Supervisor Interrupt Enable */
#define SR_SPIE		_AC(0x00000020, UL) /* Previous Supervisor IE */
#define SR_SPP		_AC(0x00000100, UL) /* Previously Supervisor */
#define SR_SUM		_AC(0x00040000, UL) /* Supervisor User Memory Access */

#define SR_FS		_AC(0x00006000, UL) /* Floating-point Status */
#define SR_FS_OFF	_AC(0x00000000, UL)
#define SR_FS_INITIAL	_AC(0x00002000, UL)
#define SR_FS_CLEAN	_AC(0x00004000, UL)
#define SR_FS_DIRTY	_AC(0x00006000, UL)

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
#define IRQ_M_SOFT		3
#define IRQ_U_TIMER		4
#define IRQ_S_TIMER		5
#define IRQ_M_TIMER		7
#define IRQ_U_EXT		8
#define IRQ_S_EXT		9
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

/* SIE (Interrupt Enable) and SIP (Interrupt Pending) flags */
#define SIE_SSIE		(_AC(0x1, UL) << IRQ_S_SOFT)
#define SIE_STIE		(_AC(0x1, UL) << IRQ_S_TIMER)
#define SIE_SEIE		(_AC(0x1, UL) << IRQ_S_EXT)

#define CSR_CYCLE		0xc00
#define CSR_TIME		0xc01
#define CSR_INSTRET		0xc02
#define CSR_SSTATUS		0x100
#define CSR_SIE			0x104
#define CSR_STVEC		0x105
#define CSR_SCOUNTEREN		0x106
#define CSR_SSCRATCH		0x140
#define CSR_SEPC		0x141
#define CSR_SCAUSE		0x142
#define CSR_STVAL		0x143
#define CSR_SIP			0x144
#define CSR_SATP		0x180
#define CSR_CYCLEH		0xc80
#define CSR_TIMEH		0xc81
#define CSR_INSTRETH		0xc82

#ifndef __ASSEMBLY__

#define csr_swap(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrw %0, " __ASM_STR(csr) ", %1"\
			      : "=r" (__v) : "rK" (__v)		\
			      : "memory");			\
	__v;							\
})

#define csr_read(csr)						\
({								\
	register unsigned long __v;				\
	__asm__ __volatile__ ("csrr %0, " __ASM_STR(csr)	\
			      : "=r" (__v) :			\
			      : "memory");			\
	__v;							\
})

#define csr_write(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrw " __ASM_STR(csr) ", %0"	\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#define csr_read_set(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrs %0, " __ASM_STR(csr) ", %1"\
			      : "=r" (__v) : "rK" (__v)		\
			      : "memory");			\
	__v;							\
})

#define csr_set(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrs " __ASM_STR(csr) ", %0"	\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#define csr_read_clear(csr, val)				\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrrc %0, " __ASM_STR(csr) ", %1"\
			      : "=r" (__v) : "rK" (__v)		\
			      : "memory");			\
	__v;							\
})

#define csr_clear(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	__asm__ __volatile__ ("csrc " __ASM_STR(csr) ", %0"	\
			      : : "rK" (__v)			\
			      : "memory");			\
})

#endif /* __ASSEMBLY__ */

#endif /* __CSR_RISCV_H_INCLUDE__ */
