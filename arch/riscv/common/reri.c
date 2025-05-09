/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#include <target/sbi.h>
#include <target/reri.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>
#include <target/ras.h>
#include <asm/mach/ras.h>
#include <asm/mach/reg.h>
#include <target/panic.h>

#define RERI_MODNAME			"reri"

#define HART_PER_CLUSTER		4
#define MAX_CLUSTERS			16

#ifndef RERI_HART_DEV_ADDR
#define RERI_HART_DEV_ADDR		(__ACPU_RAS_SPACE_BASE + ULL(0x4030000))

/* RAS Signal Priority Levels */
#define RAS_SIGNAL_DISABLED		0  /* Signaling is disabled */
#define RAS_SIGNAL_LOW_PRIORITY		1  /* Low-priority RAS signal */
#define RAS_SIGNAL_HIGH_PRIORITY	2  /* High-priority RAS signal */
#define RAS_SIGNAL_PLATFORM		3  /* Platform specific RAS signal */
#endif

#ifndef GHES_ERR_ADDR
#define GHES_ERR_ADDR		(0x4040000UL)
#endif

#ifndef GHES_ERR_SIZE
#define GHES_ERR_SIZE		(0x80000)
#endif

#if __riscv_xlen == 64
#ifdef CONFIG_ARCH_IS_MMIO_32BIT
static uint64_t reri_read(void *dev_addr)
{
	uint32_t hi, lo, tmp;

	do {
		hi = __raw_readl((caddr_t)((uintptr_t)(dev_addr) + 4));
		lo = __raw_readl((caddr_t)dev_addr);
		tmp = __raw_readl((caddr_t)((uintptr_t)(dev_addr) + 4));
	} while (unlikely(hi != tmp));

	return (uint64_t)hi << 32 | lo;
}

static void reri_write(void *dev_addr, uint64_t val)
{
	__raw_writel(HIDWORD(val), (caddr_t)((uintptr_t)(dev_addr) + 4));
	__raw_writel(LODWORD(val), (caddr_t)dev_addr);
}
#else
static uint64_t reri_read(void *dev_addr)
{
	return __raw_readq((caddr_t)dev_addr);
}

static void reri_write(void *dev_addr, uint64_t val)
{
	__raw_writeq(val, (caddr_t)dev_addr);
}
#endif

static inline uint64_t reri_get_status(void *base, int record_idx)
{
	return reri_read(base + RERI_STATUS_I(record_idx));
}

static inline void reri_set_status(void *base, int record_idx, uint64_t val)
{
	reri_write(base + RERI_STATUS_I(record_idx), val);
}

static inline uint64_t reri_get_control(void *base, int record_idx)
{
	return reri_read(base + RERI_CONTROL_I(record_idx));
}

static inline void reri_set_control(void *base, int record_idx, uint64_t val)
{
	reri_write(base + RERI_CONTROL_I(record_idx), val);
}

static void reri_clear_valid_bit(void *control_addr, int record_idx)
{
	uint64_t control;

	control = reri_get_control(control_addr, record_idx);

	control |= RERI_CTRL_SINV_EN;
	control |= RERI_CTRL_SRDP_EN;

	reri_set_control(control_addr, record_idx, control);
}

struct reri_generic_dev {
	uint64_t addr;
	uint64_t size;
	uint32_t sse_vector;
	uint16_t src_id;
	uint16_t res;
};

typedef struct reri_hart_dev {
	struct reri_generic_dev dev;
	int hart_id;
	int cluster_id;    /* Added cluster ID */
	int local_hart_id; /* Added local hart ID within cluster */
} reri_hart_dev_t;

static reri_hart_dev_t *reri_hart_devices = NULL;
static uint32_t reri_nr_harts = 0;

#define RERI_ERR_BANK_SIZE	0x1000

bh_t cpu_reri_bh;

/**
 * Return HART ID of the caller.
 */
unsigned int current_hartid()
{
	return (uint32_t)csr_read(CSR_MHARTID);
}

static int get_cluster_and_local_hart_id(int hart_id, int *cluster_id, int *local_hart_id)
{
	if (hart_id >= NR_CPUS)
		return -1;

	*cluster_id = hart_id / HART_PER_CLUSTER;
	*local_hart_id = hart_id % HART_PER_CLUSTER;

	return 0;
}

