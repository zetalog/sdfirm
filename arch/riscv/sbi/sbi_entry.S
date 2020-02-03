/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/init.h>
#include <target/arch.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_trap.h>
#include <asm/asm-offsets.h>

.macro	MOV_3R __d0, __s0, __d1, __s1, __d2, __s2
	add	\__d0, \__s0, zero
	add	\__d1, \__s1, zero
	add	\__d2, \__s2, zero
.endm

.macro	MOV_5R __d0, __s0, __d1, __s1, __d2, __s2, __d3, __s3, __d4, __s4
	add	\__d0, \__s0, zero
	add	\__d1, \__s1, zero
	add	\__d2, \__s2, zero
	add	\__d3, \__s3, zero
	add	\__d4, \__s4, zero
.endm

	__HEAD

ENTRY(__sbi_entry)
	/* mark relocate copy done */
	la	t0, _boot_status
	li	t1, 1
	REG_S	t1, 0(t0)
	fence	rw, rw

	/* Allow main firmware to save info */
	MOV_5R	s0, a0, s1, a1, s2, a2, s3, a3, s4, a4
	call	fw_save_info
	MOV_5R	a0, s0, a1, s1, a2, s2, a3, s3, a4, s4

	/* Preload HART details
	 * s7 -> HART Count
	 * s8 -> HART Stack Size
	 */
	la	a4, platform
#if __riscv_xlen == 64
	lwu	s7, SBI_PLATFORM_HART_COUNT_OFFSET(a4)
	lwu	s8, SBI_PLATFORM_HART_STACK_SIZE_OFFSET(a4)
#else
	lw	s7, SBI_PLATFORM_HART_COUNT_OFFSET(a4)
	lw	s8, SBI_PLATFORM_HART_STACK_SIZE_OFFSET(a4)
#endif

	/* Setup scratch space for all the HARTs*/
	la	tp, _fw_end
	mul	a5, s7, s8
	add	tp, tp, a5
	/* Keep a copy of tp */
	add	t3, tp, zero
	/* Counter */
	li	t2, 1
	/* hartid 0 is mandated by ISA */
	li	t1, 0
_scratch_init:
	add	tp, t3, zero
	mul	a5, s8, t1
	sub	tp, tp, a5
	li	a5, SBI_SCRATCH_SIZE
	sub	tp, tp, a5

	/* Initialize scratch space */
	/* Store fw_start and fw_size in scratch space */
	la	a4, _fw_start
	la	a5, _fw_end
	mul	t0, s7, s8
	add	a5, a5, t0
	sub	a5, a5, a4
	REG_S	a4, SBI_SCRATCH_FW_START_OFFSET(tp)
	REG_S	a5, SBI_SCRATCH_FW_SIZE_OFFSET(tp)
	/* Store next arg1 in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_next_arg1
	REG_S	a0, SBI_SCRATCH_NEXT_ARG1_OFFSET(tp)
	MOV_3R	a0, s0, a1, s1, a2, s2
	/* Store next address in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_next_addr
	REG_S	a0, SBI_SCRATCH_NEXT_ADDR_OFFSET(tp)
	MOV_3R	a0, s0, a1, s1, a2, s2
	/* Store next mode in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_next_mode
	REG_S	a0, SBI_SCRATCH_NEXT_MODE_OFFSET(tp)
	MOV_3R	a0, s0, a1, s1, a2, s2
	/* Store warm_boot address in scratch space */
	la	a4, _start_warm
	REG_S	a4, SBI_SCRATCH_WARMBOOT_ADDR_OFFSET(tp)
	/* Store platform address in scratch space */
	la	a4, platform
	REG_S	a4, SBI_SCRATCH_PLATFORM_ADDR_OFFSET(tp)
	/* Store hartid-to-scratch function address in scratch space */
	la	a4, _hartid_to_scratch
	REG_S	a4, SBI_SCRATCH_HARTID_TO_SCRATCH_OFFSET(tp)
	/* Clear tmp0 in scratch space */
	REG_S	zero, SBI_SCRATCH_TMP0_OFFSET(tp)
	/* Store firmware options in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
#ifdef FW_OPTIONS
	li	a4, FW_OPTIONS
#else
	add	a4, zero, zero
#endif
	call	fw_options
	or	a4, a4, a0
	REG_S	a4, SBI_SCRATCH_OPTIONS_OFFSET(tp)
	MOV_3R	a0, s0, a1, s1, a2, s2
	/* Move to next scratch space */
	add	t1, t1, t2
	blt	t1, s7, _scratch_init

	/* Override pervious arg1 */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_prev_arg1
	add	t1, a0, zero
	MOV_3R	a0, s0, a1, s1, a2, s2
	beqz	t1, _prev_arg1_override_done
	add	a1, t1, zero
