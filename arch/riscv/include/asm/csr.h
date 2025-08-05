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
#endif /* CONFIG_CPU_F */
#define CSR_CYCLE		0xC00
#define CSR_TIME		0xC01
#define CSR_INSTRET		0xC02
#define CSR_UHPMCOUNTER(n)	(0xC00+(n)) /* n=3..31 */
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
#define CSR_SCONTEXT		0x5A8
#endif /* CONFIG_CPU_S */
#ifdef CONFIG_CPU_H
#define CSR_HSTATUS		0x600
#define CSR_HEDELEG		0x602
#define CSR_HIDELEG		0x603
#define CSR_HIE			0x604
#define CSR_HCOUNNTEREN		0x606
#define CSR_HTVAL		0x643
#define CSR_HIP			0x644
#define CSR_HVIP		0x645
#define CSR_HTINST		0x64A
#define CSR_HGEIP		0xE12
#define CSR_HENVCFG		0x60A
#define CSR_HENVCFGH		0x61A
#define CSR_HGATP		0x680
#define CSR_HCONTEXT		0x6A8
#define CSR_HTIMEDELTA		0x605
#define CSR_HTIMEDELTAH		0x615
#define CSR_VSSTATUS		0x200
#define CSR_VSIE		0x204
#define CSR_VSTVEC		0x205
#define CSR_VSSCRATCH		0x240
#define CSR_VSEPC		0x241
#define CSR_VSCAUSE		0x242
#define CSR_VSTVAL		0x243
#define CSR_VSIP		0x244
#define CSR_VSATP		0x280
#endif /* CONFIG_CPU_H */
#define CSR_MVENDORID		0xF11
#define CSR_MARCHID		0xF12
#define CSR_MIMPID		0xF13
#define CSR_MHARTID		0xF14
#define CSR_MCONFIGPTR		0xF15
#define CSR_MSTATUS		0x300
#define CSR_MISA		0x301
#define CSR_MEDELEG		0x302
#define CSR_MIDELEG		0x303
#define CSR_MIE			0x304
#define CSR_MTVEC		0x305
#define CSR_MCOUNTEREN		0x306
#define CSR_MSTATUSH		0x310
#define CSR_MSCRATCH		0x340
#define CSR_MEPC		0x341
#define CSR_MCAUSE		0x342
#define CSR_MTVAL		0x343
#define CSR_MIP			0x344
#define CSR_MTINST		0x34A
#define CSR_MTVAL2		0x34B
#define CSR_MIPH		0x354
#define CSR_MENVCFG		0x30A
#define CSR_MENVCFGH		0x31A
#define CSR_MSECCFG		0x747
#define CSR_MSECCFGH		0x757
#ifdef CONFIG_CPU_PMP
#define CSR_PMPCFG(n)		(0x3A0+(n)) /* n=0..3 */
#define CSR_PMPADDR(n)		(0x3B0+(n)) /* n=0..15 */
/* Machine Memory Protection */
#define CSR_PMPCFG0		0x3a0
#define CSR_PMPCFG1		0x3a1
#define CSR_PMPCFG2		0x3a2
#define CSR_PMPCFG3		0x3a3
#define CSR_PMPCFG4		0x3a4
#define CSR_PMPCFG5		0x3a5
#define CSR_PMPCFG6		0x3a6
#define CSR_PMPCFG7		0x3a7
#define CSR_PMPCFG8		0x3a8
#define CSR_PMPCFG9		0x3a9
#define CSR_PMPCFG10		0x3aa
#define CSR_PMPCFG11		0x3ab
#define CSR_PMPCFG12		0x3ac
#define CSR_PMPCFG13		0x3ad
#define CSR_PMPCFG14		0x3ae
#define CSR_PMPCFG15		0x3af
#define CSR_PMPADDR0		0x3b0
#define CSR_PMPADDR1		0x3b1
#define CSR_PMPADDR2		0x3b2
#define CSR_PMPADDR3		0x3b3
#define CSR_PMPADDR4		0x3b4
#define CSR_PMPADDR5		0x3b5
#define CSR_PMPADDR6		0x3b6
#define CSR_PMPADDR7		0x3b7
#define CSR_PMPADDR8		0x3b8
#define CSR_PMPADDR9		0x3b9
#define CSR_PMPADDR10		0x3ba
#define CSR_PMPADDR11		0x3bb
#define CSR_PMPADDR12		0x3bc
#define CSR_PMPADDR13		0x3bd
#define CSR_PMPADDR14		0x3be
#define CSR_PMPADDR15		0x3bf
#define CSR_PMPADDR16		0x3c0
#define CSR_PMPADDR17		0x3c1
#define CSR_PMPADDR18		0x3c2
#define CSR_PMPADDR19		0x3c3
#define CSR_PMPADDR20		0x3c4
#define CSR_PMPADDR21		0x3c5
#define CSR_PMPADDR22		0x3c6
#define CSR_PMPADDR23		0x3c7
#define CSR_PMPADDR24		0x3c8
#define CSR_PMPADDR25		0x3c9
#define CSR_PMPADDR26		0x3ca
#define CSR_PMPADDR27		0x3cb
#define CSR_PMPADDR28		0x3cc
#define CSR_PMPADDR29		0x3cd
#define CSR_PMPADDR30		0x3ce
#define CSR_PMPADDR31		0x3cf
#define CSR_PMPADDR32		0x3d0
#define CSR_PMPADDR33		0x3d1
#define CSR_PMPADDR34		0x3d2
#define CSR_PMPADDR35		0x3d3
#define CSR_PMPADDR36		0x3d4
#define CSR_PMPADDR37		0x3d5
#define CSR_PMPADDR38		0x3d6
#define CSR_PMPADDR39		0x3d7
#define CSR_PMPADDR40		0x3d8
#define CSR_PMPADDR41		0x3d9
#define CSR_PMPADDR42		0x3da
#define CSR_PMPADDR43		0x3db
#define CSR_PMPADDR44		0x3dc
#define CSR_PMPADDR45		0x3dd
#define CSR_PMPADDR46		0x3de
#define CSR_PMPADDR47		0x3df
#define CSR_PMPADDR48		0x3e0
#define CSR_PMPADDR49		0x3e1
#define CSR_PMPADDR50		0x3e2
#define CSR_PMPADDR51		0x3e3
#define CSR_PMPADDR52		0x3e4
#define CSR_PMPADDR53		0x3e5
#define CSR_PMPADDR54		0x3e6
#define CSR_PMPADDR55		0x3e7
#define CSR_PMPADDR56		0x3e8
#define CSR_PMPADDR57		0x3e9
#define CSR_PMPADDR58		0x3ea
#define CSR_PMPADDR59		0x3eb
#define CSR_PMPADDR60		0x3ec
#define CSR_PMPADDR61		0x3ed
#define CSR_PMPADDR62		0x3ee
#define CSR_PMPADDR63		0x3ef
#endif /* CONFIG_CPU_PMP */
#define CSR_MCYCLE		0xB00
#define CSR_MINSTRET		0xB02
#define CSR_MHPMCOUNTER(n)	(0xB00+(n)) /* n=3..31 */
#define CSR_MHPMCOUNTER3	0xb03
#define CSR_MHPMCOUNTER4	0xb04
#define CSR_MHPMCOUNTER5	0xb05
#define CSR_MHPMCOUNTER6	0xb06
#define CSR_MHPMCOUNTER7	0xb07
#define CSR_MHPMCOUNTER8	0xb08
#define CSR_MHPMCOUNTER9	0xb09
#define CSR_MHPMCOUNTER10	0xb0a
#define CSR_MHPMCOUNTER11	0xb0b
#define CSR_MHPMCOUNTER12	0xb0c
#define CSR_MHPMCOUNTER13	0xb0d
#define CSR_MHPMCOUNTER14	0xb0e
#define CSR_MHPMCOUNTER15	0xb0f
#define CSR_MHPMCOUNTER16	0xb10
#define CSR_MHPMCOUNTER17	0xb11
#define CSR_MHPMCOUNTER18	0xb12
#define CSR_MHPMCOUNTER19	0xb13
#define CSR_MHPMCOUNTER20	0xb14
#define CSR_MHPMCOUNTER21	0xb15
#define CSR_MHPMCOUNTER22	0xb16
#define CSR_MHPMCOUNTER23	0xb17
#define CSR_MHPMCOUNTER24	0xb18
#define CSR_MHPMCOUNTER25	0xb19
#define CSR_MHPMCOUNTER26	0xb1a
#define CSR_MHPMCOUNTER27	0xb1b
#define CSR_MHPMCOUNTER28	0xb1c
#define CSR_MHPMCOUNTER29	0xb1d
#define CSR_MHPMCOUNTER30	0xb1e
#define CSR_MHPMCOUNTER31	0xb1f
#define CSR_MCYCLEH		0xB80
#define CSR_MINSTRETH		0xB82
#define CSR_MHPMCOUNTERH(n)	(0xB80+(n)) /* n=3..31 */

