/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)dw_i2c.h: Synopsys DesignWare I2C interface
 * $Id: dw_i2c.h,v 1.1 2019-12-23 10:58:00 zhenglv Exp $
 */

#ifndef __DW_I2C_H_INCLUDE__
#define __DW_I2C_H_INCLUDE__

#ifndef DW_I2C_BASE
#define DW_I2C_BASE(n)		(DW_I2C##n##_BASE)
#endif
#ifndef DW_I2C_REG
#define DW_I2C_REG(n, offset)	(DW_I2C_BASE(n) + (offset))
#endif
#ifndef NR_DW_I2CS
#define NR_DW_I2CS		CONFIG_I2C_MAX_MASTERS
#endif

/* Register offset */
#define IC_CON				0x00
#define IC_TAR				0x04
#define IC_SAR				0x08
#define IC_HS_MADDR			0x0c
#define IC_DATA_CMD			0x10
#define IC_SS_SCL_HCNT			0x14
#define IC_UFM_SCL_HCNT			0x14
#define IC_SS_SCL_LCNT			0x18
#define IC_UFM_SCL_LCNT			0x18
#define IC_FS_SCL_HCNT			0x1c
#define IC_UFM_TBUF_CNT			0x1c
#define IC_FS_SCL_LCNT			0x20
#define IC_HS_SCL_HCNT			0x24
#define IC_HS_SCL_LCNT			0x28
#define IC_INTR_STAT			0x2c
#define IC_INTR_MASK			0x30
#define IC_RAW_INTR_STAT		0x34
#define IC_RX_TL			0x38
#define IC_TX_TL			0x3c
#define IC_CLR_INTR			0x40
#define IC_CLR_RX_UNDER			0x44
#define IC_CLR_RX_OVER			0x48
#define IC_CLR_TX_OVER			0x4c
#define IC_CLR_RD_REQ			0x50
#define IC_CLR_TX_ABRT			0x54
#define IC_CLR_RX_DONE			0x58
#define IC_CLR_ACTIVITY			0x5c
#define IC_CLR_STOP_DET			0x60
#define IC_CLR_START_DET		0x64
#define IC_CLR_GEN_CALL			0x68
#define IC_ENABLE			0x6c
#define IC_STATUS			0x70
#define IC_TXFLR			0x74
#define IC_RXFLR			0x78
#define IC_SDA_HOLD			0x7c
#define IC_TX_ABRT_SOURCE		0x80
#define IC_SLV_DATA_NACK_ONLY		0x84
#define IC_DMA_CR			0x88
#define IC_DMA_RDLR			0x90
#define IC_SDA_SETUP			0x94
#define IC_ACK_GENERAL_CALL		0x98
#define IC_ENABLE_STATUS		0x9c
#define IC_FS_SPKLEN			0xa0
#define IC_UFM_SPKLEN			0xa0
#define IC_HS_SPKLEN			0xa4
#define IC_CLR_RESTART_DET		0xa8
#define IC_SCL_STUCK_AT_LOW_TIMEOUT	0xac
#define IC_SDA_STUCK_AT_LOW_TIMEOUT	0xb0
#define IC_CLR_SCL_STUCK_DET		0xb4
#define IC_DEVICE_ID			0xb8
#define IC_SMBUS_CLK_LOW_SEXT		0xbc
#define IC_SMBUS_CLK_LOW_MEXT		0xc0
#define IC_SMBUS_THIGH_MAX_IDLE_COUNT	0xc4
#define IC_SMBUS_INTR_STAT		0xc8
#define IC_SMBUS_INTR_MASK		0xcc
#define IC_SMBUS_RAW_INTR_STAT		0xd0
#define IC_CLR_SMBUS_INTR		0xd4
#define IC_OPTIONAL_SAR			0xd8
#define IC_SMBUS_UDID_LSB		0xdc
#define IC_SMBUS_UDID_WORD0		0xdc
#define IC_SMBUS_UDID_WORD1		0xe0
#define IC_SMBUS_UDID_WORD2		0xe4
#define IC_SMBUS_UDID_WORD3		0xe8
#define REG_TIMEOUT_RST			0xf0
#define IC_COMP_PARAM_1			0xf4
#define IC_COMP_VERSION			0xf8
#define IC_COMP_TYPE			0xfc

/* High and low times in different speed modes (in ns) */
#define MIN_SS_SCL_HIGHTIME		4000
#define MIN_SS_SCL_LOWTIME		4700
#define MIN_FS_SCL_HIGHTIME		600
#define MIN_FS_SCL_LOWTIME		1300
#define MIN_HS_SCL_HIGHTIME		60
#define MIN_HS_SCL_LOWTIME		160

/* Worst case timeout for 1 byte is kept as 2ms */
#define I2C_BYTE_TO			(CONFIG_SYS_HZ/500)
#define I2C_STOPDET_TO			(CONFIG_SYS_HZ/500)
#define I2C_BYTE_TO_BB			(I2C_BYTE_TO * 16)

