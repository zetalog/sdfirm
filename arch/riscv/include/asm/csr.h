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

#define MSTATUS_UIE			0x00000001
#define MSTATUS_SIE			0x00000002
#define MSTATUS_HIE			0x00000004
#define MSTATUS_MIE			0x00000008
#define MSTATUS_UPIE			0x00000010
#define MSTATUS_SPIE_SHIFT		5
#define MSTATUS_SPIE			(1UL << MSTATUS_SPIE_SHIFT)
#define MSTATUS_HPIE			0x00000040
#define MSTATUS_MPIE			0x00000080
#define MSTATUS_SPP_SHIFT		8
#define MSTATUS_SPP			(1UL << MSTATUS_SPP_SHIFT)
#define MSTATUS_HPP			0x00000600
#define MSTATUS_MPP_SHIFT		11
#define MSTATUS_MPP			(3UL << MSTATUS_MPP_SHIFT)
#define MSTATUS_FS			0x00006000
#define MSTATUS_XS			0x00018000
#define MSTATUS_MPRV			0x00020000
#define MSTATUS_SUM			0x00040000
#define MSTATUS_MXR			0x00080000
#define MSTATUS_TVM			0x00100000
#define MSTATUS_TW			0x00200000
#define MSTATUS_TSR			0x00400000
#define MSTATUS32_SD			0x80000000
#define MSTATUS_UXL			0x0000000300000000
#define MSTATUS_SXL			0x0000000C00000000
#define MSTATUS64_SD			0x8000000000000000

#define SSTATUS_UIE			0x00000001
#define SSTATUS_SIE			0x00000002
#define SSTATUS_UPIE			0x00000010
#define SSTATUS_SPIE			0x00000020
#define SSTATUS_SPP			0x00000100
#define SSTATUS_FS			0x00006000
#define SSTATUS_XS			0x00018000
#define SSTATUS_SUM			0x00040000
#define SSTATUS_MXR			0x00080000
#define SSTATUS32_SD			0x80000000
#define SSTATUS_UXL			0x0000000300000000
#define SSTATUS64_SD			0x8000000000000000

#define DCSR_XDEBUGVER			(3U<<30)
#define DCSR_NDRESET			(1<<29)
#define DCSR_FULLRESET			(1<<28)
#define DCSR_EBREAKM			(1<<15)
#define DCSR_EBREAKH			(1<<14)
#define DCSR_EBREAKS			(1<<13)
#define DCSR_EBREAKU			(1<<12)
#define DCSR_STOPCYCLE			(1<<10)
#define DCSR_STOPTIME			(1<<9)
#define DCSR_CAUSE			(7<<6)
#define DCSR_DEBUGINT			(1<<5)
#define DCSR_HALT			(1<<3)
#define DCSR_STEP			(1<<2)
#define DCSR_PRV			(3<<0)

#define DCSR_CAUSE_NONE			0
#define DCSR_CAUSE_SWBP			1
#define DCSR_CAUSE_HWBP			2
#define DCSR_CAUSE_DEBUGINT		3
#define DCSR_CAUSE_STEP			4
#define DCSR_CAUSE_HALT			5

#define MCONTROL_TYPE(xlen)		(0xfULL<<((xlen)-4))
#define MCONTROL_DMODE(xlen)		(1ULL<<((xlen)-5))
#define MCONTROL_MASKMAX(xlen)		(0x3fULL<<((xlen)-11))

#define MCONTROL_SELECT			(1<<19)
#define MCONTROL_TIMING			(1<<18)
#define MCONTROL_ACTION			(0x3f<<12)
#define MCONTROL_CHAIN			(1<<11)
#define MCONTROL_MATCH			(0xf<<7)
#define MCONTROL_M			(1<<6)
#define MCONTROL_H			(1<<5)
#define MCONTROL_S			(1<<4)
#define MCONTROL_U			(1<<3)
#define MCONTROL_EXECUTE		(1<<2)
#define MCONTROL_STORE			(1<<1)
#define MCONTROL_LOAD			(1<<0)

#define MCONTROL_TYPE_NONE		0
#define MCONTROL_TYPE_MATCH		2

#define MCONTROL_ACTION_DEBUG_EXCEPTION	0
#define MCONTROL_ACTION_DEBUG_MODE	1
#define MCONTROL_ACTION_TRACE_START	2
#define MCONTROL_ACTION_TRACE_STOP	3
#define MCONTROL_ACTION_TRACE_EMIT	4

