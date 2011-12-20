#include "mach.h"
#include <simul/eloop.h>

static void wsa_startup(void)
{
#ifdef WIN32
	static int wsa_started = 0;
	WSADATA data;

	if (!wsa_started) {
		if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
			return;
		wsa_started = 1;
	}
#endif
}

static void wsa_cleanup(void)
{
#ifdef WIN32
	WSACleanup();
#endif
}

void sim_init(void)
{
	wsa_startup();
	eloop_init();

	/* init controllers */
	sim_uart_init();
	sim_irq_init();
	sim_mem_init();
	sim_usb_init();
	sim_cpu_init();

	/* wait for system init done */
	sleep(1);
}

void sim_run(void)
{
	eloop_context_run(NULL);
	wsa_cleanup();
}
