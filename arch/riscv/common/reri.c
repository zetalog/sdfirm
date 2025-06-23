/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#include <target/sbi.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>
#include <target/ras.h>
#include <target/iommu.h>
#include <target/panic.h>
#include <target/reri.h>
#include <target/percpu.h>
#include <target/bench.h>
#include <target/rpmi.h>
#include <target/noc.h>

#define RERI_MODNAME			"reri"

#define HART_PER_CLUSTER		4
#define MAX_CLUSTERS			16
#define MAX_IOMMU_NUM			1
#define MAX_IOMMU_ERROR_BANKS		64 // IOATS + 63 * IOATC

#ifndef RERI_HART_DEV_ADDR
#define RERI_HART_DEV_ADDR		(__ACPU_RAS_SPACE_BASE + ULL(0x4030000))

#define IOMMU_STRIDE			ULL(0x800000000)
#define IOMMU_CRTL_CFG_BASE(i)		(ULL(0x4780000000) + (i) * (IOMMU_STRIDE))

/* RAS Signal Priority Levels */
#define RAS_SIGNAL_DISABLED		0  /* Signaling is disabled */
#define RAS_SIGNAL_LOW_PRIORITY		1  /* Low-priority RAS signal */
#define RAS_SIGNAL_HIGH_PRIORITY	2  /* High-priority RAS signal */
#define RAS_SIGNAL_PLATFORM		3  /* Platform specific RAS signal */
#endif

#ifdef CONFIG_RISCV_IOMMU_SPACET
/* IOMMU RERI interrupt numbers */
static const int iommu_reri_irqs[] = {
	IRQ_IOMMU_IOATS0_RAS,
	IRQ_IOMMU_IOATS1_RAS,
	IRQ_IOMMU_IOATS2_RAS,
	IRQ_IOMMU_IOATS3_RAS,
	IRQ_IOMMU_IOATC0_RAS,
	IRQ_IOMMU_IOATC1_RAS,
	IRQ_IOMMU_IOATC2_RAS,
	IRQ_IOMMU_IOATC3_RAS,
	IRQ_IOMMU_IOATC4_RAS,
	IRQ_IOMMU_IOATC5_RAS,
	IRQ_IOMMU_IOATC6_RAS,
	IRQ_IOMMU_IOATC7_RAS,
	IRQ_IOMMU_IOATC8_RAS,
	IRQ_IOMMU_IOATC9_RAS,
	IRQ_IOMMU_IOATC10_RAS,
	IRQ_IOMMU_IOATC11_RAS,
	IRQ_IOMMU_IOATC12_RAS,
	IRQ_IOMMU_IOATC13_RAS,
};
#define NR_IOMMU_RERI_IRQS		(sizeof(iommu_reri_irqs) / sizeof(iommu_reri_irqs[0]))
#endif

static void reri_clear_valid_bit(caddr_t base, int record_idx)
{
	reri_set(RERI_CTRL_SINV_EN | RERI_CTRL_SRDP_EN,
		 RERI_CONTROL_I(base, record_idx));
	con_log(RERI_MODNAME ": clear valid bit - record_idx=%d, reg_addr=0x%lx, reg_val=0x%llx\n",
		record_idx, RERI_CONTROL_I(base, record_idx),
		reri_read(RERI_CONTROL_I(base, record_idx)));
}

enum reri_event_source_type {
	RERI_EVENT_SOURCE_CPU,
	RERI_EVENT_SOURCE_IOMMU
};
struct reri_context {
	int target_id;
	int test_id;
	int err_bank_id_iommu;
	uint64_t target_addr;
	uint64_t bank_addr;
	uint64_t err_size;
	enum reri_event_source_type test_target_type;
	const struct reri_info *scenarios_to_use;
	int num_scenarios;
	int eid;
	const reri_source_info_t *source_info;
};

#ifdef CONFIG_SMP
DEFINE_PERCPU(struct reri_context, reri_ctx);
#define get_reri        this_cpu_ptr(&reri_ctx)
#else
static struct reri_context reri_ctx;
#define get_reri        (&reri_ctx)
#endif
struct reri_generic_dev {
	uint64_t addr;
	uint64_t size;
	uint32_t sse_vector;
	uint16_t src_id;
	uint16_t res;
};
struct reri_hart_dev {
	struct reri_generic_dev dev;
	int hart_id;
	int cluster_id;
	int local_hart_id;
};

/**
 * IOMMU includes error banks:
 *	IOATS: 1
 *	IOATC: 0 ~ 62 (maximum, discover by dtisr)
 */
struct reri_iommu_err_dev {
	struct reri_generic_dev dev;
	uint32_t err_bank_id;
	uint32_t iommu_id;
	uint32_t dtisr_index;
	uint8_t is_ioatc;
};

static struct reri_hart_dev *reri_hart_devices;
static uint32_t reri_nr_harts;
static uint32_t current_hart;

static struct reri_iommu_err_dev reri_iommu_err_dev[MAX_IOMMU_NUM * MAX_IOMMU_ERROR_BANKS];
static struct reri_iommu_err_dev *reri_iommu_err_banks = &reri_iommu_err_dev[0];
static uint32_t reri_nr_iommu_err_banks;