_prev_arg1_override_done:

	/*
	 * Relocate Flatened Device Tree (FDT)
	 * source FDT address = previous arg1
	 * destination FDT address = next arg1
	 *
	 * Note: We will preserve a0 and a1 passed by
	 * previous booting stage.
	 */
	beqz	a1, _fdt_reloc_done
	/* Mask values in a3 and a4 */
	li	a3, ~(__SIZEOF_POINTER__ - 1)
	li	a4, 0xff
	/* t1 = destination FDT start address */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_next_arg1
	add	t1, a0, zero
	MOV_3R	a0, s0, a1, s1, a2, s2
	beqz	t1, _fdt_reloc_done
	beq	t1, a1, _fdt_reloc_done
	and	t1, t1, a3
	/* t0 = source FDT start address */
	add	t0, a1, zero
	and	t0, t0, a3
	/* t2 = source FDT size in big-endian */
#if __riscv_xlen == 64
	lwu	t2, 4(t0)
#else
	lw	t2, 4(t0)
#endif
	/* t3 = bit[15:8] of FDT size */
	add	t3, t2, zero
	srli	t3, t3, 16
	and	t3, t3, a4
	slli	t3, t3, 8
	/* t4 = bit[23:16] of FDT size */
	add	t4, t2, zero
	srli	t4, t4, 8
	and	t4, t4, a4
	slli	t4, t4, 16
	/* t5 = bit[31:24] of FDT size */
	add	t5, t2, zero
	and	t5, t5, a4
	slli	t5, t5, 24
	/* t2 = bit[7:0] of FDT size */
	srli	t2, t2, 24
	and	t2, t2, a4
	/* t2 = FDT size in little-endian */
	or	t2, t2, t3
	or	t2, t2, t4
	or	t2, t2, t5
	/* t2 = destination FDT end address */
	add	t2, t1, t2
	/* FDT copy loop */
	ble	t2, t1, _fdt_reloc_done
_fdt_reloc_again:
	REG_L	t3, 0(t0)
	REG_S	t3, 0(t1)
	add	t0, t0, __SIZEOF_POINTER__
	add	t1, t1, __SIZEOF_POINTER__
	blt	t1, t2, _fdt_reloc_again
_fdt_reloc_done:

	/* mark boot hart done */
	li	t0, 2
	la	t1, _boot_status
	REG_S	t0, 0(t1)
	fence	rw, rw
	j	_start_warm

	/* waitting for boot hart done (_boot_status == 2) */
_wait_for_boot_hart:
	li	t0, 2
	la	t1, _boot_status
	REG_L	t1, 0(t1)
	/* Reduce the bus traffic so that boot hart may proceed faster */
	nop
	nop
	nop
	bne	t0, t1, _wait_for_boot_hart

_start_warm:
	/* Reset all registers for non-boot HARTs */
	li	ra, 0
	call	reset_gprs

	/* Disable and clear all interrupts */
	csrw	CSR_MIE, zero
	csrw	CSR_MIP, zero

	la	a4, platform
#if __riscv_xlen == 64
	lwu	s7, SBI_PLATFORM_HART_COUNT_OFFSET(a4)
	lwu	s8, SBI_PLATFORM_HART_STACK_SIZE_OFFSET(a4)
#else
	lw	s7, SBI_PLATFORM_HART_COUNT_OFFSET(a4)
	lw	s8, SBI_PLATFORM_HART_STACK_SIZE_OFFSET(a4)
#endif

	/* HART ID should be within expected limit */
	csrr	s6, CSR_MHARTID
	bge	s6, s7, _start_hang

	/* find the scratch space for this hart */
	la	tp, _fw_end
	mul	a5, s7, s8
	add	tp, tp, a5
	mul	a5, s8, s6
	sub	tp, tp, a5
	li	a5, SBI_SCRATCH_SIZE
	sub	tp, tp, a5

	/* update the mscratch */
	csrw	CSR_MSCRATCH, tp

	/* Setup stack */
	add	sp, tp, zero

	/* Setup trap handler */
	la	a4, _trap_handler
	csrw	CSR_MTVEC, a4
	/* Make sure that mtvec is updated */
1:	csrr	a5, CSR_MTVEC
	bne	a4, a5, 1b

	/* Initialize SBI runtime */
	csrr	a0, CSR_MSCRATCH
	call	sbi_init

	/* We don't expect to reach here hence just hang */
	j	_start_hang

	/* never reached */
	ret
ENDPROC(__sbi_entry)

	.align 3
_boot_status:
	RISCV_PTR	0