#define MCONTROL_MATCH_EQUAL		0
#define MCONTROL_MATCH_NAPOT		1
#define MCONTROL_MATCH_GE		2
#define MCONTROL_MATCH_LT		3
#define MCONTROL_MATCH_MASK_LOW		4
#define MCONTROL_MATCH_MASK_HIGH	5

#define IRQ_S_SOFT			1
#define IRQ_H_SOFT			2
#define IRQ_M_SOFT			3
#define IRQ_S_TIMER			5
#define IRQ_H_TIMER			6
#define IRQ_M_TIMER			7
#define IRQ_S_EXT			9
#define IRQ_H_EXT			10
#define IRQ_M_EXT			11
#define IRQ_COP				12
#define IRQ_HOST			13

#define MIP_SSIP			(1 << IRQ_S_SOFT)
#define MIP_HSIP			(1 << IRQ_H_SOFT)
#define MIP_MSIP			(1 << IRQ_M_SOFT)
#define MIP_STIP			(1 << IRQ_S_TIMER)
#define MIP_HTIP			(1 << IRQ_H_TIMER)
#define MIP_MTIP			(1 << IRQ_M_TIMER)
#define MIP_SEIP			(1 << IRQ_S_EXT)
#define MIP_HEIP			(1 << IRQ_H_EXT)
#define MIP_MEIP			(1 << IRQ_M_EXT)

#define SIP_SSIP			MIP_SSIP
#define SIP_STIP			MIP_STIP

#define PRV_U				0
#define PRV_S				1
#define PRV_H				2
#define PRV_M				3

#define SATP32_ASID			0x7FC00000
#define SATP32_PPN			0x003FFFFF
#define SATP64_ASID			0x0FFFF00000000000
#define SATP64_PPN			0x00000FFFFFFFFFFF

#define SATP_MODE_OFF			0
#define SATP_MODE_SV32			1
#define SATP_MODE_SV39			8
#define SATP_MODE_SV48			9
#define SATP_MODE_SV57			10
#define SATP_MODE_SV64			11

#define PMP_R				0x01
#define PMP_W				0x02
#define PMP_X				0x04
#define PMP_A				0x18
#define PMP_A_TOR			0x08
#define PMP_A_NA4			0x10
#define PMP_A_NAPOT			0x18
#define PMP_L				0x80

#define PMP_SHIFT			2
#define PMP_COUNT			16

/* page table entry (PTE) fields */
#define PTE_V				0x001 /* Valid */
#define PTE_R				0x002 /* Read */
#define PTE_W				0x004 /* Write */
#define PTE_X				0x008 /* Execute */
#define PTE_U				0x010 /* User */
#define PTE_G				0x020 /* Global */
#define PTE_A				0x040 /* Accessed */
#define PTE_D				0x080 /* Dirty */
#define PTE_SOFT			0x300 /* Reserved for Software */

#define PTE_PPN_SHIFT			10

#define PTE_TABLE(PTE)			\
	(((PTE) & (PTE_V | PTE_R | PTE_W | PTE_X)) == PTE_V)

#if __riscv_xlen == 64
#define MSTATUS_SD			MSTATUS64_SD
#define SSTATUS_SD			SSTATUS64_SD
#define RISCV_PGLEVEL_BITS		9
#else
#define MSTATUS_SD			MSTATUS32_SD
#define SSTATUS_SD			SSTATUS32_SD
#define RISCV_PGLEVEL_BITS		10
#endif
#define RISCV_PGSHIFT			12
#define RISCV_PGSIZE			(1 << RISCV_PGSHIFT)

