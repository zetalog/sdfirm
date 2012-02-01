#include <driver/pn53x.h>
#include <target/spi.h>

boolean pn53x_hw_poll_ready(void)
{
	return false;
}

void pn53x_hw_read_cmpl(scs_size_t ne)
{
}

void pn53x_hw_write_cmpl(scs_size_t nc)
{
}

uint8_t pn53x_hw_xchg_read(scs_off_t index)
{
	return 0;
}

void pn53x_hw_xchg_write(scs_off_t index, uint8_t val)
{
}

void pn53x_hw_ctrl_init(void)
{
}
