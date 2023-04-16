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
#define IC_CON(n)				DW_I2C_REG(n, 0x00)
#define IC_TAR(n)				DW_I2C_REG(n, 0x04)
#define IC_SAR(n)				DW_I2C_REG(n, 0x08)
#define IC_HS_MADDR(n)				DW_I2C_REG(n, 0x0C)
#define IC_DATA_CMD(n)				DW_I2C_REG(n, 0x10)
#define IC_SS_SCL_HCNT(n)			DW_I2C_REG(n, 0x14)
#define IC_UFM_SCL_HCNT(n)			DW_I2C_REG(n, 0x14)
#define IC_SS_SCL_LCNT(n)			DW_I2C_REG(n, 0x18)
#define IC_UFM_SCL_LCNT(n)			DW_I2C_REG(n, 0x18)
#define IC_FS_SCL_HCNT(n)			DW_I2C_REG(n, 0x1c)
#define IC_UFM_TBUF_CNT(n)			DW_I2C_REG(n, 0x1c)
#define IC_FS_SCL_LCNT(n)			DW_I2C_REG(n, 0x20)
#define IC_HS_SCL_HCNT(n)			DW_I2C_REG(n, 0x24)
#define IC_HS_SCL_LCNT(n)			DW_I2C_REG(n, 0x28)
#define IC_INTR_STAT(n)				DW_I2C_REG(n, 0x2c)
#define IC_INTR_MASK(n)				DW_I2C_REG(n, 0x30)
#define IC_RAW_INTR_STAT(n)			DW_I2C_REG(n, 0x34)
#define IC_RX_TL(n)				DW_I2C_REG(n, 0x38)
#define IC_TX_TL(n)				DW_I2C_REG(n, 0x3c)
#define IC_CLR_INTR(n)				DW_I2C_REG(n, 0x40)
#define IC_CLR_RX_UNDER(n)			DW_I2C_REG(n, 0x44)
#define IC_CLR_RX_OVER(n)			DW_I2C_REG(n, 0x48)
#define IC_CLR_TX_OVER(n)			DW_I2C_REG(n, 0x4c)
#define IC_CLR_RD_REQ(n)			DW_I2C_REG(n, 0x50)
#define IC_CLR_TX_ABRT(n)			DW_I2C_REG(n, 0x54)
#define IC_CLR_RX_DONE(n)			DW_I2C_REG(n, 0x58)
#define IC_CLR_ACTIVITY(n)			DW_I2C_REG(n, 0x5c)
#define IC_CLR_STOP_DET(n)			DW_I2C_REG(n, 0x60)
#define IC_CLR_START_DET(n)			DW_I2C_REG(n, 0x64)
#define IC_CLR_GEN_CALL(n)			DW_I2C_REG(n, 0x68)
#define IC_ENABLE(n)				DW_I2C_REG(n, 0x6c)
#define IC_STATUS(n)				DW_I2C_REG(n, 0x70)
#define IC_TXFLR(n)				DW_I2C_REG(n, 0x74)
#define IC_RXFLR(n)				DW_I2C_REG(n, 0x78)
#define IC_SDA_HOLD(n)				DW_I2C_REG(n, 0x7c)
#define IC_TX_ABRT_SOURCE(n)			DW_I2C_REG(n, 0x80)
#define IC_SLV_DATA_NACK_ONLY(n)		DW_I2C_REG(n, 0x84)
#define IC_DMA_CR(n)				DW_I2C_REG(n, 0x88)
#define IC_DMA_RDLR(n)				DW_I2C_REG(n, 0x90)
#define IC_SDA_SETUP(n)				DW_I2C_REG(n, 0x94)
#define IC_ACK_GENERAL_CALL(n)			DW_I2C_REG(n, 0x98)
#define IC_ENABLE_STATUS(n)			DW_I2C_REG(n, 0x9c)
#define IC_FS_SPKLEN(n)				DW_I2C_REG(n, 0xa0)
#define IC_UFM_SPKLEN(n)			DW_I2C_REG(n, 0xa0)
#define IC_HS_SPKLEN(n)				DW_I2C_REG(n, 0xa4)
#define IC_CLR_RESTART_DET(n)			DW_I2C_REG(n, 0xa8)
#define IC_SCL_STUCK_AT_LOW_TIMEOUT(n)		DW_I2C_REG(n, 0xac)
#define IC_SDA_STUCK_AT_LOW_TIMEOUT(n)		DW_I2C_REG(n, 0xb0)
#define IC_CLR_SCL_STUCK_DET(n)			DW_I2C_REG(n, 0xb4)
#define IC_DEVICE_ID(n)				DW_I2C_REG(n, 0xb8)
#define IC_SMBUS_CLK_LOW_SEXT(n)		DW_I2C_REG(n, 0xbc)
#define IC_SMBUS_CLK_LOW_MEXT(n)		DW_I2C_REG(n, 0xc0)
#define IC_SMBUS_THIGH_MAX_IDLE_COUNT(n)	DW_I2C_REG(n, 0xc4)
#define IC_SMBUS_INTR_STAT(n)			DW_I2C_REG(n, 0xc8)
#define IC_SMBUS_INTR_MASK(n)			DW_I2C_REG(n, 0xcc)
#define IC_SMBUS_RAW_INTR_STAT(n)		DW_I2C_REG(n, 0xd0)
#define IC_CLR_SMBUS_INTR(n)			DW_I2C_REG(n, 0xd4)
#define IC_OPTIONAL_SAR(n)			DW_I2C_REG(n, 0xd8)
#define IC_SMBUS_UDID_LSB(n)			DW_I2C_REG(n, 0xdc)
#define IC_SMBUS_UDID_WORD0(n)			DW_I2C_REG(n, 0xdc)
#define IC_SMBUS_UDID_WORD1(n)			DW_I2C_REG(n, 0xe0)
#define IC_SMBUS_UDID_WORD2(n)			DW_I2C_REG(n, 0xe4)
#define IC_SMBUS_UDID_WORD3(n)			DW_I2C_REG(n, 0xe8)
#define REG_TIMEOUT_RST				DW_I2C_REG(n, 0xf0)
#define IC_COMP_PARAM_1(n)			DW_I2C_REG(n, 0xf4)
#define IC_COMP_VERSION(n)			DW_I2C_REG(n, 0xf8)
#define IC_COMP_TYPE(n)				DW_I2C_REG(n, 0xfc)

