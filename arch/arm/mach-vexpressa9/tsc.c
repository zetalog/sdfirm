#include <target/tsc.h>
#include <target/jiffies.h>

/* 64Bits MPCore Global Timer as timestamp counter */

tsc_count_t tsc_hw_read_counter(void)
{
	return __gtmr_hw_read_counter();
}

void tsc_hw_ctrl_init(void)
{
	__gtmr_hw_enable_tsc();
	tick_update_tsc(0);
}
