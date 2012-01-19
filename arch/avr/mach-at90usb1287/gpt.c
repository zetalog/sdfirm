#include <target/gpt.h>
#include <target/irq.h>
#include <target/jiffies.h>

#define __gpt_hw_stop_timer()				\
	(__timer3_hw_disable_cs())
#define __gpt_hw_start_timer()				\
	do {						\
		__timer3_hw_write_counter(0);		\
		__timer3_hw_enable_cs(GPT_CS);		\
	} while (0)
#define __gpt_hw_reload_timer(tout_ms)			\
	__timer3_hw_write_ocra((uint16_t)mul16u(125,tout_ms))

#ifdef CONFIG_TICK
uint16_t gpt_reload;

void gpt_hw_periodic_restart(void)
{
	__gpt_hw_reload_timer(gpt_reload);
	__gpt_hw_start_timer();
}

void gpt_hw_periodic_start(void)
{
	gpt_reload = mul16u(div16u(1000, HZ), 125);
	gpt_hw_periodic_restart();
}
#else
void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	__gpt_hw_reload_timer(tout_ms);
	__gpt_hw_start_timer();
}
#endif

DEFINE_ISR(IRQ_OCF3A)
{
	__gpt_hw_stop_timer();
	tick_handler();
}

void gpt_hw_ctrl_init(void)
{
	__timer3_hw_config_mode(TIMER_HW_WGM_CTC_OC16);
	__timer3_hw_write_counter(0);

	irq_register_vector(IRQ_OCF3A, IRQ_OCF3A_isr);
	__timer3_hw_enable_ocfa();
}
