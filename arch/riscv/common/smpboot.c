#include <target/smp.h>
#include <target/arch.h>
#include <target/panic.h>
#include <target/barrier.h>
#include <target/irq.h>
#include <target/atomic.h>
#include <target/sbi.h>
#include <target/bench.h>

enum ipi_message_type {
	IPI_RESCHEDULE,
	IPI_CALL_FUNC,
	IPI_CPU_OFF,
	IPI_MAX
};

/* A collection of single bit ipi messages.  */
static struct {
	unsigned long bits;
} ipi_data[NR_CPUS] __cache_aligned;

void *__cpu_up_entry[NR_CPUS];

void smp_hw_cpu_on(cpu_t cpu, caddr_t ep)
{
	smp_mb();
	WRITE_ONCE(__cpu_up_entry[cpu], ep);
	smp_mb();
}

void smp_hw_cpu_boot(void)
{
	smp_boot_cpu = smp_processor_id();
}

static void send_ipi_single(int cpu, enum ipi_message_type op)
{
	cpu_mask_t mask;

	memset(&mask, 0, sizeof (mask));
	smp_mb__before_atomic();
	set_bit(op, (bits_t *)&ipi_data[cpu].bits);
	cpumask_set_cpu(cpu, &mask);
	smp_mb__after_atomic();

	sbi_send_ipi(cpumask_bits(&mask));
}

void smp_hw_handle_ipi(void)
{
	unsigned long *pending_ipis = &ipi_data[smp_processor_id()].bits;

	irqc_clear_irq(IRQ_SOFT);

	while (true) {
		unsigned long ops;

		/* Order bit clearing and data access. */
		mb();

		ops = xchg(pending_ipis, 0);
		if (ops == 0)
			return;

		if (ops & (1 << IPI_RESCHEDULE))
			printf("%d: IPI_RESCHEDULE rcvd\n", smp_processor_id());
		if (ops & (1 << IPI_CALL_FUNC))
			printf("%d: IPI_CALL_FUNC rcvd\n", smp_processor_id());
		if (ops & (1 << IPI_CPU_OFF))
			printf("%d: IPI_CPU_OFF rcvd\n", smp_processor_id());

		BUG_ON((ops >> IPI_MAX) != 0);

		/* Order data access and bit testing. */
		mb();
	}
}

void smp_hw_cpu_off(cpu_t cpu)
{
	send_ipi_single(cpu, IPI_CPU_OFF);
}

void smp_hw_ctrl_init(void)
{
	irq_register_vector(IRQ_SOFT, smp_hw_handle_ipi);
	irqc_enable_irq(IRQ_SOFT);
}

int ipi_sanity(caddr_t percpu_area)
{
	cpu_t cpu;

	sbi_enable_log();
	for_each_cpu(cpu, &smp_online_cpus) {
		printf("SMP: %d sending IPI to %d\n", smp_processor_id(), cpu);
		if (cpu != smp_processor_id())
			smp_cpu_off(cpu);
	}
	sbi_disable_log();
	return 1;
}
__define_testfn(ipi_sanity, 0, SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
