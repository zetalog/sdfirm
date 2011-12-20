#include <target/tsc.h>
#include <target/jiffies.h>

tsc_count_t tsc_hw_read_counter(void)
{
	return __timer1_hw_read_counter();
}

void tsc_hw_ctrl_init(void)
{
	__timer1_hw_write_counter(0);
	__timer1_hw_config_mode(TIMER_HW_WGM_NORMAL);
	__timer1_hw_enable_cs(TSC_CS);
	__timer1_hw_disable_irq();
	tick_update_tsc(0);
}
