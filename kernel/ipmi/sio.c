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
		if (argc < 3)
			return -EINVAL;
		else
			return sio_read8((uint8_t)strtoull(argv[2], 0, 0));
	}
	if (strcmp(argv[1], "write") == 0) {
		if (argc < 4)
			return -EINVAL;
		else {
			sio_write8((uint8_t)strtoull(argv[2], 0, 0), (uint8_t)strtoull(argv[3], 0, 0));
			return 0;
		}
	}
	if (strcmp(argv[1], "select") == 0) {
		sio_select((sio_dev_t)strtoull(argv[2], 0, 0));
		return 0;
	}
	if (strcmp(argv[1], "enter") == 0) {
		sio_enter();
		return 0;
	}
	if (strcmp(argv[1], "exit") == 0) {
		sio_exit();
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(sio, do_sio, "Super IO Control Commands",
	"sio write <value> <addr>\n"
	"	-sio write data\n"
	"sio read <addr>\n"
	"	-sio read data\n"
	"sio select <dev>\n"
	"	-sio select device\n"
	"sio enter\n"
	"	-enter sio\n"
	"sio exit\n"
	"	-exit sio\n"
);