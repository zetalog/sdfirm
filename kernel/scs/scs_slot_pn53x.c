#include <target/scs_slot.h>
#include <driver/pcd_pn53x.h>

scs_sid_t pn53x_sid;

static scs_err_t pn53x_slot_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_SUCCESS:
	case SCS_ERR_PROGRESS:
		return err;
	default:
		return SCS_ERR_HW_ERROR;
	}
}

uint8_t pn53x_slot_status(uint8_t status)
{
	switch (status) {
	case SCS_SLOT_STATUS_ACTIVE:
		return SCS_SLOT_STATUS_ACTIVE;
	default:
		return SCS_SLOT_STATUS_INACTIVE;
	}
}

static void pn53x_slot_select(void)
{
	/* Shouldn't be multiple pn53x chips. */
}

static scs_err_t pn53x_slot_activate(void)
{
	scs_err_t err;
	err = pn53x_activate();
	/* prepare fake ATR here. */
	return pn53x_slot_error(err);
}

static scs_err_t pn53x_slot_deactivate(void)
{
	pn53x_deactivate();
	return SCS_ERR_SUCCESS;
}

static scs_err_t pn53x_slot_xchg_block(scs_size_t nc, scs_size_t ne)
{
	return pn53x_xchg_block(nc, ne);
}

static scs_size_t pn53x_slot_xchg_avail(void)
{
	return pn53x_xchg_avail();
}

static scs_err_t pn53x_slot_xchg_write(scs_off_t index, uint8_t byte)
{
	pn53x_xchg_write(index, byte);
	return SCS_ERR_SUCCESS;
}

static uint8_t pn53x_slot_xchg_read(scs_off_t index)
{
	return pn53x_xchg_read(index);
}

scs_slot_driver_t pn53x_slot = {
	pn53x_slot_select,
	pn53x_slot_activate,
	pn53x_slot_deactivate,
	pn53x_slot_xchg_block,
	pn53x_slot_xchg_avail,
	pn53x_slot_xchg_write,
	pn53x_slot_xchg_read,
};

void pn53x_slot_init(void)
{
	pn53x_sid = scs_register_slot(&pn53x_slot);
}
