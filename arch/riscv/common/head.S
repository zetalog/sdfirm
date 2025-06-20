#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>
#include <target/task.h>

#define RV_LARGE_LOAD_OFFSET	(CONFIG_BOOT_BASE - CONFIG_LOAD_BASE)

#ifdef CONFIG_XIP_LIB
	.macro lib_enter
	addi	sp, sp, -128
	sd	ra, 120(sp)
	sd	s0, 112(sp)
	sd	s1, 104(sp)
	sd	s2, 96(sp)
	sd	s3, 88(sp)
	sd	s4, 80(sp)
	sd	s5, 72(sp)
	sd	s6, 64(sp)
	sd	s7, 56(sp)
	sd	s8, 48(sp)
	sd	s9, 40(sp)
	sd	s10, 32(sp)
	sd	s11, 24(sp)
	.endm
	.macro lib_exit
	ld	ra, 120(sp)
	ld	s0, 112(sp)
	ld	s1, 104(sp)
	ld	s2, 96(sp)
	ld	s3, 88(sp)
	ld	s4, 80(sp)
	ld	s5, 72(sp)
	ld	s6, 64(sp)
	ld	s7, 56(sp)
	ld	s8, 48(sp)
	ld	s9, 40(sp)
	ld	s10, 32(sp)
	ld	s11, 24(sp)
	addi	sp, sp, 128
	.endm
#else
	.macro lib_enter
	.endm
	.macro lib_exit
	.endm
#endif /* CONFIG_XIP_LIB */

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
#ifdef CONFIG_SMP_BOOT
	csrr	\reg, CSR_MHARTID
	get_arch_smpid \reg
#else
	li	\reg, 0
#endif
	.endm

	.macro get_hartid reg
	csrr	\reg, CSR_MHARTID
	.endm

#ifdef CONFIG_XIP_LIB
	.macro init_sp
	.endm
#else
	.macro init_sp
	get_smpid a3
#ifdef CONFIG_SMP_BOOT
	slli	sp, a3, PERCPU_STACK_SHIFT
	la	a3, (ABI_PERCPU_STACKS_START + PERCPU_STACK_SIZE)
	add	sp, sp, a3
#else
	la	sp, ABI_PERCPU_STACKS_END
#endif
	.endm
#endif

	.macro init_gpsptp
	init_gp
	get_smpid a3
#ifdef CONFIG_SMP_BOOT
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
	la	t0, \sva
	la	t1, \eva
	j	\lab_exit
\lab_loop:
	zerow	t0
	addi	t0, t0, WORD_SIZE
\lab_exit:
	bltu	t0, t1, \lab_loop
	.endm

#ifdef CONFIG_MAXPHYSMEM_128GB
	/* No large mcmodel defined for long range loading, so we have to
	 * do it ourselves.
	 */
	.macro copy_sect, sva, eva, lab_exit, lab_loop
	li	t2, RV_LARGE_LOAD_OFFSET
#ifndef CONFIG_LOAD_TEXT
	/* Find .data section load address */
	la	t0, _stext
	la	t1, _etext
	sub	t1, t1, t0
	add	t2, t2, t1
#endif
	la	t0, \sva
	add	t2, t2, t0
	la	t1, \eva
	j	\lab_exit
\lab_loop:
	copyw	t2, t0, t3
	addi	t2, t2, WORD_SIZE
	addi	t0, t0, WORD_SIZE
\lab_exit:
	bltu	t0, t1, \lab_loop
	.endm
#else
	.macro copy_sect, sla, sva, eva, lab_exit, lab_loop
	la	t2, \sla
	la	t0, \sva
	la	t1, \eva
	j	\lab_exit
\lab_loop:
	copyw	t2, t0, t3
	addi	t2, t2, WORD_SIZE
	addi	t0, t0, WORD_SIZE
\lab_exit:
	bltu	t0, t1, \lab_loop
	.endm
#endif

	.macro config_mmu, map, sat, mode
	la	\sat, \map
	srl	\sat, \sat, PAGE_SHIFT
	li	\mode, SATP_MODE
	or	\sat, \sat, \mode
	.endm

	/* The first time mapping enabling, sfence.vma may be invoked to
	 * flush spurious TLB entries.
	 */
	.macro enable_mmu, map, sat, mode
#ifdef CONFIG_MMU
	config_mmu \map, \sat, \mode
	sfence.vma
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

	.macro MOV_3R __d0, __s0, __d1, __s1, __d2, __s2
	add	\__d0, \__s0, zero
	add	\__d1, \__s1, zero
	add	\__d2, \__s2, zero
	.endm
	__HEAD

