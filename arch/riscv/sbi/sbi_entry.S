/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)sbi_entry.S: SBI mode entry point
 * $Id: sbi_entry.S,v 1.0 2020-02-11 12:58:00 zhenglv Exp $
 */

#include <target/init.h>
#include <target/sbi.h>
#include <asm/asm-offsets.h>

	.macro MOV_3R __d0, __s0, __d1, __s1, __d2, __s2
	add	\__d0, \__s0, zero
	add	\__d1, \__s1, zero
	add	\__d2, \__s2, zero
	.endm

	.macro MOV_5R __d0, __s0, __d1, __s1, __d2, __s2, __d3, __s3, __d4, __s4
	add	\__d0, \__s0, zero
	add	\__d1, \__s1, zero
	add	\__d2, \__s2, zero
	add	\__d3, \__s3, zero
	add	\__d4, \__s4, zero
	.endm

	.macro init_tp hartid, reg_tmp
#ifdef CONFIG_SMP
	slli	\reg_tmp, \hartid, PERCPU_STACK_SHIFT
	la	tp, (SBI_PERCPU_STACKS_START + PERCPU_STACK_SIZE)
	add	tp, tp, \reg_tmp
#else
	la	tp, SBI_PERCPU_STACKS_END
#endif
	li	\reg_tmp, SBI_SCRATCH_SIZE
	sub	tp, tp, \reg_tmp
	.endm

	.macro init_gpsptp hartid, reg_tmp
	init_gp
#ifdef CONFIG_SMP
	slli	\reg_tmp, \hartid, PERCPU_STACK_SHIFT
	la	sp, (SBI_PERCPU_STACKS_START + PERCPU_STACK_SIZE)
	add	sp, sp, \reg_tmp
#else
	la	sp, SBI_PERCPU_STACKS_END
#endif
	li	\reg_tmp, SBI_SCRATCH_SIZE
	sub	tp, sp, \reg_tmp
	add	sp, tp, zero
	.endm

	.macro get_tp hartid, reg_tmp, reg_res
#ifdef CONFIG_SMP
	slli	\reg_tmp, \hartid, PERCPU_STACK_SHIFT
	la	\reg_res, (SBI_PERCPU_STACKS_START + PERCPU_STACK_SIZE)
	add	\reg_res, \reg_res, \reg_tmp
#else
	la	\reg_res, SBI_PERCPU_STACKS_END
#endif
	li	\reg_tmp, SBI_SCRATCH_SIZE
	sub	\reg_res, \reg_res, \reg_tmp
	.endm

	.macro get_smpid reg
#ifdef CONFIG_SMP
	csrr	\reg, CSR_MHARTID
	get_arch_smpid \reg
#else
	li	\reg, 0
#endif
	.endm

	.macro get_hartid reg
	csrr	\reg, CSR_MHARTID
	.endm

	__HEAD

ENTRY(__sbi_entry)
	/* s6 -> HART ID */
	get_hartid s6

	/* Jump to warm boot if it is not the first core booting. */
	li	s7, BOOT_HART
	bne	s6, s7, secondary_wait_for_sbi_relocate

	/* Allow main firmware to save info */
	MOV_5R	s0, a0, s1, a1, s2, a2, s3, a3, s4, a4
	call	fw_save_info
	MOV_5R	a0, s0, a1, s1, a2, s2, a3, s3, a4, s4

	/* s7 -> SMP Count */
	li	s7, MAX_HARTS
	/* Keep a copy of tp */
	la	t3, SBI_PERCPU_STACKS_END
	/* Counter */
	li	t2, 1
	/* hartid 0 */
	li	t1, 0
