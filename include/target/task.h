#ifndef __TASK_H_INCLUDE__
#define __TASK_H_INCLUDE__

#include <target/generic.h>

#ifndef __ASSEMBLY__
typedef void (*task_call_cb)(void *priv);
#endif

#include <asm/task.h>

#ifndef __ASSEMBLY__
typedef uint8_t pid_t;
typedef void (*task_call_cb)(void *priv);

#ifdef CONFIG_MAX_TASKS
#define NR_TASKS				CONFIG_MAX_TASKS
#else
#define NR_TASKS				1
#endif
#define INVALID_PID				NR_TASKS
#define INIT_PID				0

#ifndef INIT_STACK
#define INIT_STACK				(PERCPU_STACKS_START + PERCPU_STACK_SIZE)
#define INIT_STACK_SIZE				PERCPU_STACK_SIZE
#endif /* INIT_TASK */

struct task_entry {
	pid_t pid;
	size_t stack_size;
	caddr_t stack_bottom;
	caddr_t kern_sp;
	caddr_t user_sp;
#ifdef CONFIG_ARCH_HAS_THREAD
	struct thread_entry thread;
#endif
};

#ifdef CONFIG_TASK
void task_init(void);
pid_t task_create(task_call_cb call, void *priv,
		  caddr_t stack_bottom, size_t stack_size);
void task_schedule(void);
extern struct task_entry task_entries[];
#else
#define task_init()			do { } while (0)
#define task_create(call, priv, sb, ss)	INVALID_PID
#define task_schedule()			do { } while (0)
#endif
#endif /* __ASSEMBLY__ */

#define init_task			task_entries

#endif /* __TASK_H_INCLUDE__ */
