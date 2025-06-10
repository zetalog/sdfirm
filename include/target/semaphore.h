#ifndef __SEMAPHORE_H_INCLUDE__
#define __SEMAPHORE_H_INCLUDE__

#include <target/spinlock.h>
#include <target/jiffies.h>
#include <target/task.h>

typedef struct semaphore {
	int value;
	int limit;
	spinlock_t lock;
#ifdef CONFIG_TASK
	task_wait_t wq;
#endif
} semaphore_t;

#define sem_init(sem, max, ini)				\
	do {						\
		spin_lock_init(&sem->lock);		\
		sem->limit = (max);			\
		sem->value = (ini);			\
	} while (0)

int sem_wait(semaphore_t *sem, int count, tick_t timeout);
int sem_signal(semaphore_t *sem, int count);

#endif /* __SEMAPHORE_H_INCLUDE__ */
