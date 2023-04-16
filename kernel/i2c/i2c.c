#include <target/i2c.h>
#include <target/panic.h>

#if NR_I2C_MASTERS > 1
struct i2c_master i2c_masters[NR_I2C_MASTERS];
i2c_t i2c_mid;

void i2c_master_select(i2c_t i2c)
{
	i2c_hw_master_select(i2c);
	i2c_mid = i2c;
}

i2c_t i2c_master_save(i2c_t i2c)
{
	i2c_t i2cs = i2c_mid;
	i2c_mid = i2c;
	return i2cs;
}
#else
/* slave address addressed by the i2c controller */
i2c_addr_t i2c_target;
/* address used by the i2c controller when it acts as a slave device */
i2c_addr_t i2c_address;
uint16_t i2c_freq;
uint8_t i2c_mode;
/* transfer variables */
i2c_len_t i2c_limit;
i2c_len_t i2c_current;
uint8_t i2c_status;
i2c_device_t *i2c_device = NULL;
#endif

static void i2c_config_mode(uint8_t mode, bool freq);

uint8_t i2c_bus_dir_mode(void)
{
	return I2C_BUS_DIR(i2c_mode);
}

uint8_t i2c_bus_mode(void)
{
	return I2C_BUS(i2c_mode);
}

uint8_t i2c_dir_mode(void)
{
	return I2C_DIR(i2c_mode);
}

static void i2c_transfer_reset(void)
{
	i2c_set_status(I2C_STATUS_IDLE);
	i2c_hw_transfer_reset();
}

bool i2c_last_byte(void)
{
	return (i2c_current + 1) == i2c_limit;
}

static void i2c_handle_status(void)
{
	/* check I2C controller driver to avoid this */
	BUG_ON(i2c_status == I2C_STATUS_START);

	switch (i2c_status) {
	case I2C_STATUS_ACK:
		i2c_current++;
		if (i2c_current == i2c_limit) {
			i2c_set_status(I2C_STATUS_STOP);
		} else {
			i2c_set_status(I2C_STATUS_START);
		}
		break;
	}
}

uint8_t i2c_read_byte(void)
{
	uint8_t byte;

	BUG_ON(!i2c_device);
	if (i2c_status != I2C_STATUS_START)
		return 0;

	byte = i2c_hw_read_byte();
	i2c_handle_status();

	return byte;
}

void i2c_write_byte(uint8_t byte)
{
	BUG_ON(!i2c_device);
	if (i2c_status != I2C_STATUS_START)
		return;

	i2c_hw_write_byte(byte);
	i2c_handle_status();
}

#ifdef CONFIG_I2C_MASTER
#define i2c_addr_mode(addr, mode)	((addr << 1) | mode)

#ifdef CONFIG_I2C_10BIT_ADDRESS
static void i2c_write_address(void)
{
}
#else
static void i2c_write_address(void)
{
	i2c_write_byte(i2c_addr_mode(i2c_target, i2c_dir_mode()));
}
#endif

void i2c_apply_frequency(void)
{
	i2c_hw_set_frequency(I2C_FREQ);
}

uint8_t i2c_master_submit(i2c_addr_t slave,
			  i2c_len_t txlen, i2c_len_t rxlen)
{
	/* IDLE means start, STOP means repeated start */
	BUG_ON(!i2c_device ||
	       (i2c_status != I2C_STATUS_IDLE &&
	        i2c_status != I2C_STATUS_STOP));

	i2c_target = slave;

	if (txlen > 0) {
		i2c_limit = txlen + I2C_ADDR_LEN;
		i2c_current = 0;
		i2c_config_mode(I2C_MODE_MASTER_TX, true);
		i2c_hw_start_condition(false);
		while (i2c_status == I2C_STATUS_IDLE ||
		       i2c_status == I2C_STATUS_STOP);
		i2c_write_address();
		i2c_device->iocb(txlen);
	}

	if (rxlen > 0) {
		i2c_limit = rxlen + I2C_ADDR_LEN;
		i2c_current = 0;
		i2c_config_mode(I2C_MODE_MASTER_RX, txlen == 0);
		i2c_hw_start_condition(txlen != 0);
		while (i2c_status == I2C_STATUS_IDLE ||
		       i2c_status == I2C_STATUS_STOP);
		i2c_write_address();
		i2c_device->iocb(rxlen);
	}

	return i2c_status;
}

uint8_t i2c_master_write(i2c_addr_t slave, i2c_len_t txlen)
{
	return i2c_master_submit(slave, txlen, 0);
}

