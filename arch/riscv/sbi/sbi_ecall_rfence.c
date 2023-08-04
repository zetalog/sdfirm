/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <target/sbi.h>

static int sbi_ecall_rfence_handler(unsigned long extid, unsigned long funcid,
				    struct pt_regs *regs,
				    unsigned long *out_val,
				    struct csr_trap_info *out_trap)
{
	int ret = 0;
	struct sbi_tlb_info tlb_info;
	uint32_t source_hart = sbi_current_hartid();
	__unused unsigned long vmid;

	if (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		if (!misa_extension('H'))
			return SBI_ENOTSUPP;

	switch (funcid) {
	case SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_TYPE(&tlb_info, 0, 0, 0, 0,
				  SBI_ITLB_FLUSH, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		break;
	case SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_TYPE(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_FLUSH_VMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		break;
	case SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_TYPE(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_FLUSH_VMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		break;
#ifdef CONFIG_RSICV_H
	case SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_TYPE(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_FLUSH_GVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		break;
	case SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_TYPE(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_FLUSH_GVMA_VMID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		break;
	case SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & ATP_VMID);
		vmid = vmid >> ATP_VMID_SHIFT;
		SBI_TLB_INFO_TYPE(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_FLUSH_VVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		break;
	case SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & ATP_VMID);
		vmid = vmid >> ATP_VMID_SHIFT;
		SBI_TLB_INFO_TYPE(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_FLUSH_VVMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		break;
#endif
	default:
		ret = SBI_ENOTSUPP;
	}

	return ret;
}

DEFINE_SBI_ECALL(rfence, SBI_EXT_RFENCE, SBI_EXT_RFENCE,
		 NULL, sbi_ecall_rfence_handler);