#define CSR_USTATUS			0x0
#define CSR_UIE				0x4
#define CSR_UTVEC			0x5
#define CSR_USCRATCH			0x40
#define CSR_UEPC			0x41
#define CSR_UCAUSE			0x42
#define CSR_UTVAL			0x43
#define CSR_UIP				0x44
#define CSR_HPMCOUNTER3			0xc03
#define CSR_HPMCOUNTER4			0xc04
#define CSR_HPMCOUNTER5			0xc05
#define CSR_HPMCOUNTER6			0xc06
#define CSR_HPMCOUNTER7			0xc07
#define CSR_HPMCOUNTER8			0xc08
#define CSR_HPMCOUNTER9			0xc09
#define CSR_HPMCOUNTER10		0xc0a
#define CSR_HPMCOUNTER11		0xc0b
#define CSR_HPMCOUNTER12		0xc0c
#define CSR_HPMCOUNTER13		0xc0d
#define CSR_HPMCOUNTER14		0xc0e
#define CSR_HPMCOUNTER15		0xc0f
#define CSR_HPMCOUNTER16		0xc10
#define CSR_HPMCOUNTER17		0xc11
#define CSR_HPMCOUNTER18		0xc12
#define CSR_HPMCOUNTER19		0xc13
#define CSR_HPMCOUNTER20		0xc14
#define CSR_HPMCOUNTER21		0xc15
#define CSR_HPMCOUNTER22		0xc16
#define CSR_HPMCOUNTER23		0xc17
#define CSR_HPMCOUNTER24		0xc18
#define CSR_HPMCOUNTER25		0xc19
#define CSR_HPMCOUNTER26		0xc1a
#define CSR_HPMCOUNTER27		0xc1b
#define CSR_HPMCOUNTER28		0xc1c
#define CSR_HPMCOUNTER29		0xc1d
#define CSR_HPMCOUNTER30		0xc1e
#define CSR_HPMCOUNTER31		0xc1f
#define CSR_SSTATUS			0x100
#define CSR_SIE				0x104
#define CSR_STVEC			0x105
#define CSR_SCOUNTEREN			0x106
#define CSR_SSCRATCH			0x140
#define CSR_SEPC			0x141
#define CSR_SCAUSE			0x142
#define CSR_STVAL			0x143
#define CSR_SIP				0x144
#define CSR_SATP			0x180
#define CSR_MSTATUS			0x300
#define CSR_MISA			0x301
#define CSR_MEDELEG			0x302
#define CSR_MIDELEG			0x303
#define CSR_MIE				0x304
#define CSR_MTVEC			0x305
#define CSR_MCOUNTEREN			0x306
#define CSR_MSCRATCH			0x340
#define CSR_MEPC			0x341
#define CSR_MCAUSE			0x342
#define CSR_MTVAL			0x343
#define CSR_MIP				0x344
#define CSR_PMPCFG0			0x3a0
#define CSR_PMPCFG1			0x3a1
#define CSR_PMPCFG2			0x3a2
#define CSR_PMPCFG3			0x3a3
#define CSR_PMPADDR0			0x3b0
#define CSR_PMPADDR1			0x3b1
#define CSR_PMPADDR2			0x3b2
#define CSR_PMPADDR3			0x3b3
#define CSR_PMPADDR4			0x3b4
#define CSR_PMPADDR5			0x3b5
#define CSR_PMPADDR6			0x3b6
#define CSR_PMPADDR7			0x3b7
#define CSR_PMPADDR8			0x3b8
#define CSR_PMPADDR9			0x3b9
#define CSR_PMPADDR10			0x3ba
#define CSR_PMPADDR11			0x3bb
#define CSR_PMPADDR12			0x3bc
#define CSR_PMPADDR13			0x3bd
#define CSR_PMPADDR14			0x3be
#define CSR_PMPADDR15			0x3bf
#define CSR_DSCRATCH			0x7b2
#define CSR_MHPMCOUNTER3		0xb03
#define CSR_MHPMCOUNTER4		0xb04
#define CSR_MHPMCOUNTER5		0xb05
#define CSR_MHPMCOUNTER6		0xb06
#define CSR_MHPMCOUNTER7		0xb07
#define CSR_MHPMCOUNTER8		0xb08
#define CSR_MHPMCOUNTER9		0xb09
#define CSR_MHPMCOUNTER10		0xb0a
#define CSR_MHPMCOUNTER11		0xb0b
#define CSR_MHPMCOUNTER12		0xb0c
#define CSR_MHPMCOUNTER13		0xb0d
#define CSR_MHPMCOUNTER14		0xb0e
#define CSR_MHPMCOUNTER15		0xb0f
#define CSR_MHPMCOUNTER16		0xb10
#define CSR_MHPMCOUNTER17		0xb11
#define CSR_MHPMCOUNTER18		0xb12
#define CSR_MHPMCOUNTER19		0xb13
#define CSR_MHPMCOUNTER20		0xb14
#define CSR_MHPMCOUNTER21		0xb15
#define CSR_MHPMCOUNTER22		0xb16
#define CSR_MHPMCOUNTER23		0xb17
#define CSR_MHPMCOUNTER24		0xb18
#define CSR_MHPMCOUNTER25		0xb19
#define CSR_MHPMCOUNTER26		0xb1a
#define CSR_MHPMCOUNTER27		0xb1b
#define CSR_MHPMCOUNTER28		0xb1c
#define CSR_MHPMCOUNTER29		0xb1d
#define CSR_MHPMCOUNTER30		0xb1e
#define CSR_MHPMCOUNTER31		0xb1f
#define CSR_MHPMEVENT3			0x323
#define CSR_MHPMEVENT4			0x324
#define CSR_MHPMEVENT5			0x325
#define CSR_MHPMEVENT6			0x326
#define CSR_MHPMEVENT7			0x327
#define CSR_MHPMEVENT8			0x328
#define CSR_MHPMEVENT9			0x329
#define CSR_MHPMEVENT10			0x32a
#define CSR_MHPMEVENT11			0x32b
#define CSR_MHPMEVENT12			0x32c
#define CSR_MHPMEVENT13			0x32d
#define CSR_MHPMEVENT14			0x32e
#define CSR_MHPMEVENT15			0x32f
#define CSR_MHPMEVENT16			0x330
#define CSR_MHPMEVENT17			0x331
#define CSR_MHPMEVENT18			0x332
#define CSR_MHPMEVENT19			0x333
#define CSR_MHPMEVENT20			0x334
#define CSR_MHPMEVENT21			0x335
#define CSR_MHPMEVENT22			0x336
#define CSR_MHPMEVENT23			0x337
#define CSR_MHPMEVENT24			0x338
#define CSR_MHPMEVENT25			0x339
#define CSR_MHPMEVENT26			0x33a
#define CSR_MHPMEVENT27			0x33b
#define CSR_MHPMEVENT28			0x33c
#define CSR_MHPMEVENT29			0x33d
#define CSR_MHPMEVENT30			0x33e
#define CSR_MHPMEVENT31			0x33f
#define CSR_HPMCOUNTER3H		0xc83
#define CSR_HPMCOUNTER4H		0xc84
#define CSR_HPMCOUNTER5H		0xc85
#define CSR_HPMCOUNTER6H		0xc86
#define CSR_HPMCOUNTER7H		0xc87
#define CSR_HPMCOUNTER8H		0xc88
#define CSR_HPMCOUNTER9H		0xc89
#define CSR_HPMCOUNTER10H		0xc8a
#define CSR_HPMCOUNTER11H		0xc8b
#define CSR_HPMCOUNTER12H		0xc8c
#define CSR_HPMCOUNTER13H		0xc8d
#define CSR_HPMCOUNTER14H		0xc8e
#define CSR_HPMCOUNTER15H		0xc8f
#define CSR_HPMCOUNTER16H		0xc90
#define CSR_HPMCOUNTER17H		0xc91
#define CSR_HPMCOUNTER18H		0xc92
#define CSR_HPMCOUNTER19H		0xc93
#define CSR_HPMCOUNTER20H		0xc94
#define CSR_HPMCOUNTER21H		0xc95
#define CSR_HPMCOUNTER22H		0xc96
#define CSR_HPMCOUNTER23H		0xc97
#define CSR_HPMCOUNTER24H		0xc98
#define CSR_HPMCOUNTER25H		0xc99
#define CSR_HPMCOUNTER26H		0xc9a
#define CSR_HPMCOUNTER27H		0xc9b
#define CSR_HPMCOUNTER28H		0xc9c
#define CSR_HPMCOUNTER29H		0xc9d
#define CSR_HPMCOUNTER30H		0xc9e
#define CSR_HPMCOUNTER31H		0xc9f
#define CSR_MHPMCOUNTER3H		0xb83
#define CSR_MHPMCOUNTER4H		0xb84
#define CSR_MHPMCOUNTER5H		0xb85
#define CSR_MHPMCOUNTER6H		0xb86
#define CSR_MHPMCOUNTER7H		0xb87
#define CSR_MHPMCOUNTER8H		0xb88
#define CSR_MHPMCOUNTER9H		0xb89
#define CSR_MHPMCOUNTER10H		0xb8a
#define CSR_MHPMCOUNTER11H		0xb8b
#define CSR_MHPMCOUNTER12H		0xb8c
#define CSR_MHPMCOUNTER13H		0xb8d
#define CSR_MHPMCOUNTER14H		0xb8e
#define CSR_MHPMCOUNTER15H		0xb8f
#define CSR_MHPMCOUNTER16H		0xb90
#define CSR_MHPMCOUNTER17H		0xb91
#define CSR_MHPMCOUNTER18H		0xb92
#define CSR_MHPMCOUNTER19H		0xb93
#define CSR_MHPMCOUNTER20H		0xb94
#define CSR_MHPMCOUNTER21H		0xb95
#define CSR_MHPMCOUNTER22H		0xb96
#define CSR_MHPMCOUNTER23H		0xb97
#define CSR_MHPMCOUNTER24H		0xb98
#define CSR_MHPMCOUNTER25H		0xb99
#define CSR_MHPMCOUNTER26H		0xb9a
#define CSR_MHPMCOUNTER27H		0xb9b
#define CSR_MHPMCOUNTER28H		0xb9c
#define CSR_MHPMCOUNTER29H		0xb9d
#define CSR_MHPMCOUNTER30H		0xb9e
#define CSR_MHPMCOUNTER31H		0xb9f

