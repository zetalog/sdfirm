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
 * @(#)atomic.h: RISCV specific atomicity operation interfaces
 * $Id: atomic.h,v 1.0 2020-01-05 11:18:00 zhenglv Exp $
 */

#ifndef __ATOMIC_RISCV_H_INCLUDE__
#define __ATOMIC_RISCV_H_INCLUDE__

#include <asm/barrier.h>

#ifdef CONFIG_RISCV_ATOMIC_COUNT_32
typedef int32_t atomic_count_t;
#endif
#ifdef CONFIG_RISCV_ATOMIC_COUNT_64
typedef int64_t atomic_count_t;
#endif
typedef struct { atomic_count_t counter; } atomic_t;

#define ATOMIC_INIT(i)			{ (i) }
#define INIT_ATOMIC(a, i)		((a)->counter = (i))

#define smp_hw_atomic_read(v)		READ_ONCE((v)->counter)
#define smp_hw_atomic_set(v, i)		WRITE_ONCE(((v)->counter), (i))

#define smp_hw_atomic_read_acquire(v)	__smp_load_acquire(&(v)->counter)
#define smp_hw_atomic_set_release(v, i)	__smp_store_release(&(v)->counter, (i))

#define atomic_read(v)			smp_hw_atomic_read(v)
#define atomic_read_acquire(v)		smp_hw_atomic_read_acquire(v)

#define atomic_set(v, i)		smp_hw_atomic_set(v, i)
#define atomic_set_release(v, i)	smp_hw_atomic_set_release(v, i)

/* First, the atomic ops that have no ordering constraints and therefor don't
 * have the AQ or RL bits set.  These don't return anything, so there's only
 * one version to worry about.
 */
#define ATOMIC_OP(op, asm_op, I, asm_type)				\
static __always_inline							\
void smp_hw_atomic_##op(atomic_count_t i, atomic_t *v)			\
{									\
	asm volatile(							\
		"	amo" #asm_op "." #asm_type " zero, %1, %0"	\
		: "+A" (v->counter)					\
		: "r" (I)						\
		: "memory");						\
}

/* Atomic ops that have ordered, relaxed, acquire, and release variants.
 * There's two flavors of these: the arithmatic ops have both fetch and return
 * versions, while the logical ops only have fetch versions.
 */
#define ATOMIC_FETCH_OP(op, asm_op, I, asm_type)			\
static __always_inline							\
atomic_count_t smp_hw_atomic_fetch_##op##_relaxed(atomic_count_t i,	\
					   atomic_t *v)			\
{									\
	register atomic_count_t ret;					\
	asm volatile(							\
		"	amo" #asm_op "." #asm_type " %1, %2, %0"	\
		: "+A" (v->counter), "=r" (ret)				\
		: "r" (I)						\
		: "memory");						\
	return ret;							\
}									\
static __always_inline							\
atomic_count_t smp_hw_atomic_fetch_##op(atomic_count_t i, atomic_t *v)	\
{									\
	register atomic_count_t ret;					\
	asm volatile(							\
		"	amo" #asm_op "." #asm_type ".aqrl  %1, %2, %0"	\
		: "+A" (v->counter), "=r" (ret)				\
		: "r" (I)						\
		: "memory");						\
	return ret;							\
}
#define ATOMIC_OP_RETURN(op, asm_op, c_op, I, asm_type)			\
static __always_inline							\
atomic_count_t smp_hw_atomic_##op##_return_relaxed(atomic_count_t i,	\
						   atomic_t *v)		\
{									\
        return smp_hw_atomic_fetch_##op##_relaxed(i, v) c_op I;		\
}									\
static __always_inline							\
atomic_count_t smp_hw_atomic_##op##_return(atomic_count_t i,		\
					   atomic_t *v)			\
{									\
        return smp_hw_atomic_fetch_##op(i, v) c_op I;			\
}

