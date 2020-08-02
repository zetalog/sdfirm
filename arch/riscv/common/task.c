#include <target/task.h>

void arch_hw_init_task(struct task_entry *task,
		       task_call_cb entry, void *priv)
{
	task->thread.ra = (unsigned long)entry;
	task->thread.sp = task->kern_sp;
}
