#include <target/gpt.h>
#include <target/tsc.h>
#include <target/jiffies.h>
#include <target/irq.h>
#include <target/panic.h>

void gblct_init(void)
{
	/* GEM5 only allows accessing counter registers before MMU is
	 * enabled, so making sure delay_init() is invoked before
	 * enabling MMU in head.S.
	 */
	__systick_init();
}

tsc_count_t tsc_hw_read_counter(void)
{
	return (tsc_count_t)__systick_read();
}

void tsc_hw_ctrl_init(void)
{
	gblct_init();
}

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	BUG_ON(tout_ms > GPT_MAX_TIMEOUT);

	__systick_set_timeout(tout_ms);
	__systick_unmask_irq();
}

static void gpt_hw_handle_irq(void)
{
	if (__systick_poll()) {
		__systick_mask_irq();
		tick_handler();
		__systick_unmask_irq();
	}
}

#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void)
{
	gpt_hw_handle_irq();
}

#define gpt_hw_irq_enable()
#define gpt_hw_irq_init()
#else
#define gpt_hw_irq_enable()	__systick_unmask_irq()

void gpt_hw_irq_init(void)
{
	irqc_configure_irq(IRQ_TIMER, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_TIMER, gpt_hw_handle_irq);
	irqc_enable_irq(IRQ_TIMER);
	gpt_hw_irq_enable();
}
#endif

void gpt_hw_ctrl_init(void)
{
	gblct_init();
	gpt_hw_irq_init();
}