/* 5.1.1 IC_CON */
#define IC_CON_SMBUS_PERSISTENT_SLV_ADDR_EN	_BV(19)
#define IC_CON_SMBUS_ARP_EN			_BV(18)
#define IC_CON_SMBUS_SLAVE_QUICK_EN		_BV(17)
#define IC_CON_OPTIONAL_SAR_CTRL		_BV(16)
#define IC_CON_BUS_CLEAR_FEATURE_CTRL		_BV(11)
#define IC_CON_STOP_DET_IF_MASTER_ACTIVE	_BV(10)
#define IC_CON_RX_FIFO_FULL_HLD_CTRL		_BV(9)
#define IC_CON_TX_EMPTY_CTRL			_BV(8)
#define IC_CON_STOP_DET_IFADDRESSED		_BV(7)
#define IC_CON_SLAVE_DISABLE			_BV(6)
#define IC_CON_RESTART_EN			_BV(5)
#define IC_CON_10BITADDR_MASTER			_BV(4)
#define IC_CON_10BITADDR_SLAVE			_BV(3)
#define IC_CON_SPEED_OFFSET			1
#define IC_CON_SPEED_MASK			REG_2BIT_MASK
#define IC_CON_SPEED(value)			_SET_FV(IC_CON_SPEED, value)
#define IC_CON_SPEED_STD			0x1
#define IC_CON_SPEED_FAST			0x2
#define IC_CON_SPEED_HIGH			0x3
#define IC_CON_SPEED_MAX			IC_CON_SPEED_HIGH
#define IC_CON_MASTER_MODE			_BV(0)

