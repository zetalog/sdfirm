#include <target/timer.h>
#include <target/bh.h>
#include <target/gpt.h>
#include <target/irq.h>
#include <target/jiffies.h>
#include <target/percpu.h>
#include <target/panic.h>

/* use the lowest bit to allow the maximum timeout values */
#define TIMER_FLAG_SHOT			((timeout_t)1)
#define TIMER_IDLE			((timeout_t)0)
#define TIMER_SHOT(timeout)		((timeout) & TIMER_FLAG_SHOT)
/* thus timeout values are always an even value */
#define TIMER_TIME(timeout)		((timeout) >> 1)
#define TIMER_MAKE(time)		\
	(TIMER_FLAG_SHOT | ((timeout_t)(time) << 1))

#ifdef CONFIG_SMP
struct smp_timer {
	timer_desc_t *smp_timer_descs[NR_TIMERS];
	timeout_t smp_timer_timeouts[NR_TIMERS];
	DECLARE_BITMAP(smp_timer_regs, NR_TIMERS);
	bh_t smp_timer_bh;
	tid_t smp_timer_running_tid;
	tid_t smp_timer_orders[NR_TIMERS+1];
	tick_t smp_timer_last_tick;
#ifndef CONFIG_TICK
	timeout_t smp_timer_timeout;
	timeout_t smp_timer_unshot_timeout;
#ifdef SYS_BOOTLOAD
	boolean smp_timer_polling;
#endif
#endif
};

DEFINE_PERCPU(struct smp_timer, smp_timers);

#define timer_descs		this_cpu_ptr(&smp_timers)->smp_timer_descs
#define timer_timeouts		\
	this_cpu_ptr(&smp_timers)->smp_timer_timeouts
#define timer_regs		this_cpu_ptr(&smp_timers)->smp_timer_regs
#define timer_bh		this_cpu_ptr(&smp_timers)->smp_timer_bh
#define timer_running_tid	\
	this_cpu_ptr(&smp_timers)->smp_timer_running_tid
#define timer_orders		this_cpu_ptr(&smp_timers)->smp_timer_orders
#define timer_last_tick		this_cpu_ptr(&smp_timers)->smp_timer_last_tick
#ifndef CONFIG_TICK
#define timer_timeout		this_cpu_ptr(&smp_timers)->smp_timer_timeout
#define timer_unshot_timeout	\
	this_cpu_ptr(&smp_timers)->smp_timer_unshot_timeout
#ifdef SYS_BOOTLOAD
#define timer_polling		this_cpu_ptr(&smp_timers)->smp_timer_polling
#endif
#endif
#else
timer_desc_t *timer_descs[NR_TIMERS];
timeout_t timer_timeouts[NR_TIMERS];
DECLARE_BITMAP(timer_regs, NR_TIMERS);
bh_t timer_bh;
tid_t timer_running_tid;
tid_t timer_orders[NR_TIMERS+1];
tick_t timer_last_tick;
#ifndef CONFIG_TICK
timeout_t timer_timeout;
timeout_t timer_unshot_timeout;
#ifdef SYS_BOOTLOAD
boolean timer_polling;
#endif
#endif
#endif

void __timer_del(tid_t tid)
{
	tid_t i;

	for (i = 0; i < NR_TIMERS; i++) {
		if (timer_orders[i] == INVALID_TID)
			return;
		if (timer_orders[i] == tid)
			break;
	}
	for (; i < NR_TIMERS-1; i++) {
		if (timer_orders[i] != INVALID_TID) {
			tid_t swp = timer_orders[i+1];
			timer_orders[i+1] = timer_orders[i];
			timer_orders[i] = swp;
		}
	}
	/* This ensures timer_orders[NR_TIMERS] always INVALID_TID */
	timer_orders[i] = INVALID_TID;
}

void __timer_add(tid_t tid)
{
	tid_t i, t;

	for (i = 0; i < NR_TIMERS; i++) {
		t = timer_orders[NR_TIMERS-i-1];
		/* should be already deleted */
		BUG_ON(t == tid);
		if (t != INVALID_TID) {
			/* should not exceed NR_TIMERS */
			BUG_ON(i == 0);
			if (TIMER_TIME(timer_timeouts[t]) >
			    TIMER_TIME(timer_timeouts[tid])) {
				timer_orders[NR_TIMERS-i] = t;
				timer_orders[NR_TIMERS-i-1] = INVALID_TID;
			} else {
				timer_orders[NR_TIMERS-i] = tid;
				return;
			}
		}
	}
	BUG_ON(timer_orders[0] != INVALID_TID);
	timer_orders[0] = tid;
}

