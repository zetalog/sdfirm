#ifndef __SPD5118_H_INCLUDE__
#define __SPD5118_H_INCLUDE__

#include <target/i2c.h>

#ifndef ARCH_HAVE_SPD_EEPROM
#define ARCH_HAVE_SPD_EEPROM 1
#else
#error "Multiple DDR SPD eeprom types defined"
#endif

#define SPD5_HUB(hid)		(0x50 | (hid))
#define SPD5_RCD(hid)		(0x58 | (hid))
#define SPD5_PMIC(hid)		(0x48 | (hid))
#define SPD5_TS0(hid)		(0x10 | (hid))
#define SPD5_TS1(hid)		(0x30 | (hid))

#define SPD5_EE_BLK_SIZE	64
#define SPD5_EE_BLK_CNT		16
#define SPD5_EE_PAGE_SIZE	128
#define SPD5_EE_PAGE_CNT	8
#ifdef CONFIG_SPD5118_I2C_LEGACY
#define SPD5_EE_SIZE		(SPD5_EE_PAGE_SIZE * SPD5_EE_PAGE_CNT)
#else
#define SPD5_EE_SIZE		(SPD5_EE_BLK_SIZE * SPD5_EE_BLK_CNT)
#endif

#define SPD5_Mem		_BV(7)
#define SPD5_Reg		0
#define SPD5_R			_BV(12)
#define SPD5_W			0

#define SPD5_ADR_OFFSET		0
#define SPD5_ADR_MASK		REG_6BIT_MASK
#define SPD5_ADR(adr)		_SET_FV(SPD5_ADR, adr)
#define SPD5_BLK0_OFFSET	6
#define SPD5_BLK0_MASK		REG_1BIT_MASK
#define SPD5_BLK0(blk)		_SET_FV(SPD5_BLK0, blk)
#define SPD5_BLK14_OFFSET	8
#define SPD5_BLK14_MASK		REG_4BIT_MASK
#define SPD5_BLK14(blk)		_SET_FV(SPD5_BLK14, (blk) >> 1)
#define SPD5_BLK(blk)		(SPD5_BLK0(blk) | SPD5_BLK14(blk))
#define SPD5_CMD_OFFSET		13
#define SPD5_CMD_MASK		REG_3BIT_MASK
#define SPD5_CMD(code)		_SET_FV(SPD5_CMD, code)

#define SPD5_ADR2BLK_OFFSET	6
#define SPD5_ADR2BLK_MASK	REG_5BIT_MASK
#define spd5_adr2blk(adr)	_GET_FV(SPD5_ADR2BLK, adr)

/* 2.6.8 I2C Target Protocol - Host to SPD5 Hub Device */
/* 1-Byte Addressing Mode */
#define SPD5_I2C1_MEM(b, a)	\
	((uint8_t)(SPD5_Mem | SPD5_ADR(a) | SPD5_BLK0(b)))
#define SPD5_I2C1_REG(a)	\
	((uint8_t)(SPD5_Reg | SPD5_ADR(a) | SPD5_BLK0(spd5_adr2blk(a))))
/* 2-Bytes Addressing Mode */
#define SPD5_I2C2_MEM(b, a)	\
	((uint16_t)(SPD5_Mem | SPD5_ADR(a) | SPD5_BLK(b)))
#define SPD5_I2C2_REG(a)	\
	((uint16_t)(SPD5_Reg | SPD5_ADR(a) | SPD5_BLK(spd5_adr2blk(a))))

/* Table 87 - I3C Basic Mode Only with PEC Enabled */
#define SPD5_I3C_MEM(c, b, a)	\
	((uint16_t)(SPD5_Mem | SPD5_CMD(c) | SPD5_BLK(b) | SPD5_ADR(a)))
#define SPD5_I3C_REG(c, a)	\
	((uint16_t)(SPD5_Reg | SPD5_CMD(c) | SPD5_BLK(spd5_adr2blk(a)) | SPD5_ADR(a)))

#define SPD5_W1R(blk, adr)	(SPD5_I3C_REG(0, blk, adr) | SPD5_W)
#define SPD5_R1R(blk, adr)	(SPD5_I3C_REG(0, blk, adr) | SPD5_R)
#define SPD5_W1M(blk, adr)	(SPD5_I3C_MEM(0, blk, adr) | SPD5_W)
#define SPD5_R1M(blk, adr)	(SPD5_I3C_MEM(0, blk, adr) | SPD5_R)
#define SPD5_W2R(blk, adr)	(SPD5_I3C_REG(1, blk, adr) | SPD5_W)
#define SPD5_R2R(blk, adr)	(SPD5_I3C_REG(1, blk, adr) | SPD5_R)
#define SPD5_W2M(blk, adr)	(SPD5_I3C_MEM(1, blk, adr) | SPD5_W)
#define SPD5_R2M(blk, adr)	(SPD5_I3C_MEM(1, blk, adr) | SPD5_R)
#define SPD5_W4R(blk, adr)	(SPD5_I3C_REG(2, blk, adr) | SPD5_W)
#define SPD5_R4R(blk, adr)	(SPD5_I3C_REG(2, blk, adr) | SPD5_R)
#define SPD5_W4M(blk, adr)	(SPD5_I3C_MEM(2, blk, adr) | SPD5_W)
#define SPD5_R4M(blk, adr)	(SPD5_I3C_MEM(2, blk, adr) | SPD5_R)
#define SPD5_W16R(blk, adr)	(SPD5_I3C_REG(3, blk, adr) | SPD5_W)
#define SPD5_R16R(blk, adr)	(SPD5_I3C_REG(3, blk, adr) | SPD5_R)
#define SPD5_W16M(blk, adr)	(SPD5_I3C_MEM(3, blk, adr) | SPD5_W)
#define SPD5_R16M(blk, adr)	(SPD5_I3C_MEM(3, blk, adr) | SPD5_R)

