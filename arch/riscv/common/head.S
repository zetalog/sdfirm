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
#ifndef CONFIG_SMP
	li	sp, PERCPU_STACKS_END
#endif
	.endm

	.macro	init_gp
	la	gp, _sdata
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

	.macro	copy_sect, sva, eva, sla, lab_exit, lab_loop
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

#if 1
#ifdef CONFIG_SMP
#else
	la	sp, PERCPU_STACKS_END
#endif
#else
	init_sp
#endif
#if 1
	la	gp, _sdata
#else
	init_gp
#endif

#ifndef CONFIG_XIP
	/* Handle both .text and .rodata sections */
	copy_sect __text_loc, _stext, _etext, copy_text_exit, copy_text_loop
#endif
#if 1
	/* Handle .data section */
	la	a2, __data_loc
	la	a0, __sdata
	la	a1, __edata
	j	copy_data_exit
copy_data_loop:
	lw	a3, 0(a2)
	sw	a3, 0(a0)
	addi	a2, a2, WORD_SIZE
	addi	a0, a0, WORD_SIZE
copy_data_exit:
	bltu	a0, a1, copy_data_loop
#else
	copy_sect __data_loc, __sdata, __edata, copy_data_exit, copy_data_loop
#endif
	/* Handle .bss section */
#if 1
	la	a0, __bss_start
	la	a1, __bss_stop
	j	init_bss_exit
init_bss_loop:
	sw	zero, 0(a0)
	addi	a0, a0, WORD_SIZE
init_bss_exit:
	bltu	a0, a1, init_bss_loop
#else
	zero_sect __bss_start, __bss_stop, init_bss_exit, init_bss_loop
#endif

	la	t0, __vectors
	csrw	mtvec, t0
#if 1
	csrw	0x005, t0
#else
	csrw	utvec, t0
#endif

	jal	ra, system_init
ENDPROC(__start)
