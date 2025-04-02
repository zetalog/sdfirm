#include <target/irq.h>
#include <target/panic.h>
#include <target/percpu.h>
#include <target/msi.h>

#if !defined(CONFIG_CC_ISR_VECTOR) && !defined(CONFIG_SYS_NOIRQ)
#ifndef ARCH_HAVE_VIC
#ifndef CONFIG_MAX_VECTORS
#define MAX_VECTORS	0
#else
#define MAX_VECTORS	CONFIG_MAX_VECTORS
#endif

irq_handler irq_handlers[MAX_VECTORS];
irq_t irq_nr_table[MAX_VECTORS];
uint8_t irq_nr_regs = 0;

extern void __bad_interrupt(irq_t irq);

#ifdef CONFIG_MSI
irq_t irq_map[MAX_VECTORS];
cpu_t cpu_map[MAX_VECTORS];

irq_t irq_map_parent(irq_t irq, uint8_t nr)
{
	irq_t msi;
	cpu_t cpu;

	if (irq < IRQ_PLATFORM)
		return INVALID_IRQ;
	msi = irq_register_mapping(irq, &cpu);
	irq_map[nr] = msi;
	cpu_map[nr] = cpu;
	return msi;
}

irq_t irq_mapped_msi(irq_t irq)
{
	uint8_t curr = irq_nr_regs;

	for (curr = 0; curr < irq_nr_regs; curr++) {
		if (irq == irq_nr_table[curr])
			return irq_map[curr];
	}
	return INVALID_IRQ;
}

cpu_t irq_mapped_cpu(irq_t irq)
{
	uint8_t curr = irq_nr_regs;

	for (curr = 0; curr < irq_nr_regs; curr++) {
		if (irq == irq_nr_table[curr])
			return cpu_map[curr];
	}
	return 0;
}
#else
#define irq_map_parent(irq, nr)		do { } while (0)
#endif

/* 0 ~ NR_IRQS-1 is allowed. */
void irq_register_vector(irq_t irq, irq_handler h)
{
	uint8_t curr = irq_nr_regs;

	BUG_ON(irq <= 0 || irq >= NR_IRQS);
	BUG_ON(curr == MAX_VECTORS);
	irq_nr_table[curr] = irq;
	irq_nr_regs++;
	irq_handlers[curr] = h;
	irq_map_parent(irq, curr);
}

boolean do_IRQ(irq_t irq)
{
	uint8_t curr;

	BUG_ON(irq >= NR_IRQS || irq <= 0);
	for (curr = 0; curr < irq_nr_regs; curr++) {
		if (irq == irq_nr_table[curr]) {
			irq_handlers[curr](irq);
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
	irqc_hw_mmu_init();
	irqc_hw_ctrl_init();
}

void irq_smp_vectors_init(void)
{
	irqc_hw_smp_init();
}
#else /* ARCH_HAVE_VIC */
void irq_vectors_init(void)
{
	vic_hw_vectors_init();
}

void irq_register_vector(irq_t irq, irq_handler h)
{
	vic_hw_register_irq(irq, h);
}

void irq_smp_vectors_init(void)
{
}
#endif /* ARCH_HAVE_VIC */
#else /* !CONFIG_CC_ISR_VECTOR && !CONFIG_SYS_NOIRQ */
boolean do_IRQ(irq_t irq)
{
	return false;
}

void irq_smp_vectors_init(void)
{
}
#endif /* !CONFIG_CC_ISR_VECTOR && !CONFIG_SYS_NOIRQ */

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
cpu_mask_t smp_online_cpus = C(0);
#endif

boolean irq_poll_bh(boolean sync)
{
	bh_t bh;

	if (!irq_is_polling)
		return false;
	for (bh = 0; bh < NR_BHS; bh++) {
		if (test_bit(bh, irq_poll_regs)) {
			if (!sync || (bh != console_bh))
				bh_run(bh, BH_POLLIRQ);
		}
	}
	return true;
}

void irq_register_poller(bh_t bh)
{
	BUG_ON(!smp_initialized);
	set_bit(bh, irq_poll_regs);
	irq_is_polling = true;
}

#ifdef CONFIG_SMP
void irq_smp_init(void)
{
	BUG_ON(!smp_initialized);
	irq_smp_vectors_init();
	irq_local_disable();
}
#endif /* CONFIG_SMP */

void irq_init(void)
{
	irq_vectors_init();
	irq_hw_ctrl_init();
	irq_local_disable();
}