/* I2C control register definitions */
#define IC_CON_SD			0x0040
#define IC_CON_RE			0x0020
#define IC_CON_10BITADDRMASTER		0x0010
#define IC_CON_10BITADDR_SLAVE		0x0008
#define IC_CON_SPD_MSK			0x0006
#define IC_CON_SPD_SS			0x0002
#define IC_CON_SPD_FS			0x0004
#define IC_CON_SPD_HS			0x0006
#define IC_CON_MM			0x0001

/* I2C target address register definitions */
#define TAR_ADDR			0x0050
#define TAR_DEVID			0x1400

/* I2C slave address register definitions */
#define IC_SLAVE_ADDR			0x0002

/* I2C data buffer and command register definitions */
#define IC_CMD				0x0100
#define IC_STOP				0x0200

/* I2C interrupt status register definitions */
#define IC_GEN_CALL			0x0800
#define IC_START_DET			0x0400
#define IC_STOP_DET			0x0200
#define IC_ACTIVITY			0x0100
#define IC_RX_DONE			0x0080
#define IC_TX_ABRT			0x0040
#define IC_RD_REQ			0x0020
#define IC_TX_EMPTY			0x0010
#define IC_TX_OVER			0x0008
#define IC_RX_FULL			0x0004
#define IC_RX_OVER 			0x0002
#define IC_RX_UNDER			0x0001
#define IC_INTR_ALL			0x7FFF

/* FIFO threshold register definitions */
#define IC_TL0				0x00
#define IC_TL1				0x01
#define IC_TL2				0x02
#define IC_TL3				0x03
#define IC_TL4				0x04
#define IC_TL5				0x05
#define IC_TL6				0x06
#define IC_TL7				0x07

/* I2C enable register definitions */
#define IC_ENABLE_0B			0x0001
#define IC_ENABLE_ABRT			0x0002

/* I2C status register  definitions */
#define IC_STATUS_SA			0x0040
#define IC_STATUS_MA			0x0020
#define IC_STATUS_RFF			0x0010
#define IC_STATUS_RFNE			0x0008
#define IC_STATUS_TFE			0x0004
#define IC_STATUS_TFNF			0x0002
#define IC_STATUS_ACT			0x0001

/* Speed Selection */
#define IC_SPEED_MODE_STANDARD		1
#define IC_SPEED_MODE_FAST		2 /* For Fast and Fast Plus */
#define IC_SPEED_MODE_HIGH  		3
#define IC_SPEED_MODE_MAX		IC_SPEED_MODE_HIGH

/* Starting speed for each Mode */
#define I2C_FAST_SPEED			400000
#define I2C_FAST_PLUS_SPEED		1000000
#define I2C_HIGH_SPEED			3400000
#define I2C_MAX_SPEED			I2C_HIGH_SPEED

enum dw_i2c_driver_state {
	DW_I2C_DRIVER_INIT = 0,
	DW_I2C_DRIVER_START,
	DW_I2C_DRIVER_ADDRESS,
	DW_I2C_DRIVER_TRANS,
	DW_I2C_DRIVER_STOP,
#ifdef CONFIG_I2C_DEVICE_ID
	DW_I2C_DRIVER_DEVID_START,
	DW_I2C_DRIVER_DEVID_TAR,
#endif
	DW_I2C_DRIVER_INVALID
};

struct dw_i2c_ctx {
	caddr_t base;
	uint8_t addr_mode;
	int state;
};

#if NR_DW_I2CS > 1
void dw_i2c_master_select(i2c_t i2c);
#endif
void dw_i2c_master_init(void);
void dw_i2c_set_address(i2c_addr_t addr, boolean call);
void dw_i2c_set_frequency(uint16_t khz);
void dw_i2c_start_condition(void);
void dw_i2c_stop_condition(void);
void dw_i2c_write_byte(uint8_t byte);
uint8_t dw_i2c_read_byte(void);
void dw_i2c_transfer_reset(void);
void dw_i2c_irq_init(void);

/* Non-standard APIs, used for tests */
int dw_i2c_read_bytes(uint8_t dev, uint8_t *buffer,
		      int len, unsigned int stop);
int dw_i2c_write_bytes(uint8_t dev, uint8_t *buffer,
		       int len, unsigned int stop);
int dw_i2c_read_mem(uint8_t dev, unsigned int addr,
		    int alen, uint8_t *buffer, int len);
int dw_i2c_write_mem(uint8_t dev, unsigned int addr,
		     int alen, uint8_t *buffer, int len);
int dw_i2c_read_vip(uint8_t dev, uint8_t *buffer, int len);
int dw_i2c_write_vip(uint8_t dev, unsigned int addr,
		     uint8_t *buffer, int len);
void dw_i2c_init(void);

#endif /* __DW_I2C_H_INCLUDE__ */