/* Machine Counter Setup */
#define CSR_MCOUNTINHIBIT	0x320
#define CSR_MHPMEVENT(n)	(0x320+(n)) /* n=3..31 */
#define CSR_MHPMEVENT3		0x323
#define CSR_MHPMEVENT4		0x324
#define CSR_MHPMEVENT5		0x325
#define CSR_MHPMEVENT6		0x326
#define CSR_MHPMEVENT7		0x327
#define CSR_MHPMEVENT8		0x328
#define CSR_MHPMEVENT9		0x329
#define CSR_MHPMEVENT10		0x32a
#define CSR_MHPMEVENT11		0x32b
#define CSR_MHPMEVENT12		0x32c
#define CSR_MHPMEVENT13		0x32d
#define CSR_MHPMEVENT14		0x32e
#define CSR_MHPMEVENT15		0x32f
#define CSR_MHPMEVENT16		0x330
#define CSR_MHPMEVENT17		0x331
#define CSR_MHPMEVENT18		0x332
#define CSR_MHPMEVENT19		0x333
#define CSR_MHPMEVENT20		0x334
#define CSR_MHPMEVENT21		0x335
#define CSR_MHPMEVENT22		0x336
#define CSR_MHPMEVENT23		0x337
#define CSR_MHPMEVENT24		0x338
#define CSR_MHPMEVENT25		0x339
#define CSR_MHPMEVENT26		0x33a
#define CSR_MHPMEVENT27		0x33b
#define CSR_MHPMEVENT28		0x33c
#define CSR_MHPMEVENT29		0x33d
#define CSR_MHPMEVENT30		0x33e
#define CSR_MHPMEVENT31		0x33f
#define CSR_MHPMEVENTH(n)	(0x720+(n)) /* n=3..31 */

