#ifndef __TASK_H_INCLUDE__
#define __TASK_H_INCLUDE__

#include <target/generic.h>

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

struct task_entry {
	pid_t pid;
	size_t stack_size;
	caddr_t stack_bottom;
	caddr_t sp;
};
#endif /* __ASSEMBLY__ */

#include <asm/task.h>

#ifndef __ASSEMBLY__
pid_t task_create(task_call_cb call, void *priv,
		  caddr_t stack_bottom, size_t stack_size);
void task_schedule(void);
#endif /* __ASSEMBLY__ */

#endif /* __TASK_H_INCLUDE__ */
