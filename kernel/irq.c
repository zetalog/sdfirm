#include <target/irq.h>

#ifndef CONFIG_CC_ISR_VECTOR
#ifndef ARCH_HAVE_VIC
#ifndef CONFIG_MAX_VECTORS
#define MAX_VECTORS	NR_IRQS
#else
#define MAX_VECTORS	CONFIG_MAX_VECTORS
#endif

irq_handler irq_handlers[MAX_VECTORS];
uint8_t irq_nr_table[MAX_VECTORS];
uint8_t irq_nr_regs = 0;

extern void __bad_interrupt(void);

/* 0 ~ NR_IRQS-1 is allowed. */
void irq_register_vector(uint8_t nr, irq_handler h)
{
	uint8_t curr = irq_nr_regs;

	BUG_ON(nr <= 0 || nr >= NR_IRQS);
	BUG_ON(curr == MAX_VECTORS);
	irq_nr_table[curr] = nr;
	irq_nr_regs++;
	irq_handlers[curr] = h;
}

void do_IRQ(uint8_t nr)
{
	uint8_t curr;

	BUG_ON(nr >= NR_IRQS || nr <= 0);
	for (curr = 0; curr < irq_nr_regs; curr++) {
		if (nr == irq_nr_table[curr]) {
			irq_handlers[curr]();
		}
	}
}

void irq_vectors_init(void)
{
	uint8_t i;
	for (i = 0; i < MAX_VECTORS; i++)
		irq_handlers[i] = __bad_interrupt;
}
#else
void irq_vectors_init(void)
{
	vic_hw_vectors_init();
}

void irq_register_vector(uint8_t nr, irq_handler h)
{
	vic_hw_register_irq(nr, h);
}
#endif
#endif

void irq_init(void)
{
	irq_vectors_init();
	irq_hw_ctrl_init();
	irq_local_disable();
}
