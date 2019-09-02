#include <target/delay.h>

tsc_count_t pseudo_counter;

tsc_count_t tsc_hw_read_counter(void)
{
	tsc_counter_t cnt = pseudo_counter;

	pseudo_counter += PSEUDO_COUNTER_STEP;
	return cnt;
}

void tsc_hw_ctrl_init(void)
{
	pseudo_counter = 0;
}
