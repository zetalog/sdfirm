#include <target/percpu.h>
#include <target/panic.h>
#include <target/msi.h>

__percpu(DECLARE_BITMAP(irq_msi_alloc, NR_MSI_IRQS));
__percpu(DECLARE_BITMAP(irq_msi_reserved, NR_MSI_IRQS));

irq_t irq_register_matrix(cpu_mask_t *msk)
{
	cpu_t cpu;
	bits_t *map;
	irq_t msi = INVALID_IRQ;

	for_each_cpu(cpu, msk) {
		if (smp_test_online(cpu)) {
			map = *per_cpu_ptr(&irq_msi_alloc, cpu);
			msi = find_next_clear_bit(map, NR_MSI_IRQS, 0);
			if (msi == NR_MSI_IRQS)
				continue;
		}
	}
	return msi;
}

void irq_reserve_matrix(cpu_mask_t *msk, irq_t start, irq_t end)
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
