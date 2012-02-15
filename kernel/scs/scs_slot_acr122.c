#include <target/scs_card.h>
#include <driver/acr122.h>

scs_cid_t acr122_slot_sid;
boolean acr122_slot_activated;
boolean acr122_slot_atr;

static scs_err_t acr122_slot_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_SUCCESS:
	case SCS_ERR_PROGRESS:
	case SCS_ERR_TIMEOUT:
		return err;
	default:
		return SCS_ERR_HW_ERROR;
	}
}

static void acr122_slot_select(void)
{
	/* Shouldn't be multiple pn53x chips. */
}

static scs_err_t acr122_slot_activate(void)
{
	acr122_slot_activated = true;
	acr122_slot_atr = true;
	return SCS_ERR_SUCCESS;
}

static scs_err_t acr122_slot_deactivate(void)
{
	acr122_slot_activated = false;
	return SCS_ERR_SUCCESS;
}

static scs_err_t acr122_slot_xchg_block(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	acr122_slot_atr = false;
	err = acr122_xchg_block(nc, ne);
	return acr122_slot_error(err);
}

static scs_size_t acr122_slot_xchg_avail(void)
{
	if (acr122_slot_atr)
		return 2;
	return acr122_xchg_avail();
}

static scs_err_t acr122_slot_xchg_write(scs_off_t index, uint8_t byte)
{
	acr122_write_byte(index, byte);
	return SCS_ERR_SUCCESS;
}

static uint8_t acr122_slot_xchg_read(scs_off_t index)
{
	if (acr122_slot_atr) {
		switch (index) {
		case 0x00:
			return 0x3B;
		default:
			return 0x00;
		}
	}
	return acr122_read_byte(index);
}

static uint8_t acr122_slot_status(void)
{
	if (acr122_slot_activated)
		return SCS_SLOT_STATUS_ACTIVE;
	else
		return SCS_SLOT_STATUS_INACTIVE;
}

static uint8_t acr122_slot_get_error(void)
{
	return acr122_slot_error(acr122_get_error());
}

static void acr122_slot_complete_slot(void)
{
	scs_slot_select(acr122_slot_sid);
	scs_complete_slot();
}

scs_slot_driver_t acr122_slot = {
	acr122_slot_get_error,
	acr122_slot_status,
	acr122_slot_select,
	acr122_slot_activate,
	acr122_slot_deactivate,
	acr122_slot_xchg_block,
	acr122_slot_xchg_avail,
	acr122_slot_xchg_write,
	acr122_slot_xchg_read,
};

void acr122_slot_init(void)
{
	acr122_init();
	acr122_slot_sid = scs_register_slot(&acr122_slot);
	acr122_slot_activated = false;
	acr122_register_completion(acr122_slot_complete_slot);
}
