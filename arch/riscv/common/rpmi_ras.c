#include <target/rpmi.h>
#include <target/types.h>
#include <target/reri.h>
#include <target/console.h>
#include <target/acpi.h>
#include <target/mbox.h>

struct rpmi_ras g_ras;

#ifndef CONFIG_SPACEMIT_RAS
#define SPACEMIT_RAS_MSG_TYPE_HART_ERR    0x1001
#define SPACEMIT_RAS_MSG_TYPE_IOMMU_ERR   0x1002

/* Define spacemit_ras_msg structure */
struct spacemit_ras_msg {
	uint32_t type;
	uint32_t len;
	uint8_t data[256]; /* Adjust size as needed */
};

static int ras_convert_reri_to_ghes(struct spacemit_ras_error_record *record,
				  acpi_ghes_error_info *einfo)
{
	if (!record || !einfo)
		return -1;

	memset(einfo, 0, sizeof(*einfo));

	if (RERI_STATUS_GET_CE(record->status))
		einfo->info.gpe.sev = 2;
	else if (RERI_STATUS_GET_UED(record->status))
		einfo->info.gpe.sev = 0;
	else if (RERI_STATUS_GET_UEC(record->status))
		einfo->info.gpe.sev = 1;
	else
		einfo->info.gpe.sev = 3;

	if (RERI_STATUS_GET_TT(record->status)) {
		einfo->info.gpe.validation_bits |= CPER_PROC_VALID_OPERATION;
		switch (RERI_STATUS_GET_TT(record->status)) {
		case RERI_TT_IMPLICIT_READ:
			einfo->info.gpe.operation = 3;
			break;
		case RERI_TT_EXPLICIT_READ:
			einfo->info.gpe.operation = 1;
			break;
		case RERI_TT_IMPLICIT_WRITE:
		case RERI_TT_EXPLICIT_WRITE:
			einfo->info.gpe.operation = 2;
			break;
		default:
			einfo->info.gpe.operation = 0;
			break;
		}
	}

	switch (RERI_STATUS_GET_EC(record->status)) {
	case RERI_EC_CBA:
	case RERI_EC_CSD:
	case RERI_EC_CAS:
	case RERI_EC_CUE:
		einfo->info.gpe.proc_err_type = 0x01;
		break;
	case RERI_EC_TPD:
	case RERI_EC_TPA:
	case RERI_EC_TPU:
		einfo->info.gpe.proc_err_type = 0x02;
		break;
	case RERI_EC_SBE:
		einfo->info.gpe.proc_err_type = 0x04;
		break;
	default:
		einfo->info.gpe.proc_err_type = 0x08;
		break;
	}

	if (RERI_STATUS_GET_AIT(record->status)) {
		einfo->info.gpe.validation_bits |= CPER_PROC_VALID_TARGET_ADDRESS;
		einfo->info.gpe.target_addr = record->addr_info;
	}

	einfo->info.gpe.validation_bits |= (CPER_PROC_VALID_TYPE |
					  CPER_PROC_VALID_ISA |
					  CPER_PROC_VALID_ERROR_TYPE);
	einfo->info.gpe.proc_type = GHES_PROC_TYPE_RISCV;
	einfo->info.gpe.proc_isa = GHES_PROC_ISA_RISCV64;

	return 0;
}

