#include <target/state.h>
#include <target/irq.h>

DECLARE_BITMAP(state_awakes, NR_STATES);
struct state_entry state_entries[NR_STATES];
sid_t state_nr_regs = 0;
DECLARE_BITMAP(poll_regs, NR_STATES);

/* big state machine lock */
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
#define state_lock()
#define state_unlock()
#else
#define state_lock()			irq_local_disable()
#define state_unlock()			irq_local_enable()
#endif

void poll_run(sid_t sid)
{
	if (test_bit(sid, poll_regs))
		state_run(sid, STATE_EVENT_POLL);
}

void poll_register(sid_t sid)
{
	set_bit(sid, poll_regs);
}

void __state_run(sid_t sid, uint8_t event)
{
	state_entries[sid].handler(event);
}

void state_run(sid_t sid, uint8_t event)
{
	idle_debug(IDLE_DEBUG_SID, sid);
	state_lock();
	__state_run(sid, event);
	state_unlock();
}

void bh_suspend(sid_t sid)
{	
	clear_bit((sid), state_awakes);
}

void bh_resume(sid_t sid)
{
	set_bit(sid, state_awakes);
}

boolean bh_resumed_any(void)
{
	return NR_STATES == find_next_set_bit(state_awakes,
					      NR_STATES, 0) 
	       ? false : true;
}

sid_t state_register(state_call_cb handler)
{
	sid_t sid = state_nr_regs;

	BUG_ON(sid == NR_STATES);
	state_entries[sid].handler = handler;
	state_nr_regs++;
	return sid;
}

void state_run_all(void)
{
	sid_t sid;

	for (sid = 0; sid < state_nr_regs; sid++) {
		if (test_bit(sid, state_awakes)) {
			bh_suspend(sid);
			state_run(sid, STATE_EVENT_WAKE);
		}
		poll_run(sid);
	}
}

void state_run_idle(void)
{
	idle_debug(IDLE_DEBUG_SID, DBG_SRC_IDLE);
}

void state_init(void)
{
}
