/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)fp.h: RISCV floating point definitions
 * $Id: fp.h,v 1.0 2020-02-12 10:43:00 zhenglv Exp $
 */

#ifndef __FP_RISCV_H_INCLUDE__
#define __FP_RISCV_H_INCLUDE__

#ifdef CONFIG_SBI_RISCV_F
#define GET_PRECISION(insn)	(((insn) >> 25) & 3)
#define GET_RM(insn)		(((insn) >> 12) & 7)
#define PRECISION_S		0
#define PRECISION_D		1

#ifndef __DTS__
#ifdef __riscv_flen
#define GET_F32_REG(insn, pos, regs)					\
	({								\
		register int32_t value asm("a0") =			\
			SHIFT_RIGHT(insn, (pos)-3) & 0xf8;		\
		unsigned long tmp;					\
		asm("1: auipc %0, %%pcrel_hi(get_f32_reg); add %0, %0, %1; jalr t0, %0, %%pcrel_lo(1b)"							\
		    : "=&r"(tmp), "+&r"(value)::"t0");			\
		value;							\
	})
#define SET_F32_REG(insn, pos, regs, val)				\
	({								\
		register uint32_t value asm("a0") = (val);		\
		unsigned long offset =					\
			SHIFT_RIGHT(insn, (pos)-3) & 0xf8;		\
		unsigned long tmp;					\
		asm volatile(						\
			"1: auipc %0, %%pcrel_hi(put_f32_reg); add %0, %0, %2; jalr t0, %0, %%pcrel_lo(1b)"						\
			: "=&r"(tmp)					\
			: "r"(value), "r"(offset)			\
			: "t0");					\
	})
#define init_fp_reg(i)		SET_F32_REG((i) << 3, 3, 0, 0)
#define GET_F64_REG(insn, pos, regs)					\
	({								\
		register unsigned long value asm("a0") =		\
			SHIFT_RIGHT(insn, (pos)-3) & 0xf8;		\
		unsigned long tmp;					\
		asm("1: auipc %0, %%pcrel_hi(get_f64_reg); add %0, %0, %1; jalr t0, %0, %%pcrel_lo(1b)"							\
		    : "=&r"(tmp), "+&r"(value)::"t0");			\
		sizeof(unsigned long) == 4 ?				\
			*(int64_t *)value : (int64_t)value;		\
	})
#define SET_F64_REG(insn, pos, regs, val)				\
	({								\
		uint64_t __val = (val);					\
		register unsigned long value asm("a0") =		\
			sizeof(unsigned long) == 4 ?			\
			(unsigned long)&__val : (unsigned long)__val;	\
		unsigned long offset =					\
			SHIFT_RIGHT(insn, (pos)-3) & 0xf8;		\
		unsigned long tmp;					\
		asm volatile(						\
			"1: auipc %0, %%pcrel_hi(put_f64_reg); add %0, %0, %2; jalr t0, %0, %%pcrel_lo(1b)"						\
			: "=&r"(tmp)					\
			: "r"(value), "r"(offset)			\
			: "t0");					\
	})
#define GET_FCSR()		csr_read(CSR_FCSR)
#define SET_FCSR(value)		csr_write(CSR_FCSR, (value))
#define GET_FRM()		csr_read(CSR_FRM)
#define SET_FRM(value)		csr_write(CSR_FRM, (value))
#define GET_FFLAGS()		csr_read(CSR_FFLAGS)
#define SET_FFLAGS(value)	csr_write(CSR_FFLAGS, (value))

#define SET_FS_DIRTY()		((void)0)
#else
#error "Floating point emulation not supported.\n"
#endif
#endif /* __DTS__ */

#define GET_F32_RS1(insn, regs)		(GET_F32_REG(insn, 15, regs))
#define GET_F32_RS2(insn, regs)		(GET_F32_REG(insn, 20, regs))
#define GET_F32_RS3(insn, regs)		(GET_F32_REG(insn, 27, regs))
#define GET_F64_RS1(insn, regs)		(GET_F64_REG(insn, 15, regs))
#define GET_F64_RS2(insn, regs)		(GET_F64_REG(insn, 20, regs))
#define GET_F64_RS3(insn, regs)		(GET_F64_REG(insn, 27, regs))
#define SET_F32_RD(insn, regs, val)	\
	(SET_F32_REG(insn, 7, regs, val), SET_FS_DIRTY())
#define SET_F64_RD(insn, regs, val)	\
	(SET_F64_REG(insn, 7, regs, val), SET_FS_DIRTY())

#define GET_F32_RS2C(insn, regs)	(GET_F32_REG(insn, 2, regs))
#define GET_F32_RS2S(insn, regs)	(GET_F32_REG(RVC_RS2S(insn), 0, regs))
#define GET_F64_RS2C(insn, regs)	(GET_F64_REG(insn, 2, regs))
#define GET_F64_RS2S(insn, regs)	(GET_F64_REG(RVC_RS2S(insn), 0, regs))
#endif

#endif /* __FP_RISCV_H_INCLUDE__ */
