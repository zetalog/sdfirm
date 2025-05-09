/* SPDX-License-Identifier: BSD-2-Clause
 *
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#include <target/fdt.h>
#include <target/sbi.h>
#include <sbi/sbi_bitops.h>
#include <target/irq.h>

#define DEFAULT_UART_FREQ		0
#define DEFAULT_UART_BAUD		115200
#define DEFAULT_UART_REG_SHIFT		0
#define DEFAULT_UART_REG_IO_WIDTH	1

#define DEFAULT_SIFIVE_UART_FREQ		0
#define DEFAULT_SIFIVE_UART_BAUD		115200
#define DEFAULT_SIFIVE_UART_REG_SHIFT		0
#define DEFAULT_SIFIVE_UART_REG_IO_WIDTH	4

#define DEFAULT_SHAKTI_UART_FREQ		50000000
#define DEFAULT_SHAKTI_UART_BAUD		115200

const struct fdt_match *fdt_match_node(void *fdt, int nodeoff,
				       const struct fdt_match *match_table)
{
	int ret;

	if (!fdt || nodeoff < 0 || !match_table)
		return NULL;

	while (match_table->compatible) {
		ret = fdt_node_check_compatible(fdt, nodeoff,
						match_table->compatible);
		if (!ret)
			return match_table;
		match_table++;
	}

	return NULL;
}

int fdt_find_match(void *fdt, int startoff,
		   const struct fdt_match *match_table,
		   const struct fdt_match **out_match)
{
	int nodeoff;

	if (!fdt || !match_table)
		return -ENODEV;

	while (match_table->compatible) {
		nodeoff = fdt_node_offset_by_compatible(fdt, startoff,
						match_table->compatible);
		if (nodeoff >= 0) {
			if (out_match)
				*out_match = match_table;
			return nodeoff;
		}
		match_table++;
	}

	return -ENODEV;
}

int fdt_parse_phandle_with_args(const void *fdt, int nodeoff,
				const char *prop, const char *cells_prop,
				int index, struct fdt_phandle_args *out_args)
{
	u32 i, pcells;
	int len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	if (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		return SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	if (!list)
		return SBI_ENOENT;
	list_end = list + (len / sizeof(*list));

	while (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		if (pnodeoff < 0)
			return pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		if (!val)
			return SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		if (FDT_MAX_PHANDLE_ARGS < pcells)
			return SBI_EINVAL;
		if (list + pcells > list_end)
			return SBI_ENOENT;

		if (index > 0) {
			list += pcells;
			index--;
		} else {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			for (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			return 0;
		}
	}

	return SBI_ENOENT;
}

int fdt_get_node_addr_size(const void *fdt, int node, uint64_t *addr,
			   uint64_t *size)
{
	int parent, len, i;
	int cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	unsigned long temp = 0;

	parent = fdt_parent_offset(fdt, node);
	if (parent < 0)
		return parent;
	cell_addr = fdt_address_cells(fdt, parent);
	if (cell_addr < 1)
		return -ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	if (cell_size < 0)
		return -ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	if (!prop_addr)
		return -ENODEV;
	prop_size = prop_addr + cell_addr;

	if (addr) {
		for (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		*addr = temp;
	}
	temp = 0;

	if (size) {
		for (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	return 0;
}

bool fdt_node_is_enabled(const void *fdt, int nodeoff)
{
	int len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	if (!prop)
		return true;

	if (!strncmp(prop, "okay", strlen("okay")))
		return true;

	if (!strncmp(prop, "ok", strlen("ok")))
		return true;

	return false;
}

int fdt_parse_hart_id(void *fdt, int cpu_offset, uint32_t *hartid)
{
	int len;
	const void *prop;
	const fdt32_t *val;

	if (!fdt || cpu_offset < 0)
		return -EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	if (!prop || !len)
		return -EINVAL;
	if (strncmp (prop, "cpu", strlen ("cpu")))
		return -EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	if (!val || len < sizeof(fdt32_t))
		return -EINVAL;

	if (len > sizeof(fdt32_t))
		val++;

	if (hartid)
		*hartid = fdt32_to_cpu(*val);

	return 0;
}

int fdt_parse_max_hart_id(void *fdt, uint32_t *max_hartid)
{
	uint32_t hartid;
	int err, cpu_offset, cpus_offset;

	if (!fdt)
		return -EINVAL;
	if (!max_hartid)
		return 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	if (cpus_offset < 0)
		return cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		if (err)
			continue;

		if (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	return 0;
}

int fdt_parse_shakti_uart_node(void *fdt, int nodeoffset,
			       struct platform_uart_data *uart)
{
	int len, rc;
	const fdt32_t *val;
	unsigned long reg_addr, reg_size;

	if (nodeoffset < 0 || !uart || !fdt)
		return -ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, &reg_addr, &reg_size);
	if (rc < 0 || !reg_addr || !reg_size)
		return -ENODEV;
	uart->addr = reg_addr;

	/* UART address is mandaotry. clock-frequency and current-speed may
	 * not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	if (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	else
		uart->freq = DEFAULT_SHAKTI_UART_FREQ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	if (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	else
		uart->baud = DEFAULT_SHAKTI_UART_BAUD;

	return 0;
}

int fdt_parse_sifive_uart_node(void *fdt, int nodeoffset,
			       struct platform_uart_data *uart)
{
	int len, rc;
	const fdt32_t *val;
	unsigned long reg_addr, reg_size;

	if (nodeoffset < 0 || !uart || !fdt)
		return -ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, &reg_addr, &reg_size);
	if (rc < 0 || !reg_addr || !reg_size)
		return -ENODEV;
	uart->addr = reg_addr;

	/* UART address is mandaotry. clock-frequency and current-speed may
	 * not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	if (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	else
		uart->freq = DEFAULT_SIFIVE_UART_FREQ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	if (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	else
		uart->baud = DEFAULT_SIFIVE_UART_BAUD;

	/* For SiFive UART, the reg-shift and reg-io-width are fixed .*/
	uart->reg_shift = DEFAULT_SIFIVE_UART_REG_SHIFT;
	uart->reg_io_width = DEFAULT_SIFIVE_UART_REG_IO_WIDTH;

	return 0;
}

