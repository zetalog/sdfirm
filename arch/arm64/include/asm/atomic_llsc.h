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
typedef uint16_t qspin_half_t;
typedef uint8_t qspin_quater_t;
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
typedef uint32_t qspin_half_t;
typedef uint16_t qspin_quater_t;
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

/* cmpxchg */
#define __CMPXCHG_CASE(w, sfx, name, sz, mb, acq, rel, cl)		\
static inline								\
uint##sz##_t __cmpxchg_case_##name##sz(volatile void *ptr,		\
				       unsigned long old,		\
				       uint##sz##_t new)		\
{									\
	unsigned long tmp;						\
	uint##sz##_t oldval;						\
	/* Sub-word sizes require explicit casting so that the compare  \
	 * part of the cmpxchg doesn't end up interpreting non-zero	\
	 * upper bits of the register containing "old".			\
	 */								\
	if (sz < 32)							\
		old = (uint##sz##_t)old;				\
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
	  [v] "+Q" (*(uint##sz##_t *)ptr)				\
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

#define __CMPXCHG_GEN(sfx)						\
static inline unsigned long __cmpxchg##sfx(volatile void *ptr,		\
					   unsigned long old,		\
					   unsigned long new,		\
					   int size)			\
{									\
	switch (size) {							\
	case 1:								\
		return __cmpxchg_case##sfx##_8(ptr, old, new);		\
	case 2:								\
		return __cmpxchg_case##sfx##_16(ptr, old, new);		\
	case 4:								\
		return __cmpxchg_case##sfx##_32(ptr, old, new);		\
	case 8:								\
		return __cmpxchg_case##sfx##_64(ptr, old, new);		\
	default:							\
		BUILD_BUG();						\
	}								\
	unreachable();							\
}
__CMPXCHG_GEN()
__CMPXCHG_GEN(_acq)
__CMPXCHG_GEN(_rel)
__CMPXCHG_GEN(_mb)
#undef __CMPXCHG_GEN

#define __cmpxchg_wrapper(sfx, ptr, o, n)				\
({									\
	typeof(*(ptr)) __ret;						\
	__ret = (typeof(*(ptr)))					\
		__cmpxchg##sfx((ptr), (unsigned long)(o),		\
				(unsigned long)(n), sizeof(*(ptr)));	\
	__ret;								\
})
#define smp_hw_cmpxchg_relaxed(...)	__cmpxchg_wrapper(    , __VA_ARGS__)
#define smp_hw_cmpxchg_acquire(...)	__cmpxchg_wrapper(_acq, __VA_ARGS__)
#define smp_hw_cmpxchg_release(...)	__cmpxchg_wrapper(_rel, __VA_ARGS__)
#define smp_hw_cmpxchg(...)		__cmpxchg_wrapper( _mb, __VA_ARGS__)

/* xchg */
#define __XCHG_CASE(w, sfx, name, sz, mb, nop_lse, acq, acq_lse, rel, cl)\
static inline								\
uint##sz##_t __xchg_case_##name##sz(uint##sz##_t x, volatile void *ptr)	\
{									\
	uint##sz##_t ret;						\
	unsigned long tmp;						\
	asm volatile("// __xchg" #name "\n"				\
	"	prfm	pstl1strm, %2\n"				\
	"1:	ld" #acq "xr" #sfx "\t%" #w "0, %2\n"			\
	"	st" #rel "xr" #sfx "\t%w1, %" #w "3, %2\n"		\
	"	cbnz	%w1, 1b\n"					\
	"	" #mb "\n"						\
	: "=&r" (ret), "=&r" (tmp), "+Q" (*(uint##sz##_t *)ptr)		\
	: "r" (x)							\
	: cl);								\
	return ret;							\
}
__XCHG_CASE(w, b,     ,  8,        ,    ,  ,  ,  ,         )
__XCHG_CASE(w, h,     , 16,        ,    ,  ,  ,  ,         )
__XCHG_CASE(w,  ,     , 32,        ,    ,  ,  ,  ,         )
__XCHG_CASE( ,  ,     , 64,        ,    ,  ,  ,  ,         )
__XCHG_CASE(w, b, acq_,  8,        ,    , a, a,  , "memory")
__XCHG_CASE(w, h, acq_, 16,        ,    , a, a,  , "memory")
__XCHG_CASE(w,  , acq_, 32,        ,    , a, a,  , "memory")
__XCHG_CASE( ,  , acq_, 64,        ,    , a, a,  , "memory")
__XCHG_CASE(w, b, rel_,  8,        ,    ,  ,  , l, "memory")
__XCHG_CASE(w, h, rel_, 16,        ,    ,  ,  , l, "memory")
__XCHG_CASE(w,  , rel_, 32,        ,    ,  ,  , l, "memory")
__XCHG_CASE( ,  , rel_, 64,        ,    ,  ,  , l, "memory")
__XCHG_CASE(w, b,  mb_,  8, dmb ish, nop,  , a, l, "memory")
__XCHG_CASE(w, h,  mb_, 16, dmb ish, nop,  , a, l, "memory")
__XCHG_CASE(w,  ,  mb_, 32, dmb ish, nop,  , a, l, "memory")
__XCHG_CASE( ,  ,  mb_, 64, dmb ish, nop,  , a, l, "memory")
#undef __XCHG_CASE

#define __XCHG_GEN(sfx)							\
static inline unsigned long __xchg##sfx(unsigned long x,		\
					volatile void *ptr,		\
					int size)			\
{									\
	switch (size) {							\
	case 1:								\
		return __xchg_case##sfx##_8(x, ptr);			\
	case 2:								\
		return __xchg_case##sfx##_16(x, ptr);			\
	case 4:								\
		return __xchg_case##sfx##_32(x, ptr);			\
	case 8:								\
		return __xchg_case##sfx##_64(x, ptr);			\
	default:							\
		BUILD_BUG();						\
	}								\
	unreachable();							\
}
__XCHG_GEN()
__XCHG_GEN(_acq)
__XCHG_GEN(_rel)
__XCHG_GEN(_mb)
#undef __XCHG_GEN

#define __xchg_wrapper(sfx, ptr, x)					\
({									\
	typeof(*(ptr)) __ret;						\
	__ret = (typeof(*(ptr)))					\
		__xchg##sfx((unsigned long)(x), (ptr), sizeof(*(ptr))); \
	__ret;								\
})
#define smp_hw_xchg_relaxed(...)	__xchg_wrapper(    , __VA_ARGS__)
#define smp_hw_xchg_acquire(...)	__xchg_wrapper(_acq, __VA_ARGS__)
#define smp_hw_xchg_release(...)	__xchg_wrapper(_rel, __VA_ARGS__)
#define smp_hw_xchg(...)		__xchg_wrapper( _mb, __VA_ARGS__)

/* cmpwait */
#define __CMPWAIT_CASE(w, sfx, sz)					\
static inline void __cmpwait_case_##sz(volatile void *ptr,		\
				       unsigned long val)		\
{									\
	unsigned long tmp;						\
	asm volatile(							\
	"	sevl\n"							\
	"	wfe\n"							\
	"	ldxr" #sfx "\t%" #w "[tmp], %[v]\n"			\
	"	eor	%" #w "[tmp], %" #w "[tmp], %" #w "[val]\n"	\
	"	cbnz	%" #w "[tmp], 1f\n"				\
	"	wfe\n"							\
	"1:"								\
	: [tmp] "=&r" (tmp), [v] "+Q" (*(unsigned long *)ptr)		\
	: [val] "r" (val));						\
}
__CMPWAIT_CASE(w, b, 8);
__CMPWAIT_CASE(w, h, 16);
__CMPWAIT_CASE(w,  , 32);
__CMPWAIT_CASE( ,  , 64);
#undef __CMPWAIT_CASE

#define __CMPWAIT_GEN(sfx)						\
static inline void __cmpwait##sfx(volatile void *ptr,			\
				  unsigned long val,			\
				  int size)				\
{									\
	switch (size) {							\
	case 1:								\
		return __cmpwait_case##sfx##_8(ptr, (uint8_t)val);	\
	case 2:								\
		return __cmpwait_case##sfx##_16(ptr, (uint16_t)val);	\
	case 4:								\
		return __cmpwait_case##sfx##_32(ptr, val);		\
	case 8:								\
		return __cmpwait_case##sfx##_64(ptr, val);		\
	default:							\
		BUILD_BUG();						\
	}								\
	unreachable();							\
}
__CMPWAIT_GEN()
#undef __CMPWAIT_GEN

#define __cmpwait_relaxed(ptr, val) \
	__cmpwait((ptr), (unsigned long)(val), sizeof(*(ptr)))

#endif /* __ARM64_ATOMIC_LLSC_H_INCLUDE__ */
