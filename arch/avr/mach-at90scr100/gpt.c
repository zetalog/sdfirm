#include <target/gpt.h>
#include <target/irq.h>
#include <target/jiffies.h>
#include <target/panic.h>

#define __gpt_hw_stop_timer()				\
	(__timer2_hw_disable_cs())
#define __gpt_hw_start_timer()				\
	do {						\
		__timer2_hw_write_counter(0);		\
		__timer2_hw_enable_cs(GPT_CS);		\
	} while (0)
#define __gpt_hw_reload_timer(cnt)			\
	__timer2_hw_write_ocra((uint8_t)(cnt))

#ifdef CONFIG_TICK
uint16_t gpt_reload;

void gpt_hw_periodic_restart(void)
{
	__gpt_hw_reload_timer(gpt_reload);
	__gpt_hw_start_timer();
}

void gpt_hw_periodic_start(void)
{
	/* ret_reload = GPT_FREQ * 1000 / HZ and HZ is must be 1000 in AT90SCR100 */
	gpt_reload = GPT_FREQ;
	gpt_hw_periodic_restart();
}
#else
boolean gpt_shot_on = false;
uint8_t gpt_hi, gpt_lo;

void __gpt_hw_shot_restart_timer(void)
{
	BUG_ON(!gpt_shot_on);

	if (gpt_hi) {
		__gpt_hw_reload_timer(0xFF);
		gpt_hi--;
	} else {
		__gpt_hw_reload_timer(gpt_lo);
		gpt_shot_on = false;
	}
	__gpt_hw_start_timer();
}

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	if (tout_ms * GPT_FREQ < 0xFF) {
		__gpt_hw_reload_timer(tout_ms * GPT_FREQ);
		__gpt_hw_start_timer();
	} else {
		uint16_t total;
		total = (uint16_t)mul16u(tout_ms, GPT_FREQ); 
		gpt_hi = (uint8_t)div16u(total, GPT_FREQ);
		gpt_lo = (uint8_t)mod16u(total, GPT_FREQ);

		gpt_shot_on = true;
		__gpt_hw_shot_restart_timer();
	}
}
#endif

DEFINE_ISR(IRQ_OCF2A)
{
	__gpt_hw_stop_timer();
#ifndef CONFIG_TICK
	if (gpt_shot_on) {
		__gpt_hw_shot_restart_timer();
		return;
	}
#endif
	tick_handler();
}

void gpt_hw_ctrl_init(void)
{
	__timer2_hw_config_mode(TIMER_HW_WGM_CTC_OC8);
	__timer2_hw_write_counter(0);

	irq_register_vector(IRQ_OCF2A, IRQ_OCF2A_isr);
	__timer2_hw_enable_ocfa();
}