int fdt_parse_uart8250_node(void *fdt, int nodeoffset,
			    struct platform_uart_data *uart)
{
	int len, rc;
	const fdt32_t *val;
	unsigned long reg_addr, reg_size;

	if (nodeoffset < 0 || !uart || !fdt)
		return -ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, &reg_addr, &reg_size);
	if (rc < 0 || !reg_addr || !reg_size)
		return -ENODEV;
	uart->addr = reg_addr;

	/* UART address is mandaotry. clock-frequency and current-speed may
	 * not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	if (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	else
		uart->freq = DEFAULT_UART_FREQ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	if (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	else
		uart->baud = DEFAULT_UART_BAUD;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	if (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	else
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	if (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	else
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	return 0;
}

int fdt_parse_uart8250(void *fdt, struct platform_uart_data *uart,
		       const char *compatible)
{
	int nodeoffset;

	if (!compatible || !uart || !fdt)
		return -ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	if (nodeoffset < 0)
		return nodeoffset;

	return fdt_parse_uart8250_node(fdt, nodeoffset, uart);
}

int fdt_parse_plic_node(void *fdt, int nodeoffset, struct plic_data *plic)
{
	int len, rc;
	const fdt32_t *val;
	unsigned long reg_addr, reg_size;

	if (nodeoffset < 0 || !plic || !fdt)
		return -ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, &reg_addr, &reg_size);
	if (rc < 0 || !reg_addr || !reg_size)
		return -ENODEV;
	plic->addr = reg_addr;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	if (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	return 0;
}

int fdt_parse_plic(void *fdt, struct plic_data *plic, const char *compat)
{
	int nodeoffset;

	if (!compat || !plic || !fdt)
		return -ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	if (nodeoffset < 0)
		return nodeoffset;

	return fdt_parse_plic_node(fdt, nodeoffset, plic);
}

int fdt_parse_clint_node(void *fdt, int nodeoffset, bool for_timer,
			 struct clint_data *clint)
{
	const fdt32_t *val;
	unsigned long reg_addr, reg_size;
	int i, rc, count, cpu_offset, cpu_intc_offset;
	uint32_t phandle, hwirq, hartid, first_hartid, last_hartid;
	uint32_t match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	if (nodeoffset < 0 || !clint || !fdt)
		return -ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, &reg_addr, &reg_size);
	if (rc < 0 || !reg_addr || !reg_size)
		return -ENODEV;
	clint->addr = reg_addr;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	if (!val || count < sizeof(fdt32_t))
		return -EINVAL;
	count = count / sizeof(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	clint->hart_count = 0;
	for (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		if (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		if (cpu_intc_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		if (rc)
			continue;

		if (SBI_HARTMASK_MAX_BITS <= hartid)
			continue;

		if (match_hwirq == hwirq) {
			if (hartid < first_hartid)
				first_hartid = hartid;
			if (hartid > last_hartid)
				last_hartid = hartid;
			clint->hart_count++;
		}
	}

	if ((last_hartid < first_hartid) || first_hartid == -1U)
		return -ENODEV;

	clint->first_hartid = first_hartid;
	count = last_hartid - first_hartid + 1;
	if (clint->hart_count < count)
		clint->hart_count = count;

	/* TODO: We should figure-out CLINT has_64bit_mmio from DT node */
	clint->has_64bit_mmio = true;

	return 0;
}

