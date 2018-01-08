#ifndef __CONTEXT_ARM_INCLUDE_H__
#define __CONTEXT_ARM_INCLUDE_H__

#include <target/config.h>
#include <target/types.h>
#include <asm/reg.h>

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

#endif /* __CONTEXT_ARM_INCLUDE_H__ */
