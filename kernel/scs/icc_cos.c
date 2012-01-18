#include <target/icc.h>
#include <target/cos.h>

#define COS_STATE_DESELECTED	0x00
#define COS_STATE_SELECTED	0x01

scd_t cos_scd;
uint8_t cos_state = COS_STATE_DESELECTED;

static scs_err_t icc_cos_err(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_SUCCESS:
	case SCS_ERR_PROGRESS:
		return err;
	default:
		return SCS_ERR_HW_ERROR;
	}
}

uint8_t icc_cos_state(uint8_t state)
{
	if (cos_state == COS_STATE_DESELECTED)
		return ICC_STATE_PRESENT;
	return ICC_STATE_ACTIVE;
}

static void icc_cos_select(void)
{
}

static scs_err_t icc_cos_activate(void)
{
	scs_err_t err;
	err = cos_power_on();
	if (err == SCS_ERR_SUCCESS)
		cos_state = COS_STATE_SELECTED;
	return icc_cos_err(err);
}

static scs_err_t icc_cos_deactivate(void)
{
	cos_state = COS_STATE_DESELECTED;
	return SCS_ERR_SUCCESS;
}

static scs_err_t icc_cos_xchg_block(scs_size_t nc, scs_size_t ne)
{
	/* TODO: execute COS commands */
	return SCS_ERR_SUCCESS;
}

static scs_size_t icc_cos_xchg_avail(void)
{
	return cos_xchg_avail();
}

static scs_err_t icc_cos_xchg_write(scs_off_t index, uint8_t byte)
{
	cos_xchg_write(index, byte);
	return SCS_ERR_SUCCESS;
}

static uint8_t icc_cos_xchg_read(scs_off_t index)
{
	return cos_xchg_read(index);
}

icc_driver_t icc_cos = {
	icc_cos_select,
	icc_cos_activate,
	icc_cos_deactivate,
	icc_cos_xchg_block,
	icc_cos_xchg_avail,
	icc_cos_xchg_write,
	icc_cos_xchg_read,
};

void icc_cos_seq_complete(scs_err_t err)
{
	scd_dev_select(cos_scd);
	scd_seq_complete(icc_cos_err(err));
}

void icc_cos_set_state(uint8_t state)
{
	scd_dev_select(cos_scd);
	scd_dev_set_state(icc_cos_state(state));
}

void icc_cos_init(void)
{
	cos_scd = scd_register_device(&icc_cos);
}
