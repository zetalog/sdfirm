#include <target/gpt.h>
#include <target/tsc.h>
#include <asm/timer.h>
#include <asm/mach/mpm.h>

boolean qdf2400_gblct_initialized = false;

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
	return TSC_MAX - (tsc_count_t)__systick_read();
}

void tsc_hw_ctrl_init(void)
{
	qdf2400_gblct_init();
}

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	BUG_ON(tout_ms > GPT_MAX_TIMEOUT);

	__systick_set_timeout(tout_ms);
	/* gpt_hw_irq_enable(); */
}

void gpt_hw_ctrl_init(void)
{
	qdf2400_gblct_init();
}
