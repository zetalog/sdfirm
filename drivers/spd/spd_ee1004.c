#include <target/console.h>
#include <target/ddr.h>

#define SPD_SA_MASK	0x07
#define I2C2SPD(i2c)	((i2c) & SPD_SA_MASK)

static uint8_t spd_last_page = 0;

static int spd_ee1004_set_page(uint8_t spd, uint8_t new_page)
{
	uint8_t spa;
	i2c_addr_t i2c;
	int ret;

	if (new_page > 0)
		new_page = 1;

	/* No need to switch page */
	if ((new_page == 0 && spd_last_page == 0) ||
	    (new_page != 0 && spd_last_page != 0))
		return 0;

	if (spd_last_page == 0)
		spa = SPD_DSC_SPA0;
	else
		spa = SPD_DSC_SPA1;
	i2c = (spa >> 1) | spd;
	/* XXX SPA repeats as Data */
	ret = dw_i2c_write_bytes(i2c, &spa, 1, 1);
	if (ret < 0) {
		con_err("spd_ee1004: write SPA failure i2c=%u\n", i2c);
		return ret;
	}
	spd_last_page = new_page;
	return 0;
}

static int spd_ee1004_write_dummy(i2c_addr_t i2c, uint8_t offset)
{
	int ret;

	ret = dw_i2c_write_bytes(i2c, &offset, 1, 1);
	if (ret != 1) {
		con_err("spd_ee1004: write dummy failure, i2c=0x%x\n", i2c);
		return ret;
	}
	return 0;
}

int spd_hw_read_bytes(i2c_addr_t i2c, uint16_t offset,
		      uint8_t *buffer, int len)
{
	uint8_t spd = I2C2SPD(i2c);
	int len0 = 0;
	int len1 = 0;
	int ret;

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
		spd_ee1004_set_page(spd, 0);
		spd_ee1004_write_dummy(i2c, offset);
		ret = dw_i2c_read_bytes(i2c, buffer, len0, 1);
		if (ret < 0)
			return -1;
	}
	if (len1 > 0) {
		if (offset < SPD_EE_PAGE_SIZE)
			offset = 0;
		else
			offset -= SPD_EE_PAGE_SIZE;
		spd_ee1004_set_page(spd, 1);
		spd_ee1004_write_dummy(i2c, offset);
		ret = dw_i2c_read_bytes(i2c, buffer + len0, len1, 1);
		if (ret < 0)
			return -1;
	}
	return len;
}
