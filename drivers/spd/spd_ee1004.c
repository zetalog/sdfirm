#include <target/console.h>
#include <target/i2c.h>
#include <asm/mach/spd.h>

#include "spd_ee1004.h"

#define SPD_EE1004_DEBUG

#ifdef CONFIG_SPD_BUS_KHZ
#define SPD_BUS_KHZ CONFIG_SPD_BUS_KHZ
#else
#define SPD_BUS_KHZ 100
#endif

static uint8_t ee_page = 0;

int spd_hw_init(void)
{
	unsigned int i2c_master = SPD_BUS_NUM;
	uint16_t i2c_freq = SPD_BUS_KHZ;

#ifdef SPD_EE1004_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif

	dw_i2c_init();
#ifdef SPD_EE1004_DEBUG
	con_printf("Debug: Select I2C Master = %u\n", i2c_master);
#endif
	i2c_master_select(i2c_master);
	i2c_master_init();
#ifdef SPD_EE1004_DEBUG
	con_printf("Debug: Set I2C frequency = %u KHz\n", i2c_freq);
#endif
	i2c_set_frequency(i2c_freq);
	return 0;
}

static int spd_ee_set_page(uint8_t new_page)
{
	uint8_t dsc;
	int ret;

	if (new_page > 0) {
		new_page = 1;
	}

	/* No need to switch page. */
	if ((new_page == 0 && ee_page == 0) || (new_page != 0 && ee_page != 0)) {
		return 0;
	}

	if (ee_page == 0) {
		dsc = SPD_DSC_SPA0;
	} else {
		dsc = SPD_DSC_SPA1;
	}
#ifdef SPD_EE1004_DEBUG
	con_printf("Debug: Set EE page to = %u\n", new_page);
#endif
	ret = dw_i2c_write_bytes((dsc >> 1), &dsc, 1, 1); /* XXX DSC repeats as Data */
	if (ret < 0) {
		con_printf("Error: Failed to send DSC SPAx = %u\n", dsc);
		return ret;
	}
	ee_page = new_page;
	return 0;
}

static int spd_ee_dummy_write(uint8_t dev, uint8_t addr)
{
	uint8_t dsc;
	int ret;
	dsc = SPD_DSC_WSPD | (dev << 1);
#ifdef SPD_EE1004_DEBUG
	con_printf("Debug: Do dummy write. dsc = 0x%x, addr = 0x%x\n", dsc, addr);
#endif
	ret = dw_i2c_write_bytes((dsc >> 1), &addr, 1, 1);
	if (ret != 1) {
		con_printf("Error: Failed to do dummy write. dsc = 0x%x, ret = %d\n", dsc, ret);
		return ret;
	}
	return 0;
}

#if 0
static int spd_ee_byte_write(uint8_t dev, uint8_t addr, uint8_t data)
{
	uint8_t dsc;
	uint8_t buf[] = {addr, data};
	int ret;

	dsc = SPD_DSC_WSPD | (dev << 1);
#ifdef SPD_EE1004_DEBUG
	con_printf("Debug: Do dummy wirte. dsc = 0x%x, addr = 0x%x, data = 0x%x\n", dsc, addr, data);
#endif
	ret = dw_i2c_write_bytes((dsc >> 1), buf, sizeof(buf), 1);
	if (ret != 1) {
		con_printf("Error: Failed to do byte wirte. dsc = 0x%x, ret = %d\n", dsc, ret);
		return ret;
	}
	return 1;
}
#endif

static int spd_ee_page_write(uint8_t dev, uint8_t addr, uint8_t *data, int len)
{
	uint8_t dsc;
	int i2c_tx_len;
	uint8_t i2c_tx_buf[SPD_EE_PAGE_WRITE_SIZE + 1] = {0};
	int ret;

	if (data == NULL) return -1;
	if (len <= 0 || len > SPD_EE_PAGE_WRITE_SIZE) return -1;
	if ((addr + len) > SPD_EE_PAGE_SIZE) return -1;

	dsc = SPD_DSC_WSPD | (dev << 1);
	i2c_tx_buf[0] = addr;
	for (int i = 0; i < len; i++) {
		i2c_tx_buf[i + 1] = data[i];
	}
	i2c_tx_len = len + 1;
#ifdef SPD_EE1004_DEBUG
	con_printf("Debug: Do page wirte. dsc = 0x%x, addr = 0x%x, len = 0x%x\n", dsc, addr, len);
#endif
	ret = dw_i2c_write_bytes((dsc >> 1), i2c_tx_buf, i2c_tx_len, 1);
	if (ret != i2c_tx_len) {
		con_printf("Error: Failed to do page wirte. dsc = 0x%x, ret = %d\n", dsc, ret);
		return ret;
	}
	return len;
}

static int spd_ee_write_in_page(uint8_t dev, int page, uint8_t addr, uint8_t *data, int len)
{
	int tx_len = 0;
	if (data == NULL) return -1;

	if (page == 0)
		spd_ee_set_page(0);
	else
		spd_ee_set_page(1);
	while (tx_len < len) {
		int this_len;
		if ((len - tx_len) > SPD_EE_PAGE_WRITE_SIZE)
			this_len = SPD_EE_PAGE_WRITE_SIZE;
		else
			this_len = len - tx_len;
		spd_ee_page_write(dev, (addr + tx_len), (data + tx_len), this_len);
		tx_len += this_len;
	}
	return len;
}

