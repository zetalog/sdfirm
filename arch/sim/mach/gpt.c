#include "mach.h"

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	sim_gpt_oneshot_timeout(tout_ms);
}

static void gpt_hw_handle_irq(void)
{
	tick_handler();
}

void gpt_hw_ctrl_init(void)
{
	irq_hw_register_irq(IRQ_GPT, gpt_hw_handle_irq);
	irq_hw_irq_enable(IRQ_GPT);
}
