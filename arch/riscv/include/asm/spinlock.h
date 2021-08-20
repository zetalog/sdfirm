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

#include <target/atomic.h>
#ifdef CONFIG_RISCV_SPINLOCK_GENERIC
#include <asm-generic/spinlock.h>
#endif
#include <asm/vaisra_cache.h>

#ifdef CONFIG_RISCV_SPINLOCK_TICKET
/*
 * Ticket locks are conceptually two parts, one indicating the current
 * head of the queue, and the other indicating the current tail. The lock
 * is acquired by atomically noting the tail and incrementing it by one
 * (thus adding ourself to the queue and noting our position), then
 * waiting until the head becomes equal to the the initial value of the
 * tail.
 * The pad bits in the middle are used to prevent the next_ticket number
 * overflowing into the now_serving number.
 *
 *  31     15     0
 *  +------+-------+
 *  | next | owner |
 *  +------+-------+
 */
#define TICKET_SHIFT	16
#define TICKET_BITS	16
#define TICKET_MASK	((ULL(1) << TICKET_BITS) - 1)

typedef struct {
	union {
		uint64_t lock;
		struct {
			atomic32_t owner;
			atomic32_t next;
		};
	};
} spinlock_t;

#define DEFINE_SPINLOCK(lock)		spinlock_t lock = { 0 }
#define smp_hw_spin_init(x)		WRITE_ONCE(((x)->lock), 0)

#define __ticket_matched(t, o)		((t) == (o))

static inline int smp_hw_spin_locked(spinlock_t *lock)
{
	uint32_t t = smp_hw_atomic32_read(&lock->next);
	uint32_t o = smp_hw_atomic32_read(&lock->owner);

	return !__ticket_matched(t, o);
}

static inline int smp_hw_spin_trylock(spinlock_t *lock)
{
	uint32_t t = smp_hw_atomic32_add_return(1, &lock->next);
	uint32_t o = smp_hw_atomic32_read(&lock->owner);

	if (__ticket_matched(t - 1, o))
		return 1;

	smp_hw_atomic32_sub(1, &lock->next);
	return 0;
}

static inline void smp_hw_spin_lock(spinlock_t *lock)
{
	uint32_t t = smp_hw_atomic32_add_return(1, &lock->next);
	uint32_t o;

	do {
		o = smp_hw_atomic32_read(&lock->owner);
		if (__ticket_matched(t - 1, o)) {
			__atomic_acquire_fence();
			return;
		}
		cpu_relax();
	} while (1);
}

static inline void smp_hw_spin_unlock(spinlock_t *lock)
{
	__atomic_release_fence();
	smp_hw_atomic32_add(1, &lock->owner);
}
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
	nop();
	vaisra_flush_dcache_addr((void *)(&lock->lock));
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
