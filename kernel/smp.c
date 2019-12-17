#include <target/smp.h>
#include <target/irq.h>
#include <target/bh.h>
#include <target/delay.h>
#include <target/timer.h>
#include <target/jiffies.h>
#include <target/task.h>

cpu_t smp_boot_cpu;

void smp_init(void)
{
	cpu_t cpu;

	smp_hw_cpu_boot();

	printf("SMP initializing CPU %d.\n", smp_processor_id());
	if (smp_processor_id() != smp_boot_cpu) {
		irq_init();
		tick_init();
		delay_init();
		bh_init();
		timer_init();
		task_init();
		bh_loop();
	} else {
		for (cpu = 0; cpu < NR_CPUS; cpu++) {
			if (cpu != smp_boot_cpu)
				smp_hw_cpu_on(cpu, (caddr_t)smp_init, 0);
		}
	}
}