#define CAUSE_MISALIGNED_FETCH		0x0
#define CAUSE_FETCH_ACCESS		0x1
#define CAUSE_ILLEGAL_INSTRUCTION	0x2
#define CAUSE_BREAKPOINT		0x3
#define CAUSE_MISALIGNED_LOAD		0x4
#define CAUSE_LOAD_ACCESS		0x5
#define CAUSE_MISALIGNED_STORE		0x6
#define CAUSE_STORE_ACCESS		0x7
#define CAUSE_USER_ECALL		0x8
#define CAUSE_HYPERVISOR_ECALL		0x9
#define CAUSE_SUPERVISOR_ECALL		0xa
#define CAUSE_MACHINE_ECALL		0xb
#define CAUSE_FETCH_PAGE_FAULT		0xc
#define CAUSE_LOAD_PAGE_FAULT		0xd
#define CAUSE_STORE_PAGE_FAULT		0xf

#define INSN_MATCH_LB			0x3
#define INSN_MASK_LB			0x707f
#define INSN_MATCH_LH			0x1003
#define INSN_MASK_LH			0x707f
#define INSN_MATCH_LW			0x2003
#define INSN_MASK_LW			0x707f
#define INSN_MATCH_LD			0x3003
#define INSN_MASK_LD			0x707f
#define INSN_MATCH_LBU			0x4003
#define INSN_MASK_LBU			0x707f
#define INSN_MATCH_LHU			0x5003
#define INSN_MASK_LHU			0x707f
#define INSN_MATCH_LWU			0x6003
#define INSN_MASK_LWU			0x707f
#define INSN_MATCH_SB			0x23
#define INSN_MASK_SB			0x707f
#define INSN_MATCH_SH			0x1023
#define INSN_MASK_SH			0x707f
#define INSN_MATCH_SW			0x2023
#define INSN_MASK_SW			0x707f
#define INSN_MATCH_SD			0x3023
#define INSN_MASK_SD			0x707f

