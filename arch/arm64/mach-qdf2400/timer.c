#include <target/gpt.h>
#include <target/tsc.h>
#include <target/jiffies.h>
#include <target/irq.h>

bool qdf2400_gblct_initialized = false;

void qdf2400_gblct_init(void)
{
	if (!qdf2400_gblct_initialized) {
		qdf2400_gblct_initialized = true;
		__raw_writel(MPM_CONTROL_CNTCR_EN, MPM_CONTROL_CNTCR);
		__systick_init();
	}
}

tsc_count_t tsc_hw_read_counter(void)
{
	return (tsc_count_t)__systick_read();
}

void tsc_hw_ctrl_init(void)
{
	qdf2400_gblct_init();
}

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	BUG_ON(tout_ms > GPT_MAX_TIMEOUT);

	__systick_set_timeout(tout_ms);
	__systick_unmask_irq();
}

static void gpt_hw_handle_irq(void)
{
	__systick_mask_irq();
	tick_handler();
}

void gpt_hw_ctrl_init(void)
{
	qdf2400_gblct_init();
	irqc_configure_irq(IRQ_TIMER, IRQ_LEVEL_TRIGGERED, 0);
	irq_register_vector(IRQ_TIMER, gpt_hw_handle_irq);
	irqc_enable_irq(IRQ_TIMER);
}
