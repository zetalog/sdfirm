#include <target/console.h>
#include <target/panic.h>
#include <target/ddr.h>
#include <target/cmdline.h>

#define SPD_SA_MASK	0x0E
#define I2C2SPD(i2c)	(((i2c) & SPD_SA_MASK) >> 1)

static uint16_t spd5_bytes[SPD5_NVM_WRITES];
static uint16_t spd5_cmd;
static uint8_t *spd5_buf;
static uint16_t spd5_txlen;
static uint16_t spd5_rxlen;
static bool spd5_i2c_legacy_2bytes;

#define spd5_hlen	(spd5_i2c_legacy_2bytes ? 2 : 1)

static void __spd_hw_i2c_iocb(i2c_len_t len)
{
	if (i2c_dir_mode() == I2C_MODE_TX) {
		while (len) {
			switch (spd5_txlen) {
			case 0:
				i2c_write_byte(LOBYTE(spd5_cmd));
				break;
			case 1:
				if (spd5_i2c_legacy_2bytes)
					i2c_write_byte(HIBYTE(spd5_cmd));
				else
					i2c_write_byte(spd5_buf[spd5_txlen - spd5_hlen]);
				break;
			default:
				i2c_write_byte(spd5_buf[spd5_txlen - spd5_hlen]);
				break;
			}
			spd5_txlen++;
			len--;
		}
	} else {
		while (len) {
			spd5_buf[spd5_rxlen] = i2c_read_byte();
			spd5_rxlen++;
			len--;
		}
	}
}

i2c_device_t spd5118_i2c = {
	__spd_hw_i2c_iocb,
};

int spd5_read_block(uint8_t hid, uint8_t blk, uint8_t adr,
		    uint8_t *buf, int len)
{
	i2c_register_device(&spd5118_i2c);
	if (blk >= SPD5_EE_BLK_CNT || adr >= SPD5_EE_BLK_SIZE) {
		con_err("spd5: Wrong blk/adr %d/%d\n", blk, adr);
		return -EINVAL;
	}
	if ((adr + len) > SPD5_EE_BLK_SIZE)
		len = SPD5_EE_BLK_SIZE - adr;
	spd5_buf = buf;
	spd5_txlen = 0;
	spd5_rxlen = 0;
	if (spd5_i2c_legacy_2bytes) {
		spd5_cmd = SPD5_I2C2_MEM(blk, adr);
		i2c_master_submit(SPD5_HUB(hid), 2, len);
	} else {
		spd5_cmd = SPD5_I2C1_MEM(blk, adr);
		i2c_master_submit(SPD5_HUB(hid), 1, len);
	}
	return spd5_rxlen;
}

int spd5_read_page(uint8_t hid, uint8_t page, uint8_t addr,
		   uint8_t *buf, int len)
{
	int len0 = 0;
	int len1 = 0;
	int ret, size = 0;

	if (page >= SPD5_EE_PAGE_CNT || addr >= SPD5_EE_PAGE_SIZE) {
		con_err("spd5: Wrong page/addr %d/%d\n", page, addr);
		return -EINVAL;
	}
	spd5_set_legacy_page(hid, page);
	if ((addr + len) > SPD5_EE_PAGE_SIZE)
		len = SPD5_EE_PAGE_SIZE - addr;
	if (addr + len < SPD5_EE_BLK_SIZE) {
		len0 = len;
		len1 = 0;
	} else if (addr > SPD5_EE_BLK_SIZE) {
		len0 = 0;
		len1 = len;
	} else {
		len0 = SPD5_EE_BLK_SIZE - addr;
		len1 = len - len0;
	}
	if (len0 > 0) {
		ret = spd5_read_block(hid, 0, addr, buf, len0);
		if (ret < 0)
			return ret;
		size += ret;
	}
	if (len1 > 0) {
		if (addr < SPD5_EE_BLK_SIZE)
			addr = 0;
		else
			addr -= SPD5_EE_BLK_SIZE;
		ret = spd5_read_block(hid, 1, addr, buf+len0, len1);
		if (ret < 0)
			return ret;
		size += ret;
	}
	return size;
}

int spd_hw_read_bytes(uint8_t hid, uint16_t offset, uint8_t *buf, int len)
{
	uint8_t page;
	uint8_t addr;
	uint8_t lens;
	int ret, size = 0;

	i2c_register_device(&spd5118_i2c);
	if (offset >= SPD5_EE_SIZE || (offset + len) > SPD5_EE_SIZE) {
		con_err("spd5: Invalid range [0x%x-0x%x]\n",
			offset, offset + len);
		return -1;
	}

	page = offset / SPD5_EE_PAGE_SIZE;
	addr = offset % SPD5_EE_PAGE_SIZE;
	lens = SPD5_EE_PAGE_SIZE - addr;

	while (size < len) {
		ret = spd5_read_page(hid, page, addr, buf, lens);
		if (ret < 0)
			return ret;
		len -= lens;
		size += lens;
		buf += lens;
		addr = 0;
		if (len > SPD5_EE_PAGE_SIZE)
			lens = SPD5_EE_PAGE_SIZE;
		else
			lens = len;
		page++;
	}
	return size;
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
	spd5_txlen = 0;
	spd5_rxlen = 0;
	if (spd5_i2c_legacy_2bytes) {
		spd5_cmd = SPD5_I2C2_REG(adr);
		i2c_master_submit(SPD5_HUB(hid), 2, 1);
	} else {
		spd5_cmd = SPD5_I2C1_REG(adr);
		i2c_master_submit(SPD5_HUB(hid), 1, 1);
	}
	return val;
}