#define INSN_MATCH_FLW			0x2007
#define INSN_MASK_FLW			0x707f
#define INSN_MATCH_FLD			0x3007
#define INSN_MASK_FLD			0x707f
#define INSN_MATCH_FLQ			0x4007
#define INSN_MASK_FLQ			0x707f
#define INSN_MATCH_FSW			0x2027
#define INSN_MASK_FSW			0x707f
#define INSN_MATCH_FSD			0x3027
#define INSN_MASK_FSD			0x707f
#define INSN_MATCH_FSQ			0x4027
#define INSN_MASK_FSQ			0x707f

#define INSN_MATCH_C_LD			0x6000
#define INSN_MASK_C_LD			0xe003
#define INSN_MATCH_C_SD			0xe000
#define INSN_MASK_C_SD			0xe003
#define INSN_MATCH_C_LW			0x4000
#define INSN_MASK_C_LW			0xe003
#define INSN_MATCH_C_SW			0xc000
#define INSN_MASK_C_SW			0xe003
#define INSN_MATCH_C_LDSP		0x6002
#define INSN_MASK_C_LDSP		0xe003
#define INSN_MATCH_C_SDSP		0xe002
#define INSN_MASK_C_SDSP		0xe003
#define INSN_MATCH_C_LWSP		0x4002
#define INSN_MASK_C_LWSP		0xe003
#define INSN_MATCH_C_SWSP		0xc002
#define INSN_MASK_C_SWSP		0xe003