#define RERI_ERR_BANK_SIZE	0x1000
uint64_t cpu_mask = 0;
bh_t cpu_reri_bh;
bh_t iommu_reri_bh;

const reri_source_info_t *get_source_info(uint16_t inst_id)
{
	if (inst_id <= 0x53 && source_info_table[inst_id].name != NULL)
		return &source_info_table[inst_id];

	con_log("reri: unknown source id: 0x%x\n", inst_id);

	return NULL;
}

/**
 * Return HART ID of the caller.
 */
unsigned int current_hartid(void)
{
	return (uint32_t)csr_read(CSR_MHARTID);
}

static int get_cluster_and_local_hart_id(int hart_id, int *cluster_id, int *local_hart_id)
{
	if (hart_id >= MAX_CPU_CORES)
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

static struct reri_hart_dev *get_reri_hart_dev(int hart_id)
{
	int i;

	for (i = 0; i < reri_nr_harts; i++) {
		if (reri_hart_devices[i].hart_id == hart_id)
			return &reri_hart_devices[i];
	}

	return NULL;
}

static int riscv_reri_get_hart_addr(int hart_id, uint64_t *hart_addr,
				   uint64_t *size)
{
	struct reri_hart_dev *reri_hart;
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

static uint32_t riscv_reri_get_hart_sse_vector(int hart_id,
						  uint32_t *sse_vector)
{
	struct reri_hart_dev *reri_hart;

	reri_hart = get_reri_hart_dev(hart_id);
	if (!reri_hart)
		return SBI_ENOENT;

	*sse_vector = reri_hart->dev.sse_vector;

	return 0;
}

static uint32_t riscv_reri_get_hart_src_id(int hart_id,
					   uint32_t *hart_src_id)
{
	struct reri_hart_dev *reri_hart;

	reri_hart = get_reri_hart_dev(hart_id);
	if (!reri_hart)
		return SBI_ENOENT;

	*hart_src_id = reri_hart->dev.src_id;

	return 0;
}

#ifdef CONFIG_RISCV_IOMMU_SPACET
int riscv_ioatc_num;
caddr_t riscv_ioatc_reg[MAX_RISCV_IOMMU_IOATC];

static int riscv_reri_discover_iommus(void)
{
	int i, j, k, ioatc, id;
	uint32_t sts, dtisr_val;
	uint64_t base_addr;

	riscv_ioatc_num = 0;
	reri_nr_iommu_err_banks = 0;

	for (i = 0; i < MAX_IOMMU_NUM; i++) {
		base_addr = IOMMU_CRTL_CFG_BASE(i);
		id = __raw_readl(base_addr + RISCV_IOMMU_REG_ID);

		con_log(RERI_MODNAME ": IOMMU[%d] base_addr=0x%llx, id=0x%x\n", i, base_addr, id);

		if (FIELD_GET(RISCV_IOMMU_ID_MFID, id) != JEDEC_SPACET ||
			FIELD_GET(RISCV_IOMMU_ID_BANK, id) != JEDEC_BANK(15)) {
			con_log(RERI_MODNAME ": invalid IOMMU id = 0x%x\n", id);
			continue;
		}
		/* the first error bank address: IOATS */
		reri_iommu_err_banks[reri_nr_iommu_err_banks].dev.addr = base_addr + RISCV_IOMMU_RERI_EBI_OFFSET;
		reri_iommu_err_banks[reri_nr_iommu_err_banks].dev.size = RERI_ERR_BANK_SIZE;
		reri_iommu_err_banks[reri_nr_iommu_err_banks].err_bank_id = 0;
		reri_iommu_err_banks[reri_nr_iommu_err_banks].dtisr_index = 0;
		reri_iommu_err_banks[reri_nr_iommu_err_banks].iommu_id = i;
		reri_iommu_err_banks[reri_nr_iommu_err_banks].is_ioatc = 0;
		iommu_set(RISCV_IOMMU_IORASECR_EN, RISCV_IOMMU_REG_IORASECR);
		con_log(RERI_MODNAME ": IOMMU[%d] IOATS IORASECR=0x%x\n", i, __raw_readl(base_addr + RISCV_IOMMU_REG_IORASECR));
		reri_nr_iommu_err_banks++;

		for (j = 0; j < 4; j++) {
			dtisr_val = __raw_readl(base_addr + RISCV_IOMMU_REG_DTISR(j));
			con_log(RERI_MODNAME ": IOMMU[%d] DTISR[%d]=0x%x, dtisr_addr=0x%llx\n",
							i, j, dtisr_val, base_addr + RISCV_IOMMU_REG_DTISR(j));

			if (dtisr_val == 0)
				continue;

			for (k = 0; k < 16; k++) {
				ioatc = j * 16 + k;
				/* DTI0 uses [1:0], DTI1-DTI63 use [127:2] */
				if (ioatc == 0)
					sts = dtisr_val & 0x3;
				else
					sts = (dtisr_val >> (ioatc * 2)) & 0x3;

				con_log(RERI_MODNAME ": IOMMU[%d] DTISR[%d] IOATC[%d] STS=0x%x\n",
					i, j, ioatc, sts);

				/* Check if TBU connected (status = 1) */
				if (sts == RISCV_IOMMU_DTI_TBU_IOATC) {
					riscv_ioatc_reg[riscv_ioatc_num] = RISCV_IOMMU_IOATC_BASE(base_addr, ioatc);
					reri_iommu_err_banks[reri_nr_iommu_err_banks].dev.addr =
						riscv_ioatc_reg[riscv_ioatc_num] + RISCV_IOMMU_RERI_EBI_OFFSET;
					reri_iommu_err_banks[reri_nr_iommu_err_banks].dev.size = RERI_ERR_BANK_SIZE;
					reri_iommu_err_banks[reri_nr_iommu_err_banks].err_bank_id = reri_nr_iommu_err_banks;
					reri_iommu_err_banks[reri_nr_iommu_err_banks].dtisr_index = ioatc;
					reri_iommu_err_banks[reri_nr_iommu_err_banks].iommu_id = i;
					/* Mark as IOATC */
					reri_iommu_err_banks[reri_nr_iommu_err_banks].is_ioatc = 1;

					/* enable error interrupt generation */
					iommu_set(RISCV_IOMMU_IORASECR_EN,
						riscv_ioatc_reg[riscv_ioatc_num] + RISCV_IOMMU_REG_IORASECR);
					riscv_ioatc_num++;
					reri_nr_iommu_err_banks++;
				}
			}
		}
	}

	con_log(RERI_MODNAME ": IOMMU error bank number: %d, IOATC number: %d\n",
		reri_nr_iommu_err_banks, riscv_ioatc_num);

	return 0;
}
#else
#define riscv_reri_discover_iommus	do { } while (0)
#endif

static int reri_drv_sync_generic_error_record(enum reri_event_source_type source_type,
						 uint32_t id, uint64_t bank_addr,
						 int record_index,
						 uint32_t *out_pending_sse_vector_for_cpu)
{
	uint64_t status, addr_info;
	acpi_ghes_error_info einfo;
	uint32_t acpi_ghes_source_id;
	int ret = 0;
	int tt;

	/* Initialize einfo and out_pending_sse_vector_for_cpu if applicable */
	memset(&einfo, 0, sizeof(einfo));
	if (out_pending_sse_vector_for_cpu)
		*out_pending_sse_vector_for_cpu = 0; /* Default to 0 */

	/* 1. Determine ACPI GHES Source ID */
	acpi_ghes_source_id = RERI_INST_ID(reri_read(bank_addr + RERI_BANK_INFO));

	/* 2. Read RERI error record registers */
	status = reri_read(RERI_STATUS_I(bank_addr, record_index));
	addr_info = reri_read(RERI_ADDR_INFO_I(bank_addr, record_index));

	/* 3. Clear the valid bit for this specific record */
	reri_clear_valid_bit(bank_addr, record_index);

	/* 4. Populate GHES error info (Common parts first) */
	if (RERI_STATUS_GET_CE(status))
		einfo.info.gpe.sev = 2;
	else if (RERI_STATUS_GET_UED(status))
		einfo.info.gpe.sev = 0; /* deferred, recoverable */
	else if (RERI_STATUS_GET_UEC(status))
		einfo.info.gpe.sev = 1; /* fatal error */
	else
		einfo.info.gpe.sev = 3; /* Unknown */

	tt = RERI_STATUS_GET_TT(status);
	if (tt && tt >= 4 && tt <= 7) {
		einfo.info.gpe.validation_bits |= CPER_PROC_VALID_OPERATION;
		switch (tt) {
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
		switch (RERI_STATUS_GET_EC(status)) {
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
		einfo.info.gpe.validation_bits |= CPER_PROC_VALID_TARGET_ADDRESS;
		einfo.info.gpe.target_addr = addr_info;
	}

	/* Source-specific parts for einfo */
	if (source_type == RERI_EVENT_SOURCE_CPU) {
		einfo.etype = ERROR_TYPE_GENERIC_CPU;
		einfo.info.gpe.validation_bits |= (CPER_PROC_VALID_TYPE |
						   CPER_PROC_VALID_ISA |
						   CPER_PROC_VALID_ERROR_TYPE);
		einfo.info.gpe.proc_type = GHES_PROC_TYPE_RISCV;
		einfo.info.gpe.proc_isa = GHES_PROC_ISA_RISCV64;
	} else { // RERI_EVENT_SOURCE_IOMMU
		einfo.etype = ERROR_TYPE_MEM;
	}

	/* 5. Record errors via ACPI GHES */
	acpi_ghes_record_errors(acpi_ghes_source_id, &einfo);

	/* 6. Handle SSE vector for CPU */
	if (source_type == RERI_EVENT_SOURCE_CPU && out_pending_sse_vector_for_cpu) {
		uint32_t sse_vector_val_local = 0;
		/* id here is hart_id */
		ret = riscv_reri_get_hart_sse_vector(id, &sse_vector_val_local);
		if (ret == 0)
			*out_pending_sse_vector_for_cpu = sse_vector_val_local;
	}

	return ret;
}

static void process_reri_error_bank(uint64_t bank_addr,
		enum reri_event_source_type source_type, uint32_t id_for_sync)
{
	uint64_t status_val;
	uint64_t valid_summary;
	uint64_t valid_bitmap;
	int record_index;
	uint32_t sse_vec_out;
	uint32_t n_err_recs;
	uint32_t pending_vectors[MAX_PEND_VECS];
	uint32_t nr_pending = 0;
	uint32_t nr_remaining = 0;
	int ret;

	valid_summary = reri_read(bank_addr + RERI_VALID_SUMMARY);
	n_err_recs = RERI_N_ERR_RECS(reri_read(bank_addr + RERI_BANK_INFO));

	/* Check status.v bit for each record */
	for (record_index = 0; record_index < n_err_recs; record_index++) {
		status_val = reri_read(RERI_STATUS_I(bank_addr, record_index));
		if ((status_val & RERI_STATUS_V_EN)) {
			if (source_type == RERI_EVENT_SOURCE_CPU) {
				con_log(RERI_MODNAME ": CPU RERI via status.V (record %d) for hart %u, bank 0x%llx\n",
					record_index, id_for_sync, bank_addr);
#ifndef CONFIG_SPACEMIT_RAS
				struct spacemit_ras_error_record *error_record;
				error_record->inst_id = id_for_sync;
				error_record->bank_addr = bank_addr;
				error_record->error_type = 0;
				error_record->status = status_val;
				error_record->addr_info = reri_read(RERI_ADDR_INFO_I(bank_addr, record_index));
				/* ACPU send custom error record to RMU, convert to ghes format */
				spacemit_ras_sync_hart_errs(error_record);
				rpmi_ras_sync_hart_errs(pending_vectors, &nr_pending, &nr_remaining);
#else
				/* It is not firmware-first */
				sse_vec_out = 1 << record_index;
				reri_drv_sync_generic_error_record(RERI_EVENT_SOURCE_CPU, id_for_sync,
					bank_addr, record_index, &sse_vec_out);
#endif
			} else {
				con_log(RERI_MODNAME ": IOMMU RERI (id %u) via status.V (record %d), bank 0x%llx\n",
					id_for_sync, record_index, bank_addr);
				sse_vec_out = 1 << record_index;
				reri_drv_sync_generic_error_record(RERI_EVENT_SOURCE_IOMMU, id_for_sync,
					bank_addr, record_index, NULL);

				/* Sync error via RPMI */
				ret = rpmi_ras_sync_reri_errs(pending_vectors, &nr_pending, &nr_remaining);
				if (ret) {
					con_err(RERI_MODNAME ": Failed to sync RERI errors via RPMI, ret=%d\n", ret);
				} else {
					con_log(RERI_MODNAME ": Synced %d pending vectors via RPMI, %d remaining\n",
						nr_pending, nr_remaining);
				}
			}
		}
	}
}

static void generic_reri_irq_handler(irq_t irq)
{
	int cluster_id, local_hart_id, i;
	uint64_t bank_addr;
	enum reri_event_source_type source_type;
	int irq_number = irq_ext(irq);

	if (irq_number == IRQ_RAS_CE || irq_number == IRQ_RAS_UE)
		source_type = RERI_EVENT_SOURCE_CPU;
	else
		source_type = RERI_EVENT_SOURCE_IOMMU;
	con_log(RERI_MODNAME ": handle interrupt (irq=%d)", irq);

	if (source_type == RERI_EVENT_SOURCE_CPU) {
		if (get_cluster_and_local_hart_id(current_hart, &cluster_id, &local_hart_id) != 0)
			return;
		bank_addr = get_cluster_error_bank_addr(cluster_id);
		process_reri_error_bank(bank_addr, RERI_EVENT_SOURCE_CPU, current_hart);
	} else if (source_type == RERI_EVENT_SOURCE_IOMMU) {
		for (i = 0; i < reri_nr_iommu_err_banks; i++) {
			if (reri_iommu_err_banks && reri_iommu_err_banks[i].dev.addr != 0) {
				bank_addr = reri_iommu_err_banks[i].dev.addr;
				process_reri_error_bank(bank_addr, RERI_EVENT_SOURCE_IOMMU, reri_iommu_err_banks[i].iommu_id);
			}
		}
	}
}

static void cpu_reri_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
#if __riscv_xlen == 32
		csr_clear(CSR_MIPH, MIPH_RASHP_INTP);
#else
		csr_clear(CSR_MIP, MIP_RASHP_INTP);
#endif
		generic_reri_irq_handler(IRQ_RAS_CE);
		generic_reri_irq_handler(IRQ_RAS_UE);
		return;
	}
}
#ifdef CONFIG_RISCV_IOMMU_SPACET
static void iommu_reri_bh_handler(uint8_t events)
{
	int i;

	if (events == BH_POLLIRQ) {
		for (i = 0; i < NR_IOMMU_RERI_IRQS; i++) {
			generic_reri_irq_handler(iommu_reri_irqs[i]);
		}
		return;
	}
}
#endif

#ifdef SYS_REALTIME
static void cpu_reri_poll_init(void)
{
	irq_register_poller(cpu_reri_bh);
}
#ifdef CONFIG_RISCV_IOMMU_SPACET
static void iommu_reri_poll_init(void)
{
	irq_register_poller(iommu_reri_bh);
}
#else
#define iommu_reri_irq_init()	do {} while (0)
#endif
#define cpu_reri_irq_init()	do {} while (0)
#else
static void cpu_reri_irq_init(void)
{
	irqc_configure_irq(IRQ_RAS_CE, 0, IRQ_LEVEL_TRIGGERED);
	irqc_configure_irq(IRQ_RAS_UE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_RAS_CE, generic_reri_irq_handler);
	irq_register_vector(IRQ_RAS_UE, generic_reri_irq_handler);
	irqc_enable_irq(IRQ_RAS_CE);
	irqc_enable_irq(IRQ_RAS_UE);
}
#ifdef CONFIG_RISCV_IOMMU_SPACET
static void iommu_reri_irq_init(void)
{
	int i;

	for (i = 0; i < NR_IOMMU_RERI_IRQS; i++) {
		irqc_configure_irq(iommu_reri_irqs[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(iommu_reri_irqs[i], generic_reri_irq_handler);
		irqc_enable_irq(iommu_reri_irqs[i]);
	}
}
#else
#define iommu_reri_poll_init()	do {} while (0)
#endif
#define cpu_reri_poll_init()	do {} while (0)
#endif

void reri_drv_init(void)
{
	int i, ret;
	static struct reri_hart_dev reri_hart_dev[MAX_CPU_CORES];
	uint64_t addr;

	reri_hart_devices = &reri_hart_dev[0];
	addr = RERI_HART_DEV_ADDR;
	reri_nr_harts = MAX_CPU_CORES;
	current_hart = current_hartid();
	cpu_mask = CPU_MASK;
	con_log(RERI_MODNAME ": CPU mask:0x%llx\n", cpu_mask);

	/* enable RAS clock */
	clk_enable(cpu_ras_sw_rstn);
	for (i = 0; i < reri_nr_harts; i++) {
		int cluster_id, local_hart_id;
		if (!(cpu_mask & (1ULL << i)))
			continue;

		ret = acpi_ghes_new_error_source(i / HART_PER_CLUSTER, i / HART_PER_CLUSTER);
		if (ret < 0)
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

#ifdef CONFIG_RISCV_IOMMU_SPACET
	riscv_reri_discover_iommus();

	if (reri_nr_iommu_err_banks > 0) {
		/* initialize IOMMU RERI interrupt */
		iommu_reri_bh = bh_register_handler(iommu_reri_bh_handler);
		iommu_reri_irq_init();
		iommu_reri_poll_init();
	}

#endif
	con_log(RERI_MODNAME ": RERI driver initialized\n");
}

static int riscv_reri_get_iommu_err_bank_addr(uint32_t iommu_id,
					  uint32_t err_bank_id_target,
					  uint64_t *bank_addr_out,
					  uint64_t *size_out)
{
	for (int i = 0; i < reri_nr_iommu_err_banks; i++) {
		if (reri_iommu_err_banks && reri_iommu_err_banks[i].dev.addr != 0) {
			if (reri_iommu_err_banks[i].iommu_id == iommu_id &&
			reri_iommu_err_banks[i].err_bank_id == err_bank_id_target) {
				*bank_addr_out = reri_iommu_err_banks[i].dev.addr;
				*size_out = reri_iommu_err_banks[i].dev.size;
				return 0;
			}
		}
	}

	return -ENOENT;
}

/* inject error */
#if 1
static int do_reri_test(int argc, char *argv[])
{
	int target_id, test_id, err_bank_id_iommu = 0;
	uint64_t target_addr = 0, bank_addr = 0, err_size = 0;
	const struct reri_info *scenarios_to_use = NULL;
	int num_scenarios = 0;
	enum reri_event_source_type test_target_type;
	const reri_source_info_t *source_info;
	int eid;

	if (argc < 3) {
usage:
		con_err("Usage: reri test cpu <test_id>\n");
		con_err("	   reri test ioats <iommu_id> <err_bank_id_for_iommu> <test_id>\n");
		con_err("	   reri test ioatc <iommu_id> <err_bank_id_for_iommu> <test_id>\n");
		con_err("  target_type: cpu | ioats | ioatc\n");
		con_err("	cpu: <test_id> is test id, current hart\n");
		con_err("	ioats: <iommu_id> is iommu id, <err_bank_id_for_iommu> is specific error bank within IOMMU\n");
		con_err("	ioatc: <iommu_id> is iommu id, <err_bank_id_for_iommu> is specific error bank within IOMMU\n");
		con_err("  test_id (for cpu): 0-%d\n", (int)reri_hart_num_error_records - 1);
		for (int i = 0; i < reri_hart_num_error_records; i++)
			con_err("\t%d: %s\n", i, reri_hart_error_records[i].desc);

		con_err("  test_id (for iommu): 0-%d\n",
			(int)reri_ioats_num_error_records > 0 ?
			(int)reri_ioats_num_error_records - 1 : 0);
		if (reri_ioats_num_error_records > 0) {
			for (int i = 0; i < reri_ioats_num_error_records; i++)
				con_err("\t%d: %s\n", i, reri_ioats_error_records[i].desc);
		} else {
			con_err("\tNo IOATS error scenarios defined.\n");
		}

		con_err("  test_id (for iommu): 0-%d\n",
			(int)reri_ioatc_num_error_records > 0 ?
			(int)reri_ioatc_num_error_records - 1 : 0);
		if (reri_ioatc_num_error_records > 0) {
			for (int i = 0; i < reri_ioatc_num_error_records; i++)
				con_err("\t%d: %s\n", i, reri_ioatc_error_records[i].desc);
		} else {
			con_err("\tNo IOATC error scenarios defined.\n");
		}
		return -EINVAL;
	}

	if (strcmp(argv[2], "cpu") == 0) {
		test_target_type = RERI_EVENT_SOURCE_CPU;
		scenarios_to_use = reri_hart_error_records;
		num_scenarios = reri_hart_num_error_records;
		if (argc == 4) {
			target_id = current_hartid();
			test_id = (int)strtoull(argv[3], 0, 0);
		} else {
			goto usage;
		}
	} else if (strcmp(argv[2], "ioats") == 0) {
		test_target_type = RERI_EVENT_SOURCE_IOMMU;
		scenarios_to_use = reri_ioats_error_records;
		num_scenarios = reri_ioats_num_error_records;
		if (argc == 6) {
			target_id = (int)strtoull(argv[3], 0, 0);
			err_bank_id_iommu = (int)strtoull(argv[4], 0, 0);
			test_id = (int)strtoull(argv[5], 0, 0);
		} else {
			goto usage;
		}
		if (reri_ioats_num_error_records == 0) {
			con_err("No IOATS error scenarios defined to run test.\n");
			return -EINVAL;
		}
	} else if (strcmp(argv[2], "ioatc") == 0) {
		test_target_type = RERI_EVENT_SOURCE_IOMMU;
		scenarios_to_use = reri_ioatc_error_records;
		num_scenarios = reri_ioatc_num_error_records;
		if (argc == 6) {
			target_id = (int)strtoull(argv[3], 0, 0);
			err_bank_id_iommu = (int)strtoull(argv[4], 0, 0);
			test_id = (int)strtoull(argv[5], 0, 0);
		} else {
			goto usage;
		}
		if (reri_ioatc_num_error_records == 0) {
			con_err("No IOATC error scenarios defined to run test.\n");
			return -EINVAL;
		}
	}

	if (test_id < 0 || test_id >= num_scenarios) {
		con_err("Invalid test_id %d (valid range: 0-%d for selected target)\n", test_id, num_scenarios - 1);
		return -EINVAL;
	}

	con_log(RERI_MODNAME ": Testing scenario for %s: %s\n", argv[2], scenarios_to_use[test_id].desc);

	if (test_target_type == RERI_EVENT_SOURCE_CPU) {
		if (riscv_reri_get_hart_addr(target_id, &target_addr, &err_size) != 0)
			return -EINVAL;
	} else {
		if (riscv_reri_get_iommu_err_bank_addr(target_id, err_bank_id_iommu, &target_addr, &err_size) != 0) {
			con_err("Failed to get IOMMU error bank address for iommu_id %d, err_bank_id %d\n", target_id, err_bank_id_iommu);
			return -ENOENT;
		}
	}
	bank_addr = target_addr;
	eid = 10000 * (test_id + 1);

	reri_write_mask(RERI_STATUS_SET_CE(scenarios_to_use[test_id].ce) |
			RERI_STATUS_SET_UEC(scenarios_to_use[test_id].uec) |
			RERI_STATUS_SET_UED(scenarios_to_use[test_id].ued) |
			RERI_STATUS_SET_TT(scenarios_to_use[test_id].tt)  |
			RERI_STATUS_SET_EC(scenarios_to_use[test_id].ec),
			RERI_STATUS_SET_CE(RERI_STATUS_CE_MASK) |
			RERI_STATUS_SET_UEC(RERI_STATUS_UEC_MASK) |
			RERI_STATUS_SET_UED(RERI_STATUS_UED_MASK) |
			RERI_STATUS_SET_TT(RERI_STATUS_TT_MASK)  |
			RERI_STATUS_SET_EC(RERI_STATUS_EC_MASK),
			RERI_STATUS_I(bank_addr, test_id));

	/* Trigger: inject error */
	reri_set(RERI_CTRL_ELSE_EN, RERI_CONTROL_I(bank_addr, test_id));
	reri_write_mask(RERI_CTRL_SET_CES(RAS_SIGNAL_HIGH_PRIORITY) |
			RERI_CTRL_SET_UEDS(RAS_SIGNAL_HIGH_PRIORITY) |
			RERI_CTRL_SET_UECS(RAS_SIGNAL_HIGH_PRIORITY) |
			RERI_CTRL_SET_EID(eid),
			RERI_CTRL_SET_CES(RERI_CTRL_CES_MASK) |
			RERI_CTRL_SET_UEDS(RERI_CTRL_UEDS_MASK) |
			RERI_CTRL_SET_UECS(RERI_CTRL_UECS_MASK) |
			RERI_CTRL_SET_EID(RERI_CTRL_EID_MASK),
			RERI_CONTROL_I(bank_addr, test_id));
	reri_clear(RERI_CTRL_SINV_EN, RERI_CONTROL_I(bank_addr, test_id));

	con_log(RERI_MODNAME ": record_index=%d, status=0x%llx\n",
			test_id, reri_read(RERI_STATUS_I(bank_addr, test_id)));
	con_log(RERI_MODNAME ": record_index=%d, eid=0x%x, control=0x%llx\n",
			test_id, eid, reri_read(RERI_CONTROL_I(bank_addr, test_id)));

	source_info = get_source_info(RERI_INST_ID(reri_read(bank_addr + RERI_BANK_INFO)));
	if (source_info)
		con_log(RERI_MODNAME ": source_info: %s\n", source_info->name);
	else
		con_log(RERI_MODNAME ": unknown source id\n");

	return 0;
}

DEFINE_COMMAND(reri, do_reri_test, "RERI Debug commands and error injection",
	"reri test cpu <test_id>\n"
	"  - inject error for current hart\n"
	"reri test ioats <iommu_id> <err_bank_id_for_iommu> <test_id>\n"
	"  - inject error for IOATS\n"
	"reri test ioatc <iommu_id> <err_bank_id_for_iommu> <test_id>\n"
	"  - inject error for IOATC\n"
	"target_type:\n"
	"  cpu: CPU RERI\n"
	"  ioats: IOMMU IOATS RERI\n"
	"  ioatc: IOMMU IOATC RERI\n"
	"test_id: test id, see usage output\n"
);
#else
static int do_reri_test(int argc, char *argv[])
{
	struct reri_context *ctx = get_reri;
	int ret = 0;

	if (argc < 3) {
usage:
		con_err("Usage: reri test cpu <test_id>\n");
		con_err("    reri test ioats <iommu_id> <err_bank_id_for_iommu> <test_id>\n");
		con_err("    reri test ioatc <iommu_id> <err_bank_id_for_iommu> <test_id>\n");
		con_err("  target_type: cpu | ioats | ioatc\n");
		con_err("    cpu: <test_id> is test id, current hart\n");
		con_err("    ioats: <iommu_id> is iommu id, <err_bank_id_for_iommu> is specific error bank within IOMMU\n");
		con_err("    ioatc: <iommu_id> is iommu id, <err_bank_id_for_iommu> is specific error bank within IOMMU\n");
		con_err("  test_id (for cpu): 0-%d\n", (int)reri_hart_num_error_records - 1);
		for (int i = 0; i < reri_hart_num_error_records; i++)
			con_err("\t%d: %s\n", i, reri_hart_error_records[i].desc);

		con_err("  test_id (for iommu): 0-%d\n",
			(int)reri_ioats_num_error_records > 0 ?
			(int)reri_ioats_num_error_records - 1 : 0);
		if (reri_ioats_num_error_records > 0) {
			for (int i = 0; i < reri_ioats_num_error_records; i++)
				con_err("\t%d: %s\n", i, reri_ioats_error_records[i].desc);
		} else {
			con_err("\tNo IOATS error scenarios defined.\n");
		}

		con_err("  test_id (for iommu): 0-%d\n",
			(int)reri_ioatc_num_error_records > 0 ?
			(int)reri_ioatc_num_error_records - 1 : 0);
		if (reri_ioatc_num_error_records > 0) {
			for (int i = 0; i < reri_ioatc_num_error_records; i++)
				con_err("\t%d: %s\n", i, reri_ioatc_error_records[i].desc);
		} else {
			con_err("\tNo IOATC error scenarios defined.\n");
		}
		return -EINVAL;
	}

	memset(ctx, 0, sizeof(*ctx));

	if (strcmp(argv[2], "cpu") == 0) {
		ctx->test_target_type = RERI_EVENT_SOURCE_CPU;
		ctx->scenarios_to_use = reri_hart_error_records;
		ctx->num_scenarios = reri_hart_num_error_records;
		if (argc == 4) {
			ctx->target_id = current_hartid();
			ctx->test_id = (int)strtoull(argv[3], 0, 0);
		} else {
			goto usage;
		}
	} else if (strcmp(argv[2], "ioats") == 0) {
		ctx->test_target_type = RERI_EVENT_SOURCE_IOMMU;
		ctx->scenarios_to_use = reri_ioats_error_records;
		ctx->num_scenarios = reri_ioats_num_error_records;
		if (argc == 6) {
			ctx->target_id = (int)strtoull(argv[3], 0, 0);
			ctx->err_bank_id_iommu = (int)strtoull(argv[4], 0, 0);
			ctx->test_id = (int)strtoull(argv[5], 0, 0);
		} else {
			goto usage;
		}
		if (reri_ioats_num_error_records == 0) {
			con_err("No IOATS error scenarios defined to run test.\n");
			return -EINVAL;
		}
	} else if (strcmp(argv[2], "ioatc") == 0) {
		ctx->test_target_type = RERI_EVENT_SOURCE_IOMMU;
		ctx->scenarios_to_use = reri_ioatc_error_records;
		ctx->num_scenarios = reri_ioatc_num_error_records;
		if (argc == 6) {
			ctx->target_id = (int)strtoull(argv[3], 0, 0);
			ctx->err_bank_id_iommu = (int)strtoull(argv[4], 0, 0);
			ctx->test_id = (int)strtoull(argv[5], 0, 0);
		} else {
			goto usage;
		}
		if (reri_ioatc_num_error_records == 0) {
			con_err("No IOATC error scenarios defined to run test.\n");
			return -EINVAL;
		}
	}

	if (ctx->test_id < 0 || ctx->test_id >= ctx->num_scenarios) {
		con_err("Invalid test_id %d (valid range: 0-%d for selected target)\n",
			   ctx->test_id, ctx->num_scenarios - 1);
		return -EINVAL;
	}

	con_log(RERI_MODNAME "[CPU%d]: Testing scenario for %s: %s\n",
		   current_hartid(), argv[2], ctx->scenarios_to_use[ctx->test_id].desc);

	if (ctx->test_target_type == RERI_EVENT_SOURCE_CPU) {
		if (riscv_reri_get_hart_addr(ctx->target_id, &ctx->target_addr, &ctx->err_size) != 0)
			return -EINVAL;
	} else {
		if (riscv_reri_get_iommu_err_bank_addr(ctx->target_id, ctx->err_bank_id_iommu,
							&ctx->target_addr, &ctx->err_size) != 0) {
			con_err("Failed to get IOMMU error bank address for iommu_id %d, err_bank_id %d\n",
				ctx->target_id, ctx->err_bank_id_iommu);
			return -ENOENT;
		}
	}
	ctx->bank_addr = ctx->target_addr;
	ctx->eid = 1 * (ctx->test_id + 1);

	reri_write_mask(RERI_STATUS_SET_CE(ctx->scenarios_to_use[ctx->test_id].ce) |
			RERI_STATUS_SET_UEC(ctx->scenarios_to_use[ctx->test_id].uec) |
			RERI_STATUS_SET_UED(ctx->scenarios_to_use[ctx->test_id].ued) |
			RERI_STATUS_SET_TT(ctx->scenarios_to_use[ctx->test_id].tt)  |
			RERI_STATUS_SET_EC(ctx->scenarios_to_use[ctx->test_id].ec),
			RERI_STATUS_SET_CE(RERI_STATUS_CE_MASK) |
			RERI_STATUS_SET_UEC(RERI_STATUS_UEC_MASK) |
			RERI_STATUS_SET_UED(RERI_STATUS_UED_MASK) |
			RERI_STATUS_SET_TT(RERI_STATUS_TT_MASK)  |
			RERI_STATUS_SET_EC(RERI_STATUS_EC_MASK),
			RERI_STATUS_I(ctx->bank_addr, ctx->test_id));

	/* Trigger: inject error */
	reri_set(RERI_CTRL_ELSE_EN, RERI_CONTROL_I(ctx->bank_addr, ctx->test_id));
	reri_write_mask(RERI_CTRL_SET_CES(RAS_SIGNAL_HIGH_PRIORITY) |
			RERI_CTRL_SET_UEDS(RAS_SIGNAL_HIGH_PRIORITY) |
			RERI_CTRL_SET_UECS(RAS_SIGNAL_HIGH_PRIORITY) |
			RERI_CTRL_SET_EID(ctx->eid),
			RERI_CTRL_SET_CES(RERI_CTRL_CES_MASK) |
			RERI_CTRL_SET_UEDS(RERI_CTRL_UEDS_MASK) |
			RERI_CTRL_SET_UECS(RERI_CTRL_UECS_MASK) |
			RERI_CTRL_SET_EID(RERI_CTRL_EID_MASK),
			RERI_CONTROL_I(ctx->bank_addr, ctx->test_id));
	reri_clear(RERI_CTRL_SINV_EN, RERI_CONTROL_I(ctx->bank_addr, ctx->test_id));

	con_log(RERI_MODNAME "[CPU%d]: record_index=%d, status=0x%llx\n",
		current_hartid(), ctx->test_id, reri_read(RERI_STATUS_I(ctx->bank_addr, ctx->test_id)));
	con_log(RERI_MODNAME "[CPU%d]: record_index=%d, eid=0x%x, RERI_CTRL_SET_EID(ctx->eid)=0x%llx, control=0x%llx\n",
		current_hartid(), ctx->test_id, ctx->eid, RERI_CTRL_SET_EID(ctx->eid), reri_read(RERI_CONTROL_I(ctx->bank_addr, ctx->test_id)));

	ctx->source_info = get_source_info(RERI_INST_ID(reri_read(ctx->bank_addr + RERI_BANK_INFO)));
	if (ctx->source_info)
		con_log(RERI_MODNAME "[CPU%d]: source_info: %s\n", current_hartid(), ctx->source_info->name);
	else
		con_log(RERI_MODNAME "[CPU%d]: unknown source id\n", current_hartid());

	if (ctx->test_target_type == RERI_EVENT_SOURCE_CPU) {
		generic_reri_irq_handler(IRQ_RAS_CE);
	} else {
		generic_reri_irq_handler(IRQ_IOMMU_IOATS0_RAS);
	}

	return ret;
}

int reri(caddr_t percpu_area)
{
	int ret = 0;

	ret = do_reri_test(0, NULL);
	return ret;
}

__define_testfn(reri, sizeof(struct reri_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
#endif