/* 5.1.2 IC_TAR */
#define IC_TAR_SMBUS_QUICK_CMD			_BV(16)
#define IC_TAR_DEVICE_ID			_BV(13)
#define IC_TAR_10BITADDR_MASTER			_BV(12)
#define IC_TAR_SPECIAL				_BV(11)
#define IC_TAR_GC_OR_START_OFFSET		10
#define IC_TAR_GC_OR_START_MASK			REG_1BIT_MASK
#define IC_TAR_GC_OR_START(value)		_SET_FV(IC_TAR_GC_OR_START, value)
#define IC_TAR_GENERAL_CALL			0x0
#define IC_TAR_START_BYTE			0x1
#define IC_TAR_TAR_OFFSET			0
#define IC_TAR_TAR_MASK				REG_10BIT_MASK
#define IC_TAR_TAR(value)			_SET_FV(IC_TAR_TAR, value)

/* 5.1.3 IC_SAR */
#define IC_SAR_SAR_OFFSET			0
#define IC_SAR_SAR_MASK				REG_10BIT_MASK
#define IC_SAR_SAR(value)			_SET_FV(IC_SAR_SAR, value)

/* 5.1.5 IC_DATA_CMD */
#define IC_DATA_CMD_DAT_OFFSET			0
#define IC_DATA_CMD_DAT_MASK			REG_10BIT_MASK
#define IC_DATA_CMD_DAT(dat)			_SET_FV(IC_DATA_CMD_DAT, dat)
#define IC_DATA_CMD_CMD				_BV(8)
#define IC_DATA_CMD_STOP			_BV(9)

/* 5.1.15 IC_INTR_STAT
 * 5.1.16 IC_INTR_MASK
 * 5.1.17 IC_RAW_INTR_STAT
 */
#ifdef CONFIG_DW_I2C_BUS_CLEAR
#define IC_INTR_SCL_STUCK_AT_LOW		_BV(14)
#else
#define IC_INTR_SCL_STUCK_AT_LOW		0
#endif
#define IC_INTR_MASTER_ON_HOLD			_BV(13)
#ifdef CONFIG_DW_I2C_SLV_RESTART_DET
#define IC_INTR_RESTART_DET			_BV(12)
#else
#define IC_INTR_RESTART_DET			0
#endif
#define IC_INTR_GEN_CALL			_BV(11)
#define IC_INTR_START_DET			_BV(10)
#define IC_INTR_STOP_DET			_BV(9)
#define IC_INTR_ACTIVITY			_BV(8)
#define IC_INTR_RX_DONE				_BV(7)
#define IC_INTR_TX_ABRT				_BV(6)
#define IC_INTR_RD_REQ				_BV(5)
#define IC_INTR_TX_EMPTY			_BV(4)
#define IC_INTR_TX_OVER				_BV(3)
#define IC_INTR_RX_FULL				_BV(2)
#define IC_INTR_RX_OVER 			_BV(1)
#define IC_INTR_RX_UNDER			_BV(0)
#define IC_INTR_ALL				\
	(IC_INTR_SCL_STUCK_AT_LOW |		\
	 IC_INTR_MASTER_ON_HOLD |		\
	 IC_INTR_RESTART_DET |			\
	 IC_INTR_GEN_CALL |			\
	 IC_INTR_START_DET | IC_INTR_STOP_DET |	\
	 IC_INTR_ACTIVITY |			\
	 IC_INTR_RX_DONE | IC_INTR_TX_ABRT |	\
	 IC_INTR_RD_REQ |			\
	 IC_INTR_TX_EMPTY | IC_INTR_TX_OVER |	\
	 IC_INTR_RX_FULL | IC_INTR_RX_OVER | IC_INTR_RX_UNDER)

/* 5.1.18 IC_RX_TL */
#define IC_RX_TL_TL_OFFSET			0
#define IC_RX_TL_TL_MASK			REG_8BIT_MASK
#define IC_RX_TL_TL(value)			_SET_FV(IC_RX_TL_TL, value)

