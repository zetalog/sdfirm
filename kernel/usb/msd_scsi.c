#include <target/usb_msd.h>

msd_cmpl_cb msd_scsi_cmpl = NULL;
scsi_pid_t msd_scsi_pid = INVALID_SCSI_PID;
scsi_crn_t msd_scsi_crn = INVALID_SCSI_CRN;

#define MSD_VER_DESC_PHYS_USB11	0x1728
#define MSD_VER_DESC_PHYS_USB20	0x1729

#if USB_VERSION_DEFAULT == 0x110
#define MSD_VER_DESC_PHYS_USB	MSD_VER_DESC_PHYS_USB11
#elif USB_VERSION_DEFAULT == 0x200
#define MSD_VER_DESC_PHYS_USB	MSD_VER_DESC_PHYS_USB20
#else
#error "USB version is not supported"
#endif
#define MSD_VER_DESC_XPRT_BBB	0x1730

uint8_t msd_class_max_luns(void)
{
	return scsi_max_units();
}

static boolean msd_class_dir_matched(void)
{
	if (msd_proto_get_expect() == 0) {
		return true;
	} else if (scsi_get_direction() == SCSI_CMND_IN) {
		return msd_proto_get_dir() == USB_DIR_IN;
	} else {
		return msd_proto_get_dir() == USB_DIR_OUT;
	}
}

boolean msd_class_scsi_ok(scsi_status_t err)
{
	switch (err) {
	case SCSI_STATUS_CONDITION_MET:
	case SCSI_STATUS_INTERMEDIATE_CONDITION_MET:
	case SCSI_STATUS_GOOD:
	case SCSI_STATUS_INTERMEDIATE:
		return true;
	case SCSI_STATUS_CHECK_CONDITION:
	case SCSI_STATUS_BUSY:
	case SCSI_STATUS_RESERVATION_CONFLICT:
	case SCSI_STATUS_TASK_SET_FULL:
	case SCSI_STATUS_ACA_ACTIVE:
	case SCSI_STATUS_TASK_ABORTED:
	default:
		return false;
	}
}

boolean msd_class_cb_valid(uint8_t *cb, uint8_t len)
{
	msd_scsi_crn = scsi_command_validate(0, 0, msd_scsi_pid,
					     msd_proto_get_lun(),
					     cb, len, SCSI_TASK_SIMPLE);
	if (msd_scsi_crn == INVALID_SCSI_CRN)
		return false;

	/* XXX: Command Status Determination
	 * +--------------------------+---------------------------------+
	 * | Conditions		      |	Results				|
	 * +--------------------------+---------------------------------+
	 * | dwCBWDataTransfer	DIR   |	dwCSWStatus	dCSWDataResidue |
	 * +--------------------------+---------------------------------+
	 * | H>=D		=     |	Passed		H-D		|
	 * |			      |	Failed				|
	 * +--------------------------+---------------------------------+
	 * | H<D(1)		=     |	Halted		H		|
	 * +--------------------------+---------------------------------+
	 * | -			!=(2) |	Halted		H		|
	 * +--------------------------+---------------------------------+
	 * Where:
	 * 1. !msd_class_len_matched which is called when completion
	 * 2. !msd_class_dir_matched which is called here
	 *
	 * Actual transfer data length should be:
	 * 1. =dwCBWDataTransferLength with padding
	 * 2. <dwCBWDataTransferLength with halting
	 */
	if (!msd_class_dir_matched() ||
	    !msd_class_scsi_ok(scsi_command_status(msd_scsi_crn))) {
		/* HALTED status - thus deleting the SCSI commands */
		msd_class_cb_complete();
		return false;
	}

	/* Command is now ready for execution. */
	return true;
}

static void msd_scsi_command_cmpl(void)
{
	if (msd_scsi_cmpl) {
		msd_scsi_cmpl(msd_class_get_status());
		msd_scsi_cmpl = NULL;
	}
}

uint8_t msd_scsi_inquiry(uint8_t page_code, uint8_t offset)
{
	return 0;
}

void msd_class_cb_complete(void)
{
	if (msd_scsi_crn != INVALID_SCSI_CRN) {
		scsi_command_complete(msd_scsi_crn);
		msd_scsi_crn = INVALID_SCSI_CRN;
	}
}

uint32_t msd_class_get_expect(void)
{
	if (msd_scsi_crn == INVALID_SCSI_CRN)
		return 0;
	return scsi_command_expect(msd_scsi_crn);
}

boolean msd_class_get_status(void)
{
	if (msd_scsi_crn == INVALID_SCSI_CRN)
		return false;
	return msd_class_scsi_ok(scsi_command_status(msd_scsi_crn));
}

boolean msd_class_cb_schedule(msd_cmpl_cb call)
{
	BUG_ON(msd_scsi_crn == INVALID_SCSI_CRN);

	if (!scsi_command_schedule(msd_scsi_crn, true)) {
		msd_scsi_cmpl = call;
		return false;
	}
	return true;
}

void msd_class_io(void)
{
	scsi_command_bulkio(msd_scsi_crn);
}

scsi_transport_t msd_scsi_xprt = {
	SCSI_PROTO_AT,
	MSD_VER_DESC_PHYS_USB,
	MSD_VER_DESC_XPRT_BBB,
	msd_scsi_inquiry,
	msd_scsi_command_cmpl,

	msd_proto_bulk_type,
	msd_proto_bulk_size,
	msd_proto_bulk_open,
	msd_proto_bulk_close,
	msd_proto_bulk_put,
	msd_proto_bulk_get,
	msd_proto_bulk_write,
	msd_proto_bulk_read,
};

void msd_class_reset(void)
{
}

void msd_class_start(void)
{
}

void msd_class_init(void)
{
	msd_scsi_pid = scsi_register_transport(&msd_scsi_xprt);
}
