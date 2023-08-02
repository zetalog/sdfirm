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
 * @(#)sbi.h: RISCV supervisor binary interface (SBI) interface
 * $Id: sbi.h,v 1.1 2019-12-30 10:45:00 zhenglv Exp $
 */

#ifndef __SBI_RISCV_H_INCLUDE__
#define __SBI_RISCV_H_INCLUDE__

#define SBI_CALL(ext, fid, arg0, arg1, arg2, arg3, arg4, arg5) ({	\
	register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0);		\
	register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1);		\
	register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2);		\
	register uintptr_t a3 asm ("a3") = (uintptr_t)(arg3);		\
	register uintptr_t a4 asm ("a4") = (uintptr_t)(arg4);		\
	register uintptr_t a5 asm ("a5") = (uintptr_t)(arg5);		\
	register uintptr_t a6 asm ("a6") = (uintptr_t)(fid);		\
	register uintptr_t a7 asm ("a7") = (uintptr_t)(ext);		\
	asm volatile ("ecall"						\
		      : "+r" (a0), "+r" (a1)				\
		      : "r" (a2), "r" (a3), "r" (a4), "r" (a5),		\
		        "r" (a6), "r" (a7)				\
		      : "memory");					\
	a0;								\
})

/* Lazy implementations until SBI is finalized */
#define SBI_CALL_0(ext, fid)					\
	SBI_CALL(ext, fid, 0, 0, 0, 0, 0, 0)
#define SBI_CALL_1(ext, fid, arg0)				\
	SBI_CALL(ext, fid, arg0, 0, 0, 0, 0, 0)
#define SBI_CALL_2(ext, fid, arg0, arg1)			\
	SBI_CALL(ext, fid, arg0, arg1, 0, 0, 0, 0)
#define SBI_CALL_3(ext, fid, arg0, arg1, arg2)			\
	SBI_CALL(ext, fid, arg0, arg1, arg2, 0, 0, 0)
#define SBI_CALL_4(ext, fid, arg0, arg1, arg2, arg3)		\
	SBI_CALL(ext, fid, arg0, arg1, arg2, arg3, 0, 0)
#define SBI_CALL_5(ext, fid, arg0, arg1, arg2, arg3, arg4)	\
	SBI_CALL(ext, fid, arg0, arg1, arg2, arg3, arg4, 0)
#define SBI_CALL_6(ext, fid, arg0, arg1, arg2, arg3, arg4, arg5)\
	SBI_CALL(ext, fid, arg0, arg1, arg2, arg3, arg4, arg5)

#define sbi_console_putchar(ch)	SBI_CALL_1(SBI_ECALL_CONSOLE_PUTCHAR, 0, ch)
#define sbi_console_getchar()	SBI_CALL_0(SBI_ECALL_CONSOLE_GETCHAR, 0)
#if __riscv_xlen == 32
#define sbi_set_timer(st)	SBI_CALL_2(SBI_ECALL_SET_TIMER, 0, (st), (st) >> 32)
#else
#define sbi_set_timer(st)	SBI_CALL_1(SBI_ECALL_SET_TIMER, 0, (st))
#endif
#define sbi_shutdown()		SBI_CALL_0(SBI_ECALL_SHUTDOWN, 0)
#define sbi_clear_ipi()		SBI_CALL_0(SBI_ECALL_CLEAR_IPI, 0)
#define sbi_send_ipi(hartmask)	SBI_CALL_1(SBI_ECALL_SEND_IPI, 0, hartmask)
#ifdef CONFIG_SBI_ECALL_RFENCE
#define sbi_remote_fence_i(hartmask)				\
	SBI_CALL_2(SBI_EXT_RFENCE,				\
		   SBI_EXT_RFENCE_REMOTE_FENCE_I,		\
		   hartmask, 0)
#define sbi_remote_sfence_vma(hartmask, start, size)		\
	SBI_CALL_4(SBI_EXT_RFENCE,				\
		   SBI_EXT_RFENCE_REMOTE_SFENCE_VMA,		\
		   hartmask, 0, start, size)
#define sbi_remote_sfence_vma_asid(hartmask, start, size)	\
	SBI_CALL_5(SBI_EXT_RFENCE,				\
		   SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID,	\
		   hartmask, 0, start, size, asid)
#elif defined(CONFIG_SBI_ECALL_LEGACY)
#define sbi_remote_fence_i(hartmask)				\
	SBI_CALL_1(SBI_ECALL_REMOTE_FENCE_I, 0, hartmask)
#define sbi_remote_sfence_vma(hartmask, start, size)		\
	SBI_CALL_3(SBI_ECALL_REMOTE_SFENCE_VMA, 0,		\
		   hartmask, start, size)
#define sbi_remote_sfence_vma_asid(hartmask, start, size, asid)	\
	SBI_CALL_4(SBI_ECALL_REMOTE_SFENCE_VMA_ASID, 0,		\
		   hartmask, start, size, asid)
#else
#define sbi_remote_fence_i(hartmask)
#define sbi_remote_sfence_vma(hartmask, start, size)
#define sbi_remote_sfence_vma_asid(hartmask, start, size, asid)
#endif /* CONFIG_SBI_ECALL_RFENCE */
#define sbi_enable_log()	SBI_CALL_0(SBI_ECALL_ENABLE_LOG, 0)
#define sbi_disable_log()	SBI_CALL_0(SBI_ECALL_DISABLE_LOG, 0)
#define sbi_finish(code)	SBI_CALL_1(SBI_ECALL_FINISH, 0, code)

#endif /* __SBI_RISCV_H_INCLUDE__ */
