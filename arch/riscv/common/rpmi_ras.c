#include <target/rpmi.h>
// #include <sbi_utils/ras/ghes.h>
#include <target/types.h>
#include <target/reri.h>

struct rpmi_ras {
	struct mbox_chan *chan;
};

static struct rpmi_ras g_ras;

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

int rpmi_ras_sync_reri_errs(u32 *pending_vectors, u32 *nr_pending,
			    u32 *nr_remaining)
{
	int rc = 0;
	acpi_ghesv2 err_src;
	acpi_ghes_status_block *sblock;
	u64 *gas;

	//FIXME: source_id
	rc = acpi_ghes_get_err_src_desc(/*source_id*/0, &err_src);
	if (rc)
		return -1;

	/*
	 * FIXME: Read gas address via a function that respects the
	 * gas parameters. Don't read directly after typecast.
	 */
	gas = (u64 *)(unsigned long)err_src.ghes.gas.address;
	sblock = (acpi_ghes_status_block *)(unsigned long)(*gas);

	if (!pending_vectors || !nr_pending || !nr_remaining)
		return -1;

	*nr_pending = *nr_remaining = 0;

	if (!g_ras.chan)
		return -1;

	rc = rpmi_posted_request(g_ras.chan,
				 RPMI_RAS_SRV_SYNC_HART_ERR_REQ,
				 sblock, sblock->data_len / sizeof(u32),
				 0);

	if (rc) {
		printf("%s: sync failed, rc: 0x%x\n", __func__, rc);
		return rc;
	}

	return 0;
}

void rpmi_ras_init(void)
{
	struct mbox_controller *mbox;
	uint32_t chan_args[2];

	mbox = rpmi_shmem_get_controller();
	if (!mbox)
		return;

	chan_args[0] = RPMI_SRVGRP_RAS_AGENT;
	chan_args[1] = RPMI_VERSION(1, 0);

	g_ras.chan = mbox_controller_request_chan(mbox, chan_args);
	if (!g_ras.chan)
		printf("%s: failed to request channel\n", __func__);
}

