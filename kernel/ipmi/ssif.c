#include <target/console.h>
#include <target/cmdline.h>
#include <target/ssif.h>

#ifndef SSIF_MAX_LEN
#define SSIF_MAX_LEN					32
#endif

#define PEC_POLYNOMIAL 0x07			// x^8 + x^2 + x + 1
#define PEC_INITIAL_VALUE 0x00			// PEC 初始值

static uint8_t ssif_txbuf[SSIF_MAX_LEN];
static uint8_t ssif_rxbuf[SSIF_MAX_LEN];
static uint16_t ssif_txlen;
static uint16_t ssif_rxlen;
static i2c_addr_t ssif_addr = SSIF_BASE;

static void __ssif_hw_i2c_iocb(i2c_len_t len)
{
	if (i2c_dir_mode() == I2C_MODE_TX) {
		while (len) {
			i2c_write_byte(ssif_txbuf[ssif_txlen]);
			printf("ssif tx: %d - %x\n", ssif_txlen, ssif_txbuf[ssif_txlen]);
			ssif_txlen++;
			len--;
		}
	} else {
		while (len) {
			ssif_rxbuf[ssif_rxlen] = i2c_read_byte();
			printf("ssif rx: %d - %x\n", ssif_rxlen, ssif_rxbuf[ssif_rxlen]);
			ssif_rxlen++;
			len--;
		}
	}
}

i2c_device_t ssif_i2c = {
	__ssif_hw_i2c_iocb,
};

uint8_t calculate_pec(uint8_t rw, uint8_t *buf, uint8_t length) {
	uint8_t pec = PEC_INITIAL_VALUE;
	uint8_t data[SSIF_MAX_LEN];
	int i, bit;

	data[0] = (ssif_addr << 1) | rw;
	for (i = 0; i < length; i++)
		data[i + 1] = buf[i];
	for (i = 0; i < length + 1; i++) {
		pec ^= data[i];
		for (bit = 0; bit < 8; bit++)
			if (pec & 0x80)
				pec = (pec << 1) ^ PEC_POLYNOMIAL;
			else
				pec <<= 1;
	}
	return pec;
}

uint8_t ssif_write_single(uint8_t *data, uint8_t len, uint8_t netfn, uint8_t lun, uint8_t ipmi_cmd)
{
	int i;
	i2c_register_device(&ssif_i2c);
	ssif_txlen = 0;
	ssif_rxlen = 0;
	ssif_txbuf[SSIF_W_SMBUS_CMD] = SSIF_SINGLE_WRITE;
	ssif_txbuf[SSIF_W_LEN] = len + 2;
	ssif_txbuf[SSIF_W_NETFN_LUN] = (netfn << 2) | (lun & 0x3);
	ssif_txbuf[SSIF_W_IPMI_CMD] = ipmi_cmd;
	for (i = 0; i < len; i++)
		ssif_txbuf[i + 4] = data[i];
	ssif_txbuf[i + 4] = calculate_pec(0, ssif_txbuf, len + 4);
	i2c_master_submit(ssif_addr, len + 5, 0);
	return ssif_txlen;
}

uint8_t ssif_read_single(uint8_t *data)
{
	int i;
	uint8_t len, netfn, lun, cmd, cpcode;
	i2c_register_device(&ssif_i2c);
	ssif_txbuf[SSIF_W_SMBUS_CMD] = SSIF_SINGLE_READ;
	ssif_txlen = 0;
	ssif_rxlen = 0;
	i2c_master_submit(ssif_addr, 1, 0);
	ssif_txlen = 0;
	ssif_rxlen = 0;
	i2c_master_submit(ssif_addr, 0, 4);
	len = ssif_rxbuf[SSIF_R_LEN];
	netfn = ssif_rxbuf[SSIF_R_NETFN_LUN] >> 2;
	lun = ssif_rxbuf[SSIF_R_NETFN_LUN] & 0x3;
	cmd = ssif_rxbuf[SSIF_R_IPMI_CMD];
	cpcode = ssif_rxbuf[SSIF_R_COMPLETION_CODE];
	printf("ssif read: len = %x, netfn = %x, lun = %x, ipmi_cmd = %x, compcode = %x\n", 
		len, netfn, lun, cmd, cpcode);
	i2c_master_submit(ssif_addr, 0, len);
	data = ssif_rxbuf;
	return ssif_rxlen;
}

