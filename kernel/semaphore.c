#include <target/semaphore.h>
#include <target/delay.h>

#ifdef CONFIG_TASK
#define wait_sem(sem, timeout)	task_wait(&sem->wq, current, timeout)
#define wake_sem(sem)		take_wake(&sem->wq);
#else
static inline int wait_sem(semaphore_t *sem, tick_t timeout)
{
	udelay(timeout);
	return -ETIME;
}
#define wake_sem(sem)		do { } while (0)
#endif

int sem_wait(semaphore_t *sem, int count, tick_t timeout)
{
	int ret = 0;

	spin_lock(&sem->lock);
	while (sem->value <= count - 1) {
		spin_unlock(&sem->lock);
		ret = wait_sem(sem, timeout);
		spin_lock(&sem->lock);
		if (ret != 0)
			break;
	}
	if (ret != ETIME)
		sem->value -= count;
	spin_unlock(&sem->lock);

	return ret;
}

int sem_signal(semaphore_t *sem, int count)
{
	int ret = 0;

	spin_lock(&sem->lock);
	if (sem->value + count > sem->limit)
		ret = -EINVAL;
	else {
		sem->value += count;
		if (sem->value > 0) {
			spin_unlock(&sem->lock);
			wake_sem(sem);
			spin_lock(&sem->lock);
		}
	}
	spin_unlock(&sem->lock);

	return ret;
}
