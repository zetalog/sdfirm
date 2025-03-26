#include <target/percpu.h>
#include <target/panic.h>
#include <target/msi.h>

__percpu(DECLARE_BITMAP(irq_msi_alloc, NR_MSI_IRQS));
__percpu(DECLARE_BITMAP(irq_msi_reserved, NR_MSI_IRQS));
DEFINE_PERCPU(irq_t, irq_msi_vector[NR_MSI_IRQS]);

static irq_t __irq_register_mapping(cpu_mask_t *msk, irq_t irq)
{
	cpu_t cpu;
	bits_t *map;
	irq_t msi = INVALID_IRQ;
	irq_t *vec;

	for_each_cpu(cpu, msk) {
		if (smp_test_online(cpu)) {
			map = *per_cpu_ptr(&irq_msi_alloc, cpu);
			msi = find_next_clear_bit(map, NR_MSI_IRQS, 0);
			if (msi == NR_MSI_IRQS)
				continue;
			vec = *per_cpu_ptr(&irq_msi_vector, cpu);
			vec[msi] = irq;
			break;
		}
	}
	return msi == NR_MSI_IRQS ? INVALID_IRQ : MSI_IRQ(msi);
}

irq_t irq_register_mapping(irq_t irq)
{
	return __irq_register_mapping(&smp_online_cpus, irq);
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

	map = *this_cpu_ptr(&irq_msi_alloc);
	if (!test_bit(irq, map))
		return INVALID_IRQ;

	vec = *this_cpu_ptr(&irq_msi_vector);
	return vec[msi];
}
