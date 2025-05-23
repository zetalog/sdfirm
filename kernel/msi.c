#include <target/percpu.h>
#include <target/panic.h>
#include <target/msi.h>

__percpu(DECLARE_BITMAP(irq_msi_alloc, NR_MSI_IRQS));
__percpu(DECLARE_BITMAP(irq_msi_reserved, NR_MSI_IRQS));
DEFINE_PERCPU(irq_t, irq_msi_vector[NR_MSI_IRQS]);

irq_t __irq_register_mapping(cpu_mask_t *msk, irq_t irq, cpu_t *aff)
{
	cpu_t cpu;
	bits_t *map;
	irq_t msi = INVALID_IRQ;
	irq_t *vec;

	for_each_cpu(cpu, msk) {
		map = *per_cpu_ptr(&irq_msi_alloc, cpu);
		msi = find_next_clear_bit(map, NR_MSI_IRQS, 0);
		if (msi == NR_MSI_IRQS)
			continue;
		set_bit(msi, map);
		vec = *per_cpu_ptr(&irq_msi_vector, cpu);
		vec[msi] = irq;
		if (aff)
			*aff = cpu;
		break;
	}
	return msi == NR_MSI_IRQS ? INVALID_IRQ : MSI_IRQ(msi);
}

irq_t irq_register_mapping(irq_t irq, cpu_t *cpu)
{
	return __irq_register_mapping(&smp_online_cpus, irq, cpu);
}

static void __irq_reserve_mapping(cpu_mask_t *msk, irq_t start, irq_t end)
{
	cpu_t cpu;
	bits_t *map;
	bits_t *resv;
	irq_t msi;

	for_each_cpu(cpu, msk) {
		map = *per_cpu_ptr(&irq_msi_alloc, cpu);
		resv = *per_cpu_ptr(&irq_msi_reserved, cpu);
		for (msi = start; msi <= end; msi++) {
			BUG_ON(test_bit(msi, map) || test_bit(msi, resv));
			set_bit(msi, map);
			set_bit(msi, resv);
		}
	}
}

void irq_reserve_mapping(irq_t start, irq_t end)
{
	__irq_reserve_mapping(&smp_online_cpus, start, end);
}

irq_t irq_locate_mapping(cpu_t cpu, irq_t irq)
{
	bits_t *map;
	irq_t *vec;
	irq_t msi = irq_msi(irq);

	map = *per_cpu_ptr(&irq_msi_alloc, cpu);
	if (!test_bit(msi, map))
		return INVALID_IRQ;

	vec = *this_cpu_ptr(&irq_msi_vector);
	return vec[msi];
}

void msi_init(void)
{
	msi_hw_ctrl_init();
}
