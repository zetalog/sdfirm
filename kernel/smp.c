#include <target/smp.h>
#include <target/bh.h>
#include <target/timer.h>
#include <target/task.h>
#include <target/jiffies.h>

cpu_t smp_boot_cpu;

void smp_init(void)
{
	printf("SMP initializing CPU %d.\n", smp_processor_id());

	if (smp_processor_id() != smp_boot_cpu) {
		bh_init();
		timer_init();
		task_init();
		bh_loop();
	} else {
		cpu_t cpu;

		smp_hw_cpu_boot();
		for (cpu = 0; cpu < NR_CPUS; cpu++) {
			if (cpu != smp_boot_cpu)
				smp_hw_cpu_on(cpu, (caddr_t)smp_init, 0);
		}
	}
}