#define INSN_MATCH_C_FLD		0x2000
#define INSN_MASK_C_FLD			0xe003
#define INSN_MATCH_C_FLW		0x6000
#define INSN_MASK_C_FLW			0xe003
#define INSN_MATCH_C_FSD		0xa000
#define INSN_MASK_C_FSD			0xe003
#define INSN_MATCH_C_FSW		0xe000
#define INSN_MASK_C_FSW			0xe003
#define INSN_MATCH_C_FLDSP		0x2002
#define INSN_MASK_C_FLDSP		0xe003
#define INSN_MATCH_C_FSDSP		0xa002
#define INSN_MASK_C_FSDSP		0xe003
#define INSN_MATCH_C_FLWSP		0x6002
#define INSN_MASK_C_FLWSP		0xe003
#define INSN_MATCH_C_FSWSP		0xe002
#define INSN_MASK_C_FSWSP		0xe003

#define INSN_LEN(insn)			((((insn) & 0x3) < 0x3) ? 2 : 4)

#if __riscv_xlen == 64
#define LOG_REGBYTES			3
#else
#define LOG_REGBYTES			2
#endif
#define REGBYTES			(1 << LOG_REGBYTES)

#define SH_RD				7
#define SH_RS1				15
#define SH_RS2				20
#define SH_RS2C				2

#define RV_X(x, s, n)			(((x) >> (s)) & ((1 << (n)) - 1))
#define RVC_LW_IMM(x)			((RV_X(x, 6, 1) << 2) | \
					 (RV_X(x, 10, 3) << 3) | \
					 (RV_X(x, 5, 1) << 6))
#define RVC_LD_IMM(x)			((RV_X(x, 10, 3) << 3) | \
					 (RV_X(x, 5, 2) << 6))
#define RVC_LWSP_IMM(x)			((RV_X(x, 4, 3) << 2) | \
					 (RV_X(x, 12, 1) << 5) | \
					 (RV_X(x, 2, 2) << 6))
#define RVC_LDSP_IMM(x)			((RV_X(x, 5, 2) << 3) | \
					 (RV_X(x, 12, 1) << 5) | \
					 (RV_X(x, 2, 3) << 6))
#define RVC_SWSP_IMM(x)			((RV_X(x, 9, 4) << 2) | \
					 (RV_X(x, 7, 2) << 6))
#define RVC_SDSP_IMM(x)			((RV_X(x, 10, 3) << 3) | \
					 (RV_X(x, 7, 3) << 6))
#define RVC_RS1S(insn)			(8 + RV_X(insn, SH_RD, 3))
#define RVC_RS2S(insn)			(8 + RV_X(insn, SH_RS2C, 3))
#define RVC_RS2(insn)			RV_X(insn, SH_RS2C, 5)

#define SHIFT_RIGHT(x, y)		\
	((y) < 0 ? ((x) << -(y)) : ((x) >> (y)))

#define REG_MASK			\
	((1 << (5 + LOG_REGBYTES)) - (1 << LOG_REGBYTES))

#define REG_OFFSET(insn, pos)		\
	(SHIFT_RIGHT((insn), (pos) - LOG_REGBYTES) & REG_MASK)

#define REG_PTR(insn, pos, regs)	\
	(ulong *)((ulong)(regs) + REG_OFFSET(insn, pos))

#define GET_RM(insn)			(((insn) >> 12) & 7)

#define GET_RS1(insn, regs)		(*REG_PTR(insn, SH_RS1, regs))
#define GET_RS2(insn, regs)		(*REG_PTR(insn, SH_RS2, regs))
#define GET_RS1S(insn, regs)		(*REG_PTR(RVC_RS1S(insn), 0, regs))
#define GET_RS2S(insn, regs)		(*REG_PTR(RVC_RS2S(insn), 0, regs))
#define GET_RS2C(insn, regs)		(*REG_PTR(insn, SH_RS2C, regs))
#define GET_SP(regs)			(*REG_PTR(2, 0, regs))
#define SET_RD(insn, regs, val)		(*REG_PTR(insn, SH_RD, regs) = (val))
#define IMM_I(insn)			((s32)(insn) >> 20)
#define IMM_S(insn)			(((s32)(insn) >> 25 << 5) | \
					 (s32)(((insn) >> 7) & 0x1f))
#define MASK_FUNCT3			0x7000

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
