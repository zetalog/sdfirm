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
 * @(#)atomic.h: SMP atomicity operation interfaces
 * $Id: atomic.h,v 1.0 2019-12-18 16:57:00 zhenglv Exp $
 */

#ifndef __ATOMIC_H_INCLUDE__
#define __ATOMIC_H_INCLUDE__

#include <target/irq.h>
#include <target/barrier.h>

/* The idea here is to build acquire/release variants by adding explicit
 * barriers on top of the relaxed variant. In the case where the relaxed
 * variant is already fully ordered, no additional barriers are needed.
 *
 * If an architecture overrides __atomic_acquire_fence() it will probably
 * want to define smp_mb__after_spinlock().
 */
#ifndef __atomic_acquire_fence
#define __atomic_acquire_fence		smp_mb__after_atomic
#endif

#ifndef __atomic_release_fence
#define __atomic_release_fence		smp_mb__before_atomic
#endif

#ifndef __atomic_pre_full_fence
#define __atomic_pre_full_fence		smp_mb__before_atomic
#endif

#ifndef __atomic_post_full_fence
#define __atomic_post_full_fence	smp_mb__after_atomic
#endif

#define __atomic_op_acquire(op, args...)				\
({									\
	typeof(op##_relaxed(args)) __ret  = op##_relaxed(args);		\
	__atomic_acquire_fence();					\
	__ret;								\
})

#define __atomic_op_release(op, args...)				\
({									\
	__atomic_release_fence();					\
	op##_relaxed(args);						\
})

#define __atomic_op_fence(op, args...)					\
({									\
	typeof(op##_relaxed(args)) __ret;				\
	__atomic_pre_full_fence();					\
	__ret = op##_relaxed(args);					\
	__atomic_post_full_fence();					\
	__ret;								\
})

#ifdef CONFIG_SMP
#include <asm/atomic.h>

#ifndef xchg_relaxed
#define xchg_relaxed		xchg
#define xchg_acquire		xchg
#define xchg_release		xchg
#else /* xchg_relaxed */
#ifndef xchg_acquire
#define xchg_acquire(...)	__atomic_op_acquire(xchg, __VA_ARGS__)
#endif
#ifndef xchg_release
#define xchg_release(...)	__atomic_op_release(xchg, __VA_ARGS__)
#endif
#ifndef xchg
#define xchg(...)		__atomic_op_fence(xchg, __VA_ARGS__)
#endif
#endif /* xchg_relaxed */

#ifndef cmpxchg_relaxed
#define cmpxchg_relaxed		cmpxchg
#define cmpxchg_acquire		cmpxchg
#define cmpxchg_release		cmpxchg
#else /* cmpxchg_relaxed */
#ifndef cmpxchg_acquire
#define cmpxchg_acquire(...)	__atomic_op_acquire(cmpxchg, __VA_ARGS__)
#endif
#ifndef cmpxchg_release
#define cmpxchg_release(...)	__atomic_op_release(cmpxchg, __VA_ARGS__)
#endif
#ifndef cmpxchg
#define cmpxchg(...)		__atomic_op_fence(cmpxchg, __VA_ARGS__)
#endif
#endif /* cmpxchg_relaxed */

#ifndef atomic_try_cmpxchg_relaxed
#ifdef atomic_try_cmpxchg
#define atomic_try_cmpxchg_acquire atomic_try_cmpxchg
#define atomic_try_cmpxchg_release atomic_try_cmpxchg
#define atomic_try_cmpxchg_relaxed atomic_try_cmpxchg
#endif /* atomic_try_cmpxchg */

#ifndef atomic_try_cmpxchg
static inline bool
atomic_try_cmpxchg(atomic_t *v, atomic_count_t *old, atomic_count_t new)
{
	atomic_count_t r, o = *old;
	r = atomic_cmpxchg(v, o, new);
	if (unlikely(r != o))
		*old = r;
	return likely(r == o);
}
#define atomic_try_cmpxchg atomic_try_cmpxchg
#endif
#ifndef atomic_try_cmpxchg_acquire
static inline bool
atomic_try_cmpxchg_acquire(atomic_t *v,
			   atomic_count_t *old, atomic_count_t new)
{
	atomic_count_t r, o = *old;
	r = atomic_cmpxchg_acquire(v, o, new);
	if (unlikely(r != o))
		*old = r;
	return likely(r == o);
}
#define atomic_try_cmpxchg_acquire atomic_try_cmpxchg_acquire
#endif
#ifndef atomic_try_cmpxchg_release
static inline bool
atomic_try_cmpxchg_release(atomic_t *v,
			   atomic_count_t *old, atomic_count_t new)
{
	atomic_count_t r, o = *old;
	r = atomic_cmpxchg_release(v, o, new);
	if (unlikely(r != o))
		*old = r;
	return likely(r == o);
}
#define atomic_try_cmpxchg_release atomic_try_cmpxchg_release
#endif
#ifndef atomic_try_cmpxchg_relaxed
static inline bool
atomic_try_cmpxchg_relaxed(atomic_t *v,
			   atomic_count_t *old, atomic_count_t new)
{
	atomic_count_t r, o = *old;
	r = atomic_cmpxchg_relaxed(v, o, new);
	if (unlikely(r != o))
		*old = r;
	return likely(r == o);
}
#define atomic_try_cmpxchg_relaxed atomic_try_cmpxchg_relaxed
#endif
#else /* atomic_try_cmpxchg_relaxed */
#ifndef atomic_try_cmpxchg_acquire
static inline bool
atomic_try_cmpxchg_acquire(atomic_t *v,
			   atomic_count_t *old, atomic_count_t new)
{
	bool ret = atomic_try_cmpxchg_relaxed(v, old, new);
	__atomic_acquire_fence();
	return ret;
}
#define atomic_try_cmpxchg_acquire atomic_try_cmpxchg_acquire
#endif
#ifndef atomic_try_cmpxchg_release
static inline bool
atomic_try_cmpxchg_release(atomic_t *v,
			   atomic_count_t *old, atomic_count_t new)
{
	__atomic_release_fence();
	return atomic_try_cmpxchg_relaxed(v, old, new);
}
#define atomic_try_cmpxchg_release atomic_try_cmpxchg_release
#endif
#ifndef atomic_try_cmpxchg
static inline bool
atomic_try_cmpxchg(atomic_t *v, atomic_count_t *old, atomic_count_t new)
{
	bool ret;
	__atomic_pre_full_fence();
	ret = atomic_try_cmpxchg_relaxed(v, old, new);
	__atomic_post_full_fence();
	return ret;
}
#define atomic_try_cmpxchg atomic_try_cmpxchg
#endif
#endif /* atomic_try_cmpxchg_relaxed */

#define atomic_cond_read_acquire(v, c) smp_cond_load_acquire(&(v)->counter, (c))
#define atomic_cond_read_relaxed(v, c) smp_cond_load_relaxed(&(v)->counter, (c))

#else /* CONFIG_SMP */
/* atomic_t should be 8 bit signed type */
typedef int32_t atomic_count_t;
typedef struct { atomic_count_t counter; } atomic_t;

#define ATOMIC_OP(op, c_op)						\
static inline void atomic_##op(atomic_count_t i, atomic_t *v)		\
{									\
	irq_flags_t flags;						\
	irq_local_save(flags);						\
	v->counter = v->counter c_op i;					\
	irq_local_restore(flags);					\
}
#define ATOMIC_NOP(op, c_op)						\
static inline void atomic_##op##not(atomic_count_t i, atomic_t *v)	\
{									\
	irq_flags_t flags;						\
	irq_local_save(flags);						\
	v->counter = v->counter c_op ~(i);				\
	irq_local_restore(flags);					\
}
#define ATOMIC_OP_RETURN(op, c_op)					\
static inline atomic_count_t atomic_##op##_return(atomic_count_t i,	\
						  atomic_t *v)		\
{									\
	irq_flags_t flags;						\
	atomic_count_t ret;						\
	irq_local_save(flags);						\
	ret = (v->counter = v->counter c_op i);				\
	irq_local_restore(flags);					\
	return ret;							\
}
#define ATOMIC_FETCH_OP(op, c_op)					\
static inline atomic_count_t atomic_fetch_##op(atomic_count_t i,	\
					       atomic_t *v)		\
{									\
	irq_flags_t flags;						\
	atomic_count_t ret;						\
	irq_local_save(flags);						\
	ret = v->counter;						\
	v->counter = v->counter c_op i;					\
	irq_local_restore(flags);					\
	return ret;							\
}
#define ATOMIC_FETCH_NOP(op, c_op)					\
static inline atomic_count_t atomic_fetch_##op##not(atomic_count_t i,	\
						    atomic_t *v)	\
{									\
	irq_flags_t flags;						\
	atomic_count_t ret;						\
	irq_local_save(flags);						\
	ret = v->counter;						\
	v->counter = v->counter c_op ~(i);				\
	irq_local_restore(flags);					\
	return ret;							\
}