/* 2.8.1 Write and Read Access - NVM Mmeory */
#define SPD5_NVM_WRITES		16

/* Table 102 - Register Map */
#define SPD5_MR(n)		(n)
#define SPD5_did_msb		SPD5_MR(0)  /* Device Type; MSB */
#define SPD5_did_lsb		SPD5_MR(1)  /* Device Type; LSB */
#define SPD5_rid		SPD5_MR(2)  /* Device Revision */
#define SPD5_vid0		SPD5_MR(3)  /* Vendor ID Byte0 */
#define SPD5_vid1		SPD5_MR(4)  /* Vendor ID Byte1 */
#define SPD5_dev_cap		SPD5_MR(5)  /* Device Capability */
#define SPD5_wrt		SPD5_MR(6)  /* Write Recovery Time */
#define SPD5_i2c_lcfg		SPD5_MR(11) /* I2C Legacy Mode Device Configuration */
#define SPD5_wp0		SPD5_MR(12) /* Write Protection for NVM blocks[0:7] */
#define SPD5_wp8		SPD5_MR(13) /* Write Protection for NVM blocks[8:15] */
#define SPD5_dev_cfgl		SPD5_MR(14) /* Device Configuration - Host and Local Interface IO */
#define SPD5_dev_cfg		SPD5_MR(18) /* Device Configuration */
#define SPD5_clr_ts		SPD5_MR(19) /* Clear MR51 Temperature Status Command */
#define SPD5_clr_es		SPD5_MR(20) /* Clear MR52 Error Status Command */
#define SPD5_ts_cfg		SPD5_MR(26) /* TS Configuration */
#define SPD5_irq_cfg		SPD5_MR(27) /* Interrupt Configuration */
#define SPD5_thl_lo		SPD5_MR(28) /* TS Temperature High Limit - Low Byte */
#define SPD5_thl_hi		SPD5_MR(29) /* TS Temperature High Limit - High Byte */
#define SPD5_tll_lo		SPD5_MR(30) /* TS Temperature Low Limit - Low Byte */
#define SPD5_tll_hi		SPD5_MR(31) /* TS Temperature Low Limit - High Byte */
#define SPD5_chl_lo		SPD5_MR(32) /* TS Critical Temperature High Limit - Low Byte */
#define SPD5_chl_hi		SPD5_MR(33) /* TS Critical Temperature High Limit - High Byte */
#define SPD5_cll_lo		SPD5_MR(34) /* TS Critical Temperature Low Limit - Low Byte */
#define SPD5_cll_hi		SPD5_MR(35) /* TS Critical Temperature Low Limit - High Byte */
#define SPD5_dev_sts		SPD5_MR(48) /* Device Status */
#define SPD5_tss_lo		SPD5_MR(49) /* TS Current Sensed Temperature - Low Byte */
#define SPD5_tss_hi		SPD5_MR(50) /* TS Current Sensed Temperature - High Byte */
#define SPD5_ts			SPD5_MR(51) /* TS Temperature Status */
#define SPD5_es			SPD5_MR(52) /* Hub, Thermal and NVM Error Status */

/* MR11 */
#define SPD5_I2C_LEGACY_MODE_ADDR			_BV(3)
#define SPD5_I2C_LEGACY_MODE_ADDR_POINTER_OFFSET	0
#define SPD5_I2C_LEGACY_MODE_ADDR_POINTER_MASK		REG_3BIT_MASK
#define SPD5_I2C_LEGACY_MODE_ADDR_POINTER(value)	_SET_FV(SPD5_I2C_LEGACY_MODE_ADDR_POINTER, value)

#define spd5_set_reg(l,v,a)				\
	do {						\
		uint32_t __v = spd5_read_reg((l), (a));	\
		__v |= (v);				\
		spd5_write_reg((l), __v, (a));		\
	} while (0)
#define spd5_clear_reg(l,v,a)				\
	do {						\
		uint32_t __v = spd5_read_reg((l), (a));	\
		__v &= ~(v);				\
		spd5_write_reg((l), __v, (a));		\
	} while (0)
#define spd5_write_reg_mask(l,v,m,a)			\
	do {						\
		uint32_t __v = spd5_read_reg((l), (a));	\
		__v &= ~(m);				\
		__v |= (v);				\
		spd5_write_reg((l), __v, (a));		\
	} while (0)

uint8_t spd5_read_reg(uint8_t hid, uint8_t adr);
void spd5_write_reg(uint8_t hid, uint8_t adr, uint8_t val);

#define spd5_set_legacy_page(hid, page)				\
	spd5_write_reg_mask(hid,				\
		SPD5_I2C_LEGACY_MODE_ADDR_POINTER(page),	\
		SPD5_I2C_LEGACY_MODE_ADDR_POINTER(		\
			SPD5_I2C_LEGACY_MODE_ADDR_POINTER_MASK),\
		SPD5_i2c_lcfg)

int spd_hw_read_bytes(uint8_t lsa, uint16_t offset,
		      uint8_t *buffer, int len);

#endif /* __SPD5118_H_INCLUDE__ */
