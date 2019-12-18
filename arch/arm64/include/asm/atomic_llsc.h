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
 * @(#)atomic_llsc.h: ARM64 LL/SC styple atomicity operation interfaces
 * $Id: atomic_llsc.h,v 1.0 2019-04-14 18:36:00 zhenglv Exp $
 */

#ifndef __ARM64_ATOMIC_LLSC_H_INCLUDE__
#define __ARM64_ATOMIC_LLSC_H_INCLUDE__

/* AArch64 UP and SMP safe atomic ops.  We use load exclusive and
 * store exclusive to ensure that these are atomic.  We may loop
 * to ensure that the update happens.
 *
 * NOTE: these functions do *not* follow the PCS and must explicitly
 * save any clobbered registers other than x0 (regardless of return
 * value).  This is achieved through -fcall-saved-* compiler flags for
 * this file, which unfortunately don't work on a per-function basis
 * (the optimize attribute silently ignores these options).
 */

#define ATOMIC32_OP(op, asm_op)						\
static inline void smp_hw_atomic_##op(atomic_count_t i, atomic_t *v)	\
{									\
	unsigned long tmp;						\
	atomic_count_t result;						\
	asm volatile("// atomic_" #op "\n"				\
"	prfm	pstl1strm, %2\n"					\
"1:	ldxr	%w0, %2\n"						\
"	" #asm_op "	%w0, %w0, %w3\n"				\
"	stxr	%w1, %w0, %2\n"						\
"	cbnz	%w1, 1b"						\
	: "=&r" (result), "=&r" (tmp), "+Q" (v->counter)		\
	: "Ir" (i));							\
}

#define ATOMIC32_OP_RETURN(name, mb, acq, rel, cl, op, asm_op)		\
static inline atomic_count_t						\
smp_hw_atomic_##op##_return##name(atomic_count_t i, atomic_t *v)	\
{									\
	unsigned long tmp;						\
	atomic_count_t result;						\
	asm volatile("// atomic_" #op "_return" #name "\n"		\
"	prfm	pstl1strm, %2\n"					\
"1:	ld" #acq "xr	%w0, %2\n"					\
"	" #asm_op "	%w0, %w0, %w3\n"				\
"	st" #rel "xr	%w1, %w0, %2\n"					\
"	cbnz	%w1, 1b\n"						\
"	" #mb								\
	: "=&r" (result), "=&r" (tmp), "+Q" (v->counter)		\
	: "Ir" (i)							\
	: cl);								\
	return result;							\
}

#define ATOMIC32_FETCH_OP(name, mb, acq, rel, cl, op, asm_op)		\
static inline atomic_count_t						\
smp_hw_atomic_fetch_##op##name(atomic_count_t i, atomic_t *v)		\
{									\
	unsigned long tmp;						\
	atomic_count_t val, result;					\
	asm volatile("// atomic_fetch_" #op #name "\n"			\
"	prfm	pstl1strm, %3\n"					\
"1:	ld" #acq "xr	%w0, %3\n"					\
"	" #asm_op "	%w1, %w0, %w4\n"				\
"	st" #rel "xr	%w2, %w1, %3\n"					\
"	cbnz	%w2, 1b\n"						\
"	" #mb								\
	: "=&r" (result), "=&r" (val), "=&r" (tmp), "+Q" (v->counter)	\
	: "Ir" (i)							\
	: cl);								\
	return result;							\
}

#ifdef CONFIG_ARM64_ATOMIC_COUNT_32
typedef int32_t atomic_count_t;
typedef struct { atomic_count_t counter; } atomic_t;

#define ATOMIC32_OPS(...)						\
ATOMIC32_OP(__VA_ARGS__)						\
ATOMIC32_OP_RETURN(        , dmb ish,  , l, "memory", __VA_ARGS__)	\
ATOMIC32_OP_RETURN(_relaxed,        ,  ,  ,         , __VA_ARGS__)	\
ATOMIC32_OP_RETURN(_acquire,        , a,  , "memory", __VA_ARGS__)	\
ATOMIC32_OP_RETURN(_release,        ,  , l, "memory", __VA_ARGS__)	\
ATOMIC32_FETCH_OP (        , dmb ish,  , l, "memory", __VA_ARGS__)	\
ATOMIC32_FETCH_OP (_relaxed,        ,  ,  ,         , __VA_ARGS__)	\
ATOMIC32_FETCH_OP (_acquire,        , a,  , "memory", __VA_ARGS__)	\
ATOMIC32_FETCH_OP (_release,        ,  , l, "memory", __VA_ARGS__)

ATOMIC32_OPS(add, add)
ATOMIC32_OPS(sub, sub)
#undef ATOMIC32_OPS

#define ATOMIC32_OPS(...)						\
ATOMIC32_OP(__VA_ARGS__)						\
ATOMIC32_FETCH_OP (        , dmb ish,  , l, "memory", __VA_ARGS__)	\
ATOMIC32_FETCH_OP (_relaxed,        ,  ,  ,         , __VA_ARGS__)	\
ATOMIC32_FETCH_OP (_acquire,        , a,  , "memory", __VA_ARGS__)	\
ATOMIC32_FETCH_OP (_release,        ,  , l, "memory", __VA_ARGS__)
ATOMIC32_OPS(and, and)
ATOMIC32_OPS(andnot, bic)
ATOMIC32_OPS(or, orr)
ATOMIC32_OPS(xor, eor)
#undef ATOMIC32_OPS
#endif /* CONFIG_ARM64_ATOMIC_COUNT_32 */

#undef ATOMIC32_FETCH_OP
#undef ATOMIC32_OP_RETURN
#undef ATOMIC32_OP

#define ATOMIC64_OP(op, asm_op)						\
static inline void smp_hw_atomic_##op(atomic_count_t i, atomic_t *v)	\
{									\
	atomic_count_t result;						\
	unsigned long tmp;						\
	asm volatile("// atomic64_" #op "\n"				\
"	prfm	pstl1strm, %2\n"					\
"1:	ldxr	%0, %2\n"						\
"	" #asm_op "	%0, %0, %3\n"					\
"	stxr	%w1, %0, %2\n"						\
"	cbnz	%w1, 1b"						\
	: "=&r" (result), "=&r" (tmp), "+Q" (v->counter)		\
	: "Ir" (i));							\
}

#define ATOMIC64_OP_RETURN(name, mb, acq, rel, cl, op, asm_op)		\
static inline atomic_count_t						\
smp_hw_atomic_##op##_return##name(atomic_count_t i, atomic_t *v)	\
{									\
	atomic_count_t result;						\
	unsigned long tmp;						\
	asm volatile("// atomic64_" #op "_return" #name "\n"		\
"	prfm	pstl1strm, %2\n"					\
"1:	ld" #acq "xr	%0, %2\n"					\
"	" #asm_op "	%0, %0, %3\n"					\
"	st" #rel "xr	%w1, %0, %2\n"					\
"	cbnz	%w1, 1b\n"						\
"	" #mb								\
	: "=&r" (result), "=&r" (tmp), "+Q" (v->counter)		\
	: "Ir" (i)							\
	: cl);								\
	return result;							\
}

#define ATOMIC64_FETCH_OP(name, mb, acq, rel, cl, op, asm_op)		\
static inline atomic_count_t						\
smp_hw_atomic_fetch_##op##name(atomic_count_t i, atomic_t *v)		\
{									\
	atomic_count_t result, val;					\
	unsigned long tmp;						\
	asm volatile("// atomic64_fetch_" #op #name "\n"		\
"	prfm	pstl1strm, %3\n"					\
"1:	ld" #acq "xr	%0, %3\n"					\
"	" #asm_op "	%1, %0, %4\n"					\
"	st" #rel "xr	%w2, %1, %3\n"					\
"	cbnz	%w2, 1b\n"						\
"	" #mb								\
	: "=&r" (result), "=&r" (val), "=&r" (tmp), "+Q" (v->counter)	\
	: "Ir" (i)							\
	: cl);								\
	return result;							\
}

#ifdef CONFIG_ARM64_ATOMIC_COUNT_64
typedef int64_t atomic_count_t;
typedef struct { atomic_count_t counter; } atomic_t;

#define ATOMIC64_OPS(...)						\
ATOMIC64_OP(__VA_ARGS__)						\
ATOMIC64_OP_RETURN(, dmb ish,  , l, "memory", __VA_ARGS__)		\
ATOMIC64_OP_RETURN(_relaxed,,  ,  ,         , __VA_ARGS__)		\
ATOMIC64_OP_RETURN(_acquire,, a,  , "memory", __VA_ARGS__)		\
ATOMIC64_OP_RETURN(_release,,  , l, "memory", __VA_ARGS__)		\
ATOMIC64_FETCH_OP (, dmb ish,  , l, "memory", __VA_ARGS__)		\
ATOMIC64_FETCH_OP (_relaxed,,  ,  ,         , __VA_ARGS__)		\
ATOMIC64_FETCH_OP (_acquire,, a,  , "memory", __VA_ARGS__)		\
ATOMIC64_FETCH_OP (_release,,  , l, "memory", __VA_ARGS__)

ATOMIC64_OPS(add, add)
ATOMIC64_OPS(sub, sub)
#undef ATOMIC64_OPS

#define ATOMIC64_OPS(...)						\
ATOMIC64_OP(__VA_ARGS__)						\
ATOMIC64_FETCH_OP (, dmb ish,  , l, "memory", __VA_ARGS__)		\
ATOMIC64_FETCH_OP (_relaxed,,  ,  ,         , __VA_ARGS__)		\
ATOMIC64_FETCH_OP (_acquire,, a,  , "memory", __VA_ARGS__)		\
ATOMIC64_FETCH_OP (_release,,  , l, "memory", __VA_ARGS__)

ATOMIC64_OPS(and, and)
ATOMIC64_OPS(andnot, bic)
ATOMIC64_OPS(or, orr)
ATOMIC64_OPS(xor, eor)
#undef ATOMIC64_OPS
#endif /* CONFIG_ARM64_ATOMIC_COUNT_64 */

#undef ATOMIC64_FETCH_OP
#undef ATOMIC64_OP_RETURN
#undef ATOMIC64_OP

#define __CMPXCHG_CASE(w, sfx, name, sz, mb, acq, rel, cl)		\
static inline u##sz __cmpxchg_case_##name##sz(volatile void *ptr,	\
					      unsigned long old,	\
					      u##sz new)		\
{									\
	unsigned long tmp;						\
	u##sz oldval;							\
	/* Sub-word sizes require explicit casting so that the compare  \
	 * part of the cmpxchg doesn't end up interpreting non-zero	\
	 * upper bits of the register containing "old".			\
	 */								\
	if (sz < 32)							\
		old = (u##sz)old;					\
	asm volatile(							\
	"	prfm	pstl1strm, %[v]\n"				\
	"1:	ld" #acq "xr" #sfx "\t%" #w "[oldval], %[v]\n"		\
	"	eor	%" #w "[tmp], %" #w "[oldval], %" #w "[old]\n"	\
	"	cbnz	%" #w "[tmp], 2f\n"				\
	"	st" #rel "xr" #sfx "\t%w[tmp], %" #w "[new], %[v]\n"	\
	"	cbnz	%w[tmp], 1b\n"					\
	"	" #mb "\n"						\
	"2:"								\
	: [tmp] "=&r" (tmp), [oldval] "=&r" (oldval),			\
	  [v] "+Q" (*(u##sz *)ptr)					\
	: [old] "Kr" (old), [new] "r" (new)				\
	: cl);								\
	return oldval;							\
}
__CMPXCHG_CASE(w, b,     ,  8,        ,  ,  ,         )
__CMPXCHG_CASE(w, h,     , 16,        ,  ,  ,         )
__CMPXCHG_CASE(w,  ,     , 32,        ,  ,  ,         )
__CMPXCHG_CASE( ,  ,     , 64,        ,  ,  ,         )
__CMPXCHG_CASE(w, b, acq_,  8,        , a,  , "memory")
__CMPXCHG_CASE(w, h, acq_, 16,        , a,  , "memory")
__CMPXCHG_CASE(w,  , acq_, 32,        , a,  , "memory")
__CMPXCHG_CASE( ,  , acq_, 64,        , a,  , "memory")
__CMPXCHG_CASE(w, b, rel_,  8,        ,  , l, "memory")
__CMPXCHG_CASE(w, h, rel_, 16,        ,  , l, "memory")
__CMPXCHG_CASE(w,  , rel_, 32,        ,  , l, "memory")
__CMPXCHG_CASE( ,  , rel_, 64,        ,  , l, "memory")
__CMPXCHG_CASE(w, b,  mb_,  8, dmb ish,  , l, "memory")
__CMPXCHG_CASE(w, h,  mb_, 16, dmb ish,  , l, "memory")
__CMPXCHG_CASE(w,  ,  mb_, 32, dmb ish,  , l, "memory")
__CMPXCHG_CASE( ,  ,  mb_, 64, dmb ish,  , l, "memory")
#undef __CMPXCHG_CASE

#define __CMPXCHG_DBL(name, mb, rel, cl)				\
static inline long __cmpxchg_double##name(unsigned long old1,		\
					  unsigned long old2,		\
					  unsigned long new1,		\
					  unsigned long new2,		\
					  volatile void *ptr)		\
{									\
	unsigned long tmp, ret;						\
	asm volatile("// __cmpxchg_double" #name "\n"			\
	"	prfm	pstl1strm, %2\n"				\
	"1:	ldxp	%0, %1, %2\n"					\
	"	eor	%0, %0, %3\n"					\
	"	eor	%1, %1, %4\n"					\
	"	orr	%1, %0, %1\n"					\
	"	cbnz	%1, 2f\n"					\
	"	st" #rel "xp	%w0, %5, %6, %2\n"			\
	"	cbnz	%w0, 1b\n"					\
	"	" #mb "\n"						\
	"2:"								\
	: "=&r" (tmp), "=&r" (ret), "+Q" (*(unsigned long *)ptr)	\
	: "r" (old1), "r" (old2), "r" (new1), "r" (new2)		\
	: cl);								\
	return ret;							\
}
__CMPXCHG_DBL(   ,        ,  ,         )
__CMPXCHG_DBL(_mb, dmb ish, l, "memory")
#undef __CMPXCHG_DBL

#endif /* __ARM64_ATOMIC_LLSC_H_INCLUDE__ */
