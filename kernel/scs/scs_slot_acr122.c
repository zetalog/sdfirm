#include <target/scs_slot.h>
#include <driver/acr122.h>

scs_sid_t acr122_sid;
boolean acr122_activated;

static scs_err_t acr122_slot_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_SUCCESS:
	case SCS_ERR_PROGRESS:
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
	acr122_activated = true;
	return SCS_ERR_SUCCESS;
}

static scs_err_t acr122_slot_deactivate(void)
{
	acr122_activated = false;
	return SCS_ERR_SUCCESS;
}

static scs_err_t acr122_slot_xchg_block(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	err = acr122_xchg_block(nc, ne);
	return acr122_slot_error(err);
}

static scs_size_t acr122_slot_xchg_avail(void)
{
	return acr122_xchg_avail();
}

static scs_err_t acr122_slot_xchg_write(scs_off_t index, uint8_t byte)
{
	acr122_write_byte(index, byte);
	return SCS_ERR_SUCCESS;
}

static uint8_t acr122_slot_xchg_read(scs_off_t index)
{
	return acr122_read_byte(index);
}

static uint8_t acr122_slot_status(void)
{
	if (acr122_activated)
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
	scs_slot_select(acr122_sid);
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
	acr122_sid = scs_register_slot(&acr122_slot);
	acr122_activated = false;
	acr122_register_completion(acr122_slot_complete_slot);
}
