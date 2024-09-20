#include <target/console.h>
#include <target/panic.h>
#include <target/ddr.h>
#include <target/cmdline.h>

#define SPD_SA_MASK	0x0E
#define I2C2SPD(i2c)	(((i2c) & SPD_SA_MASK) >> 1)

static uint16_t spd5_bytes[SPD5_NVM_WRITES];
static uint16_t spd5_cmd;
static uint8_t *spd5_buf;
static uint16_t spd5_len;
static bool spd5_i2c_legacy_2bytes;

static void __spd_hw_i2c_iocb(i2c_len_t len)
{
	i2c_len_t i;
	uint8_t hlen = 0;

	if (i2c_dir_mode() == I2C_MODE_TX) {
		BUG_ON(len != 1);
		i2c_write_byte(LOBYTE(spd5_cmd));
		hlen++;
		if (spd5_i2c_legacy_2bytes) {
			i2c_write_byte(HIBYTE(spd5_cmd));
			hlen++;
		}
		for (i = hlen; i < len; i++) {
			i2c_write_byte(spd5_buf[i - hlen]);
			spd5_len++;
		}
	} else {
		for (i = 0; i < len; i++) {
			spd5_buf[i] = i2c_read_byte();
			spd5_len++;
		}
	}
}

i2c_device_t spd5118_i2c = {
	__spd_hw_i2c_iocb,
};

int spd_hw_read_bytes(uint8_t lsa, uint16_t offset,
		      uint8_t *buffer, int len)
{
	return 0;
#if 0
	int len0 = 0;
	int len1 = 0;

	i2c_register_device(&spd5118);
	if (offset >= SPD_EE_SIZE || (offset + len) > SPD_EE_SIZE) {
		con_err("spd5118: Invalid range [0x%x-0x%x]\n",
			offset, offset + len);
		return -1;
	}

	if (offset + len < SPD_EE_PAGE_SIZE) {
		len0 = len;
		len1 = 0;
	} else if (offset > SPD_EE_PAGE_SIZE) {
		len0 = 0;
		len1 = len;
	} else {
		len0 = SPD_EE_PAGE_SIZE - offset;
		len1 = len - len0;
	}

	if (len0 > 0) {
		/* spd5118_set_page(0); */
		spd_pos = offset;
		spd_buf = buffer;
		i2c_master_submit(SPD_DSC_SPD(lsa), 1, len0);
	}
	if (len1 > 0) {
		if (offset < SPD_EE_PAGE_SIZE)
			offset = 0;
		else
			offset -= SPD_EE_PAGE_SIZE;
		/* spd5118_set_page(1); */
		spd_pos = offset;
		spd_buf = buffer + len0;
		spd_len = 0;
		i2c_master_submit(SPD_DSC_SPD(lsa), 1, len1);
	}
	return len;
#endif
}

const char *spd5_reg_names[] = {
	"MR0: Device Type; MSB",
	"MR1: Device Type; LSB",
	"MR2: Device Revision",
	"MR3: Vendor ID Byte0",
	"MR4: Vendor ID Byte1",
	"MR5: Device Capability",
	"MR6: Write Recovery Time",
	"RV", "RV", "RV", "RV",
	"MR11: I2C Legacy Mode Device Configuration",
	"MR12: Write Protection for NVM blocks[0:7]",
	"MR13: Write Protection for NVM blocks[8:15]",
	"MR14: Device Configuration - Host and Local Interface IO",
	"DO NOT USE",
	"RV", "RV",
	"MR18: Device Configuration",
	"MR19: Clear MR51 Temperature Status Command",
	"MR20: Clear MR52 Error Status Command",
	"RV", "RV", "RV", "RV", "RV",
	"MR26: TS Configuration",
	"MR27: Interrupt Configuration",
	"MR28: TS Temperature High Limit - Low Byte",
	"MR29: TS Temperature High Limit - High Byte",
	"MR30: TS Temperature Low Limit - Low Byte",
	"MR31: TS Temperature Low Limit - High Byte",
	"MR32: TS Critical Temperature High Limit - Low Byte",
	"MR33: TS Critical Temperature High Limit - High Byte",
	"MR34: TS Critical Temperature Low Limit - Low Byte",
	"MR35: TS Critical Temperature Low Limit - High Byte",
	"RV", "RV", "RV", "RV", "RV", "RV", "RV", "RV", "RV", "RV", "RV", "RV",
	"MR48: Device Status",
	"MR49: TS Current Sensed Temperature - Low Byte",
	"MR50: TS Current Sensed Temperature - High Byte",
	"MR51: TS Temperature Status",
	"MR52: Hub, Thermal and NVM Error Status",
};

uint8_t spd5_read_reg(uint8_t hid, uint8_t adr)
{
	uint8_t val;

	i2c_register_device(&spd5118_i2c);
	spd5_buf = &val;
	spd5_len = 0;
	if (spd5_i2c_legacy_2bytes)
		spd5_cmd = SPD5_I2C2_REG(adr);
	else
		spd5_cmd = SPD5_I2C1_REG(adr);
	i2c_master_submit(SPD5_HUB(hid), 1, 1);
	return val;
}

void spd5_write_reg(uint8_t hid, uint8_t adr, uint8_t val)
{
	i2c_register_device(&spd5118_i2c);
	spd5_buf = &val;
	spd5_len = 0;
	if (spd5_i2c_legacy_2bytes)
		spd5_cmd = SPD5_I2C2_REG(adr);
	else
		spd5_cmd = SPD5_I2C1_REG(adr);
	i2c_master_submit(SPD5_HUB(hid), 2, 0);
}

const char *spd5_reg_name(uint8_t addr)
{
	if (addr >= ARRAY_SIZE(spd5_reg_names))
		return "UNKNOWN";
	return spd5_reg_names[addr];
}

void spd5118_init(void)
{
}

static int do_spd5_reg(int argc, char *argv[])
{
	uint8_t hid;
	uint8_t adr;
	uint8_t val;

	if (argc < 5)
		return -EINVAL;
	hid = (uint8_t)strtoull(argv[3], 0, 0);
	adr = (uint8_t)strtoull(argv[4], 0, 0);
	if (strcmp(argv[2], "write") == 0) {
		if (argc < 6)
			return -EINVAL;
		val = (uint8_t)strtoull(argv[5], 0, 0);
		spd5_write_reg(hid, adr, val);
	}
	if (strcmp(argv[2], "read") == 0) {
		val = spd5_read_reg(hid, adr);
		printf("%s:\nMR%d=%02x\n",
		       spd5_reg_name(adr), adr, (uint8_t)val);
	}
	return 0;
}

static int do_spd5(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "reg") == 0)
		return do_spd5_reg(argc, argv);
	return 0;
}

DEFINE_COMMAND(spd5, do_spd5, "SPD5118 hub commands",
	"reg write <hid> <MR> <value>\n"
	"    - write hub MR register\n"
	"reg read <hid> <MR>\n"
	"    - read hub MR register\n"
);
