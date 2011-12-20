#include <target/irq.h>

void irq_hw_flags_enable(void)
{
	sei();
}

void irq_hw_flags_disable(void)
{
	cli();
}

void irq_hw_ctrl_init(void)
{
}

