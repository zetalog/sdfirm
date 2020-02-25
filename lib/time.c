#include <time.h>

#ifdef CONFIG_TIME_CLOCK
clock_t clock(void)
{
	tsc_count_t cnt = tsc_read_counter();
	return div32u(cnt, TICKS_TO_MICROSECONDS);
}
#endif
