#ifndef __ASM_GENERIC_SPINLOCK_H_INCLUDE__
#define __ASM_GENERIC_SPINLOCK_H_INCLUDE__

#include <target/atomic.h>
#include <target/barrier.h>

typedef struct spinlock {
	union {
		atomic_t val;
		struct {
			union {
				qspin_half_t locked_pending;
				struct {
					qspin_quater_t locked;
					qspin_quater_t pending;
				};
			} u;
			qspin_half_t tail;
		};
	};
} spinlock_t;

#define	__ARCH_SPIN_LOCK_UNLOCKED		{ { .val = ATOMIC_INIT(0) } }
#define __SPIN_LOCK_INITIALIZER(lockname)	__ARCH_SPIN_LOCK_UNLOCKED
#define __SPIN_LOCK_UNLOCKED(lockname)		\
	(spinlock_t ) __SPIN_LOCK_INITIALIZER(lockname)
#define DEFINE_SPINLOCK(x)			\
	spinlock_t x = __SPIN_LOCK_UNLOCKED(x)

/* Bitfields in the atomic value:
 *
 * When NR_CPUS < 16K
 *  0- 7: locked byte
 *     8: pending
 *  9-15: not used
 * 16-17: tail index
 * 18-31: tail cpu (+1)
 *
 * When NR_CPUS >= 16K
 *  0- 7: locked byte
 *     8: pending
 *  9-10: tail index
 * 11-31: tail cpu (+1)
 */
#define	_Q_SET_MASK(type)	(((1ULL << _Q_ ## type ## _BITS) - 1)\
				      << _Q_ ## type ## _OFFSET)
#define _Q_LOCKED_OFFSET	0
#define _Q_LOCKED_BITS		16
#define _Q_LOCKED_MASK		_Q_SET_MASK(LOCKED)

#define _Q_PENDING_OFFSET	(_Q_LOCKED_OFFSET + _Q_LOCKED_BITS)
#if NR_CPUS < (1U << 14)
#define _Q_PENDING_BITS		16
#else
#define _Q_PENDING_BITS		1
#endif
#define _Q_PENDING_MASK		_Q_SET_MASK(PENDING)

#define _Q_TAIL_IDX_OFFSET	(_Q_PENDING_OFFSET + _Q_PENDING_BITS)
#define _Q_TAIL_IDX_BITS	2
#define _Q_TAIL_IDX_MASK	_Q_SET_MASK(TAIL_IDX)

#define _Q_TAIL_CPU_OFFSET	(_Q_TAIL_IDX_OFFSET + _Q_TAIL_IDX_BITS)
#define _Q_TAIL_CPU_BITS	(64 - _Q_TAIL_CPU_OFFSET)
#define _Q_TAIL_CPU_MASK	_Q_SET_MASK(TAIL_CPU)

#define _Q_TAIL_OFFSET		_Q_TAIL_IDX_OFFSET
#define _Q_TAIL_MASK		(_Q_TAIL_IDX_MASK | _Q_TAIL_CPU_MASK)

#define _Q_LOCKED_VAL		(1ULL << _Q_LOCKED_OFFSET)
#define _Q_PENDING_VAL		(1ULL << _Q_PENDING_OFFSET)

#define qspin_init(lock)			\
	(INIT_ATOMIC(&(lock)->val, 0))
#define qspin_is_locked(lock)		atomic_read(&(lock)->val)
#define qspin_value_unlocked(lock)	(!atomic_read(&(lock).val))
#define qspin_is_contended(lock)	\
	(atomic_read(&(lock)->val) & ~_Q_LOCKED_MASK)

void qspin_lock(struct spinlock *lock);
int qspin_trylock(struct spinlock *lock);
#define qspin_unlock(lock)		smp_store_release(&(lock)->u.locked, 0)

#define smp_hw_spin_init(l)		qspin_init(l)
#define smp_hw_spin_is_locked(l)	qspin_is_locked(l)
#define smp_hw_spin_lock(l)		qspin_lock(l)
#define smp_hw_spin_trylock(l)		qspin_trylock(l)
#define smp_hw_spin_unlock(l)		qspin_unlock(l)

#endif /* __ASM_GENERIC_SPINLOCK_H_INCLUDE__ */
