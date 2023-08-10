/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

static unsigned long ecall_impid = SBI_OPENSBI_IMPID;

uint16_t sbi_ecall_version_major(void)
{
	return SBI_ECALL_VERSION_MAJOR;
}

uint16_t sbi_ecall_version_minor(void)
{
	return SBI_ECALL_VERSION_MINOR;
}

unsigned long sbi_ecall_get_impid(void)
{
	return ecall_impid;
}

void sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

static LIST_HEAD(ecall_exts_list);

struct sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	struct sbi_ecall_extension *t, *ret = NULL;

	list_for_each_entry(struct sbi_ecall_extension, t, &ecall_exts_list, head) {
		if (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			break;
		}
	}

	return ret;
}

int sbi_ecall_register_extension(struct sbi_ecall_extension *ext)
{
	struct sbi_ecall_extension *t;

	if (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		return SBI_EINVAL;

	list_for_each_entry(struct sbi_ecall_extension, t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		if (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		else
			return SBI_EINVAL;
	}

	INIT_LIST_HEAD(&ext->head);
	list_add_tail(&ext->head, &ecall_exts_list);

	return 0;
}

void sbi_ecall_unregister_extension(struct sbi_ecall_extension *ext)
{
	bool found = false;
	struct sbi_ecall_extension *t;

	if (!ext)
		return;

	list_for_each_entry(struct sbi_ecall_extension, t, &ecall_exts_list, head) {
		if (t == ext) {
			found = true;
			break;
		}
	}

	if (found)
		list_del_init(&ext->head);
}

int sbi_ecall_handler(struct pt_regs *regs)
{
	int ret = 0;
	struct sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	struct csr_trap_info trap = {0};
	unsigned long out_val = 0;
	bool is_0_1_spec = 0;
	unsigned long mcause = csr_read(CSR_MCAUSE);
	unsigned long mtval = csr_read(CSR_MTVAL);
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ext = sbi_ecall_find_extension(extension_id);
	if (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id,
				  regs, &out_val, &trap);
		if (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} else {
		ret = SBI_ENOTSUPP;
	}

	if (ret == SBI_ETRAP) {
		trap.epc = regs->epc;
		sbi_trap_redirect(regs, scratch, regs->epc, mcause, mtval);
	} else {
		if (ret < SBI_LAST_ERR ||
		    (extension_id != SBI_EXT_0_1_CONSOLE_GETCHAR &&
		     SBI_SUCCESS < ret)) {
			sbi_printf("%s: Invalid error %d for ext=0x%lx "
				   "func=0x%lx\n", __func__, ret,
				   extension_id, func_id);
			ret = SBI_ERR_FAILED;
		}

		/*
		 * This function should return non-zero value only in case of
		 * fatal error. However, there is no good way to distinguish
		 * between a fatal and non-fatal errors yet. That's why we treat
		 * every return value except ETRAP as non-fatal and just return
		 * accordingly for now. Once fatal errors are defined, that
		 * case should be handled differently.
		 */
		regs->epc += 4;
		regs->a0 = ret;
		if (!is_0_1_spec)
			regs->a1 = out_val;
	}

	return 0;
}

#define foreach_ecall(ecallp)		\
	for (ecallp = __sbi_ecall_start; ecallp < __sbi_ecall_end; ecallp++)

int sbi_ecall_init(void)
{
	int ret;
	struct sbi_ecall_extension *ext;

	foreach_ecall(ext) {
		ret = SBI_ENODEV;

		if (ext->register_extensions) {
			sbi_printf("Registering ecall %s\n", ext->name);
			ret = ext->register_extensions();
		}
		if (ret)
			return ret;
	}

	return 0;
}
