#include <target/gpt.h>
#include <target/jiffies.h>

#define __gpt_hw_start_timer()		(TR0 = 1)
#define __gpt_hw_stop_timer()		(TR0 = 0)
#define __gpt_hw_reload_timer(reload)			\
	do {						\
		__gpt_hw_stop_timer();			\
		TL0 = LOBYTE(reload);			\
		TH0 = HIBYTE(reload);			\
		__gpt_hw_start_timer();			\
	} while (0)
#define __gpt_hw_config_mode(mode)			\
	do {						\
		__gpt_hw_stop_timer();			\
		__timer_hw_config_mode(0, mode);	\
		__gpt_hw_start_timer();			\
	} while (0)

#ifdef CONFIG_TICK
__near__ uint16_t gpt_reload;

void gpt_hw_periodic_restart(void)
{
	__gpt_hw_reload_timer(gpt_reload);
}

void gpt_hw_periodic_start(void)
{
	gpt_reload = __timer_hw_calc_reload(1);
	__gpt_hw_reload_timer(gpt_reload);
}
#else
void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	__gpt_hw_reload_timer(__timer_hw_calc_reload(tout_ms));
}
#endif

void DEFINE_ISR(timer_isr(void), IRQ_GPT)
{
	tick_handler();
	//io_putchar(0x00);
}

void gpt_hw_ctrl_init(void)
{
	clk_hw_resume_dev(DEV_GPT0);
#ifdef CONFIG_TICK
	irq_hw_set_priority(IRQ_GPT, IRQ_PRIO_3);
#else
	irq_hw_set_priority(IRQ_GPT, IRQ_PRIO_1);
#endif
	irq_hw_enable_vector(IRQ_GPT, false);
	__gpt_hw_config_mode(TIMER_HW_MODE_TIMER|TIMER_HW_MODE_16BIT);
}
