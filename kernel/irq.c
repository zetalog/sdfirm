#include <target/irq.h>
#include <target/percpu.h>

#ifndef CONFIG_CC_ISR_VECTOR
#ifndef ARCH_HAVE_VIC
#ifndef CONFIG_MAX_VECTORS
#define MAX_VECTORS	0
#else
#define MAX_VECTORS	CONFIG_MAX_VECTORS
#endif

irq_handler irq_handlers[MAX_VECTORS];
irq_t irq_nr_table[MAX_VECTORS];
uint8_t irq_nr_regs = 0;

extern void __bad_interrupt(void);

/* 0 ~ NR_IRQS-1 is allowed. */
void irq_register_vector(irq_t nr, irq_handler h)
{
	uint8_t curr = irq_nr_regs;

	BUG_ON(nr <= 0 || nr >= NR_IRQS);
	BUG_ON(curr == MAX_VECTORS);
	irq_nr_table[curr] = nr;
	irq_nr_regs++;
	irq_handlers[curr] = h;
}

boolean do_IRQ(irq_t nr)
{
	uint8_t curr;

	BUG_ON(nr >= NR_IRQS || nr <= 0);
	for (curr = 0; curr < irq_nr_regs; curr++) {
		if (nr == irq_nr_table[curr]) {
			irq_handlers[curr]();
			return true;
		}
	}
	return false;
}

void irq_vectors_init(void)
{
	uint8_t i;
	for (i = 0; i < MAX_VECTORS; i++)
		irq_handlers[i] = __bad_interrupt;
	irqc_hw_ctrl_init();
}
#else
void irq_vectors_init(void)
{
	vic_hw_vectors_init();
}

void irq_register_vector(irq_t nr, irq_handler h)
{
	vic_hw_register_irq(nr, h);
}
#endif
#endif

#ifdef CONFIG_SMP
struct smp_poll {
	DECLARE_BITMAP(smp_irq_poll_regs, NR_BHS);
	boolean smp_irq_is_polling;
};

DEFINE_PERCPU(struct smp_poll, smp_polls);

#define irq_poll_regs		\
	this_cpu_ptr(&smp_polls)->smp_irq_poll_regs
#define irq_is_polling		\
	this_cpu_ptr(&smp_polls)->smp_irq_is_polling
#else
DECLARE_BITMAP(irq_poll_regs, NR_BHS);
boolean irq_is_polling;
#endif

boolean irq_poll_bh(void)
{
	bh_t bh;

	if (!irq_is_polling)
		return false;
	for (bh = 0; bh < NR_BHS; bh++) {
		if (test_bit(bh, irq_poll_regs))
			bh_run(bh, BH_POLLIRQ);
	}
	return true;
}

void irq_register_poller(bh_t bh)
{
	set_bit(bh, irq_poll_regs);
	irq_is_polling = true;
}

void irq_smp_init(void)
{
	irqc_hw_smp_init();
	irq_local_disable();
}

void irq_init(void)
{
	irq_vectors_init();
	irq_hw_ctrl_init();
	irq_local_disable();
}
