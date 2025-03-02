/* SPDX-License-Identifier: BSD-2-Clause
 *
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#include <target/fdt.h>
#include <target/sbi.h>
#include <sbi_utils/fdt/fdt_fixup.h>

#ifdef CONFIG_FDT_FIXUP_RESIZE
#define FDT_FIXUP_SIZE			(6 * NR_CPUS) /* 6 = disabled - ok */
#define FDT_FIXUP_STATUS_UNAVAILABLE	"disabled"
#else
#define FDT_FIXUP_SIZE			0
#define FDT_FIXUP_STATUS_UNAVAILABLE	"ng"
#endif

int fdt_add_cpu_idle_states(void *fdt, const struct sbi_cpu_idle_state *state)
{
	int cpu_node, cpus_node, err, idle_states_node;
	uint32_t count, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	if (err < 0)
		return err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	if (err < 0)
		return err;

	cpus_node = fdt_path_offset(fdt, "/cpus");
	if (cpus_node < 0)
		return cpus_node;

	/* Do nothing if the idle-states node already exists. */
	idle_states_node = fdt_subnode_offset(fdt, cpus_node, "idle-states");
	if (idle_states_node >= 0)
		return 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_node = fdt_add_subnode(fdt, cpus_node, "idle-states");
	if (idle_states_node < 0)
		return idle_states_node;

	for (count = 0; state->name; count++, phandle++, state++) {
		int idle_state_node;

		idle_state_node = fdt_add_subnode(fdt, idle_states_node,
						  state->name);
		if (idle_state_node < 0)
			return idle_state_node;

		fdt_setprop_string(fdt, idle_state_node, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_node,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		if (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_node,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_node, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "min-residency-us",
				state->min_residency_us);
		if (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_node,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "phandle", phandle);
	}

	if (count == 0)
		return 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_node, fdt, cpus_node) {
		const char *device_type;
		fdt32_t *value;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_node, "device_type", NULL);
		if (!device_type || strcmp(device_type, "cpu"))
			continue;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_node, "cpu-idle-states",
					      count * sizeof(phandle),
					      (void **)&value);
		if (err < 0)
			return err;

		/* Fill in the phandles of the idle state nodes. */
		for (uint32_t i = 0; i < count; ++i)
			value[i] = cpu_to_fdt32(phandle - count + i);
	}

	return 0;
}

static bool fdt_cpu_disabled(void *fdt, int cpu_offset)
{
	const char *status;
	int len;

	status = fdt_getprop(fdt, cpu_offset, "status", &len);
	if (!status)
		return false;
	if (len > 0) {
		if (!strcmp(status, "okay") || !strcmp(status, "ok"))
			return false;
	}
	return true;
}

void fdt_cpu_fixup(void *fdt)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	int err, cpu_offset, cpus_offset;
	uint32_t hartid;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + FDT_FIXUP_SIZE);
	if (err < 0)
		return;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	if (cpus_offset < 0)
		return;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		if (err)
			continue;

		if (sbi_platform_hart_disabled(plat, hartid))
			fdt_setprop_string(fdt, cpu_offset, "status",
					   FDT_FIXUP_STATUS_UNAVAILABLE);
	}
}

void fdt_irqs_fixup(void *fdt, const char *compat, int num)
{
	uint32_t *cells;
	int i, cells_count;
	int irqc_offset, cpu_offset, irq_offset;
	uint32_t phandle;

	irqc_offset = 0;
	while (irqc_offset >= 0 && num) {
		irqc_offset = fdt_node_offset_by_compatible(fdt, irqc_offset, compat);
		if (irqc_offset < 0)
			return;

		cells = (uint32_t *)fdt_getprop(fdt, irqc_offset,
						"interrupts-extended",
						&cells_count);
		if (!cells)
			return;

		cells_count = cells_count / sizeof(uint32_t);
		if (!cells_count)
			return;

		for (i = 0; i < (cells_count / 2); i++) {
			switch (fdt32_to_cpu(cells[2 * i + 1])) {
			case IRQ_M_SOFT:
			case IRQ_M_TIMER:
			case IRQ_M_EXT:
				cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
				break;
			default:
				phandle = fdt32_to_cpu(cells[2 * i]);
				irq_offset = fdt_node_offset_by_phandle(fdt, phandle);
				cpu_offset = fdt_parent_offset(fdt, irq_offset);
				if (fdt_cpu_disabled(fdt, cpu_offset))
					cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
			}
		}
		num--;
	}
}

void fdt_irq_fixup(void *fdt, const char *compat)
{
	fdt_irqs_fixup(fdt, compat, 1);
}

