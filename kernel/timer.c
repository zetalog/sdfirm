#include <target/timer.h>
#include <target/gpt.h>
#include <target/jiffies.h>

/* use the lowest bit to allow the maximum timeout values */
#define TIMER_FLAG_SHOT			((timeout_t)1)
#define TIMER_IDLE			((timeout_t)0)
#define TIMER_SHOT(timeout)		(timeout & TIMER_FLAG_SHOT)
/* thus timeout values are always an even value */
#define TIMER_TIME(timeout)		((timeout_t)((timeout_t)(timeout) >> 1))
#define TIMER_MAKE(shot, time)		((shot) | ((timeout_t)(time) << 1))

typedef struct timer_t {
	bh_t bh;
	timeout_t timeout;
} timer_t;

timer_t timer_entries[NR_TIMERS];
uint8_t timer_nr_regs = 0;
bh_t timer_bh = INVALID_BH;
tid_t timer_running_tid = INVALID_TID;

void __timer_reset_timeout(tid_t tid, timeout_t tout_ms)
{
	BUG_ON(tout_ms > MAX_TIMEOUT);
	timer_entries[tid].timeout = TIMER_MAKE(TIMER_FLAG_SHOT, tout_ms);
}

bh_t timer_tid2bh(tid_t tid)
{
	BUG_ON(tid >= NR_TIMERS);
	return timer_entries[tid].bh;
}

void __timer_run(tid_t tid)
{
	bh_t bh;

	timer_running_tid = tid;
	bh = timer_tid2bh(tid);
	__bh_run(bh, BH_TIMEOUT);
	timer_running_tid = INVALID_TID;
}

/* Register a timer delay to execute the state machine
 *
 * IN bh: the state machine will be executed when the timer is due
 * IN timeout: the period that will be delayed
 * OUT tid: return NR_TIMERS on error
 */
tid_t timer_register(bh_t bh, uint8_t type)
{
	tid_t tid = timer_nr_regs;
	BUG_ON(tid == NR_TIMERS);
	timer_entries[tid].bh = bh;
	timer_entries[tid].timeout = TIMER_IDLE;
	timer_nr_regs++;
	timer_running_tid = tid;
	return tid;
}

/* only if the tid equals to the running tid */
boolean timer_timeout_raised(tid_t tid, uint8_t type)
{
	return tid == timer_running_tid;
}

#ifdef CONFIG_TICK
tick_t timer_last_tick = 0;

void timer_run(uint8_t type)
{
	tid_t tid;
	timeout_t span;
	
	span = (timeout_t)(jiffies - timer_last_tick);
	timer_last_tick = jiffies;

	for (tid = 0; tid < timer_nr_regs; tid++) {
		if (TIMER_SHOT(timer_entries[tid].timeout)) {
			timeout_t tid_tout = TIMER_TIME(timer_entries[tid].timeout);
			if (span >= tid_tout) {
				timer_entries[tid].timeout = TIMER_IDLE;
				idle_debug(IDLE_DEBUG_TID, tid);
				__timer_run(tid);
			} else {
				timer_entries[tid].timeout = TIMER_MAKE(TIMER_FLAG_SHOT, tid_tout-span);
			}
		}
	}
}

#define timer_start()		bh_resume(timer_bh)
#define timer_restart()		timer_start()
#else
timeout_t timer_timeout = TIMER_MAKE(TIMER_FLAG_SHOT, 0);
timeout_t timer_unshot_timeout = 0;

void timer_run(uint8_t type)
{
	tid_t tid;
	timeout_t tout;

	BUG_ON(TIMER_SHOT(timer_timeout) != TIMER_FLAG_SHOT);
	tout = TIMER_TIME(timer_timeout);

	for (tid = 0; tid < timer_nr_regs; tid++) {
		if (TIMER_SHOT(timer_entries[tid].timeout)) {
			timeout_t tid_tout = TIMER_TIME(timer_entries[tid].timeout);
			if (tout >= tid_tout) {
				timer_entries[tid].timeout = TIMER_IDLE;
				idle_debug(IDLE_DEBUG_TID, tid);
				__timer_run(tid);
			} else {
				tid_tout -= tout;
				timer_entries[tid].timeout = TIMER_MAKE(TIMER_FLAG_SHOT, tid_tout);
			}
		}
	}

	timer_timeout = TIMER_IDLE;
	for (tid = 0; tid < timer_nr_regs; tid++) {
		if (TIMER_SHOT(timer_entries[tid].timeout)) {
			if (!TIMER_SHOT(timer_timeout)) {
				timer_timeout = timer_entries[tid].timeout;
			} else {
				if (TIMER_TIME(timer_entries[tid].timeout) < TIMER_TIME(timer_timeout))
					timer_timeout = timer_entries[tid].timeout;
			}
		}
	}
}

void timer_shot_timeout(timeout_t to_shot)
{
	if (to_shot == 0) {
		/* do not run GPT on 0 timeout shot value */
		bh_resume(timer_bh);
	} else if (to_shot < GPT_MAX_TIMEOUT) {
		timer_unshot_timeout = 0;
		gpt_oneshot_timeout(to_shot);
	} else {
		timer_unshot_timeout = to_shot - GPT_MAX_TIMEOUT;
		/* XXX: GPT_MAX_TIMEOUT Validation
		 *
		 * if GPT_MAX_TIMEOUT > max of timeout_t, following code
		 * could not get reached, so its safe to force it here
		 */
		gpt_oneshot_timeout((timeout_t)GPT_MAX_TIMEOUT);
	}
}

void timer_execute_shot(void)
{
	if (TIMER_SHOT(timer_timeout)) {
		timer_shot_timeout(TIMER_TIME(timer_timeout));
	}
}

void timer_start(void)
{
	gpt_hw_ctrl_init();
	timer_execute_shot();
}

void timer_restart(void)
{
	timer_execute_shot();
}

void tick_handler(void)
{
	if (timer_unshot_timeout)
		timer_shot_timeout(timer_unshot_timeout);
	else
		bh_resume(timer_bh);
}
#endif

void timer_schedule_shot(tid_t tid, timeout_t tout_ms)
{
	BUG_ON(tid != timer_running_tid);
	__timer_reset_timeout(tid, tout_ms);
}

void timer_handler(uint8_t event)
{
	BUG_ON(event != BH_WAKEUP);
	timer_run(TIMER_BH);
	timer_restart();
}

void timer_init(void)
{
	timer_bh = bh_register_handler(timer_handler);
	timer_start();
}