static int ras_sync_to_cpu(struct spacemit_ras_error_record *record)
{
	struct spacemit_ras_msg msg;
	int ret;

	if (!record || !g_ras.chan) {
		con_err("SPACEMIT RAS: Invalid parameters or channel\n");
		return -1;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = SPACEMIT_RAS_MSG_TYPE_HART_ERR;
	msg.len = sizeof(struct spacemit_ras_error_record);
	memcpy(msg.data, record, sizeof(struct spacemit_ras_error_record));

	ret = rpmi_normal_request_with_status(g_ras.chan,
					    RPMI_RAS_SRV_SYNC_HART_ERR_REQ,
					    &msg, sizeof(msg),
					    sizeof(msg),
					    NULL, 0, 0);
	if (ret) {
		con_err("SPACEMIT RAS: Failed to sync error record, ret=%d\n", ret);
		return ret;
	}

	con_log("SPACEMIT RAS: Successfully synced error record (inst_id=%u)\n",
		record->inst_id);

	return 0;
}

static int ras_sync_to_ghes(struct spacemit_ras_error_record *record)
{
	acpi_ghes_error_info einfo;
	int ret;

	if (!record)
		return -1;

	ret = ras_convert_reri_to_ghes(record, &einfo);
	if (ret)
		return ret;

	acpi_ghes_record_errors(record->inst_id, &einfo);
	return 0;
}

int spacemit_ras_sync_hart_errs(struct spacemit_ras_error_record *error_record)
{
	struct spacemit_ras_error_record record;
	int ret;

	if (!error_record)
		return -1;

	memcpy(&record, error_record, sizeof(record));

	ret = ras_sync_to_cpu(&record);
	if (ret)
		return ret;

	return ras_sync_to_ghes(&record);
}

/*
 * Sync error record to RMU
 *
 * @param error_record: Pointer to the error record to sync.
 *
 * @return 0 on success, -1 on failure.
*/
int spacemit_ras_sync_error_record(struct spacemit_ras_error_record *error_record)
{
	struct spacemit_ras_error_record record;

	if (!error_record)
		return -1;

	memcpy(&record, error_record, sizeof(record));

	return ras_sync_to_cpu(&record);
}
#endif /* CONFIG_SPACEMIT_RAS */

/*
 * Sync pending error vectors for a specific hart.
 * RMU send request to ACPU by RPMI to get error vectors.
 *
 * @param pending_vectors: Pointer to the array to store the pending error vectors.
 * @param nr_pending: Pointer to the number of pending error vectors.
 * @param nr_remaining: Pointer to the number of remaining error vectors.
 *
 * @return 0 on success, -1 on failure.
*/
int rpmi_ras_sync_hart_errs(u32 *pending_vectors, u32 *nr_pending,
				u32 *nr_remaining)
{
	int rc = 0;
	struct rpmi_ras_sync_hart_err_req req;
	struct rpmi_ras_sync_err_resp resp;

	if (!pending_vectors || !nr_pending || !nr_remaining) {
		printf("%s: Invalid parameters\n", __func__);
		return -1;
	}

	*nr_pending = *nr_remaining = 0;

	if (!g_ras.chan) {
		printf("%s: RPMI channel not initialized\n", __func__);
		return -1;
	}

	req.hart_id = ((unsigned int)csr_read(CSR_MHARTID));
	printf("%s: Syncing errors for hart_id=%u\n", __func__, req.hart_id);

	rc = rpmi_normal_request_with_status(g_ras.chan,
						 RPMI_RAS_SRV_SYNC_HART_ERR_REQ,
						 &req, rpmi_u32_count(req),
						 rpmi_u32_count(req),
						 &resp, rpmi_u32_count(resp),
						 rpmi_u32_count(resp));
	printf("%s: sync hart errs, rc: 0x%x\n", __func__, rc);
	if (rc) {
		printf("%s: sync failed, rc: 0x%x\n", __func__, rc);
		return rc;
	}

	if (!resp.status && resp.returned > 0 && resp.returned < MAX_PEND_VECS) {
		memcpy(pending_vectors, resp.pending_vecs,
			   resp.returned * sizeof(u32));
		*nr_pending = resp.returned;
		*nr_remaining = resp.remaining;
		printf("%s: sync success, nr_pending: %u, nr_remaining: %u\n",
			   __func__, *nr_pending, *nr_remaining);
		printf("%s: pending vectors: ", __func__);
		for (int i = 0; i < *nr_pending; i++) {
			printf("0x%x ", pending_vectors[i]);
		}
		printf("\n");
	} else {
		if (resp.status) {
			printf("%s: sync returned status %d\n",
				   __func__, resp.status);
		}

		if (resp.returned < 0 || resp.returned > MAX_PEND_VECS)
			printf("%s: invalid vector range returned %u\n",
				   __func__, resp.returned);

		return -1;
	}

	return 0;
}

int rpmi_ras_sync_reri_errs(uint32_t *pending_vectors, uint32_t *nr_pending,
				uint32_t *nr_remaining)
{
	int rc = 0;
	struct acpi_hest_generic_v2 err_src;
	acpi_ghes_status_block *sblock;
	uint64_t *gas;

	//FIXME: source_id
	rc = acpi_ghes_get_err_src_desc(/*source_id*/0, &err_src);
	if (rc)
		return -1;

	/*
	 * FIXME: Read gas address via a function that respects the
	 * gas parameters. Don't read directly after typecast.
	 */
	gas = (uint64_t *)(unsigned long)err_src.error_status_address.address;
	sblock = (acpi_ghes_status_block *)(unsigned long)(*gas);

	if (!pending_vectors || !nr_pending || !nr_remaining)
		return -1;

	*nr_pending = *nr_remaining = 0;

	if (!g_ras.chan)
		return -1;

	rc = rpmi_posted_request(g_ras.chan,
				 RPMI_RAS_SRV_SYNC_HART_ERR_REQ,
				 sblock, sblock->status.data_length / sizeof(uint32_t),
				 0);

	if (rc) {
		printf("%s: sync failed, rc: 0x%x\n", __func__, rc);
		return rc;
	}

	return 0;
}

#define GET_SERVICE_ID(msg)			\
({						\
	struct rpmi_message *mbuf = msg;	\
	mbuf->header.service_id;		\
})


static void ras_message_handler_internal(struct mbox_chan *chan, uint32_t msg_id, struct mbox_xfer *xfer)
{
	int rc = 0;
	struct rpmi_ras_sync_err_resp *resp = NULL;
	uint32_t pending_vectors[MAX_PEND_VECS];
	uint32_t nr_pending = 0, nr_remaining = 0;
	uint32_t src_list[MAX_ERR_SRCS];
	struct acpi_hest_generic_v2 err_src;
	uint32_t *req_data = NULL;
	uint32_t *resp_data = NULL;

	/* Initialize response buffer if available */
	if (xfer && xfer->rx) {
		resp = (struct rpmi_ras_sync_err_resp *)xfer->rx;
		resp_data = (uint32_t *)xfer->rx;
	}

	switch (msg_id) {
	case RPMI_RAS_SRV_SYNC_HART_ERR_REQ: {
		struct rpmi_ras_sync_hart_err_req *req = NULL;

		/* sync error vectors */
		rc = rpmi_ras_sync_hart_errs(pending_vectors, &nr_pending, &nr_remaining);
		if (rc) {
			con_log("RPMI RAS: sync error vectors failed (rc=%d)\n", rc);
			if (resp) {
				resp->status = rc;
			}
			return;
		}

		/* fill response */
		if (resp) {
			resp->status = 0;
			resp->returned = nr_pending;
			resp->remaining = nr_remaining;
			if (nr_pending > 0) {
				memcpy(resp->pending_vecs, pending_vectors,
					   nr_pending * sizeof(uint32_t));
			}
		}
		break;
	}

	case RPMI_RAS_SRV_SYNC_DEV_ERR_REQ:
		rc = rpmi_ras_sync_reri_errs(NULL, &nr_pending, &nr_remaining);
		if (rc) {
			con_log("RPMI RAS: sync RERI error failed (rc=%d)\n", rc);
			return;
		}
		break;

	case RPMI_RAS_SRV_GET_NUM_ERR_SRCS:
		con_log("RPMI RAS: get number of error sources\n");
		rc = acpi_ghes_get_num_err_srcs();
		if (rc < 0) {
			con_log("RPMI RAS: get number of error sources failed (rc=%d)\n", rc);
			return;
		}
		/* Return the number of error sources */
		if (resp_data) {
			resp_data[0] = rc;
		}
		break;

	case RPMI_RAS_SRV_GET_ERR_SRCS_ID_LIST:
		con_log("RPMI RAS: get error sources ID list\n");
		rc = acpi_ghes_get_err_srcs_list(src_list, MAX_ERR_SRCS);
		if (rc < 0) {
			con_log("RPMI RAS: get error sources ID list failed (rc=%d)\n", rc);
			return;
		}
		/* Return the error sources list */
		if (resp_data) {
			resp_data[0] = rc; /* number of sources */
			memcpy(&resp_data[1], src_list, rc * sizeof(uint32_t));
		}
		break;

	case RPMI_RAS_SRV_GET_ERR_SRC_DESC:
		/* Get source_id from request */
		uint32_t src_id = req_data ? req_data[0] : 0;
		rc = acpi_ghes_get_err_src_desc(src_id, &err_src);
		if (rc) {
			con_log("RPMI RAS: get error source description failed (rc=%d)\n", rc);
			return;
		}
		/* Return the error source description */
		if (resp_data) {
			memcpy(resp_data, &err_src, sizeof(err_src));
		}
		break;

	default:
		con_log("RPMI RAS: unknown service ID (0x%x)\n", msg_id);
		break;
	}
}

static void ras_message_handler(struct mbox_chan *chan, struct mbox_xfer *xfer)
{
	uint32_t msg_id = 0;

	if (xfer && xfer->rx) {
		msg_id = GET_SERVICE_ID(xfer->rx);
	}

	ras_message_handler_internal(chan, msg_id, xfer);
}

void rpmi_ras_init(void)
{
	struct mbox_controller *mbox;
	uint32_t chan_args[2];
	int rc;

	mbox = rpmi_get_controller();
	if (!mbox) {
		printf("RPMI RAS: get mailbox controller failed\n");
		return;
	}

	/* register RAS service */
	chan_args[0] = RPMI_SRVGRP_RAS_AGENT;
	chan_args[1] = RPMI_VERSION(1, 0);
	g_ras.chan = mbox_controller_request_chan(mbox, chan_args);

	if (!g_ras.chan) {
		printf("RPMI RAS: request RAS service channel failed\n");
		return;
	}

	/* register message handler */
	rc = rpmi_register_handler(RPMI_RAS_SRV_SYNC_HART_ERR_REQ, ras_message_handler);
	if (rc) {
		printf("RPMI RAS: register HART error handler failed (rc=%d)\n", rc);
		return;
	}

	rc = rpmi_register_handler(RPMI_RAS_SRV_SYNC_DEV_ERR_REQ, ras_message_handler);
	if (rc) {
		printf("RPMI RAS: register RERI error handler failed (rc=%d)\n", rc);
		return;
	}

	rc = rpmi_register_handler(RPMI_RAS_SRV_GET_NUM_ERR_SRCS, ras_message_handler);
	if (rc) {
		printf("RPMI RAS: register get number of error sources handler failed (rc=%d)\n", rc);
		return;
	}

	rc = rpmi_register_handler(RPMI_RAS_SRV_GET_ERR_SRCS_ID_LIST, ras_message_handler);
	if (rc) {
		printf("RPMI RAS: register get error sources ID list handler failed (rc=%d)\n", rc);
		return;
	}

	rc = rpmi_register_handler(RPMI_RAS_SRV_GET_ERR_SRC_DESC, ras_message_handler);
	if (rc) {
		printf("RPMI RAS: register get error source description handler failed (rc=%d)\n", rc);
		return;
	}

	printf("RPMI RAS: initialized\n");
}

