#include <target/scs_slot.h>
#include <driver/pn53x.h>

scs_sid_t acr122_sid;

uint8_t acr122_slot_status(uint8_t status)
{
	switch (status) {
	case SCS_SLOT_STATUS_ACTIVE:
		return SCS_SLOT_STATUS_ACTIVE;
	default:
		return SCS_SLOT_STATUS_INACTIVE;
	}
}

static void acr122_slot_select(void)
{
	/* Shouldn't be multiple pn53x chips. */
}

static scs_err_t acr122_slot_activate(void)
{
	return SCS_ERR_SUCCESS;
}

static scs_err_t acr122_slot_deactivate(void)
{
	return SCS_ERR_SUCCESS;
}

static scs_err_t acr122_slot_xchg_block(scs_size_t nc, scs_size_t ne)
{
	return SCS_ERR_UNSUPPORT;
}

static scs_size_t acr122_slot_xchg_avail(void)
{
	return SCS_ERR_UNSUPPORT;
}

static scs_err_t acr122_slot_xchg_write(scs_off_t index, uint8_t byte)
{
	pn53x_xchg_write(index, byte);
	return SCS_ERR_SUCCESS;
}

static uint8_t acr122_slot_xchg_read(scs_off_t index)
{
	return pn53x_xchg_read(index);
}

scs_slot_driver_t acr122_slot = {
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
}
