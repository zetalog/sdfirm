#include <target/irq.h>
#include <target/panic.h>
#include <target/percpu.h>

#ifdef CONFIG_SMP
struct smp_idle {
	DECLARE_BITMAP(bh_awakes, NR_BHS);
	struct bh_entry bh_entries[NR_BHS];
	bh_t bh_nr_regs;
};

DEFINE_PERCPU(struct smp_idle, smp_idles);

#define bh_awakes	this_cpu_ptr(&smp_idles)->bh_awakes
#define bh_entries	this_cpu_ptr(&smp_idles)->bh_entries
#define bh_nr_regs	this_cpu_ptr(&smp_idles)->bh_nr_regs
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

void __bh_run(bh_t bh, uint8_t event)
{
	bh_entries[bh].handler(event);
}

void bh_run(bh_t bh, uint8_t event)
{
	lock_bh();
	idle_debug(IDLE_DEBUG_SID, bh);
	__bh_run(bh, event);
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

boolean bh_resumed_any(void)
{
	return irq_is_polling ||
	       NR_BHS != find_next_set_bit(bh_awakes, NR_BHS, 0);
}

bh_t bh_register_handler(bh_cb handler)
{
	bh_t bh = bh_nr_regs;

	BUG_ON(bh == NR_BHS);
	bh_entries[bh].handler = handler;
	bh_nr_regs++;
	return bh;
}

void bh_run_all(void)
{
	bh_t bh;

	for (bh = 0; bh < bh_nr_regs; bh++) {
		if (test_bit(bh, bh_awakes)) {
			bh_suspend(bh);
			bh_run(bh, BH_WAKEUP);
		}
		irq_poll_bh(bh);
	}
}

void bh_panic(void)
{
	idle_debug(IDLE_DEBUG_SID, DBG_SRC_IDLE);
}

void bh_loop(void)
{
	irq_local_enable();
	main_debug(MAIN_DEBUG_INIT, 1);
	while (1) {
#ifdef CONFIG_IDLE
		while (!bh_resumed_any()) {
			dbg_dump(0xAA);
			wait_irq();
		}
		dbg_dump(0xAB);
#endif
		bh_run_all();
	}
}

void bh_init(void)
{
}
