#ifndef __TASK_RISCV_H_INCLUDE__
#define __TASK_RISCV_H_INCLUDE__

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
struct pt_regs {
	unsigned long zero;
	unsigned long ra;
	unsigned long sp;
	unsigned long gp;
	unsigned long tp;
	unsigned long t0;
	unsigned long t1;
	unsigned long t2;
	unsigned long s0;
	unsigned long s1;
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
	unsigned long a4;
	unsigned long a5;
	unsigned long a6;
	unsigned long a7;
	unsigned long s2;
	unsigned long s3;
	unsigned long s4;
	unsigned long s5;
	unsigned long s6;
	unsigned long s7;
	unsigned long s8;
	unsigned long s9;
	unsigned long s10;
	unsigned long s11;
	unsigned long t3;
	unsigned long t4;
	unsigned long t5;
	unsigned long t6;
	/* CSRs */
	unsigned long epc;
	unsigned long status;
	unsigned long badaddr;
	unsigned long cause;
	/* a0 value before the syscall */
	unsigned long orig_a0;
};
#endif

#endif /* __TASK_RISCV_H_INCLUDE__ */
