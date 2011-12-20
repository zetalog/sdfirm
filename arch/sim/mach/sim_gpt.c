#include "mach.h"
#include <simul/eloop.h>

static void sim_gpt_handle_tout(void *eloop, void *data)
{
	sim_irq_raise_irq(IRQ_GPT);
}

void sim_gpt_oneshot_timeout(timeout_t tout_ms)
{
	int secs, usecs;

	secs = tout_ms/1000;
	usecs = (tout_ms%1000) * 1000;

	eloop_register_timeout(NULL, secs, usecs,
			       sim_gpt_handle_tout,
			       NULL, NULL);
}
