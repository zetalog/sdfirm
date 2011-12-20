#include "mach.h"
#include <simul/eloop.h>

CRITICAL_SECTION irq_mutex;
irq_vect_cb irq_vects[NR_IRQS];
DECLARE_BITMAP(irq_flags, NR_IRQS);

extern void sim_cpu_halt(void);
extern void sim_cpu_unhalt(void);

static void sim_irq_discard(void);

void sim_irq_execute_vect(uint8_t irq)
{
	if (test_bit(irq, irq_opens)) {
		clear_bit(irq, irq_flags);
		irq_vects[irq]();
	}
}

void sim_irq_execute_all(void)
{
	uint8_t irq;

	sim_cpu_halt();
	for (irq = 0; irq < NR_IRQS; irq++) {
		if ((cpu_flags & SIM_CPU_EAI) &&
		    test_bit(irq, irq_flags)) {
			sim_irq_execute_vect(irq);
		}
	}
	for (irq = 0; irq < NR_IRQS; irq++) {
		if (test_bit(irq, irq_flags) &&
		    test_bit(irq, irq_opens)) {
			sim_irq_schedule_all();
			return;
		}
	}
	sim_irq_discard();
	sim_cpu_unhalt();
}

void sim_irq_raise_irq(uint8_t irq)
{
	set_bit(irq, irq_flags);
	sim_irq_schedule_all();
}

void sim_irq_unraise_irq(uint8_t irq)
{
	clear_bit(irq, irq_flags);
	sim_irq_schedule_all();
}

static void irq_resched(void *eloop, void *data)
{
	sim_irq_execute_all();
}

void sim_irq_schedule_all(void)
{
	EnterCriticalSection(&irq_mutex);
	eloop_register_timeout(NULL, 0, 10,
			       irq_resched,
			       NULL, NULL);
	LeaveCriticalSection(&irq_mutex);
}

void sim_irq_discard(void)
{
	eloop_cancel_timeout(NULL, irq_resched, NULL, NULL);
}

void sim_irq_register_vect(uint8_t irq, irq_vect_cb vect)
{
	irq_vects[irq] = vect;
}

void sim_irq_init(void)
{
	uint8_t irq;
	InitializeCriticalSection(&irq_mutex);
	for (irq = 0; irq < NR_IRQS; irq++) {
		irq_vects[irq] = NULL;
		irq_hw_irq_disable(irq);
	}
}
