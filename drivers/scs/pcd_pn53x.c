#include <target/pcd.h>
#include <driver/pcd_pn53x.h>

uint8_t pn53x_xchg_buffer[PN53X_BUF_SIZE];

scs_err_t pn53x_activate(void)
{
	return SCS_ERR_SUCCESS;
}

void pn53x_deactivate(void)
{
}

scs_err_t pn53x_xchg_block(scs_size_t nc, scs_size_t ne)
{
	return SCS_ERR_SUCCESS;
}

scs_size_t pn53x_xchg_avail(void)
{
	return 0;
}

scs_err_t pn53x_xchg_write(scs_off_t index, uint8_t byte)
{
	return SCS_ERR_SUCCESS;
}

uint8_t pn53x_xchg_read(scs_off_t index)
{
	return 0;
}

