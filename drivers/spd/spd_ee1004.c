#include <target/console.h>
#include <target/panic.h>
#include <target/ddr.h>

#define SPD_SA_MASK	0x0E
#define I2C2SPD(i2c)	(((i2c) & SPD_SA_MASK) >> 1)

static uint8_t *spd_buf;
static uint8_t spd_pos;
static uint8_t spd_page = 0;
static uint16_t spd_len;

static void __spd_hw_i2c_iocb(i2c_len_t len)
{
	if (i2c_dir_mode() == I2C_MODE_TX) {
		BUG_ON(len != 1);
		i2c_write_byte(spd_pos);
	} else {
		i2c_len_t i;

		for (i = 0; i < len; i++)
			spd_buf[spd_len++] = i2c_read_byte();
	}
}

i2c_device_t spd_ee1004 = {
	__spd_hw_i2c_iocb,
};

static int spd_ee1004_set_page(uint8_t new_page)
{
	i2c_addr_t spa;

	if (new_page > 0)
		new_page = 1;

	/* No need to switch page */
	if ((new_page == 0 && spd_page == 0) ||
	    (new_page != 0 && spd_page != 0))
		return 0;

	if (spd_page == 0)
		spa = SPD_DSC_PA0;
	else
		spa = SPD_DSC_PA1;
	/* XXX SPA repeats as Data */
	spd_pos = spa;
	spd_page = new_page;
	i2c_master_write(spa, 1);
	return 0;
}

int spd_hw_read_bytes(uint8_t lsa, uint16_t offset,
		      uint8_t *buffer, int len)
{
	int len0 = 0;
	int len1 = 0;

	i2c_register_device(&spd_ee1004);
	if (offset >= SPD_EE_SIZE || (offset + len) > SPD_EE_SIZE) {
		con_err("spd_ee1004: Invalid range [0x%x-0x%x]\n",
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
		/* spd_ee1004_set_page(0); */
		spd_pos = offset;
		spd_buf = buffer;
		i2c_master_read(SPD_DSC_SPD(lsa), len0);
	}
	if (len1 > 0) {
		if (offset < SPD_EE_PAGE_SIZE)
			offset = 0;
		else
			offset -= SPD_EE_PAGE_SIZE;
		/* spd_ee1004_set_page(1); */
		spd_pos = offset;
		spd_buf = buffer + len0;
		spd_len = 0;
		i2c_master_read(SPD_DSC_SPD(lsa), len1);
	}
	return len;
}

void spd_ee1004_init(void)
{
}