int spd_hw_read_bytes(uint8_t dev, uint16_t addr, uint8_t *buffer, int len)
{
	uint8_t dsc;
	int len_in_page_0 = 0;
	int len_in_page_1 = 0;
	int ret;

	if (buffer == NULL) {
		con_printf("Error: Invalid buffer = %p\n", buffer);
		return -1;
	}
	if (dev >= SPD_COUNT) {
		con_printf("Error: Invalid SPD num = %u\n", dev);
		return -1;
	}
	if (addr >= SPD_EE_SIZE || (addr + len) > SPD_EE_SIZE) {
		con_printf("Error: Invalid EE addr = 0x%x, len = %d\n", addr, len);
		return -1;
	}

	dsc = SPD_DSC_RSPD | (dev << 1);

	if (addr + len < SPD_EE_PAGE_SIZE) {
		len_in_page_0 = len;
		len_in_page_1 = 0;
	} else if (addr > SPD_EE_PAGE_SIZE) {
		len_in_page_0 = 0;
		len_in_page_1 = len;
	} else {
		len_in_page_0 = SPD_EE_PAGE_SIZE - addr;
		len_in_page_1 = len - len_in_page_0;
	}

	if (len_in_page_0 > 0) {
		int this_len = len_in_page_0;
		uint8_t this_addr = (uint8_t)addr;
		spd_ee_set_page(0);
		spd_ee_dummy_write(dev, this_addr);
#ifdef SPD_EE1004_DEBUG
		con_printf("Debug: Read EE page 0. dsc = 0x%x, len = %d\n", dsc, this_len);
#endif
		ret = dw_i2c_read_bytes((dsc >> 1), buffer, this_len, 1);
		if (ret < 0) {
			con_printf("Error: Failed to read EE page 0. dsc = 0x%x, len = %d, ret = %d\n", dsc, this_len, ret);
			return -1;
		}
	}

	if (len_in_page_1 > 0) {
		int this_len = len_in_page_1;
		uint8_t this_addr;
		if (addr < SPD_EE_PAGE_SIZE) {
			this_addr = 0;
		} else {
			this_addr = (uint8_t)(addr - SPD_EE_PAGE_SIZE);
		}
		spd_ee_set_page(1);
		spd_ee_dummy_write(dev, this_addr);
#ifdef SPD_EE1004_DEBUG
		con_printf("Debug: Read EE page 1. dsc = 0x%x, len = %d\n", dsc, this_len);
#endif
		ret = dw_i2c_read_bytes((dsc >> 1), (buffer + len_in_page_0), this_len, 1);
		if (ret < 0) {
			con_printf("Error: Failed to read EE page 1. dsc = 0x%x, len = %d, ret = %d\n", dsc, this_len, ret);
			return -1;
		}
	}
	return len;
}

int spd_hw_write_bytes(uint8_t dev, uint16_t addr, uint8_t *buffer, int len)
{
	uint8_t dsc;
	int len_in_page_0 = 0;
	int len_in_page_1 = 0;
	int this_len;
	uint8_t this_addr;
	int ret;

	if (buffer == NULL) {
		con_printf("Error: Invalid buffer = %p\n", buffer);
		return -1;
	}
	if (dev >= SPD_COUNT) {
		con_printf("Error: Invalid SPD num = %u\n", dev);
		return -1;
	}
	if (addr >= SPD_EE_SIZE || (addr + len) > SPD_EE_SIZE) {
		con_printf("Error: Invalid EE addr = 0x%x, len = %d\n", addr, len);
		return -1;
	}

	dsc = SPD_DSC_WSPD | (dev << 1);

	if (addr + len < SPD_EE_PAGE_SIZE) {
		len_in_page_0 = len;
		len_in_page_1 = 0;
	} else if (addr > SPD_EE_PAGE_SIZE) {
		len_in_page_0 = 0;
		len_in_page_1 = len;
	} else {
		len_in_page_0 = SPD_EE_PAGE_SIZE - addr;
		len_in_page_1 = len - len_in_page_0;
	}

	if (len_in_page_0 > 0) {
		this_len = len_in_page_0;
		this_addr = (uint8_t)addr;
		ret = spd_ee_write_in_page(dev, 0, this_addr, buffer, this_len);
		if (ret < 0) {
			con_printf("Error: Failed to write EE page 0. dsc = 0x%x, addr = 0x%x, len = %d, ret = %d\n", dsc, this_addr, this_len, ret);
			return -1;
		}
	}

	if (len_in_page_1 > 0) {
		this_len = len_in_page_1;
		if (addr < SPD_EE_PAGE_SIZE) {
			this_addr = 0;
		} else {
			this_addr = (uint8_t)(addr - SPD_EE_PAGE_SIZE);
		}
		ret = spd_ee_write_in_page(dev, 1, this_addr, buffer, this_len);
		if (ret < 0) {
			con_printf("Error: Failed to write EE page 1. dsc = 0x%x, addr = 0x%x, len = %d, ret = %d\n", dsc, this_addr, this_len, ret);
			return -1;
		}
	}
	return len;
}
