#include <target/jiffies.h>
#include <target/arch.h>
#include <target/tsc.h>
#include <target/gpt.h>

volatile tick_t jiffies = 0;

#ifdef CONFIG_TICK_PERIODIC
void tick_handler(void)
{
	jiffies++;
	gpt_periodic_restart();
}
#else
/* Following definition is true as HZ=1000 and TSC_FREQ=tsc_freq/1000 */
#define COUNTS_PER_JIFFY	TSC_FREQ

tsc_count_t tsc_old;

tsc_count_t tick_ticked_tsc(tsc_count_t o, tsc_count_t d)
{
	return (TSC_MAX-o)>d ? (o+d) : (d-(TSC_MAX-o));
}

void tick_update_tsc(tsc_count_t counter)
{
	tsc_old = counter;
}

#define tick_delta_tsc(o, n)	((n>o) ? (n-o) : (TSC_MAX-o+n+1))

void tick_handler(void)
{
	tsc_count_t tsc_new, delta;
	tick_t ticks;

	tsc_new = tsc_read_counter();
	delta = tick_delta_tsc(tsc_old, tsc_new);
	ticks = (tick_t)__tsc_div(delta, COUNTS_PER_JIFFY);
	if (ticks) {
		jiffies += ticks;
		/* Remainder of delta is not accounted. */
		tsc_old = tick_ticked_tsc(tsc_old,
					  __tsc_mul(ticks, COUNTS_PER_JIFFY));
	}
	gpt_periodic_restart();
}
#endif

void tick_init(void)
{
	gpt_hw_ctrl_init();
	gpt_periodic_start();
}
