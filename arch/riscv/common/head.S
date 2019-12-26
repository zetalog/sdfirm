#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>

	.macro	init_sp
	la	sp, PERCPU_STACKS_END
	.endm

	.macro	init_gp
	.option push
	.option norelax
	la	gp, __global_pointer$
	.option pop
	.endm

	.macro disable_irq
#ifdef CONFIG_RISCV_ENTR_M
	csrci	mstatus, SR_MIE
#endif
	.endm

	.macro disable_fpu
#ifdef CONFIG_CPU_F
	li	t0, SR_FS
	csrc	sstatus, t0
#endif
	.endm

	.macro	zerow regs
	REG_S	zero, 0(\regs)
	.endm

	.macro	copyw regl, regs, regv
	REG_L	\regv, 0(\regl)
	REG_S	\regv, 0(\regs)
	.endm

	.macro	zero_sect, sva, eva, lab_exit, lab_loop
	la	a0, \sva
	la	a1, \eva
	j	\lab_exit
\lab_loop:
	zerow	a0
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
	copyw	a2, a0, a3
	addi	a2, a2, WORD_SIZE
	addi	a0, a0, WORD_SIZE
\lab_exit:
	bltu	a0, a1, \lab_loop
	.endm

	__HEAD

ENTRY(__start)
	/* Disable global interrupt. */
	disable_irq

	init_sp
	init_gp

#ifdef CONFIG_LOAD_TEXT
	/* Handle both .text and .rodata sections */
	copy_sect __text_loc, _stext, _etext, copy_text_exit, copy_text_loop
#endif
#ifdef CONFIG_LOAD_DATA
	/* Handle .data section */
	copy_sect __data_loc, __sdata, __edata, copy_data_exit, copy_data_loop
#endif
	/* Handle .bss section */
	zero_sect __bss_start, __bss_stop, init_bss_exit, init_bss_loop

	la	t0, __vectors
	csrw	mtvec, t0
#ifdef CONFIG_RISCV_N
	/* csrw	utvec, t0 */
	csrw	CSR_UTVEC, t0
#endif

	jal	ra, system_init
ENDPROC(__start)
