/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __RISCV_LOCKS_H__
#define __RISCV_LOCKS_H__

typedef struct {
	volatile long lock;
} spinlock_t;

#define __RISCV_SPIN_UNLOCKED 0

#ifdef CONFIG_SMP
#define SPIN_LOCK_INIT		{ .lock = __RISCV_SPIN_UNLOCKED }
#define INIT_SPIN_LOCK(_l)	((_l)->lock = __RISCV_SPIN_UNLOCKED)

#define DEFINE_SPIN_LOCK(_l)		\
	spinlock_t _l = SPIN_LOCK_INIT(__RISCV_SPIN_UNLOCKED)
int spin_trylock(spinlock_t *lock);
void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);
#else
#define INIT_SPIN_LOCK(_l)
#define DEFINE_SPIN_LOCK(_l)
#define spin_lock(lock)			do { } while (0)
#define spin_unlock(lock)		do { } while (0)
#endif

#endif
