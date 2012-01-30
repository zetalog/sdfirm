#include <driver/pn53x.h>

uint8_t pn53x_read_byte(void)
{
	return 0;
}

void pn53x_write_byte(uint8_t val)
{
}

scs_err_t pn53x_poll_ready(uint8_t ms)
{
	return SCS_ERR_TIMEOUT;
}
