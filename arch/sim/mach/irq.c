#include "mach.h"

DECLARE_BITMAP(irq_opens, NR_IRQS);

boolean irq_hw_irq_enabled(uint8_t irq)
{
	return test_bit(irq, irq_opens);
}

void irq_hw_irq_enable(uint8_t irq)
{
	set_bit(irq, irq_opens);
}

void irq_hw_irq_disable(uint8_t irq)
{
	clear_bit(irq, irq_opens);
}

void irq_hw_register_irq(uint8_t irq, irq_vect_cb vect)
{
	sim_irq_register_vect(irq, vect);
}

void irq_hw_flags_enable(void)
{
	cpu_flags |= SIM_CPU_EAI;
	sim_irq_schedule_all();
}

void irq_hw_flags_disable(void)
{
	cpu_flags &= ~SIM_CPU_EAI;
}

void irq_hw_ctrl_init(void)
{
	cpu_flags = 0;
}