ENTRY(__start)
	lib_enter
#ifdef CONFIG_ARCH_HAS_BOOT0
	boot0_hook
#endif

#ifndef CONFIG_XIP_LIB
#ifdef CONFIG_RISCV_ENTR_M
	/* Reset all registers, except ra, gp, sp */
	init_gprs
	init_mtls
	/* Reset core to the entry point on failure */
	la	ra, _start_hang
#endif
	init_gp

	/* Disable global interrupt. */
	disable_entr_irq
#endif

	/* Jump to warm boot if it is not the boot core */
	get_hartid a6
	li	a4, 1
	sll	a4, a4, a6
	get_arch_hartmask a5
	and	a4, a4, a5
	beqz	a4, secondary_park
	/* Pick up boot core */
	get_arch_hartboot a7
	bne	a6, a7, secondary_wait_for_abi_relocate

#ifdef CONFIG_LOAD_TEXT
	/* Handle both .text and .rodata sections */
#ifdef CONFIG_MAXPHYSMEM_128GB
	copy_sect _stext, _etext, copy_text_exit, copy_text_loop
#else
	copy_sect __text_loc, _stext, _etext, copy_text_exit, copy_text_loop
#endif
#endif
#ifdef CONFIG_LOAD_DATA
	/* Handle .data section */
#ifdef CONFIG_MAXPHYSMEM_128GB
	copy_sect __sdata, __edata, copy_data_exit, copy_data_loop
#else
	copy_sect __data_loc, __sdata, __edata, copy_data_exit, copy_data_loop
#endif
#endif
#ifndef CONFIG_NO_ZERO_BSS
	/* Handle .bss section */
	zero_sect __bss_start, __bss_stop, init_bss_exit, init_bss_loop
#endif

	/* At this point we are running from link address */
#ifdef CONFIG_ARCH_HAS_BOOT1
	init_sp
	/* Note that a3 shouldn't be touched in boot hooks, boot1_hook is
	 * only invoked by the boot core.
	 */
	boot1_hook
#endif

	la	t0, abi_relocate_done
	REG_S	t0, 0(t0)
	fence	rw, rw

	j	__abi_start_warm

secondary_wait_for_abi_relocate:
	fence	rw, rw
	la	t0, abi_relocate_done
	REG_L	t1, 0(t0)
	/* Reduce the bus traffic so that boot hart may proceed faster */
	nop
	nop
	nop
	bne	t0, t1, secondary_wait_for_abi_relocate

__abi_start_warm:
#ifdef CONFIG_XIP_LIB
	lib_exit
	ret
#else
	la	ra, _start_hang

#ifdef CONFIG_ARCH_HAS_BOOT2
	init_sp
	/* Note that a3 shouldn't be touched in boot hooks, boot2_hook is
	 * invoked by all cores.
	 */
	boot2_hook
#endif

#ifdef CONFIG_SBI
	jal	ra, __sbi_entry
#else
	jal	ra, __sbi_exit
#endif
#endif
ENDPROC(__start)

#ifndef CONFIG_XIP_LIB
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
	/* Pick up boot core */
	get_arch_hartboot a7
	bne	a6, a7, secondary_start

#ifdef CONFIG_MMU
	call	bpgt_init
#endif
	call	relocate
	enable_abi
#ifdef CONFIG_SMP
	call	smp_map_init
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
	init_gp
	la	a3, secondary_park
	csrw	CSR_TVEC, a3
	ret
ENDPROC(relocate)

#define CONFIG_SMP_BOOT_QUIRK_INIT_WAIT_ENTRY	1
secondary_start:
#ifdef CONFIG_SMP_BOOT
	/* Convert hartid to smpid */
	get_arch_smpid a6
	slli	a3, a6, LGREG
	la	a4, __cpu_up_entry
	add	a4, a3, a4
#ifdef CONFIG_SMP_BOOT_QUIRK_INIT_WAIT_ENTRY
	REG_S   zero, (a4)
#endif

wait_for_cpu_up:
	fence	rw, rw
	REG_L	a7, (a4) /* entry */
	beqz	a7, wait_for_cpu_up
	fence

	call	relocate
	enable_abi
	jr	a7
#endif
#endif

.align 2
secondary_park:
	wfi
	j	secondary_park

#ifndef CONFIG_XIP_LIB
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
#endif

	.pushsection .data
	.align 3
abi_relocate_done:
	RISCV_PTR	0
	.popsection