int do_ssif_read(int argc, char *argv[])
{
	int ret;
	uint8_t buf[SSIF_MAX_LEN];
	ret = ssif_read_single(buf);
	if (ret < 0) {
		printf("ssif read error!\n");
		return ret;
	}
	hexdump(0, buf, 1, ret);
	return 0;
}

int do_ssif_write(int argc, char *argv[])
{
	int ret, i;
	uint8_t len, netfn, lun, ipmi_cmd;
	uint8_t buf[SSIF_MAX_LEN];

	if (argc < 3)
		return -EINVAL;
	len = (uint8_t)strtoull(argv[2], 0, 0);
	netfn = (uint8_t)strtoull(argv[3], 0, 0);
	lun = (uint8_t)strtoull(argv[4], 0, 0);
	ipmi_cmd = (uint8_t)strtoull(argv[5], 0, 0);
	if (len >= SSIF_MAX_LEN) {
		printf("write length oversized!\n");
		return -EINVAL;
	}
	if (len > argc - 2) {
		printf("write length not match!\n");
		return -EINVAL;
	}
	for (i = 0; i < len; i++) {
		buf[i] = (uint8_t)strtoull(argv[i + 6], 0, 0);
	}
	ret = ssif_write_single(buf, len, netfn, lun, ipmi_cmd);
	if (ret < 0) {
		printf("ssif write error!\n");
		return -ret;
	}
	return 0;
}

int do_ssif_xfer(int argc, char *argv[])
{
	int ret, i;
	uint8_t len, netfn, lun, ipmi_cmd, pec;
	uint8_t txbuf[SSIF_MAX_LEN], rxbuf[SSIF_MAX_LEN];

	if (argc < 3)
		return -EINVAL;
	len = (uint8_t)strtoull(argv[2], 0, 0);
	netfn = (uint8_t)strtoull(argv[3], 0, 0);
	lun = (uint8_t)strtoull(argv[4], 0, 0);
	ipmi_cmd = (uint8_t)strtoull(argv[5], 0, 0);
	if (len >= SSIF_MAX_LEN) {
		printf("write length oversized!\n");
		return -EINVAL;
	}
	if (len > argc - 2) {
		printf("write length not match!\n");
		return -EINVAL;
	}
	for (i = 0; i < len; i++) {
		txbuf[i] = (uint8_t)strtoull(argv[i + 6], 0, 0);
	}
	ret = ssif_write_single(txbuf, len, netfn, lun, ipmi_cmd);
	if (ret < 0) {
		printf("ssif write error!\n");
		return -ret;
	}
	ret = ssif_read_single(rxbuf);
	if (ret < 0) {
		printf("ssif read error!\n");
		return ret;
	}
	hexdump(0, rxbuf, 1, ret);
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
	if (strcmp(argv[1], "read") == 0)
		return do_ssif_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_ssif_write(argc, argv);
	if (strcmp(argv[1], "xfer") == 0)
		return do_ssif_xfer(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(ssif, do_ssif, "IPMI SSIF Commands",
	"ssif cfg addr <addr>\n"
	"       -config bmc i2c slave address\n"
	"ssif read\n"
	"       -ssif read data\n"
	"ssif write <len> <netfn> <lun> <ipmi_cmd> <byte1> [byte2] ... [byteN]\n"
	"       -ssif write data\n"
	"ssif xfer <len> <netfn> <lun> <ipmi_cmd> <byte1> [byte2] ... [byteN]\n"
	"	-ssif transfer data\n"
);