#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>

	.macro	init_sp
#ifdef CONFIG_SMP
	csrr	a3, CSR_MHARTID
	slli	a2, a3, PERCPU_STACK_SHIFT
	la	sp, PERCPU_STACKS_START
	add	sp, sp, a2
#else
	la	sp, PERCPU_STACKS_END
#endif
	.endm

	.macro	init_gp
	.option push
	.option norelax
	la	gp, __global_pointer$
	.option pop
	.endm

	.macro disable_mirq
#ifdef CONFIG_RISCV_ENTR_M
	csrci	CSR_MSTATUS, SR_MIE
#endif
	.endm

	.macro disable_sirq
	csrw	CSR_IE, zero
	csrw	CSR_IP, zero
	.endm

	.macro disable_fpu
#ifdef CONFIG_RISCV_F
	li	t0, SR_FS
	csrc	CSR_STATUS, t0
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

	.macro	config_mmu, map, sat, mode
	la	\sat, \map
	srl	\sat, \sat, PAGE_SHIFT
	li	\mode, SATP_MODE
	or	\sat, \sat, \mode
	.endm

	.macro	enable_mmu, map, sat, mode
	config_mmu \map, \sat, \mode
#ifdef CONFIG_MMU
	csrw	CSR_SATP, \sat
	sfence.vma
#endif
	.endm

	__HEAD

ENTRY(__start)
	/* Disable global interrupt. */
	disable_mirq

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

	fence	rw, rw
	/* At this point we are running from link address */

	/* Reset all registers for boot HART */
	li	ra, 0
	call	reset_gprs

#ifdef CONFIG_SBI
	jal	ra, __sbi_entry
#else
	jal	ra, __sbi_exit
#endif
ENDPROC(__start)

ENTRY(__sbi_exit)
	disable_sirq
	disable_fpu

	csrr	a3, CSR_MHARTID
	bnez	a3, .Lsecondary_start

#ifdef CONFIG_MMU
	call	bpgt_init
#endif
	call	relocate
	call	trap_init

	/* TODO: task initialization and DTB handling */
	tail	system_init
ENDPROC(__sbi_exit)

ENTRY(relocate)
	/* Point stvec to virtual address of instruction after SATP write */
	la	a0, 1f
	csrw	CSR_TVEC, a0

	/* Load boot identity page directory */
	enable_mmu mmu_id_map, a0, a1
.align 2
1:
	la	a0, .Lsecondary_park
	csrw	CSR_TVEC, a0
	init_gp
	init_sp
	ret
ENDPROC(relocate)

.Lsecondary_start:
#ifdef CONFIG_SMP
	li	a1, NR_CPUS
	bgeu	a0, a1, .Lsecondary_park

	la	a3, .Lsecondary_park
	csrw	CSR_TVEC, a3

	call	relocate
	call	trap_init
	/* tail	__smp_start */
#endif

.align 2
.Lsecondary_park:
	wfi
	j	.Lsecondary_park

ENTRY(reset_gprs)
	/* flush the instruction cache */
	fence.i
	/* Reset all registers except ra, a0, a1 and a2 */
	init_sp
	init_gp
	li	tp, 0
	li	t0, 0
	li	t1, 0
	li	t2, 0
	li	s0, 0
	li	s1, 0
	li	a3, 0
	li	a4, 0
	li	a5, 0
	li	a6, 0
	li	a7, 0
	li	s2, 0
	li	s3, 0
	li	s4, 0
	li	s5, 0
	li	s6, 0
	li	s7, 0
	li	s8, 0
	li	s9, 0
	li	s10, 0
	li	s11, 0
	li	t3, 0
	li	t4, 0
	li	t5, 0
	li	t6, 0
	csrw	CSR_MSCRATCH, 0
	ret
ENDPROC(reset_gprs)