uint8_t i2c_master_read(i2c_addr_t slave, i2c_len_t rxlen)
{
	return i2c_master_submit(slave, 0, rxlen);
}

void i2c_master_release(void)
{
	i2c_hw_stop_condition();
	i2c_transfer_reset();
}

#ifdef CONFIG_I2C_GENERAL_CALL
uint8_t i2c_general_call(uint8_t cbyte, i2c_len_t limit)
{
	uint8_t status;

	/* IDLE means start, STOP means repeated start */
	BUG_ON(!i2c_device || (cbyte == I2C_CALL_INVAL) ||
	       (i2c_status != I2C_STATUS_IDLE &&
	        i2c_status != I2C_STATUS_STOP));

	i2c_config_mode(I2C_MODE_MASTER_TX, true);
	i2c_target = cbyte;
	i2c_limit = limit + 2;
	i2c_current = 0;

	i2c_hw_start_condition(false);
	while (i2c_status == I2C_STATUS_IDLE ||
	       i2c_status == I2C_STATUS_STOP);

	i2c_write_byte(i2c_addr_mode(I2C_ADDR_CALL, I2C_MODE_TX));
	i2c_write_byte(i2c_target);
	i2c_device->iocb(limit);

	status = i2c_status;
	i2c_master_release();
	return status;
}
#endif

#ifdef CONFIG_I2C_DEVICE_ID
uint32_t i2c_probe_devid(i2c_addr_t slave)
{
	uint8_t i;
	i2c_devid_t devid = I2C_DEVID_INVAL;

	/* IDLE means start, STOP means repeated start */
	BUG_ON(!i2c_device ||
	       (i2c_status != I2C_STATUS_IDLE &&
	        i2c_status != I2C_STATUS_STOP));

	i2c_config_mode(I2C_MODE_MASTER_TX, true);
	i2c_target = slave;
	i2c_limit = 1 + I2C_ADDR_LEN;
	i2c_current = 0;

	i2c_hw_start_condition(false);
	while (i2c_status == I2C_STATUS_IDLE ||
	       i2c_status == I2C_STATUS_STOP);

	i2c_write_byte(i2c_addr_mode(I2C_ADDR_DEVID, I2C_MODE_TX));
	i2c_write_address();
	if (i2c_status != I2C_STATUS_STOP) {
		i2c_master_release();
		return I2C_DEVID_INVAL;
	}

	i2c_config_mode(I2C_MODE_MASTER_RX, false);
	i2c_target = slave;
	i2c_limit = I2C_DEVID_LEN + I2C_ADDR_LEN;
	i2c_current = 0;
	i2c_write_address();

	for (i = 0; i < 3; i++)
		devid |= (i2c_read_byte() << (3-i));
	if (i2c_status != I2C_STATUS_STOP) {
		i2c_master_release();
		return I2C_DEVID_INVAL;
	}

	i2c_master_release();
	return devid;
}
#endif
#endif

#ifdef CONFIG_I2C_SLAVE
#ifdef CONFIG_I2C_10BIT_ADDRESS
static void i2c_read_address(void)
{
}
#else
static void i2c_read_address(void)
{
	uint8_t byte;

	byte = i2c_hw_read_byte();
	I2C_SET_DIR(i2c_mode, byte & I2C_DIR_MASK);
	i2c_target = (byte & I2C_ADDR_MASK) >> I2C_ADDR_OFFSET;
}
#endif

void i2c_set_address(i2c_addr_t addr, boolean call)
{
	i2c_address = addr;
	if (call) {
		i2c_mode |= I2C_MODE_CALL;
	} else {
		i2c_mode &= ~I2C_MODE_CALL;
	}
}

void i2c_apply_address(void)
{
	if (i2c_device)
		i2c_hw_set_address(i2c_address, I2C_CALL(i2c_mode));
}
#endif

static void i2c_config_mode(uint8_t mode, bool freq)
{
	i2c_mode = mode;

	if (I2C_BUS(i2c_mode) == I2C_MODE_MASTER) {
		if (freq)
			i2c_apply_frequency();
	} else {
		i2c_apply_address();
	}
}

void i2c_register_device(i2c_device_t *dev)
{
	i2c_device = dev;
}

void i2c_set_status(uint8_t status)
{
	i2c_status = status;
}

void i2c_master_init(void)
{
	i2c_hw_ctrl_init();
}

void i2c_init(void)
{
	i2c_t i2c;
	__unused i2c_t si2c;

	for (i2c = 0; i2c <= NR_I2C_MASTERS; i2c++) {
		si2c = i2c_master_save(i2c);
		i2c_master_init();
		i2c_transfer_reset();
		i2c_master_restore(si2c);
	}
}
