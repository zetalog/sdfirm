/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */
#include <target/sbi.h>
#include <target/rpmi.h>
#include <target/mbox.h>

/**
 * Per hart RPMI CPPC fast channel size (bytes)
 * PASSIVE MODE:
 *	0x0: DESIRED_PERFORMANCE (4-byte)
 *	0x4: __RESERVED	(4-byte)
 * ACTIVE MODE: (not supported yet)
 *	0x0: MINIMUM PERFORMANCE (4-byte)
 *	0x4: MAXIMUM PERFORMANCE (4-byte)
 */
#define RPMI_CPPC_HART_FASTCHAN_SIZE		0x8

struct rpmi_cppc {
	struct mbox_chan *chan;
	bool fc_supported;
	bool fc_db_supported;
	enum rpmi_cppc_fast_channel_db_width fc_db_width;
	enum rpmi_cppc_fast_channel_cppc_mode mode;
	ulong fc_perf_request_addr;
	ulong fc_perf_feedback_addr;
	ulong fc_db_addr;
	u64 fc_db_setmask;
	u64 fc_db_preservemask;
};

struct rpmi_cppc g_rpmi_cppc[NR_CPUS];

static struct rpmi_cppc *rpmi_cppc_get_pointer(u32 hartid)
{
	return &g_rpmi_cppc[hartid];
}

