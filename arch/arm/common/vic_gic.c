#include <target/irq.h>

irq_handler __vic_hw_vector_table[NR_IRQS];

extern void __bad_interrupt(void);

void vic_hw_register_irq(uint8_t nr, irq_handler h)
{
	BUG_ON(nr >= NR_IRQS);
}

void vic_hw_irq_priority(uint8_t irq, uint8_t prio)
{
	BUG_ON(irq >= NR_IRQS);
	BUG_ON(prio > GIC_PRIORITY_MAX);
}

void vic_hw_vectors_init(void)
{
	uint8_t i;

	for (i = 0; i < NR_IRQS; i++)
		__vic_hw_vector_table[i] = __bad_interrupt;
}