#define CSR_TSELECT		0x7A0
#define CSR_TDATA1		0x7A1
#define CSR_TDATA2		0x7A2
#define CSR_TDATA3		0x7A3
#define CSR_MCONTEXT		0x7A8
#define CSR_DCSR		0x7B0
#define CSR_DPC			0x7B1
#define CSR_DSCRATCH0		0x7B2
#define CSR_DSCRATCH1		0x7B3
#ifdef CONFIG_CPU_SSTC
#define CSR_STIMECMP		0x14D
#define CSR_STIMECMPH		0x15D
#endif /* CONFIG_CPU_SSTC */
#ifdef CONFIG_CPU_SMSTATEEN
#define CSR_MSTATEEN(n)		(0x30C+(n))
#define CSR_MSTATEENH(n)	(0x31C+(n))
#define CSR_SSTATEEN(n)		(0x10C+(n))
#endif /* CONFIG_CPU_SMSTATEEN */
#ifdef CONFIG_CPU_SSCOFPMF
#define CSR_SCOUNTOVF		0xDA0
#endif /* CONFIG_CPU_SSCOFPMF */
#ifdef CONFIG_CPU_SMAIA
#define CSR_MISELECT		0x350
#define CSR_MIREG		0x351
#define CSR_MTOPI		0xFB0
#define CSR_MVIEN		0x308
#define CSR_MVIP		0x309
#ifdef CONFIG_CPU_IMSIC
#define CSR_MTOPEI		0x35C
#endif /* CONFIG_CPU_IMSIC */
#endif /* CONFIG_CPU_SMAIA */
#ifdef CONFIG_CPU_AIA
#define CSR_SISELECT		0x150
#define CSR_SIREG		0x151
#define CSR_STOPI		0xDB0
#define CSR_HVIEN		0x608
#define CSR_HVICTL		0x609
#define CSR_HVIPRIO1		0x646
#define CSR_HVIPRIO2		0x647
#define CSR_VSISELECT		0x250
#define CSR_VSIREG		0x251
#define CSR_VSTOPI		0xEB0
#ifdef CONFIG_CPU_IMSIC
#define CSR_STOPEI		0x15C
#define CSR_VSTOPEI		0x25C
#endif /* CONFIG_CPU_IMSIC */
#endif /* CONFIG_CPU_AIA */
#ifdef CONFIG_CPU_IMSIC
#define AIA_SETEIPNUM		0x00
#define AIA_IPRIO(n)		(0x30 + (n))
#define AIA_EIDELIVERY		0x70
#define AIA_EITHRESHOLD		0x72
#define AIA_EIP(n)		(0x80 + (n))
#define AIA_EIE(n)		(0xc0 + (n))
#endif /* CONFIG_CPU_IMSIC */

#define CSR_TCMCFG		0x5DB