static void rpmi_cppc_fc_db_trigger(struct rpmi_cppc *cppc)
{
	u8 db_val_u8 = 0;
	u16 db_val_u16 = 0;
	u32 db_val_u32 = 0;
	u64 db_val_u64 = 0;

	switch (cppc->fc_db_width) {
	case RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_8:
		db_val_u8 = readb((void *)cppc->fc_db_addr);
		db_val_u8 = (u8)cppc->fc_db_setmask |
				(db_val_u8 & (u8)cppc->fc_db_preservemask);

		writeb(db_val_u8, (void *)cppc->fc_db_addr);
		break;
	case RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_16:
		db_val_u16 = readw((void *)cppc->fc_db_addr);
		db_val_u16 = (u16)cppc->fc_db_setmask |
				(db_val_u16 & (u16)cppc->fc_db_preservemask);

		writew(db_val_u16, (void *)cppc->fc_db_addr);
		break;
	case RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_32:
		db_val_u32 = readl((void *)cppc->fc_db_addr);
		db_val_u32 = (u32)cppc->fc_db_setmask |
				(db_val_u32 & (u32)cppc->fc_db_preservemask);

		writel(db_val_u32, (void *)cppc->fc_db_addr);
		break;
	case RPMI_CPPC_FAST_CHANNEL_DB_WIDTH_64:
#if __riscv_xlen != 32
		db_val_u64 = readq((void *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);

		writeq(db_val_u64, (void *)cppc->fc_db_addr);
#else
		db_val_u64 = readl((void *)(cppc->fc_db_addr + 4));
		db_val_u64 <<= 32;
		db_val_u64 |= readl((void *)cppc->fc_db_addr);
		db_val_u64 = cppc->fc_db_setmask |
				(db_val_u64 & cppc->fc_db_preservemask);
		writel(db_val_u64, (void *)cppc->fc_db_addr);
		writel(db_val_u64 >> 32, (void *)(cppc->fc_db_addr + 4));
#endif
		break;
	default:
		break;
	}
}

static int rpmi_cppc_probe(unsigned long reg)
{
	int rc;
	struct rpmi_cppc *cppc;
	struct rpmi_cppc_probe_resp resp;
	struct rpmi_cppc_probe_req req;

	req.hart_id = sbi_current_hartid();
	req.reg_id = reg;

	cppc = rpmi_cppc_get_pointer(req.hart_id);
	if (!cppc)
		return SBI_ENOSYS;

	rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_PROBE_REG,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	if (rc)
		return rc;

	return resp.reg_len;
}

int rpmi_cppc_read(unsigned long reg, uint64_t *val)
{
	int rc = SBI_SUCCESS;
	struct rpmi_cppc_read_reg_req req;
	struct rpmi_cppc_read_reg_resp resp;
	struct rpmi_cppc *cppc;

	req.hart_id = csr_read(CSR_MHARTID);
	req.reg_id = reg;
	cppc = rpmi_cppc_get_pointer(req.hart_id);

	rc = rpmi_cppc_probe(reg);
	if (rc <= 0)
		return -1;

	rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_READ_REG,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	if (rc)
		return rc;

#if __riscv_xlen == 32
	*val = resp.data_lo;
#else
	*val = (u64)resp.data_hi << 32 | resp.data_lo;
#endif
	return rc;
}

int rpmi_cppc_write(unsigned long reg, uint64_t val)
{
	int rc = SBI_SUCCESS;
	u32 hart_id = sbi_current_hartid();
	struct rpmi_cppc_write_reg_req req;
	struct rpmi_cppc_write_reg_resp resp;
	struct rpmi_cppc *cppc = rpmi_cppc_get_pointer(hart_id);

	rc = rpmi_cppc_probe(reg);
	if (rc <= 0)
		return -1;

	if (reg != SBI_CPPC_DESIRED_PERF || !cppc->fc_supported) {
		req.hart_id = hart_id;
		req.reg_id = reg;
		req.data_lo = val & 0xFFFFFFFF;
		req.data_hi = val >> 32;

		rc = rpmi_normal_request_with_status(
			cppc->chan, RPMI_CPPC_SRV_WRITE_REG,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	} else {
		/* use fast path writes for desired_perf in passive mode */
		writel((u32)val, (void *)cppc->fc_perf_request_addr);

		if (cppc->fc_db_supported)
			rpmi_cppc_fc_db_trigger(cppc);
	}

	return rc;
}

static int rpmi_cppc_update_hart_scratch(struct mbox_chan *chan)
{
	int rc, i;
	bool fc_supported = false;
	bool fc_db_supported = false;
	struct rpmi_cppc_hart_list_req req;
	struct rpmi_cppc_hart_list_resp resp;
	struct rpmi_cppc_get_fastchan_offset_req hfreq;
	struct rpmi_cppc_get_fastchan_offset_resp hfresp;
	struct rpmi_cppc_get_fastchan_region_resp fresp;
	enum rpmi_cppc_fast_channel_db_width fc_db_width = 0;
	enum rpmi_cppc_fast_channel_cppc_mode cppc_mode = 0;
	struct rpmi_cppc *cppc;
	unsigned long fc_region_addr = 0;
	unsigned long fc_db_addr = 0;
	u64 fc_db_setmask = 0;
	u64 fc_db_preservemask = 0;

	rc = rpmi_normal_request_with_status(
		chan, RPMI_CPPC_SRV_GET_FAST_CHANNEL_REGION,
		NULL, 0, 0,
		&fresp, rpmi_u32_count(fresp), rpmi_u32_count(fresp));
	if (rc && rc != SBI_ENOTSUPP)
		return rc;

	/* At this point fast channel availability is confirmed */
	fc_supported = (rc != SBI_ENOTSUPP)? true : false;

	/* If fast channel is supported, add the fast channel
	 * region in root domain as MMIO RW. And, get the doorbell
	 * information from the response */
	if (fc_supported) {
#if __riscv_xlen == 32
		fc_region_addr = fresp.region_addr_lo;
		fc_region_size = fresp.region_size_lo;
		fc_db_addr = fresp.db_addr_lo;
#else
		fc_region_addr = (ulong)fresp.region_addr_hi << 32 |
							fresp.region_addr_lo;
		fc_db_addr = (ulong)fresp.db_addr_hi << 32 | fresp.db_addr_lo;

#endif

		cppc_mode = (fresp.flags & RPMI_CPPC_FAST_CHANNEL_CPPC_MODE_MASK) >>
					RPMI_CPPC_FAST_CHANNEL_CPPC_MODE_POS;
		fc_db_supported = fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_SUPPORTED;
		fc_db_width = (fresp.flags &
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_WIDTH_MASK) >>
				RPMI_CPPC_FAST_CHANNEL_FLAGS_DB_WIDTH_POS;
		fc_db_setmask = (u64)fresp.db_setmask_hi << 32 |
						fresp.db_setmask_lo;
		fc_db_preservemask = (u64)fresp.db_preservemask_hi << 32 |
						fresp.db_preservemask_lo;
	}

	/* Get the hart list and depending on the fast channel support
	 * initialize the per hart cppc structure */
	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_CPPC_SRV_GET_HART_LIST,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		if (rc)
			return rc;

		/* For each returned harts, get their fastchan offset and
		 * complete the initialization of per hart cppc structure */
		for (i = 0; i < resp.returned; i++) {
			cppc = rpmi_cppc_get_pointer(resp.hartid[i]);
			if (!cppc)
				return SBI_ENOSYS;

			cppc->chan = chan;
			cppc->mode = cppc_mode;
			cppc->fc_supported = fc_supported;

			if (fc_supported) {
				hfreq.hart_id = resp.hartid[i];
				rc = rpmi_normal_request_with_status(
						chan,
						RPMI_CPPC_SRV_GET_FAST_CHANNEL_OFFSET,
						&hfreq,
						rpmi_u32_count(hfreq),
						rpmi_u32_count(hfreq),
						&hfresp,
						rpmi_u32_count(hfresp),
						rpmi_u32_count(hfresp));
				if (rc)
					continue;

#if __riscv_xlen == 32
				cppc->fc_perf_request_addr = fc_region_addr +
							hfresp.fc_perf_request_offset_lo;
				cppc->fc_perf_feedback_addr = fc_region_addr +
							hfresp.fc_perf_feedback_offset_lo;
#else
				cppc->fc_perf_request_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_request_offset_hi << 32 |
					hfresp.fc_perf_request_offset_lo);

				cppc->fc_perf_feedback_addr = fc_region_addr +
					((ulong)hfresp.fc_perf_feedback_offset_hi << 32 |
					hfresp.fc_perf_feedback_offset_lo);
#endif
				cppc->fc_db_supported = fc_db_supported;
				cppc->fc_db_addr = fc_db_addr;
				cppc->fc_db_width = fc_db_width;

				cppc->fc_db_setmask = fc_db_setmask;
				cppc->fc_db_preservemask = fc_db_preservemask;
			}
			else {
				cppc->fc_perf_request_addr = 0;
				cppc->fc_perf_feedback_addr = 0;
				cppc->fc_db_supported = 0;
				cppc->fc_db_addr = 0;
				cppc->fc_db_width = 0;
				cppc->fc_db_setmask = 0;
				cppc->fc_db_preservemask = 0;
			}
		}
		req.start_index += resp.returned;
	} while (resp.remaining);

	return 0;
}

void rpmi_cppc_init(void)
{
	int rc;
	struct mbox_controller *mbox;
	uint32_t chan_args[2];

	mbox = rpmi_get_controller();
	if (!mbox) {
		printf("RPMI CPPC: get mailbox controller failed\n");
		return;
	}

	chan_args[0] = RPMI_SRVGRP_CPPC;
	chan_args[1] = RPMI_VERSION(1, 0);

	g_rpmi_cppc->chan = mbox_controller_request_chan(mbox, chan_args);
	if (!g_rpmi_cppc->chan) {
		printf("%s: failed to request channel\n", __func__);
		return;
	}

	/* Update per-HART scratch space */
	rc = rpmi_cppc_update_hart_scratch(g_rpmi_cppc->chan);
	if (rc) {
		mbox_controller_free_chan(g_rpmi_cppc->chan);
		return;
	}
}

