#include <target/gpt.h>
#include <target/irq.h>
#include <target/jiffies.h>
#include <target/uart.h>
#include <target/panic.h>

#ifdef CONFIG_TICK
#ifdef CONFIG_TICK_LM3S9B92_PERIODIC
/* SysTick exception as periodic tick */

uint32_t gpt_reload;

void __gpt_hw_periodic_restart(void)
{
	__systick_hw_clear_current();
	__systick_hw_enable_trap();
}

void gpt_hw_periodic_restart(void)
{
}

void gpt_hw_periodic_start(void)
{
	gpt_reload = (uint32_t)(mul32u(__SYSTICK_HW_FREQ,
				div16u(1000, HZ))) - 1;
	__systick_hw_write_reload(gpt_reload);
	__gpt_hw_periodic_restart();
}

void __gpt_hw_periodic_init(void)
{
	vic_hw_register_trap(TRAP_TICK, tick_handler);
	__systick_hw_enable_ctrl();
	vic_hw_trap_priority(TRAP_TICK, 0);
}
#else
/* 32Bits GPT0 as periodic timer */

LM3S9B92_GPT_32BIT(0)

void gpt_hw_periodic_restart(void)
{
	__timer0a_hw_write_reload((uint32_t)mul32u(GPT_COUNT_PER_MS, gpt_reload));
	__timer0a_hw_ctrl_enable();
}

void gpt_hw_periodic_start(void)
{
	gpt_reload = mul32u(div16u(1000, HZ), CLK_GPT);
	gpt_hw_periodic_restart();
}

static void __gpt_hw_handler_irq()
{
	__timer0_hw_irq_clear(TATO);
	tick_handler();	
}

static void __gpt_hw_periodic_init(void)
{
	pm_hw_resume_device(DEV_TIMER0, DEV_MODE_ON);
	vic_hw_register_irq(IRQ_GPT0A, __gpt_hw_handler_irq);
	vic_hw_irq_enable(IRQ_GPT0A);

	__timer0_hw_ctrl_disable();
	__timer0_hw_config_type(__TIMER_HW_TYPE_GPT32);
	__timer0a_hw_config_mode(__TIMER_HW_MODE_PERIODIC|__TIMER_HW_MODE_COUNT_DOWN);
	__timer0_hw_irq_enable(TATO);
}
#endif

#define __gpt_hw_oneshot_init()
#else
LM3S9B92_GPT_32BIT(0)

static void __gpt_hw_handle_irq(void)
{
	if (__timer0_hw_irq_raw(TATO)) {
		__timer0_hw_irq_clear(TATO);
		tick_handler();
	}
}

#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void)
{
	__gpt_hw_handle_irq();
}

#define gpt_hw_irq_enable()
#define gpt_hw_irq_init()
#else
#define gpt_hw_irq_enable()	__timer0_hw_irq_enable(TATO)

void gpt_hw_irq_init(void)
{
	vic_hw_register_irq(IRQ_GPT0A, __gpt_hw_handle_irq);
	vic_hw_irq_enable(IRQ_GPT0A);
}
#endif

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	BUG_ON(tout_ms > GPT_MAX_TIMEOUT);

	__timer0_hw_ctrl_disable();
	__timer0_hw_config_type(__TIMER_HW_TYPE_GPT32);
	__timer0a_hw_config_mode(__TIMER_HW_MODE_ONESHOT|__TIMER_HW_MODE_COUNT_DOWN);
	__timer0a_hw_write_reload((uint32_t)mul32u(GPT_COUNT_PER_MS, tout_ms));
	gpt_hw_irq_enable();
	__timer0a_hw_ctrl_enable();
}

static void __gpt_hw_oneshot_init(void)
{
	pm_hw_resume_device(DEV_TIMER0, DEV_MODE_ON);
	gpt_hw_irq_init();
}

#define __gpt_hw_periodic_init()
#endif

void gpt_hw_ctrl_init(void)
{
	__gpt_hw_periodic_init();
	__gpt_hw_oneshot_init();
}
