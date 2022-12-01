#include <target/task.h>
#include <target/timer.h>
#include <target/heap.h>
#include <target/irq.h>
#include <target/panic.h>
#include <target/smp.h>

#define TASK_SLICE		10
#define INIT_TASK		(&task_entries[INIT_PID])

struct task_entry task_entries[NR_TASKS];
pid_t task_nr_regs = INIT_PID+1;
struct task_entry *task_current = INIT_TASK;

tid_t task_tid = INVALID_TID;

/* This is actually a thread implementation as it passes an arg to the task
 * entry function. As far as RTOS is concerned rather than a multi-process
 * rich OS, this is sufficient.
 *
 * NOTE that, the current implementation is not SMP safe and should only be
 * used when SMP is configured out.
 */
pid_t task_create_stack(task_call_cb call, void *priv,
			caddr_t stack_bottom, size_t stack_size)
{
	struct task_entry *task;
	pid_t pid;
	irq_flags_t flags;

	irq_local_save(flags);
	BUG_ON(task_nr_regs == NR_TASKS);

	pid = task_nr_regs;
	task = &task_entries[pid];
	task_nr_regs++;

	task->pid = pid;
	task->stack_size = stack_size;
	task->stack_bottom = stack_bottom;
	task->kern_sp = (stack_bottom + stack_size);
	
	arch_hw_init_task(task, call, priv);
	irq_local_restore(flags);
	return pid;
}

pid_t task_create(task_call_cb call, void *priv, size_t stack_size)
{
	caddr_t stack;

	stack = page_alloc();
	return task_create_stack(call, NULL, stack, stack_size);
}

void task_schedule(void)
{
	irq_flags_t flags;
	struct task_entry *prev, *next;

	irq_local_save(flags);

	/* TODO: Real Task Scheduler
	 *
	 * The following only implements a simplest round-robin scheduler
	 * to demonstrate task switching ability of the firmware.
	 * It dequeues a next task and rounds to the INIT_TASK when next
	 * task is the last task.
	 */
	prev = task_current;
	next = prev+1;
	if (next == &task_entries[task_nr_regs])
		next = INIT_TASK;

	if (prev != next) {
		/* call arch specific switch to here */
		printf("Switching %d -> %d\n", prev->pid, next->pid);
		task_current = next;
		arch_hw_switch_to(prev, next, prev);
	}
	irq_local_restore(flags);
}

void task_timer_handler(void)
{
#ifdef CONFIG_TASK_TEST
	printf("task slicing...\n");
#endif
	timer_schedule_shot(task_tid, TASK_SLICE);
	task_schedule();
}

timer_desc_t task_timer = {
	TIMER_IRQ,
	task_timer_handler,
};

#ifdef CONFIG_TASK_TEST
static void task1_main(void *priv)
{
	int a = 1;

	while (1) {
		printf("task 1 scheduled - %d\n", a);
		task_schedule();
		a += 100;
	}
}

static void task2_main(void *priv)
{
	int a = 2;

	while (1) {
		printf("task 2 scheduled - %d\n", a);
		task_schedule();
		a += 1000;
	}
}

static void task_test(void)
{
	task_create(task1_main, NULL, PAGE_SIZE);
	task_create(task2_main, NULL, PAGE_SIZE);
}
#else
#define task_test()			do { } while (0)
#endif

void task_init(void)
{
	struct task_entry *task;

	task_tid = timer_register(&task_timer);
	task = INIT_TASK;
	task->pid = INIT_PID;
	task->stack_size = INIT_STACK_SIZE;
	task->stack_bottom = INIT_STACK - INIT_STACK_SIZE;
	task->kern_sp = INIT_STACK;
	timer_schedule_shot(task_tid, TASK_SLICE);
	task_test();
}
