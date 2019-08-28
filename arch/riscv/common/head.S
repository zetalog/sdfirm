#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <target/arch.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>
#include <target/paging.h>

	__VEC

__vectors:
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
#ifdef CONFIG_SMP
#else
	la	sp, PERCPU_STACKS_END
#endif
	.endm

	.macro	init_gp
	la	gp, __global_pointer$
	.endm

	.macro	zero_sect, sva, eva, lab_exit, lab_loop
	la	a0, \sva
	la	a1, \eva
	j	\lab_exit
\lab_loop:
	sw	zero, 0(a0)
	addi	a0, a0, WORD_SIZE
\lab_exit:
	bltu	a0, a1, \lab_loop
	.endm

	.macro	copy_sect, sla, sva, eva, lab_exit, lab_loop
	la	a2, \sla
	la	a0, \sva
	la	a1, \eva
	j	\lab_exit
\lab_loop:
	lw	a3, 0(a2)
	sw	a3, 0(a0)
	addi	a2, a2, WORD_SIZE
	addi	a0, a0, WORD_SIZE
\lab_exit:
	bltu	a0, a1, \lab_loop
	.endm

	__HEAD

ENTRY(__start)
	/* Disable global interrupt. */
	csrci	mstatus, 8

	init_sp
	init_gp

#ifndef CONFIG_XIP
	/* Handle both .text and .rodata sections */
	copy_sect __text_loc, _stext, _etext, copy_text_exit, copy_text_loop
#endif
#ifdef CONFIG_DATA_RAM_LOAD
	/* Handle .data section */
	copy_sect __data_loc, __sdata, __edata, copy_data_exit, copy_data_loop
#endif
	/* Handle .bss section */
	zero_sect __bss_start, __bss_stop, init_bss_exit, init_bss_loop

	la	t0, __vectors
	csrw	mtvec, t0
#if 1
	csrw	0x005, t0
#else
	csrw	utvec, t0
#endif

	jal	ra, system_init
ENDPROC(__start)
