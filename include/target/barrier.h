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
 * @(#)barrier.h: memory barrier interface
 * $Id: barrier.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __BARRIER_H_INCLUDE__
#define __BARRIER_H_INCLUDE__

#include <target/generic.h>

#define barrier()	__barrier()

#ifndef __ASSEMBLY__
/* Prevent the compiler from merging or refetching reads or writes.
 *
 * DEC Alpha may implement its own READ_ONCE clone for historical
 * reasons.
 */
#ifndef READ_ONCE
static __always_inline
void __read_once_size(const volatile void *p, void *res, int size)
{
	switch (size) {
	case 1: *(uint8_t *)res = *(volatile uint8_t *)p; break;
	case 2: *(uint16_t *)res = *(volatile uint16_t *)p; break;
	case 4: *(uint32_t *)res = *(volatile uint32_t *)p; break;
	case 8: *(uint64_t *)res = *(volatile uint64_t *)p; break;
	default:
		__barrier();
		__builtin_memcpy((void *)res, (const void *)p, size);
		__barrier();
	}
}

#define READ_ONCE(x)						\
({								\
	union { typeof(x) __val; char __c[1]; } __u;		\
	__read_once_size(&(x), __u.__c, sizeof(x));		\
	__u.__val;						\
})
#endif
#ifndef WRITE_ONCE
static __always_inline
void __write_once_size(volatile void *p, void *res, int size)
{
	switch (size) {
	case 1: *(volatile uint8_t *)p = *(uint8_t *)res; break;
	case 2: *(volatile uint16_t *)p = *(uint16_t *)res; break;
	case 4: *(volatile uint32_t *)p = *(uint32_t *)res; break;
	case 8: *(volatile uint64_t *)p = *(uint64_t *)res; break;
	default:
		__barrier();
		__builtin_memcpy((void *)p, (const void *)res, size);
		__barrier();
	}
}

#define WRITE_ONCE(x, val)					\
({								\
	union { typeof(x) __val; char __c[1]; } __u =		\
		{ .__val = (__force typeof(x)) (val) };	 	\
	__write_once_size(&(x), __u.__c, sizeof(x));		\
	__u.__val;						\
})
#endif
#endif /* __ASSEMBLY__ */

#include <asm/barrier.h>

#ifndef mb
#define mb()		__mb()
#endif

#ifndef rmb
#define rmb()		__rmb()
#endif

#ifndef wmb
#define wmb()		__wmb()
#endif

#ifndef __smp_mb
#define __smp_mb()	mb()
#endif

#ifndef __smp_rmb
#define __smp_rmb()	rmb()
#endif

#ifndef __smp_wmb
#define __smp_wmb()	wmb()
#endif

#ifndef __dma_mb
#define __dma_mb	mb()
#endif

#ifndef __dma_rmb
#define __dma_rmb()	rmb()
#endif

#ifndef __dma_wmb
#define __dma_wmb()	wmb()
#endif

#ifndef __smp_mb__before_atomic
#define __smp_mb__before_atomic()	__smp_mb()
#endif

#ifndef __smp_mb__after_atomic
#define __smp_mb__after_atomic()	__smp_mb()
#endif

#ifndef __smp_store_release
#define __smp_store_release(p, v)	\
	do {				\
		__smp_mb();		\
		WRITE_ONCE(*p, v);	\
	} while (0)
#endif

#ifndef __smp_load_acquire
#define __smp_load_acquire(p)				\
	({						\
		typeof(*p) ___p1 = READ_ONCE(*p);	\
		__smp_mb();				\
		___p1;					\
	})
#endif

#ifndef dma_mb
#define dma_mb()	__dma_mb()
#endif

#ifndef dma_rmb
#define dma_rmb()	__dma_rmb()
#endif

#ifndef dma_wmb
#define dma_wmb()	__dma_wmb()
#endif

#ifdef CONFIG_SMP

#ifndef smp_mb
#define smp_mb()	__smp_mb()
#endif

#ifndef smp_rmb
#define smp_rmb()	__smp_rmb()
#endif

#ifndef smp_wmb
#define smp_wmb()	__smp_wmb()
#endif

#ifndef smp_mb__before_atomic
#define smp_mb__before_atomic()	__smp_mb__before_atomic()
#endif

#ifndef smp_mb__after_atomic
#define smp_mb__after_atomic()	__smp_mb__after_atomic()
#endif

#ifndef smp_load_acquire
#define smp_load_acquire(p)	__smp_load_acquire(p)
#endif

#ifndef smp_store_release
#define smp_store_release(p, v)	__smp_store_release(p, v)
#endif

#else /* !CONFIG_SMP */

#ifndef smp_mb
#define smp_mb()	barrier()
#endif

#ifndef smp_rmb
#define smp_rmb()	barrier()
#endif

#ifndef smp_wmb
#define smp_wmb()	barrier()
#endif

#ifndef smp_mb__before_atomic
#define smp_mb__before_atomic()	barrier()
#endif

#ifndef smp_mb__after_atomic
#define smp_mb__after_atomic()	barrier()
#endif

#ifndef smp_store_release
#define smp_store_release(p, v)		\
	do {				\
		barrier();		\
		WRITE_ONCE(*p, v);	\
	} while (0)
#endif

#ifndef smp_load_acquire
#define smp_load_acquire(p)				\
	({						\
		typeof(*p) ___p1 = READ_ONCE(*p);	\
		barrier();				\
		___p1;					\
	})
#endif

#endif /* CONFIG_SMP */

#ifndef smp_cond_load_relaxed
#define smp_cond_load_relaxed(ptr, cond_expr) ({	\
	typeof(ptr) __PTR = (ptr);			\
	typeof(*ptr) VAL;				\
	for (;;) {					\
		VAL = READ_ONCE(*__PTR);		\
		if (cond_expr)				\
			break;				\
		cpu_relax();				\
	}						\
	VAL;						\
})
#endif

#ifndef smp_acquire__after_ctrl_dep
#define smp_acquire__after_ctrl_dep()		smp_rmb()
#endif

#ifndef smp_cond_load_acquire
#define smp_cond_load_acquire(ptr, cond_expr) ({	\
	typeof(*ptr) _val;				\
	_val = smp_cond_load_relaxed(ptr, cond_expr);	\
	smp_acquire__after_ctrl_dep();			\
	_val;						\
})
#endif

#endif /* __BARRIER_H_INCLUDE__ */
