#include <target/sio.h>
#include <target/cmdline.h>
#include <target/barrier.h>
#include <target/lpc.h>
#include <target/console.h>

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

int do_sio(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0) {
		if (argc < 4)
			return -EINVAL;
		else {
			sio_enter();
			sio_select((sio_dev_t)strtoull(argv[2], 0, 0));
			int v = sio_read8((uint8_t)strtoull(argv[3], 0, 0));
			sio_exit();
			return v;
		}
	}
	if (strcmp(argv[1], "write") == 0) {
		if (argc < 5)
			return -EINVAL;
		else {
			sio_enter();
			sio_select((sio_dev_t)strtoull(argv[2], 0, 0));
			sio_write8((uint8_t)strtoull(argv[4], 0, 0), (uint8_t)strtoull(argv[3], 0, 0));
			sio_exit();
			return 0;
		}
	}
	return -EINVAL;
}

DEFINE_COMMAND(sio, do_sio, "Super IO Control Commands",
	"sio write <dev> <addr> <value>\n"
	"	-sio write data\n"
	"sio read <dev> <addr>\n"
	"	-sio read data\n"
);