ENTRY(_hartid_to_scratch)
	add	sp, sp, -(3 * __SIZEOF_POINTER__)
	REG_S	s0, (sp)
	REG_S	s1, (__SIZEOF_POINTER__)(sp)
	REG_S	s2, (__SIZEOF_POINTER__ * 2)(sp)
	/*
	 * a0 -> HART ID (passed by caller)
	 * s0 -> HART Stack Size
	 * s1 -> HART Stack End
	 * s2 -> Temporary
	 */
	la	s2, platform
#if __riscv_xlen == 64
	lwu	s0, SBI_PLATFORM_HART_STACK_SIZE_OFFSET(s2)
	lwu	s2, SBI_PLATFORM_HART_COUNT_OFFSET(s2)
#else
	lw	s0, SBI_PLATFORM_HART_STACK_SIZE_OFFSET(s2)
	lw	s2, SBI_PLATFORM_HART_COUNT_OFFSET(s2)
#endif
	mul	s2, s2, s0
	la	s1, _fw_end
	add	s1, s1, s2
	mul	s2, s0, a0
	sub	s1, s1, s2
	li	s2, SBI_SCRATCH_SIZE
	sub	a0, s1, s2
	REG_L	s0, (sp)
	REG_L	s1, (__SIZEOF_POINTER__)(sp)
	REG_L	s2, (__SIZEOF_POINTER__ * 2)(sp)
	add	sp, sp, (3 * __SIZEOF_POINTER__)
	ret
ENDPROC(_hartid_to_scratch)

ENTRY(_start_hang)
1:
	wfi
	j	1b
ENDPROC(_start_hang)

ENTRY(_trap_handler)
	/* Swap TP and MSCRATCH */
	csrrw	tp, CSR_MSCRATCH, tp

	/* Save T0 in scratch space */
	REG_S	t0, SBI_SCRATCH_TMP0_OFFSET(tp)

	/* Check which mode we came from */
	csrr	t0, CSR_MSTATUS
	srl	t0, t0, MSTATUS_MPP_SHIFT
	and	t0, t0, PRV_M
	xori	t0, t0, PRV_M
	beq	t0, zero, _trap_handler_m_mode

	/* We came from S-mode or U-mode */
_trap_handler_s_mode:
	/* Set T0 to original SP */
	add	t0, sp, zero

	/* Setup exception stack */
	add	sp, tp, -(PT_SIZE)

	/* Jump to code common for all modes */
	j	_trap_handler_all_mode

	/* We came from M-mode */
_trap_handler_m_mode:
	/* Set T0 to original SP */
	add	t0, sp, zero

	/* Re-use current SP as exception stack */
	add	sp, sp, -(PT_SIZE)