void __timer_run(tid_t tid)
{
	timer_desc_t *timer = timer_descs[tid];

	idle_debug(IDLE_DEBUG_TID, tid);
	BUG_ON(!timer || !timer->handler);
	timer_running_tid = tid;
	__timer_del(tid);
	timer->handler();
	timer_running_tid = INVALID_TID;
}

void timer_run_timeout(uint8_t type)
{
	tid_t tid;
	timer_desc_t *timer;

	/* Only run the top most timed out timer, in such a way, the other
	 * timers can be unregistered or rescheduled from the top most timer's
	 * timeout handler.
	 */
	do {
		tid = timer_orders[0];
		timer = timer_descs[tid];
		if ((tid == INVALID_TID) ||
		    (timer_timeouts[tid] != TIMER_IDLE)) {
			break;
		}
		BUG_ON((type == TIMER_BH) && (timer->type != type));
		if (timer->type == type)
			__timer_run(tid);
	} while (1);
}

/* Register a timer delay to execute the state machine
 *
 * IN bh: the state machine will be executed when the timer is due
 * IN timeout: the period that will be delayed
 * OUT tid: return NR_TIMERS on error
 */
tid_t timer_register(timer_desc_t *timer)
{
	tid_t tid;

	BUG_ON(!smp_initialized);
	tid = find_first_clear_bit(timer_regs, NR_TIMERS);
	BUG_ON(tid == NR_TIMERS);
	timer_descs[tid] = timer;
	timer_timeouts[tid] = TIMER_IDLE;
	set_bit(tid, timer_regs);
	timer_running_tid = tid;

	return tid;
}

void timer_unregister(tid_t tid)
{
	BUG_ON(tid != timer_running_tid);
	__timer_del(tid);
	timer_descs[tid] = NULL;
	clear_bit(tid, timer_regs);
	timer_running_tid = INVALID_TID;
}

#ifdef CONFIG_TICK
void timer_bh_timeout(void)
{
	tid_t tid, i;
	timeout_t span;
	timer_desc_t *timer;
	boolean run_bh = false;
	
	span = (timeout_t)(jiffies - timer_last_tick);
	timer_last_tick = jiffies;
	for (i = 0; i < NR_TIMERS; i++) {
		tid = timer_orders[i];
		if (tid == INVALID_TID)
			break;
		timer = timer_descs[tid];
		if (TIMER_SHOT(timer_timeouts[tid])) {
			timeout_t tid_tout;

			tid_tout = TIMER_TIME(timer_timeouts[tid]);
			if (span >= tid_tout) {
				timer_timeouts[tid] = TIMER_IDLE;
				BUG_ON(timer->type != TIMER_BH);
				run_bh = true;
			} else {
				timer_timeouts[tid] = TIMER_MAKE(tid_tout-span);
			}
		}
	}
	if (run_bh)
		timer_run_timeout(TIMER_BH);
}

#define timer_start()			bh_resume(timer_bh)
#define timer_restart()			timer_start()
#define timer_poll_handler()		do { } while (0)
#define timer_recalc_timeout(shot)	do { } while (0)
#else
#ifdef SYS_BOOTLOAD
#define timer_poll_start()		(timer_polling = true)
#define timer_poll_stop()		(timer_polling = false)
#define timer_poll_init()		(irq_register_poller(timer_bh))

static void timer_poll_handler(void)
{
	if (timer_polling)
		gpt_hw_irq_poll();
}
#else
#define timer_poll_init()
#define timer_poll_start()
#define timer_poll_stop()
#define timer_poll_handler()
#endif

void timer_shot_timeout(timeout_t to_shot)
{
	if (to_shot < GPT_MAX_TIMEOUT) {
		timer_unshot_timeout = 0;
		/* Some GPTs do not kick unless there is a significant
		 * timeout timer.
		 */
		if (to_shot == 0)
			to_shot = 1;
	} else {
		/* This logic is added on MCS51 system, where TICKLESS
		 * is still implemented while the GPTs on that systems
		 * do not allow long enough timeout value.
		 */
		timer_unshot_timeout = to_shot - GPT_MAX_TIMEOUT;
		/* XXX: GPT_MAX_TIMEOUT Validation
		 *
		 * if GPT_MAX_TIMEOUT > max of timeout_t, following code
		 * could not get reached, so its safe to force it here
		 */
		to_shot = (timeout_t)GPT_MAX_TIMEOUT;
	}
	timer_poll_start();
	gpt_oneshot_timeout(to_shot);
}

