#include <driver/acr122.h>

scs_cmpl_cb acr122_complete = NULL;
struct acr122_pseudo_apdu acr122_apdu;
scs_err_t acr122_error;
uint8_t acr122_cmd[SCS_APDU_MAX];
uint8_t acr122_resp[SCS_APDU_MAX];

static void acr122_set_sw_default(void)
{
	acr122_apdu.sw1 = 0x90;
	acr122_apdu.sw2 = 0x00;
}

static void acr122_set_sw_length(uint8_t len)
{
	acr122_apdu.sw1 = 0x61;
	acr122_apdu.sw2 = len;
}

static void acr122_set_sw_error(uint8_t err)
{
	acr122_apdu.sw1 = 0x63;
	acr122_apdu.sw2 = err;
}

scs_err_t acr122_get_error(void)
{
	return acr122_error;
}

void acr122_set_error(scs_err_t err)
{
	acr122_error = err;
	switch (err) {
	case SCS_ERR_SUCCESS:
		acr122_set_sw_default();
		break;
	case SCS_ERR_TIMEOUT:
		acr122_set_sw_error(ACR122_SW2_TIMEOUT);
		break;
	case SCS_ERR_OVERRUN:
	case SCS_ERR_PARITY_ERR:
		acr122_set_sw_error(ACR122_SW2_PARITY);
		break;
	default:
		if (err & SCS_ERR_SANITY)
			acr122_set_sw_error(ACR122_SW2_SANITY);
		break;
	}
}
#define acr122_set_sanity(i)		acr122_set_error(SCS_ERR_SANITY|(i))

scs_size_t acr122_xchg_avail(void)
{
	return acr122_apdu.ne + 2;
}

uint8_t acr122_read_byte(scs_off_t index)
{
	if (index < acr122_apdu.ne)
		return acr122_resp[index];
	else if (index == acr122_apdu.ne)
		return acr122_apdu.sw1;
	else if (index == (acr122_apdu.ne+1))
		return acr122_apdu.sw2;
	return 0;
}

void acr122_write_byte(scs_off_t index, uint8_t value)
{
	if (index != 0 && acr122_error != SCS_ERR_SUCCESS)
		return;

	switch (index) {
	case 0:
		if (value != 0xFF)
			acr122_set_sanity(index);
		else
			acr122_set_error(SCS_ERR_SUCCESS);
		break;
	case 1:
		if (value != 0x00 && value != 0xC0)
			acr122_set_sanity(index);
		else
			acr122_apdu.ins = value;
		break;
	case 2:
		if (value != 0x00 && !(value & 0x40))
			acr122_set_sanity(index);
		acr122_apdu.p1 = value;
		break;
	case 3:
		if (value != 0x00 && !(value & 0x40))
			acr122_set_sanity(index);
		break;
	case 4:
		if (acr122_apdu.ins == 0x00) {
			acr122_apdu.nc = value;
			acr122_apdu.ne = 0;
		} else {
			acr122_apdu.nc = 0;
			acr122_apdu.ne = value;
		}
		break;
	default:
		if ((index-ACR122_HEAD_SIZE) >= acr122_apdu.nc)
			acr122_set_error(SCS_ERR_OVERRUN);
		else
			acr122_cmd[index-ACR122_HEAD_SIZE] = value;
		break;
	}
}

scs_err_t acr122_xchg_block(scs_size_t nc, scs_size_t ne)
{
	if (acr122_error != SCS_ERR_SUCCESS)
		return acr122_error;

	switch (acr122_apdu.ins) {
	case 0x00:
		break;
	case 0xC0:
		break;
	}
	return SCS_ERR_PROGRESS;
}

void acr122_register_completion(scs_cmpl_cb completion)
{
	acr122_complete = completion;
}

void acr122_proto_init(void)
{
}
