#include <target/tsc.h>
#include <target/jiffies.h>

#define __tsc_hw_start_timer()		(TR1 = 1)
#define __tsc_hw_stop_timer()		(TR1 = 0)
#define __tsc_hw_config_mode(mode)			\
	do {						\
		__tsc_hw_stop_timer();			\
		__timer_hw_config_mode(1, mode);	\
		__tsc_hw_start_timer();			\
	} while (0)

static void __tsc_hw_ctrl_start(void)
{
	__tsc_hw_stop_timer();
	TL1 = 0;
	TH1 = 0;
	__tsc_hw_start_timer();	
}

void tsc_hw_ctrl_init(void)
{
	clk_hw_resume_dev(DEV_GPT1);
	__tsc_hw_config_mode(TIMER_HW_MODE_TIMER|TIMER_HW_MODE_8BIT);
	__tsc_hw_ctrl_start();
	tick_update_tsc(0);
}
