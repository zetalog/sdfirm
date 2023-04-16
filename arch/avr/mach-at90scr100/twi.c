#include <target/i2c.h>
#include <target/irq.h>

#define __TWI_HW_TWCR_DEF	(_BV(TWEN) | _BV(TWINT))

void i2c_hw_transfer_reset(void)
{
	TWDR = 0xFF;
	TWCR = 0x00;
}

void __twi_hw_translate_status(void)
{
wait:
	while (!(TWCR & TWINT));

	switch (TWI_STATUS) {
	case TWI_STATUS_M_START:
	case TWI_STATUS_M_RESTART:
		if (i2c_bus_mode() != I2C_MODE_MASTER)
			i2c_set_status(I2C_STATUS_ARBI);
		else
			i2c_set_status(I2C_STATUS_START);
		break;
	case TWI_STATUS_MT_ADDR_ACK:
	case TWI_STATUS_MT_DATA_ACK:
		if (i2c_bus_dir_mode() != I2C_MODE_MASTER_TX)
			i2c_set_status(I2C_STATUS_ARBI);
		else
			i2c_set_status(I2C_STATUS_ACK);
		break;
	case TWI_STATUS_MR_ADDR_ACK:
	case TWI_STATUS_MR_DATA_ACK:
		if (i2c_bus_dir_mode() != I2C_MODE_MASTER_RX)
			i2c_set_status(I2C_STATUS_ARBI);
		else
			i2c_set_status(I2C_STATUS_ACK);
		break;
	case TWI_STATUS_MT_ADDR_NACK:
	case TWI_STATUS_MT_DATA_NACK:
		if (i2c_bus_dir_mode() != I2C_MODE_MASTER_TX)
			i2c_set_status(I2C_STATUS_ARBI);
		else
			i2c_set_status(I2C_STATUS_NACK);
		break;
	case TWI_STATUS_MR_ADDR_NACK:
	case TWI_STATUS_MR_DATA_NACK:
		if (i2c_bus_dir_mode() != I2C_MODE_MASTER_RX)
			i2c_set_status(I2C_STATUS_ARBI);
		else
			i2c_set_status(I2C_STATUS_NACK);
		break;
	case TWI_STATUS_M_ARB_LOST:
	case TWI_STATUS_ERROR:
		i2c_set_status(I2C_STATUS_ARBI);
		break;
	case TWI_STATUS_NONE:
		goto wait;
	}
}

void i2c_hw_start_condition(bool sr)
{
	TWCR = __TWI_HW_TWCR_DEF | _BV(TWSTA);
	__twi_hw_translate_status();
}

void i2c_hw_stop_condition(void)
{
	TWCR = __TWI_HW_TWCR_DEF | _BV(TWSTO);
}

void i2c_hw_write_byte(uint8_t byte)
{
	TWDR = byte;
	TWCR = __TWI_HW_TWCR_DEF;
	__twi_hw_translate_status();
}

uint8_t i2c_hw_read_byte(void)
{
	__twi_hw_translate_status();
	return TWDR;
}

void i2c_hw_set_frequency(uint16_t khz)
{
	uint16_t tmp;
	uint8_t twps;

	/* calc TWBR * (4 ^ TWPS) */
	tmp = div16u(div16u(CLK_CPU, khz) - 16, 2);

	/* calc TWPS */
	twps = (uint8_t)(__fls16(tmp)) / 2;
	if (twps > TWSR_TWPS_MAX)
		twps = TWSR_TWPS_MAX;
	TWSR &= ~TWSR_TWPS_MASK;
	TWSR |= (twps << TWSR_TWPS_OFFSET);

	/* calc TWBR */
	TWBR = div16u(tmp, ((uint16_t)(1)<<(2*twps)));
}

void i2c_hw_set_address(i2c_addr_t addr, boolean call)
{
	TWAR &= ~TWI_ADDR_MASK;
	TWAR = (addr << TWI_ADDR_OFFSET) | (call ? _BV(TWGCE) : 0);
}

void i2c_hw_ctrl_init(void)
{
	AT90SCR100_POWER_ENABLE(PRR0, PRTWI);
}
