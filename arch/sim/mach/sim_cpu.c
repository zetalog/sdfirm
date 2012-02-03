#include "mach.h"

HANDLE sys_thread;
DWORD sys_tid;
boolean sys_exit = false;
irq_flags_t cpu_flags;
boolean sys_suspended;

DWORD WINAPI sys_thread_routine(void *args)
{
	system_init();
	return sys_tid;
}

void sim_cpu_init(void)
{
	sys_suspended = 0;
	sys_thread = CreateThread(NULL, 0, sys_thread_routine,
				  NULL, 0, &sys_tid);
}

void sim_cpu_halt(void)
{
	if (!sys_suspended) {
		SuspendThread(sys_thread);
		sys_suspended = 1;
	}
}

void sim_cpu_unhalt(void)
{
	if (sys_suspended) {
		ResumeThread(sys_thread);
		sys_suspended = 0;
	}
}
