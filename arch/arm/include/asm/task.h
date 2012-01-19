#ifndef __TASK_ARM_H_INCLUDE__
#define __TASK_ARM_H_INCLUDE__

#include <target/config.h>
#include <asm/mach/arch.h>

#define INIT_STACK	RAMEND
#define INIT_STACK_SIZE	256

struct pt_regs {
	uint8_t dummy;
};

extern void  __arch_hw_init_task(
		struct task_entry *task, 
		task_call_cb call, 
		void *priv);

extern struct task_entry* __arch_hw_switch_to(
		struct task_entry *prev, 
		struct task_entry *next); 

#define arch_hw_init_task(task, call, priv)		\
	do {						\
		__arch_hw_init_task(task, call, priv);	\
	} while (0);

#define arch_hw_switch_to(prev, next, last)		\
	do {						\
		last = __arch_hw_switch_to(prev, next);	\
	} while (0);		 

#endif /* __TASK_ARM_H_INCLUDE__ */
