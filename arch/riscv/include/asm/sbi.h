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

#define SBI_CALL(which, arg0, arg1, arg2, arg3) ({		\
	register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0);	\
	register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1);	\
	register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2);	\
	register uintptr_t a3 asm ("a3") = (uintptr_t)(arg3);	\
	register uintptr_t a7 asm ("a7") = (uintptr_t)(which);	\
	asm volatile ("ecall"					\
		      : "+r" (a0)				\
		      : "r" (a1), "r" (a2), "r" (a3), "r" (a7)	\
		      : "memory");				\
	a0;							\
})

/* Lazy implementations until SBI is finalized */
#define SBI_CALL_0(which) SBI_CALL(which, 0, 0, 0, 0)
#define SBI_CALL_1(which, arg0) SBI_CALL(which, arg0, 0, 0, 0)
#define SBI_CALL_2(which, arg0, arg1) SBI_CALL(which, arg0, arg1, 0, 0)
#define SBI_CALL_3(which, arg0, arg1, arg2) \
		   SBI_CALL(which, arg0, arg1, arg2, 0)
#define SBI_CALL_4(which, arg0, arg1, arg2, arg3) \
		   SBI_CALL(which, arg0, arg1, arg2, arg3)

#define sbi_console_putchar(ch)	SBI_CALL_1(SBI_ECALL_CONSOLE_PUTCHAR, ch)
#define sbi_console_getchar()	SBI_CALL_0(SBI_ECALL_CONSOLE_GETCHAR)
#if __riscv_xlen == 32
#define sbi_set_timer(st)	SBI_CALL_2(SBI_ECALL_SET_TIMER, (st), (st) >> 32)
#else
#define sbi_set_timer(st)	SBI_CALL_1(SBI_ECALL_SET_TIMER, (st))
#endif
#define sbi_shutdown()		SBI_CALL_0(SBI_ECALL_SHUTDOWN)
#define sbi_clear_ipi()		SBI_CALL_0(SBI_ECALL_CLEAR_IPI)
#define sbi_send_ipi(hartmask)	SBI_CALL_1(SBI_ECALL_SEND_IPI, hartmask)
#define sbi_remote_fence_i(hartmask)				\
	SBI_CALL_1(SBI_ECALL_REMOTE_FENCE_I, hartmask)
#define sbi_remote_sfence_vma(hartmask, start, size)		\
	SBI_CALL_3(SBI_ECALL_REMOTE_SFENCE_VMA, hartmask, start, size)
#define sbi_remote_sfence_vma_asid(hartmask, start, size, asid)	\
	SBI_CALL_4(SBI_ECALL_REMOTE_SFENCE_VMA_ASID,		\
		   hartmask, start, size, asid)
#define sbi_enable_log()	SBI_CALL_0(SBI_ECALL_ENABLE_LOG)
#define sbi_disable_log()	SBI_CALL_0(SBI_ECALL_DISABLE_LOG)

#endif /* __SBI_RISCV_H_INCLUDE__ */