static uint64_t get_cluster_error_bank_addr(int cluster_id)
{
	/*
	 * In case all clusters share the same Error bank address space.
	 * The actual access is controlled by the cluster ID of the accessing core.
	 * So we just return the base address.
	 */
	return RERI_HART_DEV_ADDR;
}

static reri_hart_dev_t *get_reri_hart_dev(int hart_id)
{
	int i;

	for (i = 0; i < reri_nr_harts; i++) {
		if (reri_hart_devices[i].hart_id == hart_id) {
			return &reri_hart_devices[i];
		}
	}

	return NULL;
}

static int riscv_reri_get_hart_addr(int hart_id, uint64_t *hart_addr,
				    uint64_t *size)
{
	reri_hart_dev_t *reri_hart;
	int cluster_id, local_hart_id;

	reri_hart = get_reri_hart_dev(hart_id);
	if (!reri_hart)
		return SBI_ENOENT;

	if (get_cluster_and_local_hart_id(hart_id, &cluster_id, &local_hart_id) != 0)
		return SBI_EINVAL;

	*hart_addr = get_cluster_error_bank_addr(cluster_id);
	*size = RERI_ERR_BANK_SIZE;

	return SBI_SUCCESS;
}

static uint32_t riscv_reri_get_hart_sse_vector(int hart_id, uint32_t *sse_vector)
{
	reri_hart_dev_t *reri_hart;

	reri_hart = get_reri_hart_dev(hart_id);
	if (!reri_hart)
		return SBI_ENOENT;

	*sse_vector = reri_hart->dev.sse_vector;

	return 0;
}

static uint32_t riscv_reri_get_hart_src_id(int hart_id, uint32_t *hart_src_id)
{
	reri_hart_dev_t *reri_hart;

	reri_hart = get_reri_hart_dev(hart_id);
	if (!reri_hart)
		return SBI_ENOENT;

	*hart_src_id = reri_hart->dev.src_id;

	return 0;
}

void cpu_reri_irq_handler(void) {
	int current_hart = current_hartid();
	int cluster_id, local_hart_id;
	uint64_t bank_addr, status;
	uint64_t valid_summary, valid_bitmap;

	if (get_cluster_and_local_hart_id(current_hart, &cluster_id, &local_hart_id) != 0)
		return;

	bank_addr = get_cluster_error_bank_addr(cluster_id);
	valid_summary = reri_read((void *)(bank_addr + RERI_VALID_SUMMARY));

	if ((valid_summary & RERI_SV_EN) && (valid_summary & ~0x1ULL)) {
		valid_bitmap = valid_summary & RERI_VALID_BITMAP_MASK;
		for (int i = 0; i < MAX_ERROR_RECORDS; i++) {
			if (valid_bitmap & (1ULL << i)) {
				con_log(RERI_MODNAME ": cpu reri interrupt triggered (valid_summary bit[0]=1)\n");
				u32 vector = 1 << i;
				reri_drv_sync_hart_errs(current_hart, &vector);
			}
		}
	} else if (!(valid_summary & RERI_SV_EN)) {
		/* Check status.v bit for each record */
		for (int i = 0; i < MAX_ERROR_RECORDS; i++) {
			status = reri_get_status((void *)bank_addr, i);
			if ((status & RERI_STATUS_V_EN) == 1) {
				con_log(RERI_MODNAME ": Found valid error in record %d\n", i);
				u32 vector = 1 << i;
				reri_drv_sync_hart_errs(current_hart, &vector);
			}
		}
	}
}

static void cpu_reri_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		cpu_reri_irq_handler();
		return;
	}
}
#ifdef SYS_REALTIME
static void cpu_reri_poll_init(void)
{
	irq_register_poller(cpu_reri_bh);
}
#define cpu_reri_irq_init()	do {} while (0)
#else
static void cpu_reri_irq_init(void)
{
	irqc_configure_irq(IRQ_RAS_CE, 0, IRQ_LEVEL_TRIGGERED);
	irqc_configure_irq(IRQ_RAS_UE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_RAS_CE, (irq_handler)cpu_reri_irq_handler);
	irq_register_vector(IRQ_RAS_UE, (irq_handler)cpu_reri_irq_handler);
	irqc_enable_irq(IRQ_RAS_CE);
	irqc_enable_irq(IRQ_RAS_UE);
}
#define cpu_reri_poll_init()	do {} while (0)
#endif

