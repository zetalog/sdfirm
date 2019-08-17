#ifndef __TASK_ARM_H_INCLUDE__
#define __TASK_ARM_H_INCLUDE__

#include <target/config.h>
#include <asm/mach/arch.h>

#define INIT_STACK	RAMEND
#define INIT_STACK_SIZE	256

struct smc_context {
	uint32_t	r0;
	uint32_t	r1;
	uint32_t	r2;
	uint32_t	r3;
	uint32_t	r4;
	uint32_t	r5;
	uint32_t	r6;
	uint32_t	r7;
	uint32_t	r8;
	uint32_t	r9;
	uint32_t	r10;
	uint32_t	r11;
	uint32_t	r12;
	uint32_t	sp_usr;
	uint32_t	sp_svc;
	uint32_t	sp_mon;
	uint32_t	sp_abt;
	uint32_t	sp_und;
	uint32_t	sp_irq;
	uint32_t	sp_fiq;
	uint32_t	lr_usr;
	uint32_t	lr_svc;
	uint32_t	lr_mon;
	uint32_t	lr_abt;
	uint32_t	lr_und;
	uint32_t	lr_irq;
	uint32_t	lr_fiq;
	/* SPSR only exists in system modes */
	uint32_t	spsr_svc;
	uint32_t	spsr_mon;
	uint32_t	spsr_abt;
	uint32_t	spsr_und;
	uint32_t	spsr_irq;
	uint32_t	spsr_fiq;
};

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