#ifdef CONFIG_RISCV_ATOMIC_COUNT_32
#define ATOMIC_OPS(op, asm_op, I)	 ATOMIC_OP(op, asm_op, I, w)
#endif
#ifdef CONFIG_RISCV_ATOMIC_COUNT_64
#define ATOMIC_OPS(op, asm_op, I)	 ATOMIC_OP(op, asm_op, I, d)
#endif

ATOMIC_OPS(add, add,  i)
ATOMIC_OPS(sub, add, -i)
ATOMIC_OPS(and, and,  i)
ATOMIC_OPS( or,  or,  i)
ATOMIC_OPS(xor, xor,  i)
#undef ATOMIC_OP
#undef ATOMIC_OPS

#ifdef CONFIG_RISCV_ATOMIC_COUNT_32
#define ATOMIC_OPS(op, asm_op, c_op, I)			\
        ATOMIC_FETCH_OP( op, asm_op,       I, w)	\
        ATOMIC_OP_RETURN(op, asm_op, c_op, I, w)
#endif
#ifdef CONFIG_RISCV_ATOMIC_COUNT_64
#define ATOMIC_OPS(op, asm_op, c_op, I)			\
        ATOMIC_FETCH_OP( op, asm_op,       I, d)	\
        ATOMIC_OP_RETURN(op, asm_op, c_op, I, d)
#endif

ATOMIC_OPS(add, add, +,  i)
ATOMIC_OPS(sub, add, +, -i)
#undef ATOMIC_OPS

#ifdef CONFIG_RISCV_ATOMIC_COUNT_32
#define ATOMIC_OPS(op, asm_op, I)	ATOMIC_FETCH_OP(op, asm_op, I, w)
#endif
#ifdef CONFIG_RISCV_ATOMIC_COUNT_64
#define ATOMIC_OPS(op, asm_op, I)	ATOMIC_FETCH_OP(op, asm_op, I, d)
#endif

ATOMIC_OPS(and, and, i)
ATOMIC_OPS( or,  or, i)
ATOMIC_OPS(xor, xor, i)
#undef ATOMIC_OPS
#undef ATOMIC_FETCH_OP
#undef ATOMIC_OP_RETURN

/* Atomic compare and exchange.  Compare OLD with MEM, if identical,
 * store NEW in MEM.  Return the initial value in MEM.  Success is
 * indicated by comparing RETURN with OLD.
 */