ATOMIC_OP_RETURN(add, +)
ATOMIC_OP_RETURN(sub, -)
ATOMIC_FETCH_OP(add, +)
ATOMIC_FETCH_OP(sub, -)
ATOMIC_FETCH_OP(and, &)
ATOMIC_FETCH_OP(or, |)
ATOMIC_FETCH_OP(xor, ^)
ATOMIC_FETCH_NOP(and, &)
ATOMIC_FETCH_NOP(or, |)
ATOMIC_FETCH_NOP(xor, ^)
ATOMIC_OP(and, &)
ATOMIC_OP(or, |)
ATOMIC_OP(xor, ^)
ATOMIC_NOP(and, &)
ATOMIC_NOP(or, |)
ATOMIC_NOP(xor, ^)

#undef ATOMIC_OP
#undef ATOMIC_NOP
#undef ATOMIC_OP_RETURN
#undef ATOMIC_FETCH_OP
#undef ATOMIC_FETCH_NOP

#define ATOMIC_INIT(i)			{(i)}
#define INIT_ATOMIC(a, i)		((a)->counter = (i))

#define atomic_read(v)			READ_ONCE((v)->counter)
#define atomic_set(v, i)		WRITE_ONCE(((v)->counter), (i))
#define atomic_inc(v)			atomic_add((v), l)
#define atomic_dec(v)			atomic_sub((v), l)

#define atomic_dec_return(v)		atomic_sub_return(1, (v))
#define atomic_inc_return(v)		atomic_add_return(1, (v))
#define atomic_fetch_inc(v)		atomic_fetch_add(1, (v))
#define atomic_fetch_dec(v)		atomic_fetch_sub(1, (v))
#define atomic_add_and_test(i,v)	(atomic_add_return((i), (v)) == 0)
#define atomic_sub_and_test(i,v)	(atomic_sub_return((i), (v)) == 0)
#define atomic_inc_and_test(v)		(atomic_inc_return(v) == 0)
#define atomic_dec_and_test(v)		(atomic_sub_return(1, (v)) == 0)
#endif

#endif /* __ATOMIC_H_INCLUDE__ */
