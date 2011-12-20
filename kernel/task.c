#include <target/task.h>
#include <target/state.h>
#include <target/timer.h>
#include <target/heap.h>
#include <target/irq.h>

#define TASK_SLICE		10
#define INIT_TASK		(&task_entries[INIT_PID])

struct task_entry task_entries[NR_TASKS];
pid_t task_nr_regs = INIT_PID+1;
struct task_entry *task_current = INIT_TASK;

sid_t task_sid = INVALID_SID;
tid_t task_tid = INVALID_TID;

pid_t task_create(task_call_cb call, void *priv,
		  caddr_t stack_bottom, size_t stack_size)
{
	struct task_entry *task;
	pid_t pid = task_nr_regs;
	irq_flags_t flags;

	BUG_ON(pid == NR_TASKS);

	task = &task_entries[pid];
	task_nr_regs++;

	task->pid = pid;
	task->stack_size = stack_size;
	task->stack_bottom = stack_bottom;
	task->sp = (stack_bottom + stack_size - 1);
	
	irq_local_save(flags);
	arch_hw_init_task(task, call, priv);
	irq_local_restore(flags);
	return pid;
}

void task_schedule(void)
{
	irq_flags_t flags;
	struct task_entry *prev, *next;

	/* TODO: real task schedular */
	prev = task_current;
	next = prev+1;
	if (next == &task_entries[task_nr_regs])
		next = INIT_TASK;

	irq_local_save(flags);
	/* call arch specific switch to here */
	arch_hw_switch_to(prev, next, prev);
	irq_local_restore(flags);
}

void task_handler(uint8_t event)
{
	switch (event) {
	case STATE_EVENT_SHOT:
		if (timer_timeout_raised(task_tid, TIMER_DELAYABLE)) {
			task_schedule();
			timer_schedule_shot(task_tid, TASK_SLICE);
		}
		break;
	default:
		BUG();
		break;
	}
}

void task_init(void)
{
	struct task_entry *task;

	task_sid = state_register(task_handler);
	task_tid = timer_register(task_sid, TIMER_DELAYABLE);

	task = INIT_TASK;
	task->pid = INIT_PID;
	task->stack_size = INIT_STACK_SIZE;
	task->stack_bottom = INIT_STACK - INIT_STACK_SIZE;
	task->sp = INIT_STACK;

	timer_schedule_shot(task_tid, TASK_SLICE);
}