void reri_drv_init(void)
{
	int i, ret;
	static reri_hart_dev_t reri_hart_dev[NR_CPUS];
	reri_hart_devices = &reri_hart_dev[0];

	uint64_t addr = RERI_HART_DEV_ADDR;
	reri_nr_harts = NR_CPUS;

	con_log(RERI_MODNAME ": Initializing RERI driver\n");
	con_log(RERI_MODNAME ": Base address: 0x%llx\n", addr);
	con_log(RERI_MODNAME ": Number of harts: %d\n", reri_nr_harts);
	con_log(RERI_MODNAME ": Harts per cluster: %d\n", HART_PER_CLUSTER);
	con_log(RERI_MODNAME ": Number of clusters: %d\n", MAX_CLUSTERS);
	con_log(RERI_MODNAME ": Note: All clusters share the same Error bank address space\n");

	/* enable RAS clock */
	clk_enable(cpu_ras_sw_rstn);
	acpi_ghes_init(GHES_ERR_ADDR, GHES_ERR_SIZE);

	for (i = 0; i < reri_nr_harts; i++) {
		int cluster_id, local_hart_id;

		if ((ret = acpi_ghes_new_error_source(i, 0)) < 0)
			continue;

		if (get_cluster_and_local_hart_id(i, &cluster_id, &local_hart_id) != 0)
			continue;

		/* All clusters share the same Error bank address space */
		reri_hart_devices[i].dev.addr = get_cluster_error_bank_addr(cluster_id);
		reri_hart_devices[i].dev.size = RERI_ERR_BANK_SIZE;
		reri_hart_devices[i].dev.sse_vector = i;
		reri_hart_devices[i].dev.src_id = i;
		reri_hart_devices[i].hart_id = i;
		reri_hart_devices[i].cluster_id = cluster_id;
		reri_hart_devices[i].local_hart_id = local_hart_id;

		con_log(RERI_MODNAME ": Hart %d (Cluster %d, Local Hart %d) RERI registers:\n",
			i, cluster_id, local_hart_id);
		con_log(RERI_MODNAME ":   - Base address: 0x%llx (shared by all clusters)\n",
			reri_hart_devices[i].dev.addr);
	}

	/* initialize CPU RERI interrupt */
	cpu_reri_bh = bh_register_handler(cpu_reri_bh_handler);
	cpu_reri_irq_init();
	cpu_reri_poll_init();

	con_log(RERI_MODNAME ": RERI driver initialized\n");
}