#define __cmpxchg_relaxed(ptr, old, new, size)				\
({									\
	__typeof__(ptr) __ptr = (ptr);					\
	__typeof__(*(ptr)) __old = (old);				\
	__typeof__(*(ptr)) __new = (new);				\
	__typeof__(*(ptr)) __ret;					\
	register unsigned int __rc;					\
	switch (size) {							\
	case 4:								\
		__asm__ __volatile__ (					\
			"0:	lr.w %0, %2\n"				\
			"	bne  %0, %z3, 1f\n"			\
			"	sc.w %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	case 8:								\
		__asm__ __volatile__ (					\
			"0:	lr.d %0, %2\n"				\
			"	bne %0, %z3, 1f\n"			\
			"	sc.d %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	default:							\
		BUG();						\
	}								\
	__ret;								\
})

#define cmpxchg_relaxed(ptr, o, n)					\
({									\
	__typeof__(*(ptr)) _o_ = (o);					\
	__typeof__(*(ptr)) _n_ = (n);					\
	(__typeof__(*(ptr))) __cmpxchg_relaxed((ptr),			\
					_o_, _n_, sizeof(*(ptr)));	\
})

#define __cmpxchg_acquire(ptr, old, new, size)				\
({									\
	__typeof__(ptr) __ptr = (ptr);					\
	__typeof__(*(ptr)) __old = (old);				\
	__typeof__(*(ptr)) __new = (new);				\
	__typeof__(*(ptr)) __ret;					\
	register unsigned int __rc;					\
	switch (size) {							\
	case 4:								\
		__asm__ __volatile__ (					\
			"0:	lr.w %0, %2\n"				\
			"	bne  %0, %z3, 1f\n"			\
			"	sc.w %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			RISCV_ACQUIRE_BARRIER				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	case 8:								\
		__asm__ __volatile__ (					\
			"0:	lr.d %0, %2\n"				\
			"	bne %0, %z3, 1f\n"			\
			"	sc.d %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			RISCV_ACQUIRE_BARRIER				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	default:							\
		BUG();						\
	}								\
	__ret;								\
})

#define cmpxchg_acquire(ptr, o, n)					\
({									\
	__typeof__(*(ptr)) _o_ = (o);					\
	__typeof__(*(ptr)) _n_ = (n);					\
	(__typeof__(*(ptr))) __cmpxchg_acquire((ptr),			\
					_o_, _n_, sizeof(*(ptr)));	\
})

#define __cmpxchg_release(ptr, old, new, size)				\
({									\
	__typeof__(ptr) __ptr = (ptr);					\
	__typeof__(*(ptr)) __old = (old);				\
	__typeof__(*(ptr)) __new = (new);				\
	__typeof__(*(ptr)) __ret;					\
	register unsigned int __rc;					\
	switch (size) {							\
	case 4:								\
		__asm__ __volatile__ (					\
			RISCV_RELEASE_BARRIER				\
			"0:	lr.w %0, %2\n"				\
			"	bne  %0, %z3, 1f\n"			\
			"	sc.w %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	case 8:								\
		__asm__ __volatile__ (					\
			RISCV_RELEASE_BARRIER				\
			"0:	lr.d %0, %2\n"				\
			"	bne %0, %z3, 1f\n"			\
			"	sc.d %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	default:							\
		BUG();						\
	}								\
	__ret;								\
})

#define cmpxchg_release(ptr, o, n)					\
({									\
	__typeof__(*(ptr)) _o_ = (o);					\
	__typeof__(*(ptr)) _n_ = (n);					\
	(__typeof__(*(ptr))) __cmpxchg_release((ptr),			\
					_o_, _n_, sizeof(*(ptr)));	\
})

#define __cmpxchg(ptr, old, new, size)					\
({									\
	__typeof__(ptr) __ptr = (ptr);					\
	__typeof__(*(ptr)) __old = (old);				\
	__typeof__(*(ptr)) __new = (new);				\
	__typeof__(*(ptr)) __ret;					\
	register unsigned int __rc;					\
	switch (size) {							\
	case 4:								\
		__asm__ __volatile__ (					\
			"0:	lr.w %0, %2\n"				\
			"	bne  %0, %z3, 1f\n"			\
			"	sc.w.rl %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			"	fence rw, rw\n"				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	case 8:								\
		__asm__ __volatile__ (					\
			"0:	lr.d %0, %2\n"				\
			"	bne %0, %z3, 1f\n"			\
			"	sc.d.rl %1, %z4, %2\n"			\
			"	bnez %1, 0b\n"				\
			"	fence rw, rw\n"				\
			"1:\n"						\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)			\
			: "memory");					\
		break;							\
	default:							\
		BUG();						\
	}								\
	__ret;								\
})

#define cmpxchg(ptr, o, n)						\
({									\
	__typeof__(*(ptr)) _o_ = (o);					\
	__typeof__(*(ptr)) _n_ = (n);					\
	(__typeof__(*(ptr))) __cmpxchg((ptr),				\
				       _o_, _n_, sizeof(*(ptr)));	\
})

#define cmpxchg32(ptr, o, n)						\
({									\
	BUG_ON(sizeof(*(ptr)) != 4);				\
	cmpxchg((ptr), (o), (n));					\
})

#define cmpxchg64(ptr, o, n)						\
({									\
	BUG_ON(sizeof(*(ptr)) != 8);				\
	cmpxchg((ptr), (o), (n));					\
})

#define ATOMIC_OP(c_t, size)						\
static __always_inline							\
c_t smp_hw_atomic_cmpxchg_relaxed(atomic_t *v, c_t o, c_t n)		\
{									\
	return __cmpxchg_relaxed(&(v->counter), o, n, size);		\
}									\
static __always_inline							\
c_t smp_hw_atomic_cmpxchg_acquire(atomic_t *v, c_t o, c_t n)		\
{									\
	return __cmpxchg_acquire(&(v->counter), o, n, size);		\
}									\
static __always_inline							\
c_t smp_hw_atomic_cmpxchg_release(atomic_t *v, c_t o, c_t n)		\
{									\
	return __cmpxchg_release(&(v->counter), o, n, size);		\
}									\
static __always_inline							\
c_t smp_hw_atomic_cmpxchg(atomic_t *v, c_t o, c_t n)			\
{									\
	return __cmpxchg(&(v->counter), o, n, size);			\
}

#ifdef CONFIG_RISCV_ATOMIC_COUNT_32
#define ATOMIC_OPS()							\
	ATOMIC_OP( int, 4)
#endif
#ifdef CONFIG_RISCV_ATOMIC_COUNT_64
#define ATOMIC_OPS()							\
	ATOMIC_OP(long, 8)
#endif

ATOMIC_OPS()

#define atomic_cmpxchg_relaxed		smp_hw_atomic_cmpxchg_relaxed
#define atomic_cmpxchg_acquire		smp_hw_atomic_cmpxchg_acquire
#define atomic_cmpxchg_release		smp_hw_atomic_cmpxchg_release
#define atomic_cmpxchg			smp_hw_atomic_cmpxchg

#undef ATOMIC_OPS
#undef ATOMIC_OP

#define atomic_add(i, v)		smp_hw_atomic_add(i, v)
#define atomic_sub(i, v)		smp_hw_atomic_sub(i, v)
#define atomic_and(i, v)		smp_hw_atomic_and(i, v)
#define atomic_or(i, v)			smp_hw_atomic_or(i, v)
#define atomic_xor(i, v)		smp_hw_atomic_xor(i, v)
#define atomic_andnot(i, v)		smp_hw_atomic_and(~(i), v)

#define atomic_fetch_add(i, v)		smp_hw_atomic_fetch_add(i, v)
#define atomic_fetch_sub(i, v)		smp_hw_atomic_fetch_sub(i, v)
#define atomic_fetch_and(i, v)		smp_hw_atomic_fetch_and(i, v)
#define atomic_fetch_or(i, v)		smp_hw_atomic_fetch_or(i, v)
#define atomic_fetch_xor(i, v)		smp_hw_atomic_fetch_xor(i, v)
#define atomic_fetch_add_relaxed(i, v)	smp_hw_atomic_fetch_add_relaxed(i, v)
#define atomic_fetch_sub_relaxed(i, v)	smp_hw_atomic_fetch_sub_relaxed(i, v)
#define atomic_fetch_and_relaxed(i, v)	smp_hw_atomic_fetch_and_relaxed(i, v)
#define atomic_fetch_or_relaxed(i, v)	smp_hw_atomic_fetch_or_relaxed(i, v)
#define atomic_fetch_xor_relaxed(i, v)	smp_hw_atomic_fetch_xor_relaxed(i, v)
#define atomic_add_return(i, v)		smp_hw_atomic_add_return(i, v)
#define atomic_sub_return(i, v)		smp_hw_atomic_sub_return(i, v)
#define atomic_add_return_relaxed(i, v)	smp_hw_atomic_add_return_relaxed(i, v)
#define atomic_sub_return_relaxed(i, v)	smp_hw_atomic_sub_return_relaxed(i, v)

#endif /* __ATOMIC_RISCV_H_INCLUDE__ */
