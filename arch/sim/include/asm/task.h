#ifndef __TASK_SIM_H_INCLUDE__
#define __TASK_SIM_H_INCLUDE__

#include <target/types.h>

#define INIT_STACK_SIZE		0x1000
#define INIT_STACK		(sim_stack)

extern caddr_t sim_stack;

#endif /* __TASK_SIM_H_INCLUDE__ */
