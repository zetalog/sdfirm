#include <target/sbi.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>
#include <sbi_utils/ras/apei_tables.h>
#include <sbi_utils/ras/ghes.h>

struct rpmi_ras {
        struct mbox_chan *chan;
};

extern struct mbox_chan g_chan;

static struct rpmi_ras ras = {
	.chan = &g_chan
};

int rpmi_ras_sync_hart_errs(u32 *pending_vectors, u32 *nr_pending,
			    u32 *nr_remaining)
{
	int rc = 0;
	struct rpmi_ras_sync_hart_err_req req;
	struct rpmi_ras_sync_err_resp resp;

	if (!pending_vectors || !nr_pending || !nr_remaining)
		return -1;

	*nr_pending = *nr_remaining = 0;

	if (!ras.chan)
		return -1;

	rc = rpmi_normal_request_with_status(ras.chan,
					     RPMI_RAS_SRV_SYNC_HART_ERR_REQ,
					     &req, rpmi_u32_count(req),
					     rpmi_u32_count(req),
					     &resp, rpmi_u32_count(resp),
					     rpmi_u32_count(resp));

	if (rc) {
		printf("%s: sync failed, rc: 0x%x\n", __func__, rc);
		return rc;
	}

	if (!resp.status && resp.returned > 0 && resp.returned < MAX_PEND_VECS) {
		memcpy(pending_vectors, resp.pending_vecs,
		       resp.returned * sizeof(u32));
		*nr_pending = resp.returned;
		*nr_remaining = resp.remaining;
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
	gas = (u64 *)(ulong)err_src.ghes.gas.address;
	sblock = (acpi_ghes_status_block *)(ulong)(*gas);

	if (!pending_vectors || !nr_pending || !nr_remaining)
		return -1;

	*nr_pending = *nr_remaining = 0;

	if (!ras.chan)
		return -1;

	rc = rpmi_posted_request(ras.chan,
				 RPMI_RAS_SRV_SYNC_HART_ERR_REQ,
				 sblock, sblock->data_len / sizeof(u32),
				 0);

	if (rc) {
		printf("%s: sync failed, rc: 0x%x\n", __func__, rc);
		return rc;
	}

	return 0;
}