/* MSTATUS/SSTATUS/HSTATUS/BSSTATUS */
#define SR_UIE		_AC(0x00000001, UL) /* User Interrupt Enable */
#define SR_UPIE		_AC(0x00000010, UL) /* User Previous IE */
#define SR_UBE		_AC(0x00000040, UL)
#ifdef CONFIG_CPU_S
#define SR_SIE		_AC(0x00000002, UL) /* Supervisor Interrupt Enable */
#define SR_SPIE_SHIFT	5
#define SR_SPIE		_AC(0x00000020, UL) /* Supervisor Previous IE */
#define SR_SPP_SHIFT	8
#define SR_SPP		_AC(0x00000100, UL) /* Previously Supervisor */
#define SR_SUM		_AC(0x00040000, UL) /* Supervisor User Memory Access */
#endif /* CONFIG_CPU_S */
#ifdef CONFIG_CPU_H
#define SR_HIE		_AC(0x00000004, UL) /* Hypervisor Interrupt Enable */
#define SR_HPIE		_AC(0x00000040, UL) /* Hypervisor Previous IE */
#define SR_HPP		_AC(0x00000600, UL) /* Previously Hypervisor */
#endif /* CONFIG_CPU_H */
#define SR_MIE		_AC(0x00000008, UL) /* Machine Interrupt Enable */
#define SR_MPIE		_AC(0x00000080, UL) /* Machine Previous IE */
#define SR_MPP_SHIFT	11
#define SR_MPP		_AC(0x00001800, UL) /* Previously Machine */
#define SR_MPRV		_AC(0x00020000, UL)
#define SR_MXR		_AC(0x00080000, UL)
#define SR_VS		_AC(0x00000600, UL) /* Vector extension */
#ifdef CONFIG_CPU_V
#define SR_VS_OFF	_AC(0x00000000, UL)
#define SR_VS_INITIAL	_AC(0x00000200, UL)
#define SR_VS_CLEAN	_AC(0x00000400, UL)
#define SR_VS_DIRTY	_AC(0x00000600, UL)
#endif /* CONFIG_CPU_V */
#define SR_FS		_AC(0x00006000, UL) /* Floating-point Status */
#ifdef CONFIG_CPU_F
#define SR_FS_OFF	_AC(0x00000000, UL)
#define SR_FS_INITIAL	_AC(0x00002000, UL)
#define SR_FS_CLEAN	_AC(0x00004000, UL)
#define SR_FS_DIRTY	_AC(0x00006000, UL)
#endif /* CONFIG_CPU_F */
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
#define SR_TVM		_AC(0x00100000, UL)
#define SR_TW		_AC(0x00200000, UL)
#define SR_TSR		_AC(0x00400000, UL)
#ifdef CONFIG_64BIT
#define SR_UXL		_AC(0x0000000300000000, UL)
#define SR_SXL		_AC(0x0000000C00000000, UL)
#define SR_SBE		_AC(0x0000001000000000, UL)
#define SR_MBE		_AC(0x0000002000000000, UL)
#define SR_GVA		_AC(0x0000004000000000, UL)
#define SR_MPV		_AC(0x0000008000000000, UL)
#define SR_GVA_SHIFT	38
#else
#define SR_SBE		_AC(0x00000010, UL)
#define SR_MBE		_AC(0x00000020, UL)
#define SR_GVA		_AC(0x00000040, UL)
#define SR_GVA_SHIFT	6
#endif

#define HSTATUS_VTSR			_UL(0x00400000)
#define HSTATUS_VTW			_UL(0x00200000)
#define HSTATUS_VTVM			_UL(0x00100000)
#define HSTATUS_VGEIN			_UL(0x0003f000)
#define HSTATUS_VGEIN_SHIFT		12
#define HSTATUS_HU			_UL(0x00000200)
#define HSTATUS_SPVP			_UL(0x00000100)
#define HSTATUS_SPV			_UL(0x00000080)
#define HSTATUS_GVA			_UL(0x00000040)
#define HSTATUS_VSBE			_UL(0x00000020)

/* MISA */
#define HYPERVISOR_EXT  _AC(0x00000080, UL)

/* MIP/MIE/SIP/SIE/BSIP/BSIE */
#define IR_USI		(_AC(0x1, UL) << IRQ_U_SOFT)
#define IR_UTI		(_AC(0x1, UL) << IRQ_U_TIMER)
#define IR_UEI		(_AC(0x1, UL) << IRQ_U_EXT)
#define IR_SSI		(_AC(0x1, UL) << IRQ_S_SOFT)
#define IR_STI		(_AC(0x1, UL) << IRQ_S_TIMER)
#define IR_SEI		(_AC(0x1, UL) << IRQ_S_EXT)
#define IR_HSI		(_AC(0x1, UL) << IRQ_H_SOFT)
#define IR_HTI		(_AC(0x1, UL) << IRQ_H_TIMER)
#define IR_HEI		(_AC(0x1, UL) << IRQ_H_EXT)
#define IR_MSI		(_AC(0x1, UL) << IRQ_M_SOFT)
#define IR_MTI		(_AC(0x1, UL) << IRQ_M_TIMER)
#define IR_MEI		(_AC(0x1, UL) << IRQ_M_EXT)