static void timer_restart(void)
{
	if (TIMER_SHOT(timer_timeout))
		timer_shot_timeout(TIMER_TIME(timer_timeout));
}

static void __timer_recalc_timeout(boolean shot)
{
	tid_t tid, i;
	timeout_t span, tout;
	tick_t tick = tick_get_counter();

	if (shot) {
		/* FIXME:
		 * Not precise timeouts to avoid timeout overheats.
		 */
		span = TIMER_TIME(timer_timeout);
	} else {
		span = tick - timer_last_tick;
		if (span < 0)
			span = 0;
	}
	timer_last_tick = tick;

	timer_timeout = TIMER_IDLE;
	for (i = 0; i < NR_TIMERS; i++) {
		tid = timer_orders[i];
		if (tid == INVALID_TID)
			break;
		if (TIMER_SHOT(timer_timeouts[tid])) {
			tout = TIMER_TIME(timer_timeouts[tid]) - span;
			/* Some GPTs do not kick unless there is a
			 * significant timeout timer.
			 */
			if (!shot && tout <= 0)
				tout = 1;
			timer_timeouts[tid] = TIMER_MAKE(tout);
			if (!TIMER_SHOT(timer_timeout))
				timer_timeout = timer_timeouts[tid];
			else
				BUG_ON(TIMER_TIME(timer_timeouts[tid] <
				       TIMER_TIME(timer_timeout)));
		}
	}
}

void timer_recalc_timeout(boolean shot)
{
	__timer_recalc_timeout(shot);
	timer_restart();
}

void timer_bh_timeout(void)
{
	timer_run_timeout(TIMER_BH);
	timer_recalc_timeout(true);
}

void timer_start(void)
{
	gpt_hw_ctrl_init();
	bh_resume(timer_bh);
	timer_poll_init();
}

void timer_irq_timeout(void)
{
	tid_t tid, i;
	timeout_t tout;
	timer_desc_t *timer;
	boolean run_irq = false, run_bh = false;

	if (TIMER_SHOT(timer_timeout) != TIMER_FLAG_SHOT)
		return;

	tout = TIMER_TIME(timer_timeout);
	for (i = 0; i < NR_TIMERS; i++) {
		tid = timer_orders[i];
		if (tid == INVALID_TID)
			break;
		timer = timer_descs[tid];
		if (TIMER_SHOT(timer_timeouts[tid])) {
			timeout_t tid_tout = TIMER_TIME(timer_timeouts[tid]);
			if (tout >= tid_tout) {
				timer_timeouts[tid] = TIMER_IDLE;
				if (timer->type != TIMER_IRQ) {
					run_bh = true;
					bh_resume(timer_bh);
				} else {
					run_irq = true;
				}
			} else {
				tid_tout -= tout;
				timer_timeouts[tid] = TIMER_MAKE(tid_tout);
			}
		}
	}
	if (!run_bh)
		timer_recalc_timeout(true);
	if (run_irq)
		timer_run_timeout(TIMER_IRQ);
}

void tick_handler(void)
{
	timer_poll_stop();
	if (timer_unshot_timeout)
		timer_shot_timeout(timer_unshot_timeout);
	else
		timer_irq_timeout();
}
#endif

void timer_schedule_shot(tid_t tid, timeout_t tout_ms)
{
	BUG_ON(tout_ms > MAX_TIMEOUT);
	__timer_del(tid);
	__timer_recalc_timeout(false);
	if (tout_ms <= 0)
		tout_ms = 1;
	timer_timeouts[tid] = TIMER_MAKE(tout_ms);
	__timer_add(tid);
	if (!TIMER_SHOT(timer_timeout) ||
	    tout_ms < TIMER_TIME(timer_timeout))
		timer_timeout = timer_timeouts[tid];
	timer_restart();
}

void timer_bh_handler(uint8_t event)
{
	if (event == BH_WAKEUP) {
		timer_bh_timeout();
		timer_restart();
	} else if (event == BH_POLLIRQ) {
		timer_poll_handler();
	}
}

#ifdef CONFIG_TIMER_TEST
tid_t tid_1;
tid_t tid_2;
tid_t tid_3;

#ifdef CONFIG_TIMER_TEST_MAX_SHOTS
#define TIMER_TEST_SHOTS	CONFIG_TIMER_TEST_MAX_SHOTS
#else
#define TIMER_TEST_SHOTS	-1
#endif

int timer_nr_tests = TIMER_TEST_SHOTS;

