#include <target/icc.h>
#include <target/ifd.h>

__near__ scd_t icc_ifds[NR_IFD_SLOTS];

scd_t icc_ifd_dev(ifd_sid_t sid)
{
	BUG_ON(sid >= NR_IFD_SLOTS);
	return icc_ifds[sid];
}

ifd_sid_t icc_ifd_slot(scd_t scd)
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		if (scd == icc_ifds[sid])
			return sid;
	}
	BUG();
	return INVALID_IFD_SID;
}

scs_err_t icc_ifd_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_SUCCESS:
	case SCS_ERR_PROGRESS:
	case SCS_ERR_OVERRUN:
	case SCS_ERR_TIMEOUT:
	case SCS_ERR_NOTPRESENT:
		return err;
	default:
		return SCS_ERR_HW_ERROR;
	}
}

uint8_t icc_ifd_state(uint8_t state)
{
	switch (state) {
	case IFD_SLOT_STATE_NOTPRESENT:
		return ICC_STATE_NOTPRESENT;
	case IFD_SLOT_STATE_PRESENT:
	case IFD_SLOT_STATE_SELECTED:
	case IFD_SLOT_STATE_ACTIVATED:
		return ICC_STATE_PRESENT;
	case IFD_SLOT_STATE_ATR_READY:
	case IFD_SLOT_STATE_PPS_READY:
		return ICC_STATE_ACTIVE;
	case IFD_SLOT_STATE_HWERROR:
	default:
		return ICC_STATE_HWERROR;
	}
}

static void icc_ifd_select(void)
{
	ifd_sid_select(icc_ifd_slot(scd_id));
}

static scs_err_t icc_ifd_activate(void)
{
	scs_err_t err;
	err = icc_ifd_power_on(true);
	return icc_ifd_error(err);
}

static scs_err_t icc_ifd_deactivate(void)
{
	scs_err_t err;
	err = ifd_power_off();
	return icc_ifd_error(err);
}

static scs_err_t icc_ifd_xchg_block(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	err = ifd_xchg_block(nc, ne);
	return icc_ifd_error(err);
}

static scs_size_t icc_ifd_xchg_avail(void)
{
	return ifd_xchg_avail();
}

static scs_err_t icc_ifd_write_byte(scs_off_t index, uint8_t byte)
{
	scs_err_t err;
	err = ifd_write_byte(index, byte);
	return icc_ifd_error(err);
}

static uint8_t icc_ifd_read_byte(scs_off_t index)
{
	return ifd_read_byte(index);
}

icc_driver_t icc_ifd = {
	icc_ifd_select,
	icc_ifd_activate,
	icc_ifd_deactivate,
	icc_ifd_xchg_block,
	icc_ifd_xchg_avail,
	icc_ifd_write_byte,
	icc_ifd_read_byte,
};

void icc_ifd_seq_complete(scs_err_t err)
{
	scd_dev_select(icc_ifd_dev(ifd_slid));
	scd_seq_complete(icc_ifd_error(err));
}

void icc_ifd_set_state(uint8_t state)
{
	scd_dev_select(icc_ifd_dev(ifd_slid));
	scd_dev_set_state(icc_ifd_state(state));
}

void icc_ifd_init()
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		icc_ifds[sid] = scd_register_device(&icc_ifd);
	}
}