#ifdef CONFIG_RAS
#if __riscv_xlen == 64
#define MIP_RASHP_INTP	(_UL(1) << IRQ_RAS_UE)
#else
#define MIPH_RASHP_INTP	(_UL(1) << (IRQ_RAS_UE - 32))
#endif
#endif


/* SATP/HGATP */
#ifndef CONFIG_64BIT
#define ATP_PPN		_AC(0x003FFFFF, UL)
#define ATP_MODE_32	_AC(0x80000000, UL)
#define ATP_PPN		_AC(0x003FFFFF, UL)
#define ATP_VMID_SHIFT	22
#define ATP_VMID	_AC(0x1FC00000, UL)
#else
#define ATP_PPN		_AC(0x00000FFFFFFFFFFF, UL)
#define ATP_MODE_39	_AC(0x8000000000000000, UL)
#define ATP_MODE_48	_AC(0x9000000000000000, UL)
#define ATP_MODE_57	_AC(0xA000000000000000, UL)
#define ATP_MODE_64	_AC(0xB000000000000000, UL)
#define ATP_PPN		_AC(0x00000FFFFFFFFFFF, UL)
#define ATP_VMID_SHIFT	44
#define ATP_VMID	_AC(0x03FFF00000000000, UL)
#endif

/* MCAUSE/SCAUSE/BSCAUSE */
#define ICR_IRQ_FLAG	(_AC(1, UL) << (__riscv_xlen - 1))

/* MENVCFG */
#define ECR_FIOM	_AC(0x00000001, UL) /* Fence of IO implies memory */
#ifdef CONFIG_RISCV_ZICBOM
#define ECR_CBIE	_AC(0x00000030, UL) /* Zicbom extension */
#define ECR_CBCFE	_AC(0x00000040, UL) /* Zicbom extension */
#else /* CONFIG_RISCV_ZICBOM */
#define ECR_CBIE	_AC(0x00000000, UL) /* Zicbom extension */
#define ECR_CBCFE	_AC(0x00000000, UL) /* Zicbom extension */
#endif /* CONFIG_RISCV_ZICBOM */
#ifdef CONFIG_RISCV_ZICBOZ
#define ECR_CBZE	_AC(0x00000080, UL) /* Zicboz extension */
#else /* CONFIG_RISCV_ZICBOZ */
#define ECR_CBZE	_AC(0x00000000, UL) /* Zicboz extension */
#endif /* CONFIG_RISCV_ZICBOZ */
#ifdef CONFIG_64BIT
#ifdef CONFIG_RISCV_SVPBMT
#define ECR_PBMTE	_AC(0x4000000000000000, UL) /* Svpbmt extension */
#else /* CONFIG_RISCV_SVPBMT */
#define ECR_PBMTE	_AC(0x0000000000000000, UL) /* Svpbmt extension */
#endif /* CONFIG_RISCV_SVPBMT */
#ifdef CONFIG_RISCV_SSTC
#define ECR_STCE	_AC(0x8000000000000000, UL) /* Sstc extension */
#else /* CONFIG_RISCV_SSTC */
#define ECR_STCE	_AC(0x0000000000000000, UL) /* Sstc extension */
#endif /* CONFIG_RISCV_SSTC */
#else
#ifdef CONFIG_RISCV_SVPBMT
#define ECR_PBMTE	_AC(0x40000000, UL) /* Svpbmt extension */
#else /* CONFIG_RISCV_SVPBMT */
#define ECR_PBMTE	_AC(0x00000000, UL) /* Svpbmt extension */
#endif /* CONFIG_RISCV_SVPBMT */
#ifdef CONFIG_RISCV_SSTC
#define ECR_STCE	_AC(0x80000000, UL) /* Sstc extension */
#else /* CONFIG_RISCV_SSTC */
#define ECR_STCE	_AC(0x00000000, UL) /* Sstc extension */
#endif /* CONFIG_RISCV_SSTC */
#endif

/* MSECCFG */
#define SCR_MML		_AC(0x00000001, UL)
#define SCR_MMWP	_AC(0x00000002, UL)
#define SCR_RLB		_AC(0x00000004, UL)
#define SCR_USEED	_AC(0x00000100, UL)
#define SCR_SSEED	_AC(0x00000200, UL)