_scratch_init:
	init_tp	t1, a5

	/* Initialize scratch space */
	/* Store fw_start and fw_size in scratch space */
	la	a4, _start
	la	a5, _end
	REG_S	a4, SBI_SCRATCH_FW_START(tp)
	REG_S	a5, SBI_SCRATCH_FW_SIZE(tp)
	/* Store next arg1 in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_next_arg1
	REG_S	a0, SBI_SCRATCH_NEXT_ARG1(tp)
	MOV_3R	a0, s0, a1, s1, a2, s2
	/* Store next address in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_next_addr
	REG_S	a0, SBI_SCRATCH_NEXT_ADDR(tp)
	MOV_3R	a0, s0, a1, s1, a2, s2
	/* Store next mode in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
	call	fw_next_mode
	REG_S	a0, SBI_SCRATCH_NEXT_MODE(tp)
	MOV_3R	a0, s0, a1, s1, a2, s2
	/* Store warm_boot address in scratch space */
	la	a4, __sbi_entry_warm
	REG_S	a4, SBI_SCRATCH_WARMBOOT_ADDR(tp)
	/* Store platform address in scratch space */
	la	a4, platform
	REG_S	a4, SBI_SCRATCH_PLATFORM_ADDR(tp)
	/* Store hartid-to-scratch function address in scratch space */
	la	a4, _hartid_to_scratch
	REG_S	a4, SBI_SCRATCH_HARTID_TO_SCRATCH(tp)
	/* Clear tmp0 in scratch space */
	REG_S	zero, SBI_SCRATCH_TMP0(tp)
	/* Store firmware options in scratch space */
	MOV_3R	s0, a0, s1, a1, s2, a2
#ifdef FW_OPTIONS
	li	a4, FW_OPTIONS
#else
	add	a4, zero, zero
#endif
	call	fw_options
	or	a4, a4, a0
	REG_S	a4, SBI_SCRATCH_OPTIONS(tp)
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

#ifdef CONFIG_FDT
	/* Relocate Flatened Device Tree (FDT)
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
#endif

	la	t0, sbi_relocate_done
	REG_S	t0, 0(t0)
	fence	rw, rw

	j	__sbi_entry_warm

	/* waitting for boot hart done */
secondary_wait_for_sbi_relocate:
	la	t0, sbi_relocate_done
	REG_L	t1, 0(t0)
	/* Reduce the bus traffic so that boot hart may proceed faster */
	nop
	nop
	nop
	bne	t0, t1, secondary_wait_for_sbi_relocate

__sbi_entry_warm:
	/* Init GP, SP, TP */
	init_gpsptp s6, a5
	/* update the mscratch */
	csrw	CSR_MSCRATCH, tp

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

ENTRY(_hartid_to_scratch)
	add	sp, sp, -(2 * __SIZEOF_POINTER__)
	REG_S	s0, (sp)
	REG_S	s1, (__SIZEOF_POINTER__)(sp)

	/* a0 -> HART ID (passed by caller)
	 * s0 -> Temporary
	 * s1 -> HART Stack End
	 */
	get_tp	a0, s0, s1
	sub	a0, s1, zero

	REG_L	s0, (sp)
	REG_L	s1, (__SIZEOF_POINTER__)(sp)
	add	sp, sp, (2 * __SIZEOF_POINTER__)
	ret
ENDPROC(_hartid_to_scratch)

ENTRY(_trap_handler)
	/* Swap TP and MSCRATCH */
	csrrw	tp, CSR_MSCRATCH, tp

	/* Save T0 in scratch space */
	REG_S	t0, SBI_SCRATCH_TMP0(tp)

	/* Check which mode we came from */
	csrr	t0, CSR_MSTATUS
	srl	t0, t0, SR_MPP_SHIFT
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
	REG_L	t0, SBI_SCRATCH_TMP0(tp)

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
#ifdef CONFIG_RISCV_EXIT_M
	li	a0, PRV_M
#endif
#ifdef CONFIG_RISCV_EXIT_S
	li	a0, PRV_S
#endif
#ifdef CONFIG_RISCV_EXIT_U
	li	a0, PRV_U
#endif
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

	.pushsection .data
	.align 3
sbi_relocate_done:
	RISCV_PTR	0
	.popsection
