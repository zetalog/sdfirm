#include <target/irq.h>
#include <target/panic.h>
#include <target/percpu.h>

#ifdef CONFIG_SMP
struct smp_idle {
	DECLARE_BITMAP(smp_bh_awakes, NR_BHS);
	struct bh_entry smp_bh_entries[NR_BHS];
	bh_t smp_bh_nr_regs;
};

DEFINE_PERCPU(struct smp_idle, smp_idles);

#define bh_awakes	this_cpu_ptr(&smp_idles)->smp_bh_awakes
#define bh_entries	this_cpu_ptr(&smp_idles)->smp_bh_entries
#define bh_nr_regs	this_cpu_ptr(&smp_idles)->smp_bh_nr_regs
#else
DECLARE_BITMAP(bh_awakes, NR_BHS);
struct bh_entry bh_entries[NR_BHS];
bh_t bh_nr_regs = 0;
#endif

/* big background lock */
#ifdef SYS_REALTIME
/* XXX: Interrupted Timer Handled In Time
 *
 * Whenever we talk about the realtime, what we mean include followings:
 * 1. interrupt timer (itimer):
 *    timeout events can be raised in time by the system
 * 2. real time clock (rtc):
 *    both of the communication entities can refer to a 3rd party
 *    authoritative "real" time axis
 *
 * Referring to a real time axis is critical for system throuputs.  Since
 * throuput varies inversely with time, a stretched time axis will always
 * lead to the lower throuputs.  To ensure real time axis, system codes
 * need to measure a 3rd party clock authority, then correct the time
 * error in their algorithm based on the measurements.  Which means it is
 * ensured by the system codes' quality, not by the system architecture.
 *
 * Since we are talking about realtime in the system architecture, it
 * implies the in time timer issue.  No global IRQ masking means timeout
 * handler in the interrupt context can be handled in time!
 */
#define lock_bh()
#define unlock_bh()
#else
#define lock_bh()			irq_local_disable()
#define unlock_bh()			irq_local_enable()
#endif

void bh_run(bh_t bh, uint8_t event)
{
	lock_bh();
	idle_debug(IDLE_DEBUG_SID, bh);
	bh_entries[bh].handler(event);
	unlock_bh();
}

void bh_suspend(bh_t bh)
{	
	clear_bit((bh), bh_awakes);
}

void bh_resume(bh_t bh)
{
	set_bit(bh, bh_awakes);
}

#ifdef CONFIG_SMP
void bh_resume_smp(bh_t bh, cpu_t cpu)
{
	struct smp_idle *idle = per_cpu_ptr(&smp_idles, cpu);

	set_bit(bh, idle->smp_bh_awakes);
}
#endif

static bh_t bh_resumed(bh_t last_bh, boolean recursive)
{
	bh_t bh;

	bh = find_next_set_bit(bh_awakes, NR_BHS, last_bh);
	if (last_bh == 0 || bh != INVALID_BH || !recursive)
		return bh;
	return find_next_set_bit(bh_awakes, NR_BHS, 0);
}

static bh_t bh_run_once(bh_t bh, boolean recursive)
{
	bh_t next_bh;

	next_bh = bh_resumed(bh, recursive);
	if (next_bh == INVALID_BH)
		return next_bh;
	bh_suspend(next_bh);
	bh_run(next_bh, BH_WAKEUP);
	next_bh++;
	if (recursive && next_bh == INVALID_BH)
		next_bh = 0;
	return next_bh;
}

bh_t bh_register_handler(bh_cb handler)
{
	bh_t bh = bh_nr_regs;

	BUG_ON(bh == NR_BHS);
	bh_entries[bh].handler = handler;
	bh_nr_regs++;
	return bh;
}

void bh_panic(void)
{
	idle_debug(IDLE_DEBUG_SID, DBG_SRC_IDLE);
}

static void __bh_sync(boolean recursive)
{
	bh_t bh = 0;

	while (bh != INVALID_BH)
		bh = bh_run_once(bh, recursive);
}

void bh_sync(void)
{
	__bh_sync(true);
}

#ifdef CONFIG_IDEL
static void bh_irq_wait(void)
{
	dbg_dump(0xAA);
	wait_irq();
	dbg_dump(0xAB);
}
#else
#define bh_irq_wait()		do { } while (0)
#endif

void bh_loop(void)
{
	irq_local_enable();
	main_debug(MAIN_DEBUG_INIT, 1);
	do {
		__bh_sync(false);
		if (!irq_poll_bh())
			bh_irq_wait();
	} while (1);
}

void bh_init(void)
{
}