/* 5.1.19 IC_TX_TL */
#define IC_TX_TL_TL_OFFSET			0
#define IC_TX_TL_TL_MASK			REG_8BIT_MASK
#define IC_TX_TL_TL(value)			_SET_FV(IC_TX_TL_TL, value)

/* 5.1.31 IC_ENABLE */
#define IC_ENABLE_SMBUS_ALTER_EN		_BV(18)
#define IC_ENABLE_SMBUS_SUSPEND_EN		_BV(17)
#define IC_ENABLE_SMBUS_CLK_RESET		_BV(16)
#define IC_ENABLE_SDA_STUCK_RECOVERY_ENABLE	_BV(3)
#define IC_ENABLE_TX_CMD_BLOCK			_BV(2)
#define IC_ENABLE_ABORT				_BV(1)
#define IC_ENABLE_ENABLE			_BV(0)

/* 5.1.32 IC_STATUS */
#define IC_STATUS_SMBUS_ALTER_STATUS		_BV(20)
#define IC_STATUS_SMBUS_SUSPEND_STATUS		_BV(19)
#define IC_STATUS_SMBUS_SLAVE_ADDR_RESOLVED	_BV(18)
#define IC_STATUS_SMBUS_SLAVE_ADDR_VALID	_BV(17)
#define IC_STATUS_SMBUS_QUICK_CMD_BIT		_BV(16)
#define IC_STATUS_SDA_STUCK_NOT_RECOVERED	_BV(11)
#define IC_STATUS_SLV_HOLD_RX_FIFO_FULL		_BV(10)
#define IC_STATUS_SLV_HOLD_TX_FIFO_EMPTY	_BV(9)
#define IC_STATUS_MST_HOLD_RX_FIFO_FULL		_BV(8)
#define IC_STATUS_MST_HOLD_TX_FIFO_EMPTY	_BV(7)
#define IC_STATUS_SLV_ACTIVITY			_BV(6)
#define IC_STATUS_MST_ACTIVITY			_BV(5)
#define IC_STATUS_RFF				_BV(4)
#define IC_STATUS_RFNE				_BV(3)
#define IC_STATUS_TFE				_BV(2)
#define IC_STATUS_TFNF				_BV(1)
#define IC_STATUS_ACTIVITY			_BV(0)

/* 5.1.36 IC_TX_ABRT_SOURCE */
#define IC_TX_ABRT_SOURCE_TX_FLUSH_CNT_OFFSET		23
#define IC_TX_ABRT_SOURCE_TX_FLUSH_CNT_MASK		REG_9BIT_MASK
#define IC_TX_ABRT_SOURCE_TX_FLUSH_CNT(value)		_GET_FV(IC_TX_ABRT_SOURCE_TX_FLUSH_CNT, value)
#define IC_TX_ABRT_SOURCE_ABRT_DEVICE_WRITE		_BV(20)
#define IC_TX_ABRT_SOURCE_ABRT_DEVICE_SLVADDR_NOACK	_BV(19)
#define IC_TX_ABRT_SOURCE_ABRT_DEVICE_NOACK		_BV(18)
#define IC_TX_ABRT_SOURCE_ABRT_SDA_STUCK_AT_LOW		_BV(17)
#define IC_TX_ABRT_SOURCE_ABRT_USER_ABRT		_BV(16)
#define IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX		_BV(15)
#define IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST		_BV(14)
#define IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO		_BV(13)
#define IC_TX_ABRT_SOURCE_ARB_LOST			_BV(12)
#define IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS		_BV(11)
#define IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT		_BV(10)
#define IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT		_BV(9)
#define IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT		_BV(8)
#define IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET		_BV(7)
#define IC_TX_ABRT_SOURCE_ABRT_HAS_ACKDET		_BV(6)
#define IC_TX_ABRT_SOURCE_ABRT_GCALL_READ		_BV(5)
#define IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK		_BV(4)
#define IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK		_BV(3)
#define IC_TX_ABRT_SOURCE_ABRT_10B_ADDR2_NOACK		_BV(2)
#define IC_TX_ABRT_SOURCE_ABRT_10B_ADDR1_NOACK		_BV(1)
#define IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK		_BV(0)