/* inject error */
static int do_reri_test(int argc, char *argv[])
{
	int hart_id, test_id;
	uint64_t hart_addr, bank_addr, err_size;
	uint64_t status, control;

	struct {
		uint32_t ec;    // Error Code
		uint32_t tt;    // Transaction Type
		uint32_t ce;    // Corrected Error (CE)
		uint32_t uec;   // Uncorrected Urgent Error (UUE)
		uint32_t ued;   // Uncorrected Deferred Error (UDE)
		const char *desc;
	} error_scenarios[] = {
		/* L2C Errors */
		{RERI_EC_SDC, RERI_TT_CUSTOM, 1, 0, 0, "L2C SnoopFilter ECC Error (CE)"},
		{RERI_EC_SDC, RERI_TT_CUSTOM, 0, 1, 0, "L2C SnoopFilter ECC Error (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "L2C TAG ECC Error (CE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 0, 1, 0, "L2C TAG ECC Error (UUE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "L2C DATA ECC Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 0, 0, 1, "L2C DATA ECC Error (UDE)"},

		/* Core0 Errors */
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core0 IFU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core0 ICACHE TAG parity Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core0 ICACHE DATA parity Error (CE)"},
		{RERI_EC_TPA, RERI_TT_CUSTOM, 1, 0, 0, "core0 jTLB TAG parity Error (CE)"},
		{RERI_EC_TPD, RERI_TT_CUSTOM, 1, 0, 0, "core0 jTLB DATA parity Error (CE)"},
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core0 LSU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core0 DCACHE TAG ECC Error (CE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 0, 1, 0, "core0 DCACHE TAG ECC Error (UUE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core0 DCACHE DATA ECC Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 0, 0, 1, "core0 DCACHE DATA ECC Error (UDE)"},

		/* Core1 Errors */
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core1 IFU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core1 ICACHE TAG parity Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core1 ICACHE DATA parity Error (CE)"},
		{RERI_EC_TPA, RERI_TT_CUSTOM, 1, 0, 0, "core1 jTLB TAG parity Error (CE)"},
		{RERI_EC_TPD, RERI_TT_CUSTOM, 1, 0, 0, "core1 jTLB DATA parity Error (CE)"},
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core1 LSU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core1 DCACHE TAG ECC Error (CE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 0, 1, 0, "core1 DCACHE TAG ECC Error (UUE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core1 DCACHE DATA ECC Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 0, 0, 1, "core1 DCACHE DATA ECC Error (UDE)"},

		/* Core2 Errors */
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core2 IFU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core2 ICACHE TAG parity Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core2 ICACHE DATA parity Error (CE)"},
		{RERI_EC_TPA, RERI_TT_CUSTOM, 1, 0, 0, "core2 jTLB TAG parity Error (CE)"},
		{RERI_EC_TPD, RERI_TT_CUSTOM, 1, 0, 0, "core2 jTLB DATA parity Error (CE)"},
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core2 LSU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core2 DCACHE TAG ECC Error (CE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 0, 1, 0, "core2 DCACHE TAG ECC Error (UUE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core2 DCACHE DATA ECC Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 0, 0, 1, "core2 DCACHE DATA ECC Error (UDE)"},

		/* Core3 Errors */
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core3 IFU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core3 ICACHE TAG parity Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core3 ICACHE DATA parity Error (CE)"},
		{RERI_EC_TPA, RERI_TT_CUSTOM, 1, 0, 0, "core3 jTLB TAG parity Error (CE)"},
		{RERI_EC_TPD, RERI_TT_CUSTOM, 1, 0, 0, "core3 jTLB DATA parity Error (CE)"},
		{RERI_EC_CDA, RERI_TT_CUSTOM, 0, 1, 0, "core3 LSU consume poison (UUE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 1, 0, 0, "core3 DCACHE TAG ECC Error (CE)"},
		{RERI_EC_CAS, RERI_TT_CUSTOM, 0, 1, 0, "core3 DCACHE TAG ECC Error (UUE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 1, 0, 0, "core3 DCACHE DATA ECC Error (CE)"},
		{RERI_EC_CBA, RERI_TT_CUSTOM, 0, 0, 1, "core3 DCACHE DATA ECC Error (UDE)"},
	};

	if (argc < 2) {
		con_err("Usage: reri test [test_id]\n");
		con_err("  test_id:\n");
		con_err("    0: L2C SnoopFilter ECC Error (CE)\n");
		con_err("    1: L2C SnoopFilter ECC Error (UUE)\n");
		con_err("    2: L2C TAG ECC Error (CE)\n");
		con_err("    3: L2C TAG ECC Error (UUE)\n");
		con_err("    4: L2C DATA ECC Error (CE)\n");
		con_err("    5: L2C DATA ECC Error (UDE)\n");
		con_err("    6: Core0 IFU consume poison (UUE)\n");
		con_err("    7: Core0 ICACHE TAG parity Error (CE)\n");
		con_err("    8: Core0 ICACHE DATA parity Error (CE)\n");
		con_err("    9: Core0 jTLB TAG parity Error (CE)\n");
		con_err("    10: Core0 jTLB DATA parity Error (CE)\n");
		con_err("    11: Core0 LSU consume poison (UUE)\n");
		con_err("    12: Core0 DCACHE TAG ECC Error (CE)\n");
		con_err("    13: Core0 DCACHE TAG ECC Error (UUE)\n");
		con_err("    14: Core0 DCACHE DATA ECC Error (CE)\n");
		con_err("    15: Core0 DCACHE DATA ECC Error (UDE)\n");
		con_err("    16: Core1 IFU consume poison (UUE)\n");
		con_err("    17: Core1 ICACHE TAG parity Error (CE)\n");
		con_err("    18: Core1 ICACHE DATA parity Error (CE)\n");
		con_err("    19: Core1 jTLB TAG parity Error (CE)\n");
		con_err("    20: Core1 jTLB DATA parity Error (CE)\n");
		con_err("    21: Core1 LSU consume poison (UUE)\n");
		con_err("    22: Core1 DCACHE TAG ECC Error (CE)\n");
		con_err("    23: Core1 DCACHE TAG ECC Error (UUE)\n");
		con_err("    24: Core1 DCACHE DATA ECC Error (CE)\n");
		con_err("    25: Core1 DCACHE DATA ECC Error (UDE)\n");
		con_err("    26: Core2 IFU consume poison (UUE)\n");
		con_err("    27: Core2 ICACHE TAG parity Error (CE)\n");
		con_err("    28: Core2 ICACHE DATA parity Error (CE)\n");
		con_err("    29: Core2 jTLB TAG parity Error (CE)\n");
		con_err("    30: Core2 jTLB DATA parity Error (CE)\n");
		con_err("    31: Core2 LSU consume poison (UUE)\n");
		con_err("    32: Core2 DCACHE TAG ECC Error (CE)\n");
		con_err("    33: Core2 DCACHE TAG ECC Error (UUE)\n");
		con_err("    34: Core2 DCACHE DATA ECC Error (CE)\n");
		con_err("    35: Core2 DCACHE DATA ECC Error (UDE)\n");
		con_err("    36: Core3 IFU consume poison (UUE)\n");
		con_err("    37: Core3 ICACHE TAG parity Error (CE)\n");
		con_err("    38: Core3 ICACHE DATA parity Error (CE)\n");
		con_err("    39: Core3 jTLB TAG parity Error (CE)\n");
		con_err("    40: Core3 jTLB DATA parity Error (CE)\n");
		con_err("    41: Core3 LSU consume poison (UUE)\n");
		con_err("    42: Core3 DCACHE TAG ECC Error (CE)\n");
		con_err("    43: Core3 DCACHE TAG ECC Error (UUE)\n");
		con_err("    44: Core3 DCACHE DATA ECC Error (CE)\n");
		con_err("    45: Core3 DCACHE DATA ECC Error (UDE)\n");
		return -EINVAL;
	}

	test_id = (argc > 2) ? (int)strtoull(argv[2], 0, 0) : -1;
	hart_id = current_hartid();

	if (test_id < 0 || test_id > 45) {
		con_err("Invalid test_id %d (valid range: 0-45)\n", test_id);
		return -EINVAL;
	}

	con_log(RERI_MODNAME ": Testing scenario: %s\n", error_scenarios[test_id].desc);

	if (riscv_reri_get_hart_addr(hart_id, &hart_addr, &err_size) != 0)
		return -EINVAL;

	bank_addr = hart_addr;
	status = reri_get_status((void *)bank_addr, 0) |
		 RERI_STATUS_SET_CE(error_scenarios[test_id].ce)  |
		 RERI_STATUS_SET_UEC(error_scenarios[test_id].uec) |
		 RERI_STATUS_SET_UED(error_scenarios[test_id].ued) |
	         RERI_STATUS_SET_TT(error_scenarios[test_id].tt)  |
		 RERI_STATUS_SET_EC(error_scenarios[test_id].ec);
	reri_set_status((void *)bank_addr, 0, status);

	/* trigger: inject error */
	control = reri_get_control((void *)bank_addr, 0)  |
		  RERI_CTRL_ELSE_EN                        |
		  RERI_CTRL_SET_CES(RAS_SIGNAL_HIGH_PRIORITY)  |
		  RERI_CTRL_SET_UEDS(RAS_SIGNAL_HIGH_PRIORITY) |
		  RERI_CTRL_SET_UECS(RAS_SIGNAL_HIGH_PRIORITY) |
		  RERI_CTRL_SET_EID(100)                     |
		  ~RERI_CTRL_SINV_EN;

	reri_set_control((void *)bank_addr, 0, control);

	con_log(RERI_MODNAME ": inst_id=0x%llx\n",
		RERI_INST_ID(reri_read((void *)bank_addr + RERI_BANK_INFO)));
	return 0;
}

DEFINE_COMMAND(reri, do_reri_test, "RERI Debug commands",
	"reri test <test_id>\n"
	"  - test specific error scenario for given hart\n"
	"test_id:\n"
	"  0-5: L2C errors\n"
	"  6-15: Core0 errors\n"
	"  16-23: Core1 errors\n"
	"  24-31: Core2 errors\n"
	"  32-39: Core3 errors\n"
);

int reri_drv_sync_hart_errs(u32 hart_id, u32 *pending_vectors)
{
	int ret;
	uint64_t bank_addr, hart_addr, err_size;
	uint64_t eaddr, status;
	int tt;
	acpi_ghes_error_info einfo;
	uint32_t hart_src_id, sse_vector;

	if (riscv_reri_get_hart_addr(hart_id, &hart_addr, &err_size) != 0)
		return 0;

	if (riscv_reri_get_hart_src_id(hart_id, &hart_src_id) != 0)
		return 0;

	bank_addr = hart_addr;

	status = reri_get_status((void *)bank_addr, 0);
	eaddr = reri_read((void *)bank_addr + RERI_ADDR_INFO_I(0));

	reri_clear_valid_bit((void *)bank_addr, 0);

	if (RERI_STATUS_GET_CE(status))
		einfo.info.gpe.sev = 2;
	else if (RERI_STATUS_GET_UED(status))
		einfo.info.gpe.sev = 0; /* deferred, recoverable? */
	else if (RERI_STATUS_GET_UEC(status))
		einfo.info.gpe.sev = 1; /* fatal error */
	else
		einfo.info.gpe.sev = 3; /* Unknown */

	einfo.info.gpe.validation_bits = (GPE_PROC_TYPE_VALID |
					  GPE_PROC_ISA_VALID |
					  GPE_PROC_ERR_TYPE_VALID);

	einfo.info.gpe.proc_type = GHES_PROC_TYPE_RISCV;
	einfo.info.gpe.proc_isa = GHES_PROC_ISA_RISCV64;

	tt = RERI_STATUS_GET_TT(status);

	if (tt && tt >= 4 && tt <= 7) {
		einfo.info.gpe.validation_bits |= GPE_OP_VALID;

		/* Transaction type */
		switch(tt) {
		case RERI_TT_IMPLICIT_READ:
			einfo.info.gpe.operation = 3;
			break;
		case RERI_TT_EXPLICIT_READ:
			einfo.info.gpe.operation = 1;
			break;
		case RERI_TT_IMPLICIT_WRITE:
		case RERI_TT_EXPLICIT_WRITE:
			einfo.info.gpe.operation = 2;
			break;
		default:
			einfo.info.gpe.operation = 0;
			break;
		}

		/* Translate error codes from RERI */
		switch(RERI_STATUS_GET_EC(status)) {
		case RERI_EC_CBA:
		case RERI_EC_CSD:
		case RERI_EC_CAS:
		case RERI_EC_CUE:
			einfo.info.gpe.proc_err_type = 0x01;
			break;
		case RERI_EC_TPD:
		case RERI_EC_TPA:
		case RERI_EC_TPU:
			einfo.info.gpe.proc_err_type = 0x02;
			break;
		case RERI_EC_SBE:
			einfo.info.gpe.proc_err_type = 0x04;
			break;
		case RERI_EC_HSE:
		case RERI_EC_ITD:
		case RERI_EC_ITO:
		case RERI_EC_IWE:
		case RERI_EC_IDE:
		case RERI_EC_SMU:
		case RERI_EC_SMD:
		case RERI_EC_SMS:
		case RERI_EC_PIO:
		case RERI_EC_PUS:
		case RERI_EC_PTO:
		case RERI_EC_SIC:
			einfo.info.gpe.proc_err_type = 0x08;
			break;
		default:
			einfo.info.gpe.proc_err_type = 0x00;
			break;
		}
	}

	/* Address type */
	if (RERI_STATUS_GET_AIT(status)) {
		einfo.info.gpe.validation_bits |= GPE_TARGET_ADDR_VALID;
		einfo.info.gpe.target_addr = eaddr;
	}

	einfo.etype = ERROR_TYPE_GENERIC_CPU;

	/* Update the CPER record */
	acpi_ghes_record_errors(hart_src_id, &einfo);

	if ((ret = riscv_reri_get_hart_sse_vector(hart_id, &sse_vector)) != 0)
		return ret;

	*pending_vectors = sse_vector;

	return 0;
}
#endif
