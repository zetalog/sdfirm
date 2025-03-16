#include <target/console.h>
#include <target/cmdline.h>
#include <target/ssif.h>

#ifndef SSIF_MAX_LEN
#define SSIF_MAX_LEN					32
#endif

static uint8_t ssif_buf[SSIF_MAX_LEN];
static uint16_t ssif_txlen;
static uint16_t ssif_rxlen;
static i2c_addr_t ssif_addr = SSIF_BASE;

static void __ssif_hw_i2c_iocb(i2c_len_t len)
{
	if (i2c_dir_mode() == I2C_MODE_TX) {
		while (len) {
			i2c_write_byte(ssif_buf[ssif_txlen]);
			ssif_txlen++;
			len--;
		}
	} else {
		while (len) {
			ssif_buf[ssif_rxlen] = i2c_read_byte();
			ssif_rxlen++;
			len--;
		}
	}
}

i2c_device_t ssif_i2c = {
	__ssif_hw_i2c_iocb,
};

int ssif_write_single(uint8_t *data, uint8_t len)
{
	int i;
	i2c_register_device(&ssif_i2c);
	ssif_txlen = 0;
	ssif_rxlen = 0;
	ssif_buf[SSIF_W_SMBUS_CMD] = SSIF_SINGLE_WRITE;
	ssif_buf[SSIF_W_LEN] = len - 1;
	ssif_buf[SSIF_W_NETFN] = 0;
	ssif_buf[SSIF_W_LUN] = 0;
	for (i = 0; i < len; i++)
		ssif_buf[i + 4] = data[i];
	i2c_master_submit(ssif_addr, len + 4, 0);
	return ssif_txlen;
}

uint8_t ssif_read_single(uint8_t *data)
{
	int i;
	uint8_t len, netfn, lun, cmd, cpcode;
	i2c_register_device(&ssif_i2c);
	ssif_buf[SSIF_R_SMBUS_CMD] = SSIF_SINGLE_READ;
	i2c_master_submit(ssif_addr, 1, 0);
	i2c_master_submit(ssif_addr, 0, 3);
	len = ssif_buf[SSIF_R_LEN];
	netfn = ssif_buf[SSIF_R_NETFN];
	lun = ssif_buf[SSIF_R_LUN];
	ssif_txlen = 0;
	ssif_rxlen = 0;
	i2c_master_submit(ssif_addr, 0, len + 2);
	data = ssif_buf;
	return ssif_rxlen;
}

int do_ssif_xfer(int argc, char *argv[])
{
	int ret, i;
	int wlen;
	uint8_t buf[SSIF_MAX_LEN];

	if (argc < 3)
		return -EINVAL;
	ssif_addr = (uint8_t)strtoull(argv[2], 0, 0);
	wlen = (uint8_t)strtoull(argv[3], 0, 0);
	if (wlen >= SSIF_MAX_LEN) {
		printf("write length oversized!\n");
		return -EINVAL;
	}
	if (wlen > argc - 2) {
		printf("write length not match!\n");
		return -EINVAL;
	}
	for (i = 0; i < wlen; i++) {
		buf[i] = (uint8_t)strtoull(argv[i + 3], 0, 0);
	}
	ret = ssif_write_single(buf, wlen);
	if (ret < 0) {
		printf("ssif write error!\n");
		return -ret;
	}
	ret = ssif_read_single(buf);
	if (ret < 0) {
		printf("ssif read error!\n");
		return ret;
	}
	hexdump(0, buf, 1, ret);
	return 0;
}

int do_ssif(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "cfg") == 0) {
		if (strcmp(argv[2], "addr") == 0)
			ssif_addr = (uint8_t)strtoull(argv[3], 0, 0);
	}
	if (strcmp(argv[1], "xfer") == 0)
		return do_ssif_xfer(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(ssif, do_ssif, "IPMI SSIF Commands",
	"ssif cfg addr <addr>\n"
	"       -config bmc i2c slave address\n"
	"ssif xfer <wlen> <byte1> [byte2] ... [byteN]\n"
	"	-ssif transfer data\n"
);