#ifdef CONFIG_RISCV_SMAIA
int fdt_parse_imsic_node(void *fdt, int nodeoff, struct imsic_data *imsic)
{
	const fdt32_t *val;
	struct imsic_regs *regs;
	unsigned long reg_addr, reg_size;
	int i, rc, len, nr_parent_irqs;

	if (nodeoff < 0 || !imsic || !fdt)
		return SBI_ENODEV;

	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	if (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		nr_parent_irqs = len / 2;
		for (i = 0; i < len; i += 2) {
			if (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				break;
			}
		}
	} else {
		return SBI_EINVAL;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	if (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	else
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	if (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} else {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		if ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	if (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	else
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	if (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	else
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	if (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	else
		return SBI_EINVAL;

	for (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	for (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff,
					    &reg_addr, &reg_size);
		if (rc < 0 || !reg_addr || !reg_size)
			break;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	if (!imsic->regs[0].size)
		return SBI_EINVAL;

	return 0;
}

int fdt_parse_compat_addr(void *fdt, unsigned long *addr,
			  const char *compatible)
{
	int nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	if (nodeoffset < 0)
		return nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, addr, NULL);
	if (rc < 0 || !addr)
		return -ENODEV;

	return 0;
}

int fdt_parse_aplic_node(void *fdt, int nodeoff, struct aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	struct imsic_data imsic;
	int i, j, d, dcnt, len, noff, rc;
	unsigned long reg_addr, reg_size;
	struct aplic_delegate_data *deleg;

	if (nodeoff < 0 || !aplic || !fdt)
		return SBI_ENODEV;
	memset(aplic, 0, sizeof(*aplic));

	rc = fdt_get_node_addr_size(fdt, nodeoff, &reg_addr, &reg_size);
	if (rc < 0 || !reg_addr || !reg_size)
		return SBI_ENODEV;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	if (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	if (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		for (i = 0; i < len; i += 2) {
			if (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				break;
			}
		}
		aplic->num_idc = len / 2;
		goto aplic_msi_parent_done;
	}

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	if (val && len >= sizeof(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		if (noff < 0)
			return noff;

		rc = fdt_parse_imsic_node(fdt, noff, &imsic);
		if (rc)
			return rc;

		rc = imsic_data_check(&imsic);
		if (rc)
			return rc;

		aplic->targets_mmode = imsic.targets_mmode;

		if (imsic.targets_mmode) {
			aplic->has_msicfg_mmode = true;
			aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
			aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
			aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
			aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
			if (aplic->msicfg_mmode.hhxs <
					(2 * IMSIC_MMIO_PAGE_SHIFT))
				return SBI_EINVAL;
			aplic->msicfg_mmode.hhxs -= 24;
			aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
		} else {
			goto aplic_msi_parent_done;
		}

		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		if (!val || len < sizeof(fdt32_t))
			goto aplic_msi_parent_done;

		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		if (noff < 0)
			return noff;

		val = fdt_getprop(fdt, noff, "msi-parent", &len);
		if (!val || len < sizeof(fdt32_t))
			goto aplic_msi_parent_done;

		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		if (noff < 0)
			return noff;

		rc = fdt_parse_imsic_node(fdt, noff, &imsic);
		if (rc)
			return rc;

		rc = imsic_data_check(&imsic);
		if (rc)
			return rc;

		if (!imsic.targets_mmode) {
			aplic->has_msicfg_smode = true;
			aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
			aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
			aplic->msicfg_smode.hhxw = imsic.group_index_bits;
			aplic->msicfg_smode.hhxs = imsic.group_index_shift;
			if (aplic->msicfg_smode.hhxs <
					(2 * IMSIC_MMIO_PAGE_SHIFT))
				return SBI_EINVAL;
			aplic->msicfg_smode.hhxs -= 24;
			aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
		}
	}
aplic_msi_parent_done:

	for (d = 0; d < APLIC_MAX_DELEGS; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	if (!del || len < (3 * sizeof(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / sizeof(fdt32_t);
	for (i = 0; i < dcnt; i += 3) {
		if (d >= APLIC_MAX_DELEGS)
			break;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		if (!val || len < sizeof(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / sizeof(fdt32_t);
		for (j = 0; j < len; j++) {
			if (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			break;
		}

		if (child_found) {
			d++;
		} else {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	return 0;
}

bool fdt_check_imsic_mlevel(void *fdt)
{
	const fdt32_t *val;
	int i, len, noff = 0;

	if (!fdt)
		return false;

	while ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		if (val && len > sizeof(fdt32_t)) {
			len = len / sizeof(fdt32_t);
			for (i = 0; i < len; i += 2) {
				if (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					return true;
			}
		}
	}

	return false;
}
#endif
