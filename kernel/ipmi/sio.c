#include <target/sio.h>

void sio_enter(void)
{
	lpc_io_write8(SIO_HW_ENTER, SIO_INDEX_0);
	lpc_io_write8(SIO_HW_ENTER, SIO_INDEX_0);
}

void sio_exit(void)
{
	lpc_io_write8(SIO_HW_EXIT, SIO_INDEX_0);
}

void sio_select(sio_dev_t dev)
{
	lpc_io_write8(0x07, SIO_INDEX_0);
	lpc_io_write8(dev, SIO_DATA_0);
}

void sio_write8(uint8_t v, uint8_t a)
{
	lpc_io_write8(a, SIO_INDEX_0);
	lpc_io_write8(v, SIO_DATA_0);
}

uint8_t sio_read8(uint8_t a)
{
	lpc_io_write8(a, SIO_INDEX_0);
	return lpc_io_read8(SIO_DATA_0);
}
