#include <target/smp.h>
#include <target/arch.h>
#include <target/barrier.h>

void *__cpu_up_entry[NR_CPUS];

void smp_hw_cpu_on(cpu_t cpu, caddr_t ep)
{
	smp_mb();
	WRITE_ONCE(__cpu_up_entry[cpu], ep);
}

void smp_hw_cpu_boot(void)
{
	smp_boot_cpu = csr_read(CSR_MHARTID);
}
