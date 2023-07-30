/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/init.h>
#include <target/arch.h>
#include <asm/asm-offsets.h>

	/*
	 * We assume that faulting instruction is 4-byte long and blindly
	 * increment SEPC by 4.
	 *
	 * The trap info will be saved as follows:
	 * A3 <- pointer struct csr_trap_info
	 * A4 <- temporary
	 */

ENTRY(__csr_expected_trap)
	/* Without H-extension so, MTVAL2 and MTINST CSRs and GVA not available */
	csrr	a4, CSR_MEPC
	REG_S	a4, CSR_TRAP_INFO_EPC(a3)
	csrr	a4, CSR_MCAUSE
	REG_S	a4, CSR_TRAP_INFO_CAUSE(a3)
	csrr	a4, CSR_MTVAL
	REG_S	a4, CSR_TRAP_INFO_TVAL(a3)
	REG_S	zero, CSR_TRAP_INFO_TVAL2(a3)
	REG_S	zero, CSR_TRAP_INFO_TINST(a3)
	REG_S	zero, CSR_TRAP_INFO_GVA(a3)
	csrr	a4, CSR_MEPC
	addi	a4, a4, 4
	csrw	CSR_MEPC, a4
	mret
ENDPROC(__csr_expected_trap)

ENTRY(__csr_expected_trap_hext)
	/* With H-extension so, MTVAL2 and MTINST CSRs and GVA available */
	csrr	a4, CSR_MEPC
	REG_S	a4, CSR_TRAP_INFO_EPC(a3)
	csrr	a4, CSR_MCAUSE
	REG_S	a4, CSR_TRAP_INFO_CAUSE(a3)
	csrr	a4, CSR_MTVAL
	REG_S	a4, CSR_TRAP_INFO_TVAL(a3)
	csrr	a4, CSR_MTVAL2
	REG_S	a4, CSR_TRAP_INFO_TVAL2(a3)
	csrr	a4, CSR_MTINST
	REG_S	a4, CSR_TRAP_INFO_TINST(a3)

	/* Extract GVA bit from MSTATUS or MSTATUSH */
#if __riscv_xlen == 32
	csrr	a4, CSR_MSTATUSH
#else
	csrr	a4, CSR_MSTATUS
#endif
	srli	a4, a4, SR_GVA_SHIFT
	andi	a4, a4, 1
	REG_S	a4, CSR_TRAP_INFO_GVA(a3)

	csrr	a4, CSR_MEPC
	addi	a4, a4, 4
	csrw	CSR_MEPC, a4
	mret
ENDPROC(__csr_expected_trap_hext)