#define IC_TX_ABRT_SOURCE_NOACK				\
	(IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK |		\
	 IC_TX_ABRT_SOURCE_ABRT_10B_ADDR1_NOACK |	\
	 IC_TX_ABRT_SOURCE_ABRT_10B_ADDR2_NOACK |	\
	 IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK |		\
	 IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK |		\
	 IC_TX_ABRT_SOURCE_ABRT_GCALL_READ |		\
	 IC_TX_ABRT_SOURCE_ABRT_DEVICE_NOACK |		\
	 IC_TX_ABRT_SOURCE_ABRT_DEVICE_SLVADDR_NOACK |	\
	 IC_TX_ABRT_SOURCE_ABRT_DEVICE_WRITE)

/* Starting speed for each Mode */
#define I2C_FAST_SPEED			400000
#define I2C_FAST_PLUS_SPEED		1000000
#define I2C_HIGH_SPEED			3400000
#define I2C_MAX_SPEED			I2C_HIGH_SPEED

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

enum dw_i2c_driver_state {
	DW_I2C_DRIVER_INIT = 0,
	DW_I2C_DRIVER_START,
#ifdef CONFIG_I2C_DEVICE_ID
	DW_I2C_DRIVER_DEVID_START,
	DW_I2C_DRIVER_DEVID_TAR,
#endif
	DW_I2C_DRIVER_ADDRESS,
	DW_I2C_DRIVER_TX,
	DW_I2C_DRIVER_DATA,
	DW_I2C_DRIVER_STOP,
	DW_I2C_DRIVER_INVALID
};

struct dw_i2c_ctx {
	uint8_t addr_mode;
	uint8_t last_tx_byte;
	int state;
};

#ifdef CONFIG_ARCH_IS_DW_I2C_TX_BUFFER_DEPTH_8
#define DW_I2C_TX_FIFO_SIZE		8
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_TX_BUFFER_DEPTH_16
#define DW_I2C_TX_FIFO_SIZE		16
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_TX_BUFFER_DEPTH_32
#define DW_I2C_TX_FIFO_SIZE		32
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_TX_BUFFER_DEPTH_64
#define DW_I2C_TX_FIFO_SIZE		64
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_TX_BUFFER_DEPTH_128
#define DW_I2C_TX_FIFO_SIZE		128
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_TX_BUFFER_DEPTH_256
#define DW_I2C_TX_FIFO_SIZE		256
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_RX_BUFFER_DEPTH_8
#define DW_I2C_RX_FIFO_SIZE		8
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_RX_BUFFER_DEPTH_16
#define DW_I2C_RX_FIFO_SIZE		16
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_RX_BUFFER_DEPTH_32
#define DW_I2C_RX_FIFO_SIZE		32
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_RX_BUFFER_DEPTH_64
#define DW_I2C_RX_FIFO_SIZE		64
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_RX_BUFFER_DEPTH_128
#define DW_I2C_RX_FIFO_SIZE		128
#endif
#ifdef CONFIG_ARCH_IS_DW_I2C_RX_BUFFER_DEPTH_256
#define DW_I2C_RX_FIFO_SIZE		256
#endif

#define dw_i2c_ctrl_disable()		\
	__raw_clearl(IC_ENABLE_ENABLE, IC_ENABLE(dw_i2cd))
#define dw_i2c_ctrl_enable()		\
	__raw_setl(IC_ENABLE_ENABLE, IC_ENABLE(dw_i2cd))

#if NR_DW_I2CS > 1
void dw_i2c_master_select(i2c_t i2c);
#endif
void dw_i2c_master_init(void);
void dw_i2c_set_address(i2c_addr_t addr, boolean call);
void dw_i2c_set_frequency(uint16_t khz);
void dw_i2c_start_condition(bool sr);
void dw_i2c_stop_condition(void);
void dw_i2c_write_byte(uint8_t byte);
uint8_t dw_i2c_read_byte(void);
void dw_i2c_transfer_reset(void);
void dw_i2c_irq_init(void);

#endif /* __DW_I2C_H_INCLUDE__ */
