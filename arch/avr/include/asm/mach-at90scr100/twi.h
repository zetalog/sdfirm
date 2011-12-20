#ifndef __TWI_AT90SCR100_H_INCLUDE__
#define __TWI_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>

#ifdef CONFIG_TWI_AT90SCR100
#ifndef ARCH_HAVE_I2C
#define ARCH_HAVE_I2C		1
#else
#error "Multiple I2C controller defined"
#endif
#endif

#define TWBR	_SFR_MEM8(0xB8)
#define TWSR	_SFR_MEM8(0xB9)

#define TWAR	_SFR_MEM8(0xBA)
/* general call recognition enable */
#define TWGCE	0

#define TWDR	_SFR_MEM8(0xBB)

/* TWI control register */
#define TWCR	_SFR_MEM8(0xBC)
/* interrupt flag */
#define TWINT	7
/* enable acknowledge */
#define TWEA	6
/* start condition */
#define TWSTA	5
/* stop condition */
#define TWSTO	4
/* write collision */
#define TWWC	3
/* enable */
#define TWEN	2
/* interrupt enable */
#define TWIE	0

#define TWAMR   _SFR_MEM8(0xBD)

/* TWAR/TWAMR use this as a mask of slave address */
#define TWI_ADDR_OFFSET		1
#define TWI_ADDR_MASK		(0x7F<<TWI_ADDR_OFFSET)

#define TWSR_TWS_OFFSET		3
#define TWSR_TWS_MASK		(0x1F<<TWSR_TWS_OFFSET)
#define TWSR_TWPS_OFFSET	0
#define TWSR_TWPS_MASK		(0x03<<TWSR_TWPS_OFFSET)
#define TWSR_TWPS_MAX		3

#define TWI_STATUS			(TWSR & TWSR_TWS_MASK)
#define TWI_STATUS_M_START		0x08
#define TWI_STATUS_M_RESTART		0x10
#define TWI_STATUS_M_ARB_LOST		0x38
#define TWI_STATUS_MT_ADDR_ACK		0x18
#define TWI_STATUS_MT_ADDR_NACK		0x20
#define TWI_STATUS_MT_DATA_ACK		0x28
#define TWI_STATUS_MT_DATA_NACK		0x30

#define TWI_STATUS_MR_ADDR_ACK		0x40
#define TWI_STATUS_MR_ADDR_NACK		0x48
#define TWI_STATUS_MR_DATA_ACK		0x50
#define TWI_STATUS_MR_DATA_NACK		0x58

#define TWI_STATUS_NONE			0xF8
#define TWI_STATUS_ERROR		0x00

#ifdef CONFIG_TWI_AT90SCR100_FREQ_100K
#define I2C_HW_FREQ			100
#endif
#ifdef CONFIG_TWI_AT90SCR100_FREQ_200K
#define I2C_HW_FREQ			200
#endif
#ifdef CONFIG_TWI_AT90SCR100_FREQ_300K
#define I2C_HW_FREQ			300
#endif

void i2c_hw_transfer_reset(void);
void i2c_hw_start_condition(void);
void i2c_hw_stop_condition(void);
void i2c_hw_write_byte(uint8_t byte);
uint8_t i2c_hw_read_byte(void);

void i2c_hw_set_frequency(uint16_t khz);
void i2c_hw_set_address(i2c_addr_t addr, boolean call);

void i2c_hw_ctrl_init(void);

#endif /* __TWI_AT90SCR100_H_INCLUDE__ */
