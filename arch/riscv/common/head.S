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

	.macro init_fpu
#ifdef CONFIG_RISCV_F
	li	t0, SR_FS_INITIAL
	csrs	CSR_STATUS, t0
#else
	li	t0, SR_FS
	csrc	CSR_STATUS, t0
#endif
#ifdef CONFIG_RISCV_V
	li	t0, SR_VS_INITIAL
	csrs	CSR_STATUS, t0
#else
	li	t0, SR_VS
	csrc	CSR_STATUS, t0
#endif
#if defined(CONFIG_RISCV_F) || defined(CONFIG_RISCV_V)
	csrwi	fcsr, 0
#endif
	.endm

	.macro init_mtls
	li	t0, SR_MPRV
	csrc	CSR_MSTATUS, 0
	csrw	CSR_MSCRATCH, 0
	.endm

	.macro init_stls
	csrw	CSR_SSCRATCH, 0
	.endm

	.macro get_smpid reg
#ifdef CONFIG_SMP
	csrr	\reg, CSR_MHARTID
	get_arch_smpid \reg
#else
	li	\reg, 0
#endif
	.endm

	.macro get_hartmask reg
	li	\reg, HART_ALL
	.endm

	.macro get_hartid reg
	csrr	\reg, CSR_MHARTID
	.endm

	.macro init_gpsptp
	init_gp
#ifdef CONFIG_SMP
	get_smpid a3
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
	fence	rw, rw
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
	fence	rw, rw
	.endm

	__HEAD

ENTRY(__start)
	init_gp
#ifdef CONFIG_RISCV_ENTR_M
	/* Reset all registers, except ra, gp, sp */
	init_gprs
	init_mtls
	/* Reset core to the entry point on failure */
	la	ra, __start
#endif

	/* Disable global interrupt. */
	disable_entr_irq

	/* Jump to warm boot if it is not the boot core */
	get_hartid a6
	li	t0, 1
	sll	t0, t0, a6
	get_hartmask t1
	and	t0, t0, t1
	beqz	t0, secondary_park
	/* Pick up boot core */
	li	a7, BOOT_HART
	bne	a6, a7, secondary_wait_for_abi_relocate

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
	fence	rw, rw
	init_gp
	disable_exit_irq
	init_fpu

	/* TODO:
	 * Do we need to initialize gp, tp, sp before invoking PIC?
	 * Remove the following lines if we don't.
	 */
	init_gpsptp

	get_hartid a6
	li	a7, BOOT_HART
	bne	a6, a7, secondary_start

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

	init_gp

	/* Point stvec to the CPU park entry */
	.align 2
1:
	la	a3, secondary_park
	csrw	CSR_TVEC, a3
	ret
ENDPROC(relocate)

secondary_start:
#ifdef CONFIG_SMP
	/* Convert hartid to smpid */
	get_arch_smpid a6
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
	li	a0, 0xFFFFFFFF00
#ifdef CONFIG_SMP
	get_hartid a1
	addi	a1, a1, 1
	slli	a1, a1, 4
	addi	a1, a1, 0x4
#else
	li	a1, 0x4
#endif
	sd	a1, 0(a0)
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