/* MSTATEEN/SSTATEEN */
#ifdef CONFIG_RISCV_SMSTATEEN
#define SER_STATEEN	_AC(0x8000000000000000, UL)
#define SER_HSENVCFG	_AC(0x4000000000000000, UL)
#ifdef CONFIG_RISCV_AIA
#define SER_SVSLCT	_AC(0x1000000000000000, UL)
#define SER_AIA		_AC(0x0800000000000000, UL)
#define SER_IMSIC	_AC(0x0400000000000000, UL)
#else /* CONFIG_RISCV_AIA */
#define SER_SVSLCT	_AC(0x0000000000000000, UL)
#define SER_AIA		_AC(0x0000000000000000, UL)
#define SER_IMSIC	_AC(0x0000000000000000, UL)
#endif /* CONFIG_RISCV_AIA */
#define SER_CONTEXT	_AC(0x0200000000000000, UL)
#define SER_FCSR	_AC(0x0000000000000002, UL)
#define SER_CS		_AC(0x0000000000000001, UL)
#else /* CONFIG_RISCV_SMSTATEEN */
#define SER_STATEEN	_AC(0x0000000000000000, UL)
#define SER_HSENVCFG	_AC(0x0000000000000000, UL)
#define SER_SVSLCT	_AC(0x0000000000000000, UL)
#define SER_AIA		_AC(0x0000000000000000, UL)
#define SER_IMSIC	_AC(0x0000000000000000, UL)
#define SER_CONTEXT	_AC(0x0000000000000000, UL)
#define SER_FCSR	_AC(0x0000000000000000, UL)
#define SER_CS		_AC(0x0000000000000000, UL)
#endif /* CONFIG_RISCV_SMSTATEEN */

/* AIA registers */
/* TOPI */
#define TOPI_IID_SHIFT		16
#define TOPI_IID_MASK		REG_12BIT_MASK
#define TOPI_IID_GENMASK	GENMASK(11, 0)
#define TOPI_IPRIO_SHIFT	0
#define TOPI_IPRIO_MASK		REG_8BIT_MASK
#define TOPI_IPRIO_GENMASK	GENMASK(7, 0)
#define TOPI_IPRIO_BITS		8

/* TOPEI */
#define TOPEI_ID_SHIFT		16
#define TOPEI_ID_MASK		REG_11BIT_MASK
#define TOPEI_ID_GENMASK	GENMASK(10, 0)
#define TOPEI_PRIO_SHIFT	0
#define TOPEI_PRIO_MASK		REG_11BIT_MASK
#define TOPEI_PRIO_GENMASK	GENMASK(10, 0)

#define ISELECT_IPRIO(n)	(0x30 + (n))
#define ISELECT_MASK		REG_9BIT_MASK
#define ISELECT_GENMASK		GENMASK(8, 0)

#define IRQ_S_SOFT			1
#define IRQ_VS_SOFT			2
#define IRQ_M_SOFT			3
#define IRQ_S_TIMER			5
#define IRQ_VS_TIMER			6
#define IRQ_M_TIMER			7
#define IRQ_S_EXT			9
#define IRQ_VS_EXT			10
#define IRQ_M_EXT			11
#define IRQ_S_GEXT			12
#define IRQ_PMU_OVF 			13

#define MIP_SSIP			(_UL(1) << IRQ_S_SOFT)
#define MIP_VSSIP			(_UL(1) << IRQ_VS_SOFT)
#define MIP_MSIP			(_UL(1) << IRQ_M_SOFT)
#define MIP_STIP			(_UL(1) << IRQ_S_TIMER)
#define MIP_VSTIP			(_UL(1) << IRQ_VS_TIMER)
#define MIP_MTIP			(_UL(1) << IRQ_M_TIMER)
#define MIP_SEIP			(_UL(1) << IRQ_S_EXT)
#define MIP_VSEIP			(_UL(1) << IRQ_VS_EXT)
#define MIP_MEIP			(_UL(1) << IRQ_M_EXT)
#define MIP_SGEIP			(_UL(1) << IRQ_S_GEXT)
#define MIP_LCOFIP			(_UL(1) << IRQ_PMU_OVF)

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

#if __riscv_xlen == 64

/* 64-bit read for VS-stage address translation (RV64) */
#define INSN_PSEUDO_VS_LOAD		0x00003000

/* 64-bit write for VS-stage address translation (RV64) */
#define INSN_PSEUDO_VS_STORE	0x00003020

#elif __riscv_xlen == 32

/* 32-bit read for VS-stage address translation (RV32) */
#define INSN_PSEUDO_VS_LOAD		0x00002000

/* 32-bit write for VS-stage address translation (RV32) */
#define INSN_PSEUDO_VS_STORE	0x00002020

#endif

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
	(unsigned long *)((unsigned long)(regs) + REG_OFFSET(insn, pos))

#define GET_RM(insn)			(((insn) >> 12) & 7)

