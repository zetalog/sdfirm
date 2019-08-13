#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <target/arch.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>
#include <target/paging.h>

	__VEC

	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq
	jal	x0, __handle_irq

	/* reset vector */
	jal	x0, __start

	/* Illegal instrution exception */
	jal	x0, __handle_illegal_instr

	/* ecall handler */
	jal	x0, __handle_ecall

	/* LSU error */
	jal	x0, __handle_lsu

	.macro	init_sp
#ifndef CONFIG_SMP
	li	sp, PERCPU_STACKS_END
#endif
	.endm

	.macro	init_gp
	la	gp, _sdata
	.endm

	.macro	init_bss
	.endm

	.macro	init_data
	.endm

	.macro	init_text
	.endm

	__HEAD

ENTRY(__start)
	/* Disable global interrupt. */
	csrci	mstatus, 8

	init_sp
	init_gp
	init_bss
	init_data
#ifndef CONFIG_XIP
	init_text
#endif

	jal	ra, system_init
ENDPROC(__start)
