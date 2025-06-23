/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <target/sbi.h>
#include <target/console.h>

static int sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  struct pt_regs *regs,
				  unsigned long *out_val,
				  struct csr_trap_info *out)
{
	// ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
	// 		MSTATUS_MPP_SHIFT;
	//sbi_printf("Entering sbi_ecall_dbcn_handler funcid=%d\n", funcid);
	switch (funcid) {
	case SBI_EXT_DBCN_CONSOLE_WRITE:
	case SBI_EXT_DBCN_CONSOLE_READ:
		/*
		 * On RV32, the M-mode can only access the first 4GB of
		 * the physical address space because M-mode does not have
		 * MMU to access full 34-bit physical address space.
		 *
		 * Based on above, we simply fail if the upper 32bits of
		 * the physical address (i.e. a2 register) is non-zero on
		 * RV32.
		 *
		 * Analogously, we fail if the upper 64bit of the
		 * physical address (i.e. a2 register) is non-zero on
		 * RV64.
		 */
		if (regs->a2)
			return SBI_ERR_FAILED;

		// if (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
		// 			regs->a1, regs->a0, smode,
		// 			SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
		// 	return SBI_ERR_INVALID_PARAM;
		sbi_hart_map_saddr(regs->a1, regs->a0);
		if (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			*out_val = puts((const char *)regs->a1);
		else
			*out_val = gets((char *)regs->a1, regs->a0);
		sbi_hart_unmap_saddr();
		return 0;
	case SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		return 0;
	default:
		break;
	}

	return SBI_ENOTSUPP;
}

DEFINE_SBI_ECALL(dbcn, SBI_EXT_DBCN, SBI_EXT_DBCN,
		 NULL, sbi_ecall_dbcn_handler);
