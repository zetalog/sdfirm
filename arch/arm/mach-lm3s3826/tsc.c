#include <target/tsc.h>
#include <target/jiffies.h>

#ifdef CONFIG_TSC_LM3S3826_GPT1
/* 32Bits GPT1 as timestamp counter */

LM3S3826_GPT_32BIT(1)

tsc_count_t tsc_hw_read_counter(void)
{
	return TSC_MAX - (tsc_count_t)__timer1a_hw_read_count();
}

void tsc_hw_ctrl_init(void)
{
	pm_hw_resume_device(DEV_TIMER1, DEV_MODE_ON);
	__timer1_hw_ctrl_disable();
	__timer1_hw_config_type(__TIMER_HW_TYPE_GPT32);
	__timer1a_hw_config_mode(__TIMER_HW_MODE_PERIODIC|__TIMER_HW_MODE_COUNT_DOWN);
	__timer1a_hw_write_reload(TSC_MAX);
	__timer1a_hw_ctrl_enable();
	tick_update_tsc(0);
}
#else
tsc_count_t tsc_hw_read_counter(void)
{
	return TSC_MAX - (tsc_count_t)__systick_hw_read_current();
}

void tsc_hw_ctrl_init(void)
{
	__systick_hw_disable_trap();
	__systick_hw_write_reload(TSC_MAX);
	__systick_hw_enable_ctrl();
	tick_update_tsc(0);
}
#endif
