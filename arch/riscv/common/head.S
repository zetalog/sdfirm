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

	.macro	zero_sect, sva, eva
	la	a0, \sva
	la	a1, \eva
	j	zero_sect_loop
1:
	sw	zero, 0(a0)
	addi	a0, a0, 4
zero_sect_loop:
	bltu	a0, a1, 1b
	.endm

	.macro	copy_sect, sva, eva, sla
	la	a2, \sla
	la	a0, \sva
	la	a1, \eva
	j	copy_sect_loop
1:
	lw	a3, 0(a2)
	sw	a3, 0(a0)
	addi	a2, a2, 4
	addi	a0, a0, 4
copy_sect_loop:
	bltu	a0, a1, 1b
	.endm

	/* Handle only .data section */
	.macro	init_data
	copy_sect __data_loc, __sdata, __edata
	.endm

	/* Handle only .bss section */
	.macro	init_bss
	zero_sect __bss_start, __bss_stop
	.endm

	/* Handle both .text and .rodata sections */
	.macro	init_text
	copy_sect __text_loc, _stext, _etext
	.endm

	__HEAD

ENTRY(__start)
	/* Disable global interrupt. */
	csrci	mstatus, 8

	init_sp
	init_gp

#ifndef CONFIG_XIP
	init_text
#endif
	init_data
	init_bss

	jal	ra, system_init
ENDPROC(__start)
