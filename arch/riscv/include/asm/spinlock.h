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
 * @(#)spinlock.h: RISCV specific spinlock interfaces
 * $Id: spinlock.h,v 1.0 2020-01-05 21:43:00 zhenglv Exp $
 */

#ifndef __SPINLOCK_RISCV_H_INCLUDE__
#define __SPINLOCK_RISCV_H_INCLUDE__

#ifdef CONFIG_RISCV_SPINLOCK_GENERIC
#include <asm-generic/spinlock.h>
#endif

#ifdef CONFIG_RISCV_SPINLOCK_RAW
typedef struct {
	volatile unsigned int lock;
} spinlock_t;

#define DEFINE_SPINLOCK(lock)		spinlock_t lock = { 0 }

#define smp_hw_spin_init(x)		((x)->lock = 0)
#define smp_hw_spin_locked(x)		(READ_ONCE((x)->lock) != 0)

static inline void smp_hw_spin_unlock(spinlock_t *lock)
{
	smp_store_release(&lock->lock, 0);
}

static inline int smp_hw_spin_trylock(spinlock_t *lock)
{
	int tmp = 1, busy;

	asm volatile (
		"	amoswap.w %0, %2, %1\n"
		RISCV_ACQUIRE_BARRIER
		: "=r" (busy), "+A" (lock->lock)
		: "r" (tmp)
		: "memory");

	return !busy;
}

static inline void smp_hw_spin_lock(spinlock_t *lock)
{
	while (1) {
		if (smp_hw_spin_locked(lock))
			continue;

		if (smp_hw_spin_trylock(lock))
			break;
	}
}
#endif

#endif /* __SPINLOCK_RISCV_H_INCLUDE__ */