#define GET_RS1(insn, regs)		(*REG_PTR(insn, SH_RS1, regs))
#define GET_RS2(insn, regs)		(*REG_PTR(insn, SH_RS2, regs))
#define GET_RS1S(insn, regs)		(*REG_PTR(RVC_RS1S(insn), 0, regs))
#define GET_RS2S(insn, regs)		(*REG_PTR(RVC_RS2S(insn), 0, regs))
#define GET_RS2C(insn, regs)		(*REG_PTR(insn, SH_RS2C, regs))
#define GET_SP(regs)			(*REG_PTR(2, 0, regs))
#define SET_RD(insn, regs, val)		(*REG_PTR(insn, SH_RD, regs) = (val))
#define IMM_I(insn)			((int32_t)(insn) >> 20)
#define IMM_S(insn)			(((int32_t)(insn) >> 25 << 5) | \
					 (int32_t)(((insn) >> 7) & 0x1f))
#define MASK_FUNCT3			0x7000

#ifdef CONFIG_RISCV_EXIT_M
#define CSR_STATUS		CSR_MSTATUS
#define CSR_IE			CSR_MIE
#define CSR_TVEC		CSR_MTVEC
#define CSR_SCRATCH		CSR_MSCRATCH
#define CSR_EPC			CSR_MEPC
#define CSR_CAUSE		CSR_MCAUSE
#define CSR_TVAL		CSR_MTVAL
#define CSR_IP			CSR_MIP
#define CSR_IDELEG		CSR_MIDELEG
#define CSR_COUNTINHIBIT	CSR_MCOUNTINHIBIT
#define CSR_HPMEVENT(n)		CSR_MHPMEVENT(n)
#define CSR_HPMCOUNTER(n)	CSR_MHPMCOUNTER(n)
#define CSR_ISELECT		CSR_MISELECT
#define CSR_IREG		CSR_MIREG
#define CSR_TOPI		CSR_MTOPI
#ifdef CONFIG_CPU_IMSIC
#define CSR_TOPEI		CSR_MTOPEI
#endif /* CONFIG_CPU_IMSIC */

#define SR_IE			SR_MIE
#define SR_PIE			SR_MPIE
#define SR_PP			SR_MPP
#endif
#ifdef CONFIG_RISCV_EXIT_S
#define CSR_STATUS		CSR_SSTATUS
#define CSR_IE			CSR_SIE
#define CSR_TVEC		CSR_STVEC
#define CSR_SCRATCH		CSR_SSCRATCH
#define CSR_EPC			CSR_SEPC
#define CSR_CAUSE		CSR_SCAUSE
#define CSR_TVAL		CSR_STVAL
#define CSR_IP			CSR_SIP
#define CSR_IDELEG		CSR_SIDELEG
#define CSR_COUNTINHIBIT	CSR_SCOUNTINHIBIT
#define CSR_HPMEVENT(n)		CSR_SHPMEVENT(n)
#define CSR_HPMCOUNTER(n)	CSR_SHPMCOUNTER(n)
#define CSR_ISELECT		CSR_SISELECT
#define CSR_IREG		CSR_SIREG
#define CSR_TOPI		CSR_STOPI
#ifdef CONFIG_CPU_IMSIC
#define CSR_TOPEI		CSR_STOPEI
#endif /* CONFIG_CPU_IMSIC */

#define SR_IE			SR_SIE
#define SR_PIE			SR_SPIE
#define SR_PP			SR_SPP
#endif

#if __riscv_xlen == 64
#define MHPMEVENT_OF			(_UL(1) << 63)
#define MHPMEVENT_MINH			(_UL(1) << 62)
#define MHPMEVENT_SINH			(_UL(1) << 61)
#define MHPMEVENT_UINH			(_UL(1) << 60)
#define MHPMEVENT_VSINH			(_UL(1) << 59)
#define MHPMEVENT_VUINH			(_UL(1) << 58)
#else
#define MHPMEVENTH_OF			(_ULL(1) << 31)
#define MHPMEVENTH_MINH			(_ULL(1) << 30)
#define MHPMEVENTH_SINH			(_ULL(1) << 29)
#define MHPMEVENTH_UINH			(_ULL(1) << 28)
#define MHPMEVENTH_VSINH		(_ULL(1) << 27)
#define MHPMEVENTH_VUINH		(_ULL(1) << 26)

#define MHPMEVENT_OF			(MHPMEVENTH_OF << 32)
#define MHPMEVENT_MINH			(MHPMEVENTH_MINH << 32)
#define MHPMEVENT_SINH			(MHPMEVENTH_SINH << 32)
#define MHPMEVENT_UINH			(MHPMEVENTH_UINH << 32)
#define MHPMEVENT_VSINH			(MHPMEVENTH_VSINH << 32)
#define MHPMEVENT_VUINH			(MHPMEVENTH_VUINH << 32)
#endif

#define MHPMEVENT_SSCOF_MASK		_ULL(0xFFFF000000000000)