void spd5_write_reg(uint8_t hid, uint8_t adr, uint8_t val)
{
	i2c_register_device(&spd5118_i2c);
	spd5_buf = &val;
	spd5_txlen = 0;
	spd5_rxlen = 0;
	if (spd5_i2c_legacy_2bytes) {
		spd5_cmd = SPD5_I2C2_REG(adr);
		i2c_master_submit(SPD5_HUB(hid), 3, 0);
	} else {
		spd5_cmd = SPD5_I2C1_REG(adr);
		i2c_master_submit(SPD5_HUB(hid), 2, 0);
	}
}

void spd5_cfg_legacy_mode(uint8_t hid, bool mode_2bytes)
{
	if (mode_2bytes)
		spd5_set_reg(hid, SPD5_I2C_LEGACY_MODE_ADDR, SPD5_i2c_lcfg);
	else
		spd5_clear_reg(hid, SPD5_I2C_LEGACY_MODE_ADDR, SPD5_i2c_lcfg);
	spd5_i2c_legacy_2bytes = mode_2bytes;
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
		if (adr == SPD5_MR(11)) {
			printf("Please use \"cfg legacy\" command\n");
			return -EINVAL;
		}
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

static int do_spd5_mem(int argc, char *argv[])
{
	uint8_t hid;
	uint8_t blk_or_page;
	uint8_t adr = 0;
	uint8_t buf[SPD5_EE_PAGE_SIZE];
	size_t size;

	if (argc < 5)
		return -EINVAL;
	hid = (uint8_t)strtoull(argv[3], 0, 0);
	blk_or_page = (uint8_t)strtoull(argv[4], 0, 0);
	if (argc >= 5)
		adr = (uint8_t)strtoull(argv[5], 0, 0);
	if (strcmp(argv[2], "block") == 0) {
		if (spd5_i2c_legacy_2bytes) {
		       if (blk_or_page > 1)
				return -EINVAL;
		} else {
			if (blk_or_page > SPD5_EE_BLK_CNT)
				return -EINVAL;
		}
		size = spd5_read_block(hid, blk_or_page,
				       adr, buf, SPD5_EE_BLK_SIZE);
		hexdump(0, buf, 1, size);
	}
	if (strcmp(argv[2], "page") == 0) {
		if (blk_or_page > SPD5_EE_PAGE_CNT)
			return -EINVAL;
		size = spd5_read_page(hid, blk_or_page,
				      adr, buf, SPD5_EE_PAGE_SIZE);
		hexdump(0, buf, 1, size);
	}
	return 0;
}

static int do_spd5_cfg(int argc, char *argv[])
{
	uint8_t hid;
	uint8_t page;

	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "legacy") == 0) {
		if (argc < 5)
			return -EINVAL;
		hid = (uint8_t)strtoull(argv[3], 0, 0);
		if (strcmp(argv[4], "1") == 0)
			spd5_cfg_legacy_mode(hid, false);
		else if (strcmp(argv[4], "2") == 0)
			spd5_cfg_legacy_mode(hid, true);
		else
			return -EINVAL;
		if (argc >= 5) {
			page = (uint8_t)strtoull(argv[5], 0, 0);
			spd5_set_legacy_page(hid, page);
		}
	}
	return 0;
}

static int do_spd5(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "reg") == 0)
		return do_spd5_reg(argc, argv);
	if (strcmp(argv[1], "mem") == 0)
		return do_spd5_mem(argc, argv);
	if (strcmp(argv[1], "cfg") == 0)
		return do_spd5_cfg(argc, argv);
	return 0;
}

DEFINE_COMMAND(spd5, do_spd5, "SPD5118 hub commands",
	"reg write <hid> <MR> <value>\n"
	"    - write hub MR register\n"
	"reg read <hid> <MR>\n"
	"    - read hub MR register\n"
	"mem block <hid> <block> [address]\n"
	"    - read hub NVM eeprom block\n"
	"mem page <hid> <page> [address]\n"
	"    - read hub NVM eeprom page\n"
	"cfg legacy <hid> <1|2> [page]\n"
	"    - config I2C legacy mode\n"
	"      1: 1 byte addressing (default)\n"
	"      2: 2 byte addressing\n"
);