#ifdef CONFIG_TIMER_TEST_SHUTDOWN
#define timer_test_shutdown()		board_shutdown()
#else
#define timer_test_shutdown()		do { } while (0)
#endif

void timer_test_handler(void)
{
	printf("timeout %d on %d, 0x%016lx\n", timer_running_tid,
	       smp_processor_id(), (unsigned long)tick_get_counter());
	if (timer_nr_tests) {
#ifdef CONFIG_TIMER_TEST_TIMEOUT
		timer_schedule_shot(timer_running_tid, CONFIG_TIMER_TEST_SHOT_PERIOD);
#else
		timer_schedule_shot(timer_running_tid, 5000);
#endif
		if (timer_nr_tests > 0)
			timer_nr_tests--;
	} else {
		printf("time test end %d on %d, 0x%016lx\n", timer_running_tid,
				smp_processor_id(), (unsigned long)tick_get_counter());
		timer_test_shutdown();
	}
}

timer_desc_t timer_1 = {
	TIMER_IRQ,
	timer_test_handler,
};

timer_desc_t timer_2 = {
	TIMER_BH,
	timer_test_handler,
};

timer_desc_t timer_3 = {
	TIMER_IRQ,
	timer_test_handler,
};

void timer_test(void)
{
	tid_1 = timer_register(&timer_1);
#ifndef CONFIG_TIMER_TEST_TIMEOUT
	timer_schedule_shot(tid_1, 20);
	BUG_ON(timer_orders[0] != tid_1);
	BUG_ON(timer_orders[1] != INVALID_TID);
	BUG_ON(timer_orders[2] != INVALID_TID);

	tid_2 = timer_register(&timer_2);
	timer_schedule_shot(tid_2, 30);
	BUG_ON(timer_orders[0] != tid_1);
	BUG_ON(timer_orders[1] != tid_2);
	BUG_ON(timer_orders[2] != INVALID_TID);
	timer_schedule_shot(tid_2, 10);
	BUG_ON(timer_orders[0] != tid_2);
	BUG_ON(timer_orders[1] != tid_1);
	BUG_ON(timer_orders[2] != INVALID_TID);
	timer_schedule_shot(tid_2, 30);
	BUG_ON(timer_orders[0] != tid_1);
	BUG_ON(timer_orders[1] != tid_2);
	BUG_ON(timer_orders[2] != INVALID_TID);

	tid_3 = timer_register(&timer_3);
	timer_schedule_shot(tid_3, 10);
	BUG_ON(timer_orders[0] != tid_3);
	BUG_ON(timer_orders[1] != tid_1);
	BUG_ON(timer_orders[2] != tid_2);
	timer_schedule_shot(tid_3, 25);
	BUG_ON(timer_orders[0] != tid_1);
	BUG_ON(timer_orders[1] != tid_3);
	BUG_ON(timer_orders[2] != tid_2);
	timer_schedule_shot(tid_3, 40);
	BUG_ON(timer_orders[0] != tid_1);
	BUG_ON(timer_orders[1] != tid_2);
	BUG_ON(timer_orders[2] != tid_3);

	timer_running_tid = tid_1;
	timer_unregister(tid_1);
	BUG_ON(timer_orders[0] != tid_2);
	BUG_ON(timer_orders[1] != tid_3);
	BUG_ON(timer_orders[2] != INVALID_TID);

	timer_running_tid = tid_2;
	timer_unregister(tid_2);
	BUG_ON(timer_orders[0] != tid_3);
	BUG_ON(timer_orders[1] != INVALID_TID);
	BUG_ON(timer_orders[2] != INVALID_TID);

	timer_running_tid = tid_3;
	timer_unregister(tid_3);
	BUG_ON(find_first_clear_bit(timer_regs, NR_TIMERS) != 0);
	BUG_ON(timer_orders[0] != INVALID_TID);
	BUG_ON(timer_orders[1] != INVALID_TID);
	BUG_ON(timer_orders[2] != INVALID_TID);
#else
	timer_schedule_shot(timer_running_tid, CONFIG_TIMER_TEST_SHOT_PERIOD);
#endif
}
#else
#define timer_test()
#endif

void timer_init(void)
{
	tid_t tid;

	BUG_ON(!smp_initialized);

	/* The last timer order indexing value is always INVALID_TID */
	timer_running_tid = INVALID_TID;
	timer_timeout = TIMER_IDLE;
	for (tid = 0; tid < NR_TIMERS+1; tid++)
		timer_orders[tid] = INVALID_TID;
	timer_bh = bh_register_handler(timer_bh_handler);
	timer_start();
	timer_test();
}
