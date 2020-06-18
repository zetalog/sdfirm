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

#define CSR_USTATUS		0x000
#define CSR_UIE			0x004
#define CSR_UTVEC		0x005
#define CSR_USCRATCH		0x040
#define CSR_UEPC		0x041
#define CSR_UCAUSE		0x042
#define CSR_UTVAL		0x043
#define CSR_UIP			0x044
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
#ifdef CONFIG_CPU_S
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

/* MSTATUS/SSTATUS/HSTATUS/BSSTATUS */
#define SR_UIE		_AC(0x00000001, UL) /* User Interrupt Enable */
#define SR_UPIE		_AC(0x00000010, UL) /* User Previous IE */
#define SR_MIE		_AC(0x00000008, UL) /* Machine Interrupt Enable */
#define SR_MPIE		_AC(0x00000080, UL) /* Machine Previous IE */
#define SR_MPP_SHIFT	11
#define SR_MPP		_AC(0x00001800, UL) /* Previously Machine */
#ifdef CONFIG_CPU_S
#define SR_SIE		_AC(0x00000002, UL) /* Supervisor Interrupt Enable */
#define SR_SPIE		_AC(0x00000020, UL) /* Supervisor Previous IE */
#define SR_SPP_SHIFT	8
#define SR_SPP		_AC(0x00000100, UL) /* Previously Supervisor */
#define SR_SUM		_AC(0x00040000, UL) /* Supervisor User Memory Access */
#endif
#ifdef CONFIG_CPU_H
#define SR_HIE		_AC(0x00000004, UL) /* Hypervisor Interrupt Enable */
#define SR_HPIE		_AC(0x00000040, UL) /* Hypervisor Previous IE */
#define SR_HPP		_AC(0x00000600, UL) /* Previously Hypervisor */
#endif

#define SR_FS		_AC(0x00006000, UL) /* Floating-point Status */
#ifdef CONFIG_CPU_F
#define SR_FS_OFF	_AC(0x00000000, UL)
#define SR_FS_INITIAL	_AC(0x00002000, UL)
#define SR_FS_CLEAN	_AC(0x00004000, UL)
#define SR_FS_DIRTY	_AC(0x00006000, UL)
#endif

#define SR_VS		_AC(0x01800000, UL) /* Vector extension */
#ifdef CONFIG_CPU_V
#define SR_VS_OFF	_AC(0x00000000, UL)
#define SR_VS_INITIAL	_AC(0x00800000, UL)
#define SR_VS_CLEAN	_AC(0x01000000, UL)
#define SR_VS_DIRTY	_AC(0x01800000, UL)
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

/* MIP/MIE/SIP/SIE/BSIP/BSIE */
#define IR_USI			(_AC(0x1, UL) << IRQ_U_SOFT)
#define IR_UTI			(_AC(0x1, UL) << IRQ_U_TIMER)
#define IR_UEI			(_AC(0x1, UL) << IRQ_U_EXT)
#define IR_SSI			(_AC(0x1, UL) << IRQ_S_SOFT)
#define IR_STI			(_AC(0x1, UL) << IRQ_S_TIMER)
#define IR_SEI			(_AC(0x1, UL) << IRQ_S_EXT)
#define IR_HSI			(_AC(0x1, UL) << IRQ_H_SOFT)
#define IR_HTI			(_AC(0x1, UL) << IRQ_H_TIMER)
#define IR_HEI			(_AC(0x1, UL) << IRQ_H_EXT)
#define IR_MSI			(_AC(0x1, UL) << IRQ_M_SOFT)
#define IR_MTI			(_AC(0x1, UL) << IRQ_M_TIMER)
#define IR_MEI			(_AC(0x1, UL) << IRQ_M_EXT)

/* SATP/HGATP */
#ifndef CONFIG_64BIT
#define ATP_PPN		_AC(0x003FFFFF, UL)
#define ATP_MODE_32	_AC(0x80000000, UL)
#else
#define ATP_PPN		_AC(0x00000FFFFFFFFFFF, UL)
#define ATP_MODE_39	_AC(0x8000000000000000, UL)
#define ATP_MODE_48	_AC(0x9000000000000000, UL)
#endif

/* MCAUSE/SCAUSE/BSCAUSE */
#define ICR_IRQ_FLAG			(_AC(1, UL) << (__riscv_xlen - 1))

#define PRV_U				0
#define PRV_S				1
#define PRV_H				2
#define PRV_M				3

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

#ifdef CONFIG_RISCV_EXIT_M
#define CSR_STATUS	CSR_MSTATUS
#define CSR_IE		CSR_MIE
#define CSR_TVEC	CSR_MTVEC
#define CSR_SCRATCH	CSR_MSCRATCH
#define CSR_EPC		CSR_MEPC
#define CSR_CAUSE	CSR_MCAUSE
#define CSR_TVAL	CSR_MTVAL
#define CSR_IP		CSR_MIP

#define SR_IE		SR_MIE
#define SR_PIE		SR_MPIE
#define SR_PP		SR_MPP
#endif
#ifdef CONFIG_RISCV_EXIT_S
#define CSR_STATUS	CSR_SSTATUS
#define CSR_IE		CSR_SIE
#define CSR_TVEC	CSR_STVEC
#define CSR_SCRATCH	CSR_SSCRATCH
#define CSR_EPC		CSR_SEPC
#define CSR_CAUSE	CSR_SCAUSE
#define CSR_TVAL	CSR_STVAL
#define CSR_IP		CSR_SIP

#define SR_IE		SR_SIE
#define SR_PIE		SR_SPIE
#define SR_PP		SR_SPP
#endif

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

static __inline int misa_extension(char ext)
{
	return csr_read(CSR_MISA) & (1 << (ext - 'A'));
}

static __inline int misa_xlen(void)
{
	return ((long)csr_read(CSR_MISA) < 0) ? 64 : 32;
}

static __inline void misa_string(char *out, unsigned int out_sz)
{
	unsigned long i, val = csr_read(CSR_MISA);

	for (i = 0; i < 26; i++) {
		if (val & (1 << i)) {
			*out = 'A' + i;
			out++;
		}
	}
	*out = '\0';
	out++;
}
#endif /* __ASSEMBLY__ */

#endif /* __CSR_RISCV_H_INCLUDE__ */