#ifdef CONFIG_RISCV_DEBUG_CSR
#define csr_dbg(...)			con_dbg(__VA_ARGS__)
#else
#define csr_dbg(...)			do { } while (0)
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
	csr_dbg("csr: read %s %08lx\n", __ASM_STR(csr), __v);	\
	__v;							\
})
#define csr_write(csr, val)					\
({								\
	unsigned long __v = (unsigned long)(val);		\
	csr_dbg("csr: write %s %08lx\n", __ASM_STR(csr), __v);	\
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
	csr_dbg("csr: set %s %08lx\n", __ASM_STR(csr), __v);	\
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
	csr_dbg("csr: clr %s %08lx\n", __ASM_STR(csr), __v);	\
	asm volatile("csrc	" __ASM_STR(csr) ", %0"		\
		     : : "rK" (__v) : "memory");		\
})

#define csr_read_allowed(csr, trap)					\
	({								\
	register unsigned long tinfo asm("a3") = (unsigned long)trap;	\
	register unsigned long ttmp asm("a4");				\
	register unsigned long mtvec = csr_expected_trap_addr();	\
	register unsigned long value = 0;				\
	((struct csr_trap_info *)(trap))->cause = 0;			\
	asm volatile(							\
		"add %2, %1, zero\n"					\
		"csrrw %0, " __ASM_STR(CSR_MTVEC) ", %0\n"		\
		"csrr %3, " __ASM_STR(csr) "\n"				\
		"csrw " __ASM_STR(CSR_MTVEC) ", %0"			\
	    : "+&r"(mtvec), "+&r"(tinfo),				\
	      "+&r"(ttmp), "=r"(value)					\
	    :								\
	    : "memory");						\
	value;								\
	})

#define csr_write_allowed(csr, trap, value)				\
	({								\
	register unsigned long tinfo asm("a3") = (unsigned long)trap;	\
	register unsigned long ttmp asm("a4");				\
	register unsigned long mtvec = csr_expected_trap_addr();	\
	((struct csr_trap_info *)(trap))->cause = 0;			\
	asm volatile(							\
		"add %2, %1, zero\n"					\
		"csrrw %0, " __ASM_STR(CSR_MTVEC) ", %0\n"		\
		"csrw " __ASM_STR(csr) ", %3\n"				\
		"csrw " __ASM_STR(CSR_MTVEC) ", %0"			\
	    : "+&r"(mtvec), "+&r"(tinfo),				\
	      "+&r"(ttmp)						\
	    : "rK"(value)						\
	    : "memory");						\
	})

#ifdef CONFIG_RISCV_AIA
#define aia_csr_write(__c, __v)				\
	do {						\
		csr_write(CSR_ISELECT, __c);		\
		csr_write(CSR_IREG, __v);		\
		csr_dbg("aia: %d = %04x\n", __c, __v);	\
	} while (0)
#define aia_csr_read(__c)				\
	({						\
		unsigned long __v;			\
		csr_write(CSR_ISELECT, __c);		\
		__v = csr_read(CSR_IREG);		\
		__v;					\
	})
#define aia_csr_read_clear(__c, __v)			\
	({						\
	 	unsigned long ____v;			\
		csr_write(CSR_ISELECT, __c);		\
		____v = csr_read_clear(CSR_IREG, __v);	\
		csr_dbg("aia: %ld &=~ %04lx\n", __c, __v);\
		____v;					\
	})
#define aia_csr_set(__c, __v)				\
	do {						\
		csr_write(CSR_ISELECT, __c);		\
		csr_set(CSR_IREG, __v);			\
		csr_dbg("aia: %ld |= %04lx\n", __c, __v);\
	} while (0)
#define aia_csr_clear(__c, __v)				\
	do {						\
		csr_write(CSR_ISELECT, __c);		\
		csr_clear(CSR_IREG, __v);		\
		csr_dbg("aia: %ld &=~ %04lx\n", __c, __v);\
	} while (0)
#endif

/** Representation of trap details */
struct csr_trap_info {
	/** epc Trap program counter */
	unsigned long epc;
	/** cause Trap exception cause */
	unsigned long cause;
	/** tval Trap value */
	unsigned long tval;
	/** tval2 Trap value 2 */
	unsigned long tval2;
	/** tinst Trap instruction */
	unsigned long tinst;
	/** gva Guest virtual address in tval flag */
	unsigned long gva;
};

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

extern void (*csr_expected_trap)(void);
static __inline unsigned long csr_expected_trap_addr(void)
{
	return (unsigned long)csr_expected_trap;
}

void csr_init(void);
#endif /* __ASSEMBLY__ */

#endif /* __CSR_RISCV_H_INCLUDE__ */
