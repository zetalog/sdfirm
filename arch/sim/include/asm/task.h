#ifndef __TASK_SIM_H_INCLUDE__
#define __TASK_SIM_H_INCLUDE__

#include <target/types.h>

#define INIT_STACK_SIZE		0x1000
#define INIT_STACK		(sim_stack)

extern caddr_t sim_stack;

#define arch_hw_init_task(task, call, priv)
#define arch_hw_switch_to(prev, next, last)

#endif /* __TASK_SIM_H_INCLUDE__ */
