#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>

	.macro disable_mirq
	csrw	CSR_MIE, zero
	csrw	CSR_MIP, zero
	.endm

	.macro disable_sirq
	csrw	CSR_SIE, zero
	csrw	CSR_SIP, zero
	.endm

	.macro disable_entr_irq
#ifdef CONFIG_RISCV_ENTR_M
	disable_mirq
#endif
#ifdef CONFIG_RISCV_ENTR_S
	disable_sirq
#endif
	.endm

	.macro disable_exit_irq
	csrw	CSR_IE, zero
	csrw	CSR_IP, zero
	.endm

	.macro disable_fpu
#ifdef CONFIG_RISCV_F
	li	t0, SR_FS
	csrc	CSR_STATUS, t0
#endif
	.endm

	.macro init_mtls
	csrw	CSR_MSCRATCH, 0
	.endm

	.macro init_stls
	csrw	CSR_SSCRATCH, 0
	.endm

	.macro init_gpsptp
	init_gp
#ifdef CONFIG_SMP
	csrr	a3, CSR_MHARTID
	slli	sp, a3, PERCPU_STACK_SHIFT
	la	a3, (ABI_PERCPU_STACKS_START + PERCPU_STACK_SIZE)
	add	sp, sp, a3
#else
	la	sp, ABI_PERCPU_STACKS_END
#endif
	li	a3, SCRATCH_SIZE
	sub	tp, sp, a3
	add	sp, tp, zero
	REG_S	sp, SCRATCH_SP(tp);
	.endm

	.macro zerow regs
	REG_S	zero, 0(\regs)
	.endm

	.macro copyw regl, regs, regv
	REG_L	\regv, 0(\regl)
	REG_S	\regv, 0(\regs)
	.endm

	.macro zero_sect, sva, eva, lab_exit, lab_loop
	la	a0, \sva
	la	a1, \eva
	j	\lab_exit
\lab_loop:
	zerow	a0
	addi	a0, a0, WORD_SIZE
\lab_exit:
	bltu	a0, a1, \lab_loop
	.endm

	.macro copy_sect, sla, sva, eva, lab_exit, lab_loop
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

	.macro config_mmu, map, sat, mode
	la	\sat, \map
	srl	\sat, \sat, PAGE_SHIFT
	li	\mode, SATP_MODE
	or	\sat, \sat, \mode
	.endm

	.macro enable_mmu, map, sat, mode
#ifdef CONFIG_MMU
	config_mmu \map, \sat, \mode
	csrw	CSR_SATP, \sat
	sfence.vma
#endif
	.endm

	/* Enable ABI environment, gp, tp, sp and vectors */
	.macro enable_abi
	init_gpsptp
	call	trap_init
	.endm

	__HEAD

ENTRY(__start)
#ifdef CONFIG_RISCV_ENTR_M
	/* Reset core to the entry point on failure */
	la	ra, __start
	/* Reset all registers, except ra, gp, sp */
	init_gprs
	init_mtls
#endif
	/* Disable global interrupt. */
	disable_entr_irq

	/* Jump to warm boot if it is not the first core booting */
	csrr	a6, CSR_MHARTID
	li	a5, NR_CPUS
	bge	a6, a5, secondary_park
	/* hartid 0 is the mandatory core */
	bnez	a6, secondary_wait_for_abi_relocate

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

	la	t0, abi_relocate_done
	REG_S	t0, 0(t0)
	fence	rw, rw

	/* At this point we are running from link address */
	j	__abi_start_warm

secondary_wait_for_abi_relocate:
	la	t0, abi_relocate_done
	REG_L	t1, 0(t0)
	/* Reduce the bus traffic so that boot hart may proceed faster */
	nop
	nop
	nop
	bne	t0, t1, secondary_wait_for_abi_relocate

__abi_start_warm:
	la	ra, _start_hang
#ifdef CONFIG_SBI
	jal	ra, __sbi_entry
#else
	jal	ra, __sbi_exit
#endif
ENDPROC(__start)

ENTRY(__sbi_exit)
	disable_exit_irq
	disable_fpu

	csrr	a6, CSR_MHARTID
	bnez	a6, secondary_start

	/* TODO:
	 * Do we need to initialize gp, tp, sp before invoking PIC?
	 * Remove the following lines if we don't.
	 */
	init_gpsptp

#ifdef CONFIG_MMU
	call	bpgt_init
#endif
	call	relocate
	enable_abi
#ifdef CONFIG_SMP
	call	smp_boot
#endif

	/* Initialize task */
#ifdef CONFIG_TASK
	la	tp, init_task
#endif

	tail	system_init
ENDPROC(__sbi_exit)

ENTRY(relocate)
	/* Point stvec to virtual address of instruction after SATP write */
	la	a3, 1f
	csrw	CSR_TVEC, a3

	/* Load boot identity page directory */
	enable_mmu mmu_id_map, a3, a4

	/* Point stvec to the CPU park entry */
	.align 2
1:
	la	a3, secondary_park
	csrw	CSR_TVEC, a3
	ret
ENDPROC(relocate)

secondary_start:
#ifdef CONFIG_SMP
	slli	a3, a6, LGREG
	la	a4, __cpu_up_entry
	add	a4, a3, a4

wait_for_cpu_up:
	REG_L	a7, (a4) /* entry */
	beqz	a7, wait_for_cpu_up
	fence

	call	relocate
	enable_abi
	jr	a7
#endif

.align 2
secondary_park:
	wfi
	j	secondary_park

#ifndef CONFIG_SPIKE_SHUTDOWN_OVPSIM
ENTRY(_start_hang)
1:
	wfi
	j	1b
ENDPROC(_start_hang)
#endif /* CONFIG_SPIKE_SHUTDOWN_OVPSIM */

ENTRY(get_sp)
	add	a0, sp, zero
	ret
ENDPROC(get_sp)

ENTRY(get_tp)
	add	a0, tp, zero
	ret
ENDPROC(get_tp)

ENTRY(sim_shutdown)
	j	test_section_end
ENTRY(test_section_end)
#ifdef CONFIG_SPIKE_SHUTDOWN_DUOWEN
	csrw	satp, zero
	li	x10, 0xFFFFFFFF00
	li	x11, 0x14
	sd	x11, 0(x10)
	fence
#endif /* CONFIG_SPIKE_SHUTDOWN_DUOWEN */
END(test_section_end)
#ifdef CONFIG_SPIKE_SHUTDOWN_OVPSIM
ENTRY(write_tohost)
	j	write_tohost
END(write_tohost)
#endif /* CONFIG_SPIKE_SHUTDOWN_OVPSIM */
	ret
ENDPROC(sim_shutdown)

	.pushsection .data
	.align 3
abi_relocate_done:
	RISCV_PTR	0
	.popsection
