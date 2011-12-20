#include <target/config.h>
#include <target/generic.h>
#include <target/cos.h>

#define SCOS_ATR_SIZE	17

static uint8_t scos_atr[SCOS_ATR_SIZE] = {
	0x7D, 0x94, 0x00, 0x00,
	0x57, 0x44, 0x35, 0x65,
	0xF4, 0x86, 0x93, 0x07,
	0x17, 0xBD, 0x06, 0x20,
	0x1E,
};

void cos_vs_parse(void)
{
}

cos_err_t cos_vs_activate(void)
{
	uint8_t i;
	for (i = 0; i < SCOS_ATR_SIZE; i++)
		cos_write_resp(scos_atr[i]);
	return COS_ERR_SUCCESS;
}

void cos_vs_deactivate(void)
{
}

void cos_vs_init(void)
{
}
