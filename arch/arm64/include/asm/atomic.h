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
 * @(#)atomic.h: ARM64 specific atomicity operation interfaces
 * $Id: atomic.h,v 1.0 2019-04-14 18:34:00 zhenglv Exp $
 */

#ifndef __ARM64_ATOMIC_H_INCLUDE__
#define __ARM64_ATOMIC_H_INCLUDE__

#ifdef CONFIG_ARM64_ATOMIC_CAS
#include <asm/atomic_cas.h>
#else
#include <asm/atomic_llsc.h>
#endif

#define ATOMIC_INIT(i)			{ (i) }

#define smp_hw_atomic_read(v)		READ_ONCE((v)->counter)
#define smp_hw_atomic_set(v, i)		WRITE_ONCE(((v)->counter), (i))

#define atomic_read(v)			smp_hw_atomic_read(v)
#define atomic_read_acquire(v)		smp_hw_atomic_read_acquire(v)

#define atomic_set(v, i)		smp_hw_atomic_set(v, i)
#define atomic_set_release(v, i)	smp_hw_atomic_set_release(v, i)

#define atomic_add(i, v)		smp_hw_atomic_add(i, v)
#define atomic_add_return(i, v)		smp_hw_atomic_add_return(i, v)
#define atomic_add_return_acquire(i, v)	smp_hw_atomic_add_return_acquire(i, v)
#define atomic_add_return_release(i, v)	smp_hw_atomic_add_return_release(i, v)
#define atomic_add_return_relaxed(i, v)	smp_hw_atomic_add_return_relaxed(i, v)
#define atomic_fetch_add(i, v)		smp_hw_atomic_fetch_add(i, v)
#define atomic_fetch_add_acquire(i, v)	smp_hw_atomic_fetch_add_acquire(i, v)
#define atomic_fetch_add_release(i, v)	smp_hw_atomic_fetch_add_release(i, v)
#define atomic_fetch_add_relaxed(i, v)	smp_hw_atomic_fetch_add_relaxed(i, v)

#define atomic_sub(i, v)		smp_hw_atomic_sub(i, v)
#define atomic_sub_return_relaxed(i, v)	smp_hw_atomic_sub_return_relaxed(i, v)
#define atomic_sub_return_acquire(i, v)	smp_hw_atomic_sub_return_acquire(i, v)
#define atomic_sub_return_release(i, v)	smp_hw_atomic_sub_return_release(i, v)
#define atomic_fetch_sub_relaxed(i, v)	smp_hw_atomic_fetch_sub_relaxed(i, v)
#define atomic_fetch_sub_acquire(i, v)	smp_hw_atomic_fetch_sub_acquire(i, v)
#define atomic_fetch_sub_release(i, v)	smp_hw_atomic_fetch_sub_release(i, v)

#define atomic_inc(v)			smp_hw_atomic_inc(v)
#define atomic_inc_return(v)		smp_hw_atomic_inc_return(v)
#define atomic_inc_return_acquire(v)	smp_hw_atomic_inc_return_acquire(v)
#define atomic_inc_return_release(v)	smp_hw_atomic_inc_return_release(v)
#define atomic_inc_return_relaxed(v)	smp_hw_atomic_inc_return_relaxed(v)
#define atomic_fetch_inc(v)		smp_hw_atomic_fetch_inc(v)
#define atomic_fetch_inc_acquire(v)	smp_hw_atomic_fetch_inc_acquire(v)
#define atomic_fetch_inc_release(v)	smp_hw_atomic_fetch_inc_release(v)
#define atomic_fetch_inc_relaxed(v)	smp_hw_atomic_fetch_inc_relaxed(v)

#define atomic_dec(v)			smp_hw_atomic_dec(v)
#define atomic_dec_return(v)		smp_hw_atomic_dec_return(v)
#define atomic_dec_return_acquire(v)	smp_hw_atomic_dec_return_acquire(v)
#define atomic_dec_return_release(v)	smp_hw_atomic_dec_return_release(v)
#define atomic_dec_return_relaxed(v)	smp_hw_atomic_dec_return_relaxed(v)
#define atomic_fetch_dec(v)		smp_hw_atomic_fetch_dec(v)
#define atomic_fetch_dec_acquire(v)	smp_hw_atomic_fetch_dec_acquire(v)
#define atomic_fetch_dec_release(v)	smp_hw_atomic_fetch_dec_release(v)
#define atomic_fetch_dec_relaxed(v)	smp_hw_atomic_fetch_dec_relaxed(v)

#define atomic_and(i, v)		smp_hw_atomic_and(i, v)
#define atomic_and_return(i, v)		smp_hw_atomic_and_return(i, v)
#define atomic_and_return_acquire(i, v)	smp_hw_atomic_and_return_acquire(i, v)
#define atomic_and_return_release(i, v)	smp_hw_atomic_and_return_release(i, v)
#define atomic_and_return_relaxed(i, v)	smp_hw_atomic_and_return_relaxed(i, v)
#define atomic_fetch_and(i, v)		smp_hw_atomic_fetch_and(i, v)
#define atomic_fetch_and_acquire(i, v)	smp_hw_atomic_fetch_and_acquire(i, v)
#define atomic_fetch_and_release(i, v)	smp_hw_atomic_fetch_and_release(i, v)
#define atomic_fetch_and_relaxed(i, v)	smp_hw_atomic_fetch_and_relaxed(i, v)

#define atomic_or(i, v)			smp_hw_atomic_or(i, v)
#define atomic_or_return(i, v)		smp_hw_atomic_or_return(i, v)
#define atomic_or_return_acquire(i, v)	smp_hw_atomic_or_return_acquire(i, v)
#define atomic_or_return_release(i, v)	smp_hw_atomic_or_return_release(i, v)
#define atomic_or_return_relaxed(i, v)	smp_hw_atomic_or_return_relaxed(i, v)
#define atomic_fetch_or(i, v)		smp_hw_atomic_fetch_or(i, v)
#define atomic_fetch_or_acquire(i, v)	smp_hw_atomic_fetch_or_acquire(i, v)
#define atomic_fetch_or_release(i, v)	smp_hw_atomic_fetch_or_release(i, v)
#define atomic_fetch_or_relaxed(i, v)	smp_hw_atomic_fetch_or_relaxed(i, v)

#define atomic_andnot(i, v)			\
	smp_hw_atomic_andnot(i, v)
#define atomic_andnot_return(i, v)		\
	smp_hw_atomic_andnot_return(i, v)
#define atomic_andnot_return_acquire(i, v)	\
	smp_hw_atomic_andnot_return_acquire(i, v)
#define atomic_andnot_return_release(i, v)	\
	smp_hw_atomic_andnot_return_release(i, v)
#define atomic_andnot_return_relaxed(i, v)	\
	smp_hw_atomic_andnot_return_relaxed(i, v)
#if 0
#define atomic_fetch_andnot(i, v)		\
	smp_hw_atomic_fetch_andnot(i, v)
#endif
#define atomic_fetch_andnot_acquire(i, v)	\
	smp_hw_atomic_fetch_andnot_acquire(i, v)
#define atomic_fetch_andnot_release(i, v)	\
	smp_hw_atomic_fetch_andnot_release(i, v)
#define atomic_fetch_andnot_relaxed(i, v)	\
	smp_hw_atomic_fetch_andnot_relaxed(i, v)

#define atomic_xor(i, v)		smp_hw_atomic_xor(i, v)
#define atomic_xor_return(i, v)		smp_hw_atomic_xor_return(i, v)
#define atomic_xor_return_acquire(i, v)	smp_hw_atomic_xor_return_acquire(i, v)
#define atomic_xor_return_release(i, v)	smp_hw_atomic_xor_return_release(i, v)
#define atomic_xor_return_relaxed(i, v)	smp_hw_atomic_xor_return_relaxed(i, v)
#define atomic_fetch_xor(i, v)		smp_hw_atomic_fetch_xor(i, v)
#define atomic_fetch_xor_acquire(i, v)	smp_hw_atomic_fetch_xor_acquire(i, v)
#define atomic_fetch_xor_release(i, v)	smp_hw_atomic_fetch_xor_release(i, v)
#define atomic_fetch_xor_relaxed(i, v)	smp_hw_atomic_fetch_xor_relaxed(i, v)

#define atomic_xchg(v, i)		smp_hw_atomic_xchg(v, i)
#define atomic_xchg_acquire(v, i)	smp_hw_atomic_xchg_acquire(v, i)
#define atomic_xchg_release(v, i)	smp_hw_atomic_xchg_release(v, i)
#define atomic_xchg_relaxed(v, i)	smp_hw_atomic_xchg_relaxed(v, i)

#define atomic_cmpxchg(v, o, n)		smp_hw_atomic_cmpxchg(v, o, n)
#define atomic_cmpxchg_acquire(v, o, n)	smp_hw_atomic_cmpxchg_acquire(v, o, n)
#define atomic_cmpxchg_release(v, o, n)	smp_hw_atomic_cmpxchg_release(v, o, n)
#define atomic_cmpxchg_relaxed(v, o, n)	smp_hw_atomic_cmpxchg_relaxed(v, o, n)

#define smp_hw_atomic_xchg_relaxed(v, new) \
	smp_hw_xchg_relaxed(&((v)->counter), (new))
#define smp_hw_atomic_xchg_acquire(v, new) \
	smp_hw_xchg_acquire(&((v)->counter), (new))
#define smp_hw_atomic_xchg_release(v, new) \
	smp_hw_xchg_release(&((v)->counter), (new))
#define smp_hw_atomic_xchg(v, new) \
	smp_hw_xchg(&((v)->counter), (new))

#define smp_hw_atomic_cmpxchg_relaxed(v, old, new) \
	smp_hw_cmpxchg_relaxed(&((v)->counter), (old), (new))
#define smp_hw_atomic_cmpxchg_acquire(v, old, new) \
	smp_hw_cmpxchg_acquire(&((v)->counter), (old), (new))
#define smp_hw_atomic_cmpxchg_release(v, old, new) \
	smp_hw_cmpxchg_release(&((v)->counter), (old), (new))
#define smp_hw_atomic_cmpxchg(v, old, new) \
	smp_hw_cmpxchg(&((v)->counter), (old), (new))

#define atomic_add_and_test(i, v)	smp_hw_atomic_add_and_test(i, v)
#define atomic_sub_and_test(i, v)	smp_hw_atomic_sub_and_test(i, v)
#define atomic_dec_and_test(i, v)	smp_hw_atomic_dec_and_test(i, v)
#define atomic_inc_and_test(i, v)	smp_hw_atomic_inc_and_test(i, v)

#define xchg(ptr, ...)			smp_hw_xchg(ptr, __VA_ARGS__)
#define xchg_acquire(ptr, ...)		smp_hw_xchg_acquire(ptr, __VA_ARGS__)
#define xchg_release(ptr, ...)		smp_hw_xchg_release(ptr, __VA_ARGS__)
#define xchg_relaxed(ptr, ...)		smp_hw_xchg_relaxed(ptr, __VA_ARGS__)
#define cmpxchg(ptr, ...)		smp_hw_cmpxchg(ptr, __VA_ARGS__)
#define cmpxchg_acquire(ptr, ...)	smp_hw_cmpxchg_acquire(ptr, __VA_ARGS__)
#define cmpxchg_release(ptr, ...)	smp_hw_cmpxchg_release(ptr, __VA_ARGS__)
#define cmpxchg_relaxed(ptr, ...)	smp_hw_cmpxchg_relaxed(ptr, __VA_ARGS__)

#endif /* __ARM64_ATOMIC_H_INCLUDE__ */
