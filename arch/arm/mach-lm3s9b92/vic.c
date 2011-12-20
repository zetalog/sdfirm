#include <target/irq.h>

/*
 * When configuring the OFFSET field, the offset must be aligned to the
 * number of exception entries in the vector table.
 * Because there are 54 interrupts, 16 exceptions, the minimum alignment
 * is 128 words.
 */
#define __VIC_HW_ALIGN	__attribute__((aligned(128*4)))
irq_handler __VIC_HW_ALIGN __vic_hw_vector_table[NR_IRQS+NR_TRAPS];

extern irq_handler __lovec[NR_TRAPS];
extern void __bad_interrupt(void);

void vic_hw_register_trap(uint8_t nr, irq_handler h)
{
	BUG_ON(nr >= NR_TRAPS);
	__vic_hw_vector_table[nr] = h;
}

void vic_hw_register_irq(uint8_t nr, irq_handler h)
{
	BUG_ON(nr >= NR_IRQS);
	__vic_hw_vector_table[NR_TRAPS+nr] = h;
}

void vic_hw_irq_priority(uint8_t irq, uint8_t prio)
{
	BUG_ON(irq >= NR_IRQS);
	BUG_ON(prio > __VIC_HW_PRIO_MAX);
	__raw_clearl(VIC_PRIO_V(7, irq), VIC_PRIO_A(PRI0, irq));
	__raw_setl(VIC_PRIO_V(prio, irq), VIC_PRIO_A(PRI0, irq));
}

void vic_hw_trap_priority(uint8_t trap, uint8_t prio)
{
	BUG_ON(trap < __VIC_HW_PRIO_TRAP_MIN || trap >= NR_TRAPS);
	BUG_ON(prio > __VIC_HW_PRIO_MAX);
	__raw_clearl(VIC_PRIO_V(7, trap), VIC_PRIO_A(SYSPRI1-4, trap));
	__raw_setl(VIC_PRIO_V(prio, trap), VIC_PRIO_A(SYSPRI1-4, trap));
}

void vic_hw_vectors_init(void)
{
	uint8_t i;
	
	for (i = 0; i < NR_TRAPS; i++)
		__vic_hw_vector_table[i] = __lovec[i];
	for (i = 0; i < NR_IRQS; i++)
		__vic_hw_vector_table[NR_TRAPS+i] = __bad_interrupt;
	__raw_writel((unsigned long)__vic_hw_vector_table, VTABLE);

	/* disable IT folding */
	__raw_setl_atomic(DISFOLD, ACTLR);

	/* disable all IRQs */
	__raw_writel(0xFFFFFFFF, DIS0);
	__raw_writel(0xFFFFFFFF, DIS1);
}
