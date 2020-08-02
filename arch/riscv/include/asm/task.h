#ifndef __TASK_RISCV_H_INCLUDE__
#define __TASK_RISCV_H_INCLUDE__

#include <target/smp.h>
#include <target/page.h>

#define MACHINE_STACK_SIZE		PAGE_SIZE
#define MENTRY_HLS_OFFSET		\
	(INTEGER_CONTEXT_SIZE + SOFT_FLOAT_CONTEXT_SIZE)
#define MENTRY_FRAME_SIZE		(MENTRY_HLS_OFFSET + HLS_SIZE)
#define MENTRY_IPI_OFFSET		(MENTRY_HLS_OFFSET)
#define MENTRY_IPI_PENDING_OFFSET	(MENTRY_HLS_OFFSET + SZREG)

#ifdef __riscv_flen
# define SOFT_FLOAT_CONTEXT_SIZE	0
#else
# define SOFT_FLOAT_CONTEXT_SIZE	(8 * 32)
#endif
#define HLS_SIZE			64
#define INTEGER_CONTEXT_SIZE		(32 * SZREG)

#ifndef __ASSEMBLY__
struct thread_entry {
	unsigned long ra;
	unsigned long sp;
	unsigned long s[12];
	cpu_t cpu;
};

extern struct task_entry *__switch_to(struct task_entry *,
				      struct task_entry *);

#define arch_hw_switch_to(prev, next, last)		\
do {							\
	struct task_entry *__prev = (prev);		\
	struct task_entry *__next = (next);		\
	((last) = __switch_to(__prev, __next));		\
} while (0)

void arch_hw_init_task(struct task_entry *task,
		       task_call_cb entry, void *priv);
#endif

#endif /* __TASK_RISCV_H_INCLUDE__ */
