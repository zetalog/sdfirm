/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

uint16_t sbi_ecall_version_major(void)
{
	return SBI_ECALL_VERSION_MAJOR;
}

uint16_t sbi_ecall_version_minor(void)
{
	return SBI_ECALL_VERSION_MINOR;
}

#ifdef CONFIG_SBI_V20

static unsigned long ecall_impid = SBI_OPENSBI_IMPID;
unsigned long sbi_ecall_get_impid(void)
{
	return ecall_impid;
}

void sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}
#endif

static int sbi_load_hart_mask_unpriv(struct sbi_scratch *scratch,
				     struct unpriv_trap *uptrap,
				     ulong *pmask, ulong *hmask)
{
	ulong mask = sbi_hart_available_mask();

	if (pmask) {
		mask = load_ulong(pmask, scratch, uptrap);
		if (uptrap->cause)
			return ETRAP;
	}
	*hmask = mask;
	return 0;
}

static int sbi_ecall_legacy_handler(unsigned long extid, unsigned long funcid,
				    struct pt_regs *regs,
				    unsigned long *out_val,
				    struct csr_trap_info *out_trap)
{
	int ret = 0;
	__unused struct sbi_tlb_info tlb_info;
	uint32_t source_hart = sbi_current_hartid();
	ulong hmask = 0;
	struct unpriv_trap uptrap;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	switch (extid) {
	case SBI_ECALL_SET_TIMER:
		sbi_trap_log("%d: ECALL_SET_TIMER\n", source_hart);
#if __riscv_xlen == 32
		sbi_timer_event_start(scratch,
				      (((uint64_t)regs->a1 << 32) |
				        (uint64_t)regs->a0));
#else
		sbi_timer_event_start(scratch, (uint64_t)regs->a0);
#endif
		ret = 0;
		break;
	case SBI_ECALL_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		ret = 0;
		break;
	case SBI_ECALL_CONSOLE_GETCHAR:
#ifdef CONFIG_SBI_V20
		ret = sbi_getc();
#else
		regs->a0 = sbi_getc();
		ret = 0;
#endif
		break;
	case SBI_ECALL_CLEAR_IPI:
		sbi_trap_log("%d: ECALL_CLEAR_IPI\n", source_hart);
		sbi_ipi_clear_smode(scratch);
		ret = 0;
		break;
	case SBI_ECALL_SEND_IPI:
		sbi_trap_log("%d: ECALL_SEND_IPI\n", source_hart);
		ret = sbi_load_hart_mask_unpriv(scratch, &uptrap,
						(ulong *)regs->a0, &hmask);
		if (ret != SBI_ETRAP) {
			ret = sbi_ipi_send_many(hmask, 0,
						SBI_IPI_EVENT_SOFT, NULL);
		}
		break;
	case SBI_ECALL_REMOTE_FENCE_I:
		sbi_trap_log("%d: ECALL_REMOTE_FENCE_I\n", source_hart);
		ret = sbi_load_hart_mask_unpriv(scratch, &uptrap,
						(ulong *)regs->a0, &hmask);
		if (ret != SBI_ETRAP) {
			SBI_TLB_INFO_TYPE(&tlb_info, 0, 0, 0, 0,
					  SBI_ITLB_FLUSH, source_hart);
			ret = sbi_tlb_request(hmask, 0, &tlb_info);
		}
		break;
	case SBI_ECALL_REMOTE_SFENCE_VMA:
		sbi_trap_log("%d: ECALL_REMOTE_SFENCE_VMA\n", source_hart);
		ret = sbi_load_hart_mask_unpriv(scratch, &uptrap,
						(ulong *)regs->a0, &hmask);
		if (ret != SBI_ETRAP) {
			SBI_TLB_INFO_TYPE(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_FLUSH_VMA, source_hart);
			ret = sbi_tlb_request(hmask, 0, &tlb_info);
		}
		break;
	case SBI_ECALL_REMOTE_SFENCE_VMA_ASID:
		sbi_trap_log("%d: ECALL_REMOTE_SFENCE_VMA_ASID\n", source_hart);
		ret = sbi_load_hart_mask_unpriv(scratch, &uptrap,
						(ulong *)regs->a0, &hmask);
		if (ret != SBI_ETRAP) {
			SBI_TLB_INFO_TYPE(&tlb_info, regs->a1, regs->a2, regs->a3, 0,
					  SBI_TLB_FLUSH_VMA_ASID, source_hart);
			ret = sbi_tlb_request(hmask, 0, &tlb_info);
		}
		break;
	case SBI_ECALL_SHUTDOWN:
		sbi_trap_log("%d: ECALL_SHUTDOWN\n", source_hart);
		sbi_system_shutdown(scratch, 0);
		ret = 0;
		break;
	case SBI_ECALL_FINISH:
		sbi_trap_log("%d: ECALL_FINISH\n", source_hart);
		sbi_system_finish(scratch, regs->a0);
		ret = 0;
		break;
	case SBI_ECALL_ENABLE_LOG:
		sbi_enable_trap_log();
		ret = 0;
		break;
	case SBI_ECALL_DISABLE_LOG:
		sbi_disable_trap_log();
		ret = 0;
		break;
	case SBI_ECALL_GET_CLK_FREQ:
		sbi_trap_log("%d: ECALL_GET_CLK_FREQ\n", source_hart);
		regs->a0 = sbi_clock_get_freq(regs->a0);
		ret = 0;
		break;
	case SBI_ECALL_SET_CLK_FREQ:
		sbi_trap_log("%d: ECALL_SET_CLK_FREQ\n", source_hart);
		sbi_clock_set_freq(regs->a0, regs->a1);
		ret = 0;
		break;
	case SBI_ECALL_ENABLE_CLK:
		sbi_trap_log("%d: ECALL_ENABLE_CLK\n", source_hart);
		sbi_clock_enable(regs->a0);
		ret = 0;
		break;
	case SBI_ECALL_DISABLE_CLK:
		sbi_trap_log("%d: ECALL_DISABLE_CLK\n", source_hart);
		sbi_clock_disable(regs->a0);
		ret = 0;
		break;
	case SBI_ECALL_CONFIG_PIN_MUX:
		sbi_trap_log("%d: ECALL_CONFIG_PIN_MUX\n", source_hart);
		sbi_pin_config_mux(regs->a0, regs->a1);
		ret = 0;
		break;
	case SBI_ECALL_CONFIG_PIN_PAD:
		sbi_trap_log("%d: ECALL_CONFIG_PIN_PAD\n", source_hart);
		sbi_pin_config_pad(regs->a0, regs->a1);
		ret = 0;
		break;
	default:
		regs->a0 = -ENOTSUP;
		ret	 = 0;
		break;
	};

	return ret;
}

#ifdef CONFIG_SBI_V20
DEFINE_SBI_ECALL(legacy,
		 SBI_EXT_0_1_SET_TIMER, SBI_EXT_0_1_SHUTDOWN,
		 NULL, sbi_ecall_legacy_handler);
#else
int sbi_ecall_handler(struct pt_regs *regs)
{
	unsigned long out_val = 0;
	struct csr_trap_info trap = {0};
	struct unpriv_trap uptrap;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	int ret;

	ret = sbi_ecall_legacy_handler(regs->a7, regs->a6, regs,
				       &out_val, &trap);

	if (!ret) {
		regs->epc += 4;
	} else if (ret == ETRAP) {
		ret = 0;
		sbi_trap_redirect(regs, scratch, regs->epc,
				  uptrap.cause, uptrap.tval);
	}

	return ret;
}

int sbi_ecall_init(void)
{
	return 0;
}
#endif