static int fdt_resv_memory_update_node(void *fdt, unsigned long addr,
				       unsigned long size, int index,
				       int parent, bool no_map)
{
	int na = fdt_address_cells(fdt, 0);
	int ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_high, addr_low;
	fdt32_t size_high, size_low;
	int subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	char name[32];

	addr_high = (uint64_t)addr >> 32;
	addr_low = addr;
	size_high = (uint64_t)size >> 32;
	size_low = size;

	if (na > 1 && addr_high)
		snprintf(name, sizeof(name), "mmode_pmp%d@%x,%x", index,
			 addr_high, addr_low);
	else
		snprintf(name, sizeof(name), "mmode_pmp%d@%x", index,
			 addr_low);
	subnode = fdt_add_subnode(fdt, parent, name);
	if (subnode < 0)
		return subnode;

	if (no_map) {
		/* Tell operating system not to create a virtual mapping of
		 * the region as part of its standard mapping of system
		 * memory.
		 */
		err = fdt_setprop_empty(fdt, subnode, "no-map");
		if (err < 0)
			return err;
	}

	/* encode the <reg> property value */
	val = reg;
	if (na > 1)
		*val++ = cpu_to_fdt32(addr_high);
	*val++ = cpu_to_fdt32(addr_low);
	if (ns > 1)
		*val++ = cpu_to_fdt32(size_high);
	*val++ = cpu_to_fdt32(size_low);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * sizeof(fdt32_t));
	if (err < 0)
		return err;

	return 0;
}

#ifdef CONFIG_RISCV_PMP
static int fdt_fixup_pmp(void *fdt, int parent)
{
	int err;
	unsigned long log2len;
	unsigned long prot, size;
	phys_addr_t addr;
	int i, j;

	for (i = 0, j = 0; i < PMP_COUNT; i++) {
		err = pmp_get(i, &prot, &addr, &log2len);
		if (err)
			continue;
		if (log2len < __riscv_xlen)
			size = 1UL << log2len;
		else
			size = 0;
		if (!(prot & PMP_A))
			continue;
		if (prot & (PMP_R | PMP_W | PMP_X))
			continue;

		fdt_resv_memory_update_node(fdt, addr, size, j, parent, false);
		j++;
	}
	return 0;
}
#else
static int fdt_fixup_pmp(void *fdt, int parent)
{
	return 0;
}
#endif

static int fdt_fixup_fw(void *fdt, int parent)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	unsigned long size;
	phys_addr_t addr;

	/* Update the DT with firmware start & size even if PMP is
	 * not supported. This makes sure that supervisor OS is
	 * always aware of SBI resident memory area.
	 */
	addr = scratch->fw_start & ~(scratch->fw_size - 1UL);
	size = (1UL << __ilog2_u64(__roundup64(scratch->fw_size)));
	return fdt_resv_memory_update_node(fdt, addr, size, 0, parent, true);
}

/* PMP is used to protect SBI firmware to safe-guard it from buggy S-mode
 * software, see pmp_init(). The protected memory region information needs
 * to be conveyed to S-mode software (e.g.: operating system) via some
 * well-known method.
 *
 * With device tree, this can be done by inserting a child node of the
 * reserved memory node which is used to specify one or more regions of
 * reserved memory.
 *
 * For the reserved memory node bindings, see Linux kernel documentation at
 * Documentation/devicetree/bindings/reserved-memory/reserved-memory.txt
 *
 * Some additional memory spaces may be protected by platform codes via PMP
 * as well, and corresponding child nodes will be inserted.
 */
int fdt_reserved_memory_fixup(void *fdt)
{
	int parent;
	int err;
	int na = fdt_address_cells(fdt, 0);
	int ns = fdt_size_cells(fdt, 0);

	/* Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	if (err < 0)
		return err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	if (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		if (parent < 0)
			return parent;

		/* reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */
		err = fdt_setprop_empty(fdt, parent, "ranges");
		if (err < 0)
			return err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		if (err < 0)
			return err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		if (err < 0)
			return err;
	}

	/* Assumes the given device tree does not contain any memory region
	 * child node protected by PMP. Normally PMP programming happens at
	 * M-mode firmware. The memory space used by SBI is protected.
	 * Some additional memory spaces may be protected by platform codes.
	 *
	 * With above assumption, we create child nodes directly.
	 */
	fdt_fixup_fw(fdt, parent);
	return fdt_fixup_pmp(fdt, parent);
}

int fdt_reserved_memory_nomap_fixup(void *fdt)
{
	int parent, subnode;
	int err;

	/* Locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	if (parent < 0)
		return parent;

	fdt_for_each_subnode(subnode, fdt, parent) {
		/* Tell operating system not to create a virtual mapping of
		 * the region as part of its standard mapping of system
		 * memory.
		 */
		err = fdt_setprop_empty(fdt, subnode, "no-map");
		if (err < 0)
			return err;
	}

	return 0;
}

static SBI_LIST_HEAD(fixup_list);

int fdt_register_general_fixup(struct fdt_general_fixup *fixup)
{
	struct fdt_general_fixup *f;

	if (!fixup || !fixup->name || !fixup->do_fixup)
		return SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		if (f == fixup)
			return SBI_EALREADY;
	}

	SBI_INIT_LIST_HEAD(&fixup->head);
	sbi_list_add_tail(&fixup->head, &fixup_list);

	return 0;
}

void fdt_unregister_general_fixup(struct fdt_general_fixup *fixup)
{
	if (!fixup)
		return;

	sbi_list_del(&fixup->head);
}

void fdt_fixups(void *fdt)
{
	fdt_reserved_memory_fixup(fdt);
	fdt_pmu_fixup(fdt);
}