_trap_handler_all_mode:
	/* Save original SP (from T0) on stack */
	REG_S	t0, PT_SP(sp)

	/* Restore T0 from scratch space */
	REG_L	t0, SBI_SCRATCH_TMP0_OFFSET(tp)

	/* Save T0 on stack */
	REG_S	t0, PT_T0(sp)

	/* Swap TP and MSCRATCH */
	csrrw	tp, CSR_MSCRATCH, tp

	/* Save MEPC and MSTATUS CSRs */
	csrr	t0, CSR_MEPC
	REG_S	t0, PT_EPC(sp)
	csrr	t0, CSR_MSTATUS
	REG_S	t0, PT_STATUS(sp)

	/* Save all general regisers except SP and T0 */
	REG_S	zero, PT_ZERO(sp)
	REG_S	ra, PT_RA(sp)
	REG_S	gp, PT_GP(sp)
	REG_S	tp, PT_TP(sp)
	REG_S	t1, PT_T1(sp)
	REG_S	t2, PT_T2(sp)
	REG_S	s0, PT_S0(sp)
	REG_S	s1, PT_S1(sp)
	REG_S	a0, PT_A0(sp)
	REG_S	a1, PT_A1(sp)
	REG_S	a2, PT_A2(sp)
	REG_S	a3, PT_A3(sp)
	REG_S	a4, PT_A4(sp)
	REG_S	a5, PT_A5(sp)
	REG_S	a6, PT_A6(sp)
	REG_S	a7, PT_A7(sp)
	REG_S	s2, PT_S2(sp)
	REG_S	s3, PT_S3(sp)
	REG_S	s4, PT_S4(sp)
	REG_S	s5, PT_S5(sp)
	REG_S	s6, PT_S6(sp)
	REG_S	s7, PT_S7(sp)
	REG_S	s8, PT_S8(sp)
	REG_S	s9, PT_S9(sp)
	REG_S	s10, PT_S10(sp)
	REG_S	s11, PT_S11(sp)
	REG_S	t3, PT_T3(sp)
	REG_S	t4, PT_T4(sp)
	REG_S	t5, PT_T5(sp)
	REG_S	t6, PT_T6(sp)

	/* Call C routine */
	add	a0, sp, zero
	csrr	a1, CSR_MSCRATCH
	call	sbi_trap_handler

	/* Restore all general regisers except SP and T0 */
	REG_L	ra, PT_RA(sp)
	REG_L	gp, PT_GP(sp)
	REG_L	tp, PT_TP(sp)
	REG_L	t1, PT_T1(sp)
	REG_L	t2, PT_T2(sp)
	REG_L	s0, PT_S0(sp)
	REG_L	s1, PT_S1(sp)
	REG_L	a0, PT_A0(sp)
	REG_L	a1, PT_A1(sp)
	REG_L	a2, PT_A2(sp)
	REG_L	a3, PT_A3(sp)
	REG_L	a4, PT_A4(sp)
	REG_L	a5, PT_A5(sp)
	REG_L	a6, PT_A6(sp)
	REG_L	a7, PT_A7(sp)
	REG_L	s2, PT_S2(sp)
	REG_L	s3, PT_S3(sp)
	REG_L	s4, PT_S4(sp)
	REG_L	s5, PT_S5(sp)
	REG_L	s6, PT_S6(sp)
	REG_L	s7, PT_S7(sp)
	REG_L	s8, PT_S8(sp)
	REG_L	s9, PT_S9(sp)
	REG_L	s10, PT_S10(sp)
	REG_L	s11, PT_S11(sp)
	REG_L	t3, PT_T3(sp)
	REG_L	t4, PT_T4(sp)
	REG_L	t5, PT_T5(sp)
	REG_L	t6, PT_T6(sp)

	/* Restore MEPC and MSTATUS CSRs */
	REG_L	t0, PT_EPC(sp)
	csrw	CSR_MEPC, t0
	REG_L	t0, PT_STATUS(sp)
	csrw	CSR_MSTATUS, t0

	/* Restore T0 */
	REG_L	t0, PT_T0(sp)

	/* Restore SP */
	REG_L	sp, PT_SP(sp)

	mret
ENDPROC(_trap_handler)

ENTRY(fw_save_info)
	/*
	 * We can only use a0, a1, a2, a3, and a4 registers here.
	 * The a0, a1, and a2 registers will be same as passed by
	 * previous booting stage.
	 * Nothing to be returned here.
	 */
	ret
ENDPROC(fw_save_info)

ENTRY(fw_prev_arg1)
	/*
	 * We can only use a0, a1, and a2 registers here.
	 * The previous arg1 should be returned in 'a0'.
	 */
#ifdef FW_PAYLOAD_FDT_PATH
	la	a0, fdt_bin
#else
	add	a0, zero, zero
#endif
	ret
ENDPROC(fw_prev_arg1)

ENTRY(fw_next_arg1)
	/*
	 * We can only use a0, a1, and a2 registers here.
	 * The next arg1 should be returned in 'a0'.
	 */
#ifdef FW_PAYLOAD_FDT_ADDR
	li	a0, FW_PAYLOAD_FDT_ADDR
#else
	add	a0, zero, zero
#endif
	ret
ENDPROC(fw_next_arg1)

ENTRY(fw_next_addr)
	/*
	 * We can only use a0, a1, and a2 registers here.
	 * The next address should be returned in 'a0'.
	 */
#if 0
	la	a0, payload_bin
#else
	la	a0, __sbi_exit
#endif
	ret
ENDPROC(fw_next_addr)

ENTRY(fw_next_mode)
	/*
	 * We can only use a0, a1, and a2 registers here.
	 * The next address should be returned in 'a0'.
	 */
	li	a0, PRV_S
	ret
ENDPROC(fw_next_mode)

ENTRY(fw_options)
	/*
	 * We can only use a0, a1, and a2 registers here.
	 * The 'a4' register will have default options.
	 * The next address should be returned in 'a0'.
	 */
	add	a0, zero, zero
	ret
ENDPROC(fw_options)

#ifdef FW_PAYLOAD_FDT_PATH
	.align 4
	.pushsection .text, "ax", %progbits
	.globl fdt_bin
fdt_bin:
	.incbin	FW_PAYLOAD_FDT_PATH
	.popsection
#endif

	.align 4
	.pushsection .payload, "ax", %progbits
	.globl payload_bin
payload_bin:
#ifndef FW_PAYLOAD_PATH
	wfi
	j	payload_bin
#else
	.incbin	FW_PAYLOAD_PATH
#endif
	.popsection
