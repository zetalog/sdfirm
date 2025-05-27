/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2025
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
 * @(#)dw_mipi_i3c.h: Synopsys DesignWare MIPI I3C interface
 * $Id: dw_mipi_i3c.h,v 1.1 2025-04-07 10:58:00 zhenglv Exp $
 */

#ifndef __DW_MIPI_I3C_H_INCLUDE__
#define __DW_MIPI_I3C_H_INCLUDE__

#ifndef DW_MIPI_I3C_BASE
#define DW_MIPI_I3C_BASE(n)		(DW_MIPI_I3C##n##_BASE)
#endif
#ifndef DW_MIPI_I3C_REG
#define DW_MIPI_I3C_REG(n, offset)	(DW_MIPI_I3C_BASE(n) + (offset))
#endif
#ifndef NR_DW_I3CS
#define NR_DW_I3CS			CONFIG_I3C_MAX_MASTERS
#endif

#ifdef CONFIG_DW_MIPI_I3C_MAX_XFERS
#define DW_MIPI_I3C_MAX_XFERS		CONFIG_DW_MIPI_I3C_MAX_XFERS
#else
#define DW_MIPI_I3C_MAX_XFERS		2
#endif
#ifdef CONFIG_DW_MIPI_I3C_MAX_DEVS
#define DW_MIPI_I3C_MAX_DEVS		CONFIG_DW_MIPI_I3C_MAX_DEVS
#else
#define DW_MIPI_I3C_MAX_DEVS		32
#endif

#ifdef CONFIG_ARCH_IS_DW_MIPI_I3C_HCI
#include <target/i3c_hci.h>
#else /* CONFIG_ARCH_IS_DW_MIPI_I3C_HCI */
#ifndef CONFIG_DW_MIPI_I3C_SLAVE_LITE
#define DEVICE_CTRL(n)				DW_MIPI_I3C_REG(n, 0x000)
#define DEVICE_ADDR(n)				DW_MIPI_I3C_REG(n, 0x004)
#define COMMAND_QUEUE_PORT(n)			DW_MIPI_I3C_REG(n, 0x00C)
#define RESPONSE_QUEUE_PORT(n)			DW_MIPI_I3C_REG(n, 0x010)
#define RX_DATA_PORT(n)				DW_MIPI_I3C_REG(n, 0x014)
#define TX_DATA_PORT(n)				DW_MIPI_I3C_REG(n, 0x014)
#define QUEUE_THLD_CTRL(n)			DW_MIPI_I3C_REG(n, 0x01C)
#define DATA_BUFFER_THLD_CTRL(n)		DW_MIPI_I3C_REG(n, 0x020)
#define RESET_CTRL(n)				DW_MIPI_I3C_REG(n, 0x034)
#define SLV_EVENT_STATUS(n)			DW_MIPI_I3C_REG(n, 0x038)
#define INTR_STATUS(n)				DW_MIPI_I3C_REG(n, 0x03C)
#define INTR_STATUS_EN(n)			DW_MIPI_I3C_REG(n, 0x040)
#define INTR_SIGNAL_EN(n)			DW_MIPI_I3C_REG(n, 0x044)
#define INTR_FORCE(n)				DW_MIPI_I3C_REG(n, 0x048)
#define QUEUE_STATUS_LEVEL(n)			DW_MIPI_I3C_REG(n, 0x04C)
#define DATA_BUFFER_STATUS_LEVEL(n)		DW_MIPI_I3C_REG(n, 0x050)
#define DEVICE_ADDR_TABLE_POINTER(n)		DW_MIPI_I3C_REG(n, 0x05C)
#ifdef CONFIG_DW_MIPI_I3C_DAA
#define DEV_CHAR_TABLE_POINTER(n)		DW_MIPI_I3C_REG(n, 0x060)
#endif /* CONFIG_DW_MIPI_I3C_DAA */
#define VENDOR_SPECIFIC_TABLE_POINTER(n)	DW_MIPI_I3C_REG(n, 0x06C)
#endif /* !CONFIG_DW_MIPI_I3C_SLAVE_LITE */

#define HW_CAPBILITY(n)				DW_MIPI_I3C_REG(n, 0x008)

#ifdef CONFIG_DW_MIPI_I3C_MASTER
#define IBI_QUEUE_DATA(n)			DW_MIPI_I3C_REG(n, 0x018)
#define IBI_QUEUE_STATUS(n)			DW_MIPI_I3C_REG(n, 0x018)
#define IBI_QUEUE_CTRL(n)			DW_MIPI_I3C_REG(n, 0x024)
#ifndef CONFIG_DW_MIPI_I3C_IBI_DATA
#ifdef CONFIG_DW_MIPI_I3C_SECONDARY_MASTER
#define IBI_MR_REQ_REJECT(n)			DW_MIPI_I3C_REG(n, 0x02C)
#endif /* CONFIG_DW_MIPI_I3C_SECONDARY_MASTER */
#define IBI_SIR_REQ_REJECT(n)			DW_MIPI_I3C_REG(n, 0x030)
#endif /* CONFIG_DW_MIPI_I3C_IBI_DATA */
#endif /* CONFIG_DW_MIPI_I3C_MASTER */

#if defined(CONFIG_DW_MIPI_I3C_MASTER) || (!defined(CONFIG_DW_MIPI_I3C_LITE) && defined(CONFIG_DW_MIPI_I3C_HAS_EXTND_TIMING))
#define PRESENT_STATE(n)			DW_MIPI_I3C_REG(n, 0x054)
#endif /* CONFIG_DW_MIPI_I3C_MASTER || CONFIG_DW_MIPI_I3C_HAS_EXTND_TIMING */
#if !defined(CONFIG_DW_MIPI_I3C_LITE)
#define CCC_DEVICE_STATUS(n)			DW_MIPI_I3C_REG(n, 0x058)
#endif

#ifndef CONFIG_DW_MIPI_I3C_LITE
#ifdef CONFIG_DW_MIPI_I3C_SLV_UNIQUE_ID_PROG
#define SLV_MIPI_ID_VALUE(n)			DW_MIPI_I3C_REG(n, 0x070)
#endif /* CONFIG_DW_MIPI_I3C_SLV_UNIQUE_ID_PROG */
#define SLV_PID_VALUE(n)			DW_MIPI_I3C_REG(n, 0x074)
#define SLV_CHAR_CTRL(n)			DW_MIPI_I3C_REG(n, 0x078)
#define SLV_MAX_LEN(n)				DW_MIPI_I3C_REG(n, 0x07C)
#define MAX_READ_TURNAROUND(n)			DW_MIPI_I3C_REG(n, 0x080)
#define MAX_DATA_SPEED(n)			DW_MIPI_I3C_REG(n, 0x084)
#endif /* CONFIG_DW_MIPI_I3C_LITE */

#define SLV_INTR_REQ(n)				DW_MIPI_I3C_REG(n, 0x08C)
#define SLV_TSX_SYMBL_TIMING(n)			DW_MIPI_I3C_REG(n, 0x090)
#define DEVICE_CTRL_EXTENDED(n)			DW_MIPI_I3C_REG(n, 0x0B0)
#define SCL_I3C_OD_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0B4)
#define SCL_I3C_PP_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0B8)
#define SCL_I2C_FM_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0BC)
#define SCL_I2C_FMP_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0C0)
#define SCL_EXT_LCNT_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0C8)
#define SCL_EXT_TERMN_LCNT_TIMING(n)		DW_MIPI_I3C_REG(n, 0x0CC)
#define SDA_HOLD_SWITCH_DLY_TIMING(n)		DW_MIPI_I3C_REG(n, 0x0D0)
#define BUS_FREE_AVAIL_TIMING(n)		DW_MIPI_I3C_REG(n, 0x0D4)
#define BUS_IDLE_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0D8)
#define SCL_LOW_MST_EXT_TIMEOUT(n)		DW_MIPI_I3C_REG(n, 0x0DC)
#define I3C_VER_ID(n)				DW_MIPI_I3C_REG(n, 0x0E0)
#define I3C_VER_TYPE(n)				DW_MIPI_I3C_REG(n, 0x0E4)
#define QUEUE_SIZE_CAPABILITY(n)		DW_MIPI_I3C_REG(n, 0x0E8)

#define DEV_ADDR_TABLE_LOC(n, dat, i)		DW_MIPI_I3C_REG(n, (dat) + ((i) << 2))
#ifdef CONFIG_DW_MIPI_I3C_MASTER
#define DEV_CHAR_TABLE1_LOC1(n)			DW_MIPI_I3C_REG(n, 0x200)
#endif /* CONFIG_DW_MIPI_I3C_MASTER */
#ifdef CONFIG_DW_MIPI_I3C_SECONDARY_MASTER
#define SEC_DEV_CHAR_TABLE1(n)			DW_MIPI_I3C_REG(n, 0x200)
#endif /* CONFIG_DW_MIPI_I3C_SECONDARY_MASTER */
#define DEV_CHAR_TABLE1_LOC2(n)			DW_MIPI_I3C_REG(n, 0x204)
#define DEV_CHAR_TABLE1_LOC3(n)			DW_MIPI_I3C_REG(n, 0x208)
#define DEV_CHAR_TABLE1_LOC4(n)			DW_MIPI_I3C_REG(n, 0x20C)
#define DEV_ADDR_TABLE1_LOC1(n)			DW_MIPI_I3C_REG(n, 0x220)
#define DEV_ADDR_TABLE_LOC1(n)			DW_MIPI_I3C_REG(n, 0x220)
#endif /* CONFIG_ARCH_IS_DW_MIPI_I3C_HCI */

/* 5.2.1 DEVICE_CTRL */
#define DW_IBA_INCLUDE				_BV(0)
#define DW_I2C_SLAVE_PRESENT			_BV(7)
#define DW_HOT_JOIN_CTRL			_BV(8)
#define DW_IDLE_CNT_MULTIPLIER_OFFSET		24
#define DW_IDLE_CNT_MULTIPLIER_MASK		REG_2BIT_MASK
#define DW_IDLE_CNT_MULTIPLIER(value)		_SET_FV(DW_IDLE_CNT_MULTIPLIER, value)
#define DW_ADAPTIVE_I2C_I3C			_BV(27)
#define DW_DMA_ENABLE				_BV(28)
#define DW_ABORT				_BV(29)
#define DW_RESUME				_BV(30)
#define DW_ENABLE				_BV(31)
/* 5.2.2 DEVICE_ADDR */
#define DW_STATIC_ADDR_OFFSET			0
#define DW_STATIC_ADDR_MASK			REG_7BIT_MASK
#define DW_STATIC_ADDR(value)			_SET_FV(DW_STATIC_ADDR, value)
#define DW_STATIC_ADDR_VALID			_BV(15)
#define DW_DYNAMIC_ADDR_OFFSET			16
#define DW_DYNAMIC_ADDR_MASK			REG_6BIT_MASK
#define DW_DYNAMIC_ADDR(value)			_SET_FV(DW_DYNAMIC_ADDR, value)
#define DW_DYNAMIC_ADDR_VALID			_BV(31)
/* 5.2.3 HW_CAPABILITY */
#define DW_DEVICE_ROLE_CONFIG_OFFSET		0
#define DW_DEVICE_ROLE_CONFIG_MASK		REG_3BIT_MASK
#define DW_DEVICE_ROLE_CONFIG(value)		_GET_FV(DW_DEVICE_ROLE_CONFIG, value)
#define DW_HDR_DDR_EN				_BV(3)
#define DW_HDR_TS_EN				_BV(4)
#define DW_CLOCK_PERIOD_OFFSET			5
#define DW_CLOCK_PERIOD_MASK			REG_6BIT_MASK
#define DW_CLOCK_PERIOD(value)			_GET_FV(DW_CLOCK_PERIOD, value)
#define DW_HDR_TX_CLOCK_PERIOD_OFFSET		11
#define DW_HDR_TX_CLOCK_PERIOD_MASK		REG_6BIT_MASK
#define DW_HDR_TX_CLOCK_PERIOD(value)		_GET_FV(DW_HDR_TX_CLOCK_PERIOD, value)
#define DW_DMA_EN				_BV(17)
#define DW_SLV_HJ_CAP				_BV(18)
#define DW_SLV_IBI_CAP				_BV(19)
/* 5.2.4 COMMAND_QUEUE_PORT
 * 2.6.6.1 Command Data Structure
 */
#define DW_COMMAND_CMD_ATTR_OFFSET		0
#define DW_COMMAND_CMD_ATTR_MASK		REG_2BIT_MASK
#define DW_COMMAND_CMD_ATTR(value)		_SET_FV(DW_COMMAND_CMD_ATTR, value)
#define DW_COMMAND_TRANSFER_COMMAND		0x0
#define DW_COMMAND_TRANSFER_ARGUMENT		0x1
#define DW_COMMAND_SHORT_DATA_ARGUMENT		0x2
#define DW_COMMAND_ADDRESS_ASIGNMENT_COMMAND	0x3
/* Transfer Command Data Structure
 * Address Assignment Command Data Structure
 */
#define DW_COMMAND_TID_OFFSET			3
#define DW_COMMAND_TID_MASK			REG_4BIT_MASK
#define DW_COMMAND_TID(value)			_SET_FV(DW_COMMAND_TID, value)
#define DW_COMMAND_CMD_OFFSET			7
#define DW_COMMAND_CMD_MASK			REG_8BIT_MASK
#define DW_COMMAND_CMD(value)			_SET_FV(DW_COMMAND_CMD, value)
#define DW_COMMAND_DEV_INDEX_OFFSET		16
#define DW_COMMAND_DEV_INDEX_MASK		REG_5BIT_MASK
#define DW_COMMAND_DEV_INDEX(value)		_SET_FV(DW_COMMAND_DEV_INDEX, value)
#define DW_COMMAND_ROC				_BV(26)
#define DW_COMMAND_TOC				_BV(30)
/* Transfer Command Data Structure */
#define DW_COMMAND_CP				_BV(15)
#define DW_COMMAND_SPEED_OFFSET			21
#define DW_COMMAND_SPEED_MASK			REG_3BIT_MASK
#define DW_COMMAND_SPEED(value)			_SET_FV(DW_COMMAND_SPEED, value)
#define DW_COMMAND_I3C_SDR0			0
#define DW_COMMAND_I3C_SDR1			1
#define DW_COMMAND_I3C_SDR2			2
#define DW_COMMAND_I3C_SDR3			3
#define DW_COMMAND_I3C_SDR4			4
#define DW_COMMAND_I3C_HDR_TS			5
#define DW_COMMAND_I3C_HDR_DDR			6
#define DW_COMMAND_I3C_I2C_FAST			7
#define DW_COMMAND_I2C_FAST			0
#define DW_COMMAND_I2C_FAST_PLUS		1
#define DW_COMMAND_DBP				_BV(25)
#define DW_COMMAND_SDAP				_BV(27)
#define DW_COMMAND_RnW				_BV(28)
#define DW_COMMAND_PEC				_BV(31)
/* Address Assignment Command Data Structure */
#define DW_COMMAND_DEV_COUNT_OFFSET		21
#define DW_COMMAND_DEV_COUNT_MASK		REG_5BIT_MASK
#define DW_COMMAND_DEV_COUNT(value)		_SET_FV(DW_COMMAND_DEV_COUNT, value)
/* Transfer Argument Data Structure */
#define DW_COMMAND_DB_OFFSET			8
#define DW_COMMAND_DB_MASK			REG_8BIT_MASK
#define DW_COMMAND_DB(value)			_SET_FV(DW_COMMAND_DB, value)
#define DW_COMMAND_DATA_LENGTH_OFFSET		16
#define DW_COMMAND_DATA_LENGTH_MASK		REG_16BIT_MASK
#define DW_COMMAND_DATA_LENGTH(value)		_SET_FV(DW_COMMAND_DATA_LENGTH, value)
/* Shot Data Argument Data Structure */
#define DW_COMMAND_BYTE_STRB_OFFSET		3
#define DW_COMMAND_BYTE_STRB_MASK		REG_3BIT_MASK
#define DW_COMMAND_BYTE_STRB(value)		_SET_FV(DW_COMMAND_BYTE_STRB, value)
#define DW_COMMAND_DATA_BYTE_1_OFFSET		8
#define DW_COMMAND_DATA_BYTE_1_MASK		REG_8BIT_MASK
#define DW_COMMAND_DATA_BYTE_1(value)		_SET_FV(DW_COMMAND_DATA_BYTE_1, value)
#define DW_COMMAND_DATA_BYTE_2_OFFSET		16
#define DW_COMMAND_DATA_BYTE_2_MASK		REG_8BIT_MASK
#define DW_COMMAND_DATA_BYTE_2(value)		_SET_FV(DW_COMMAND_DATA_BYTE_2, value)
#define DW_COMMAND_DATA_BYTE_3_OFFSET		24
#define DW_COMMAND_DATA_BYTE_3_MASK		REG_8BIT_MASK
#define DW_COMMAND_DATA_BYTE_3(value)		_SET_FV(DW_COMMAND_DATA_BYTE_3, value)
/* 5.2.5 RESPONSE_QUEUE_PORT
 * 2.6.6.2 Response Data Structure
 */
#define DW_RESPONSE_DL_OFFSET			0
#define DW_RESPONSE_DL_MASK			REG_16BIT_MASK
#define DW_RESPONSE_DL(value)			_GET_FV(DW_RESPONSE_DL, value)
#define DW_RESPONSE_CCCT_OFFSET			16
#define DW_RESPONSE_CCCT_MASK			REG_8BIT_MASK
#define DW_RESPONSE_CCCT(value)			_GET_FV(DW_RESPONSE_CCCT, value)
#define DW_RESPONSE_TID_OFFSET			24
#define DW_RESPONSE_TID_MASK			REG_4BIT_MASK
#define DW_RESPONSE_TID(value)			_GET_FV(DW_RESPONSE_TID, value)
#define DW_RESPONSE_ERR_STS_OFFSET		28
#define DW_RESPONSE_ERR_STS_MASK		REG_4BIT_MASK
#define DW_RESPONSE_ERR_STS(value)		_GET_FV(DW_RESPONSE_ERR_STS, value)
#define DW_RESPONSE_NO_ERROR			0
#define DW_RESPONSE_CRC_ERROR			1
#define DW_RESPONSE_PARITY_ERROR		2
#define DW_RESPONSE_FRAME_ERROR			3
#define DW_RESPONSE_I3C_BRAODCAST_ADDRESS_NACK	4
#define DW_RESPONSE_ADDRESS_NACK		5
#define DW_RESPONSE_RX_OVERFLOW_TX_UNDERFLOW	6
#define DW_RESPONSE_TRANSFER_ABORTED		8
#define DW_RESPONSE_I2C_SLAVE_WRITE_DATA_NACK	9
#define DW_RESPONSE_PEC_ERROR			12
/* 5.2.9 IBI_QUEUE_STATUS
 * 2.6.6.3 IBI Status and Data Structure
 */
#define DW_IBI_STATUS_DL_OFFSET			0
#define DW_IBI_STATUS_DL_MASK			REG_8BIT_MASK
#define DW_IBI_STATUS_DL(value)			_GET_FV(DW_IBI_STATUS_DL, value)
#define DW_IBI_STATUS_ID_OFFSET			8
#define DW_IBI_STATUS_ID_MASK			REG_8BIT_MASK
#define DW_IBI_STATUS_ID(value)			_GET_FV(DW_IBI_STATUS_ID, value)
#define DW_IBI_STATUS_STS_OFFSET		28
#define DW_IBI_STATUS_STS_MASK			REG_4BIT_MASK
#define DW_IBI_STATUS_STS(value)		_GET_FV(DW_IBI_STATUS_STS, value)
#define DW_IBI_QUEUE_ADDR(x)			(DW_IBI_STATUS_ID(x) >> 1)
#define DW_IBI_QUEUE_RNW(x)			(DW_IBI_STATUS_ID(x) & _BV(0))
#define DW_IBI_TYPE_MR(x)			\
	((DW_IBI_QUEUE_ADDR(x) != I3C_ADDR_HOTJOIN) && !DW_IBI_QUEUE_RNW(x))
#define DW_IBI_TYPE_HJ(x)			\
	((DW_IBI_QUEUE_ADDR(x) == I3C_ADDR_HOTJOIN) && !DW_IBI_QUEUE_RNW(x))
#define DW_IBI_TYPE_SIRQ(x)			\
	((DW_IBI_QUEUE_ADDR(x) != I3C_ADDR_HOTJOIN) && DW_IBI_QUEUE_RNW(x))
/* 5.2.10 QUEUE_THLD_CTRL */
#define DW_CMD_EMPTY_BUF_THLD_OFFSET		0
#define DW_CMD_EMPTY_BUF_THLD_MASK		REG_8BIT_MASK
#define DW_CMD_EMPTY_BUF_THLD(value)		_SET_FV(DW_CMD_EMPTY_BUF_THLD, value)
#define DW_RESP_BUF_THLD_OFFSET			8
#define DW_RESP_BUF_THLD_MASK			REG_8BIT_MASK
#define DW_RESP_BUF_THLD(value)			_SET_FV(DW_RESP_BUF_THLD, value)
#define DW_IBI_DATA_THLD_OFFSET			16
#define DW_IBI_DATA_THLD_MASK			REG_8BIT_MASK
#define DW_IBI_DATA_THLD(value)			_SET_FV(DW_IBI_DATA_THLD, value)
#define DW_IBI_STATUS_THLD_OFFSET		24
#define DW_IBI_STATUS_THLD_MASK			REG_8BIT_MASK
#define DW_IBI_STATUS_THLD(value)		_SET_FV(DW_IBI_STATUS_THLD, value)
/* 5.2.11 DATA_BUFFER_THLD_CTRL */
#define DW_TX_EMPTY_BUF_THLD_OFFSET		0
#define DW_TX_EMPTY_BUF_THLD_MASK		REG_3BIT_MASK
#define DW_TX_EMPTY_BUF_THLD(value)		_SET_FV(DW_TX_EMPTY_BUF_THLD, value)
#define DW_RX_BUF_THLD_OFFSET			8
#define DW_RX_BUF_THLD_MASK			REG_3BIT_MASK
#define DW_RX_BUF_THLD(value)			_SET_FV(DW_RX_BUF_THLD, value)
#define DW_TX_START_THLD_OFFSET			16
#define DW_TX_START_THLD_MASK			REG_3BIT_MASK
#define DW_TX_START_THLD(value)			_SET_FV(DW_TX_START_THLD, value)
#define DW_RX_START_THLD_OFFSET			24
#define DW_RX_START_THLD_MASK			REG_3BIT_MASK
#define DW_RX_START_THLD(value)			_SET_FV(DW_TX_START_THLD, value)
/* 5.2.12 IBI_QUEUE_CTRL */
#define DW_NOTIFY_HJ_REJECTED			_BV(0)
#define DW_NOTIFY_MR_REJECTED			_BV(1)
#define DW_NOTIFY_SIR_REJECTED			_BV(3)
/* 5.2.13 IBI_MR_REQ_REJECT
 * 5.2.14 IBI_SIR_REQ_REJECT
 */
#define DW_IBI_REQ_REJECT_ALL			GENMASK(31, 0)
/* 5.2.15 RESET_CTRL */
#define DW_SOFT_RST				_BV(0)
#define DW_CMD_QUEUE_RST			_BV(1)
#define DW_RESP_QUEUE_RST			_BV(2)
#define DW_TX_FIFO_RST				_BV(3)
#define DW_RX_FIFO_RST				_BV(4)
#define DW_IBI_QUEUE_RST			_BV(5)
#define DW_BUS_RESET_TYPE_OFFSET		29
#define DW_BUS_RESET_TYPE_MASK			REG_2BIT_MASK
#define DW_BUS_RESET_TYPE(value)		_SET_FV(DW_BUS_RESET_TYPE, value)
#define DW_BUS_RESET				_BV(31)
/* 5.2.16 SLV_EVENT_STATUS */
#define DW_SIR_EN				_BV(0)
#define DW_MR_EN				_BV(1)
#define DW_HJ_EN				_BV(3)
#define DW_ACTIVITY_STATE_OFFSET		4
#define DW_ACTIVITY_STATE_MASK			REG_2BIT_MASK
#define DW_ACTIVITY_STATE(value)		_GET_FV(DW_ACTIVITY_STATE, value)
#define DW_MRL_UPDATED				_BV(6)
#define DW_MWL_UPDATED				_BV(7)
/* 5.2.17 INTR_STATUS
 * 5.2.18 INTR_STATUS_EN
 * 5.2.19 INTR_SIGNAL_EN
 * 5.2.20 INTR_FORCE
 */
#define INTR_BUS_RESET_DONE			_BV(15)
#define INTR_BUSOWNER_UPDATED			_BV(13)
#define INTR_IBI_UPDATED			_BV(12)
#define INTR_READ_REQ_RECV			_BV(11)
#define INTR_DEFSLV				_BV(10)
#define INTR_TRANSFER_ERR			_BV(9)
#define INTR_DYN_ADDR_ASSIGN			_BV(8)
#define INTR_CCC_UPDATED			_BV(6)
#define INTR_TRANSFER_ABORT			_BV(5)
#define INTR_RESP_READY				_BV(4)
#define INTR_CMD_QUEUE_READY			_BV(3)
#define INTR_IBI_THLD				_BV(2)
#define INTR_RX_THLD				_BV(1)
#define INTR_TX_THLD				_BV(0)
#ifdef CONFIG_DW_MIPI_I3C_SLAVE_LITE
#define INTR_ALWAYS				\
	(INTR_TRANSFER_ERR |			\
	 INTR_RESP_READY |			\
	 INTR_CMD_QUEUE_READY |			\
	 INTR_RX_THLD | INTR_TX_THLD)
#else
#define INTR_ALWAYS				\
	(INTR_TRANSFER_ERR |			\
	 INTR_TRANSFER_ABORT |			\
	 INTR_RESP_READY |			\
	 INTR_CMD_QUEUE_READY |			\
	 INTR_IBI_THLD |			\
	 INTR_RX_THLD | INTR_TX_THLD)
#endif
#ifdef CONFIG_DW_MIPI_I3C_MASTER
#define INTR_MST				\
	(INTR_BUS_RESET_DONE |			\
	 INTR_ALWAYS)
#else
#define INTR_MST				0
#endif
#ifdef CONFIG_DW_MIPI_I3C_SLAVE
#ifdef CONFIG_DW_MIPI_I3C_SLAVE_LITE
#define INTR_SLV				\
	(INTR_IBI_UPDATED)
#else
#define INTR_SLV				\
	(INTR_IBI_UPDATED |			\
	 INTR_READ_REQ_RECV |			\
	 INTR_DYN_ADDR_ASSIGN |			\
	 INTR_CCC_UPDATED)
#endif
#else
#define INTR_SLV				0
#endif
#ifdef CONFIG_DW_MIPI_I3C_SECONDARY_MASTER
#define INTR_MIXED				\
	(INTR_BUSOWNER_UPDATED |		\
	 INTR_IBI_UPDATED |			\
	 INTR_DEFSLV)
#else
#define INTR_MIXED				0
#endif
#define INTR_ALL	(INTR_MST | INTR_SLV | INTR_MIXED)
/* 5.2.21 QUEUE_STATUS_LEVEL */
#define DW_CMD_QUEUE_EMPTY_LOC_OFFSET		0
#define DW_CMD_QUEUE_EMPTY_LOC_MASK		REG_8BIT_MASK
#define DW_CMD_QUEUE_EMPTY_LOC(value)		_GET_FV(DW_CMD_QUEUE_EMPTY_LOC, value)
#define DW_RESP_BUF_BLR_OFFSET			8
#define DW_RESP_BUF_BLR_MASK			REG_8BIT_MASK
#define DW_RESP_BUF_BLR(value)			_GET_FV(DW_RESP_BUF_BLR, value)
#define DW_IBI_BUF_BLR_OFFSET			16
#define DW_IBI_BUF_BLR_MASK			REG_8BIT_MASK
#define DW_IBI_BUF_BLR(value)			_GET_FV(DW_IBI_BUF_BLR, value)
#define DW_IBI_STS_CNT_OFFSET			24
#define DW_IBI_STS_CNT_MASK			REG_5BIT_MASK
#define DW_IBI_STS_CNT(value)			_GET_FV(DW_IBI_STS_CNT, value)
/* 5.2.22 DATA_BUFFER_STATUS_LEVEL */
#define DW_TX_BUF_EMPTY_LOC_OFFSET		0
#define DW_TX_BUF_EMPTY_LOC_MASK		REG_8BIT_MASK
#define DW_TX_BUF_EMPTY_LOC(value)		_GET_FV(DW_TX_BUF_EMPTY_LOC, value)
#define DW_RX_BUF_BLR_OFFSET			DW_MIPI_I3C_HW_BLR_REG_BIT_WD
#define DW_RX_BUF_BLR_MASK			REG_8BIT_MASK
#define DW_RX_BUF_BLR(value)			_GET_FV(DW_RX_BUF_BLR, value)
/* 5.2.23 PRESENT_STATE */
#define DW_SCL_LINE_SIGNAL_LEVEL		_BV(0)
#define DW_SDA_LINE_SIGNAL_LEVEL		_BV(1)
#define DW_CURRENT_MASTER			_BV(2)
#define DW_CM_TFR_STS_OFFSET			8
#define DW_CM_TFR_STS_MASK			REG_6BIT_MASK
#define DW_CM_TFR_STS(value)			_GET_FV(DW_CM_TFR_STS, value)
#define DW_CM_TFR_ST_STS_OFFSET			16
#define DW_CM_TFR_ST_STS_MASK			REG_6BIT_MASK
#define DW_CM_TFR_ST_STS(value)			_GET_FV(DW_CM_TFR_ST_STS, value)
#define DW_CMD_TID_OFFSET			24
#define DW_CMD_TID_MASK				REG_4BIT_MASK
#define DW_CMD_TID(value)			_GET_FV(DW_CMD_TID, value)
#define DW_MASTER_IDLE				_BV(28)
/* 5.2.24 CCC_DEVICE_STATUS */
#define DW_PENDING_INTR_OFFSET			0
#define DW_PENDING_INTR_MASK			REG_4BIT_MASK
#define DW_PENDING_INTR(value)			_GET_FV(DW_PENDING_INTR, value)
#define DW_PROTOCOL_ERR				_BV(6)
#define DW_ACTIVITY_MODE_OFFSET			6
#define DW_ACTIVITY_MODE_MASK			REG_2BIT_MASK
#define DW_ACTIVITY_MODE(value)			_GET_FV(DW_ACTIVITY_MODE, value)
#define DW_UNDERFLOW_ERR			_BV(8)
#define DW_SLAVE_BUSY				_BV(9)
#define DW_OVERFLOW_ERR				_BV(10)
#define DW_DATA_NOT_READY			_BV(11)
#define DW_BUFFER_NOT_AVAIL			_BV(12)
#define DW_FRAME_ERROR				_BV(13)
/* 5.2.25 DEVICE_ADDR_TABLE_POINTER */
#define DW_P_DEV_ADDR_TABLE_START_ADDR_OFFSET	0
#define DW_P_DEV_ADDR_TABLE_START_ADDR_MASK	REG_16BIT_MASK
#define DW_P_DEV_ADDR_TABLE_START_ADDR(value)	_GET_FV(DW_P_DEV_ADDR_TABLE_START_ADDR, value)
#define DW_DEV_ADDR_TABLE_DEPTH_OFFSET		16
#define DW_DEV_ADDR_TABLE_DEPTH_MASK		REG_16BIT_MASK
#define DW_DEV_ADDR_TABLE_DEPTH(value)		_GET_FV(DW_DEV_ADDR_TABLE_DEPTH, value)
/* 5.2.26 DEV_CHAR_TABLE_POINTER */
#define DW_P_DEV_CHAR_TABLE_START_ADDR_OFFSET	0
#define DW_P_DEV_CHAR_TABLE_START_ADDR_MASK	REG_12BIT_MASK
#define DW_P_DEV_CHAR_TABLE_START_ADDR(value)	_GET_FV(DW_P_DEV_CHAR_TABLE_START_ADDR, value)
#define DW_DEV_CHAR_TABLE_DEPTH_OFFSET		12
#define DW_DEV_CHAR_TABLE_DEPTH_MASK		REG_7BIT_MASK
#define DW_DEV_CHAR_TABLE_DEPTH(value)		_GET_FV(DW_DEV_CHAR_TABLE_DEPTH, value)
#define DW_PRESENT_DEV_CHAR_TABLE_INDEX_OFFSET	19
#define DW_PRESENT_DEV_CHAR_TABLE_INDEX_MASK	REG_13BIT_MASK
#define DW_PRESENT_DEV_CHAR_TABLE_INDEX(value)	_GET_FV(DW_PRESENT_DEV_CHAR_TABLE_INDEX, value)
/* 5.2.27 VENDOR_SPECIFIC_REG_POINTER */
#define DW_P_VENDOR_REG_START_ADDR_OFFSET	0
#define DW_P_VENDOR_REG_START_ADDR_MASK		REG_16BIT_MASK
#define DW_P_VENDOR_REG_START_ADDR(value)	_GET_FV(DW_P_VENDOR_REG_START_ADDR, value)
/* 5.2.28 SLV_MIPI_ID_VALUE */
#define DW_SLV_PROV_ID_SEL			_BV(0)
#define DW_SLV_MIPI_MFG_ID_OFFSET		1
#define DW_SLV_MIPI_MFG_ID_MASK			REG_15BIT_MASK
#define DW_SLV_MIPI_MFG_ID(value)		_SET_FV(DW_SLV_MIPI_MFG_ID, value)
/* 5.2.29 SLV_PID_VALUE */
#define DW_SLV_PID_DCR_OFFSET			0
#define DW_SLV_PID_DCR_MASK			REG_12BIT_MASK
#define DW_SLV_PID_DCR(value)			_SET_FV(DW_SLV_PID_DCR, value)
#define DW_SLV_INST_ID_OFFSET			12
#define DW_SLV_INST_ID_MASK			REG_4BIT_MASK
#define DW_SLV_INST_ID(value)			_SET_FV(DW_SLV_INST_ID, value)
#define DW_SLV_PART_ID_OFFSET			16
#define DW_SLV_PART_ID_MASK			REG_16BIT_MASK
#define DW_SLV_PART_ID(value)			_SET_FV(DW_SLV_PART_ID, value)
/* 5.2.30 SLV_CHAR_CTRL */
#define DW_MAX_DATA_SPEED_LIMIT			_BV(0)
#define DW_IBI_REQUEST_CAPABLE			_BV(1)
#define DW_IBI_PAYLOAD				_BV(2)
#define DW_OFFLINE_CAPABLE			_BV(3)
#define DW_BRIDGE_IDENTIFIER			_BV(4)
#define DW_HDR_CAPABLE				_BV(5)
#define DW_DEVICE_ROLE				_BV(6)
#define DW_DCR_OFFSET				8
#define DW_DCR_MASK				REG_8BIT_MASK
#define DW_DCR(value)				_SET_FV(DW_DCR, value)
#define DW_HDR_CAP_OFFSET			16
#define DW_HDR_CAP_MASK				REG_8BIT_MASK
#define DW_HDR_CAP(value)			_GET_FV(DW_HDR_CAP, value)
/* 5.2.31 SLV_MAX_LEN */
#define DW_MWL_OFFSET				0
#define DW_MWL_MASK				REG_16BIT_MASK
#define DW_MWL(value)				_GET_FV(DW_MWL, value)
#define DW_MRL_OFFSET				16
#define DW_MRL_MASK				REG_16BIT_MASK
#define DW_MRL(value)				_GET_FV(DW_MRL, value)
/* 5.2.32 MAX_READ_TURNAROUND */
#define DW_MXDS_MAX_RD_TURN_OFFSET		0
#define DW_MXDS_MAX_RD_TURN_MASK		REG_24BIT_MASK
#define DW_MXDS_MAX_RD_TURN(value)		_GET_FV(DW_MXDS_MAX_RD_TURN, value)
/* 5.2.33 MAX_DATA_SPEED */
#define DW_MXDS_MAX_WR_SPEED_OFFSET		0
#define DW_MXDS_MAX_WR_SPEED_MASK		REG_3BIT_MASK
#define DW_MXDS_MAX_WR_SPEED(value)		_SET_FV(DW_MXDS_MAX_WR_SPEED, value)
#define DW_MXDS_MAX_RD_SPEED_OFFSET		8
#define DW_MXDS_MAX_RD_SPEED_MASK		REG_3BIT_MASK
#define DW_MXDS_MAX_RD_SPEED(value)		_SET_FV(DW_MXDS_MAX_RD_SPEED, value)
#define DW_MXDS_CLK_DATA_TURN_OFFSET		16
#define DW_MXDS_CLK_DATA_TURN_MASK		REG_3BIT_MASK
#define DW_MXDS_CLK_DATA_TURN(value)		_SET_FV(DW_MXDS_CLK_DATA_TURN, value)
/* 5.2.34 SLV_INTR_REQ */
#define DW_SIR					_BV(0)
#define DW_SIR_CTRL_OFFSET			1
#define DW_SIR_CTRL_MASK			REG_2BIT_MASK
#define DW_SIR_CTRL(value)			_SET_FV(DW_SIR_CTRL, value)
#define DW_MR					_BV(3)
#define DW_IBI_STS_OFFSET			8
#define DW_IBI_STS_MASK				REG_2BIT_MASK
#define DW_IBI_STS(value)			_GET_FV(DW_IBI_STS, value)
/* 5.2.35 SLV_TSX_SYMBL_TIMING */
#define DW_SLV_TSX_SYMBL_CNT_OFFSET		0
#define DW_SLV_TSX_SYMBL_CNT_MASK		REG_6BIT_MASK
#define DW_SLV_TSX_SYMBL_CNT(value)		_SET_FV(DW_SLV_TSX_SYMBL_CNT, value)
/* 5.2.36 DEVICE_CTRL_EXTENDED */
#define DW_DEV_OPERATION_MODE_OFFSET		0
#define DW_DEV_OPERATION_MODE_MASK		REG_2BIT_MASK
#define DW_DEV_OPERATION_MODE(value)		_SET_FV(DW_DEV_OPERATION_MODE, value)
#define DW_REQMST_ACK_CTRL			_BV(3)
/* 5.2.37 SCL_I3C_OD_TIMING */
#define DW_I3C_OD_LCNT_OFFSET			0
#define DW_I3C_OD_LCNT_MASK			REG_8BIT_MASK
#define DW_I3C_OD_LCNT(value)			_SET_FV(DW_I3C_OD_LCNT, value)
#define DW_I3C_OD_HCNT_OFFSET			16
#define DW_I3C_OD_HCNT_MASK			REG_8BIT_MASK
#define DW_I3C_OD_HCNT(value)			_SET_FV(DW_I3C_OD_LCNT, value)
/* 5.2.38 SCL_I3C_PP_TIMING */
#define DW_I3C_PP_LCNT_OFFSET			0
#define DW_I3C_PP_LCNT_MASK			REG_8BIT_MASK
#define DW_I3C_PP_LCNT(value)			_SET_FV(DW_I3C_PP_LCNT, value)
#define DW_I3C_PP_HCNT_OFFSET			16
#define DW_I3C_PP_HCNT_MASK			REG_8BIT_MASK
#define DW_I3C_PP_HCNT(value)			_SET_FV(DW_I3C_PP_HCNT, value)
#define DW_I3C_PP_CNT_MIN			5
/* 5.2.39 SCL_I2C_FM_TIMING */
#define DW_I2C_FM_LCNT_OFFSET			0
#define DW_I2C_FM_LCNT_MASK			REG_16BIT_MASK
#define DW_I2C_FM_LCNT(value)			_SET_FV(DW_I2C_FM_LCNT, value)
#define DW_I2C_FM_HCNT_OFFSET			16
#define DW_I2C_FM_HCNT_MASK			REG_16BIT_MASK
#define DW_I2C_FM_HCNT(value)			_SET_FV(DW_I2C_FM_HCNT, value)
/* 5.2.40 SCL_I2C_FMP_TIMING */
#define DW_I2C_FMP_LCNT_OFFSET			0
#define DW_I2C_FMP_LCNT_MASK			REG_16BIT_MASK
#define DW_I2C_FMP_LCNT(value)			_SET_FV(DW_I2C_FMP_LCNT, value)
#define DW_I2C_FMP_HCNT_OFFSET			16
#define DW_I2C_FMP_HCNT_MASK			REG_16BIT_MASK
#define DW_I2C_FMP_HCNT(value)			_SET_FV(DW_I2C_FMP_HCNT, value)
/* 5.2.41 SCL_EXT_LCNT_TIMING */
#define DW_I3C_EXT_LCNT_1_OFFSET		0
#define DW_I3C_EXT_LCNT_1_MASK			REG_8BIT_MASK
#define DW_I3C_EXT_LCNT_1(value)		_SET_FV(DW_I3C_EXT_LCNT_1, value)
#define DW_I3C_EXT_LCNT_2_OFFSET		8
#define DW_I3C_EXT_LCNT_2_MASK			REG_8BIT_MASK
#define DW_I3C_EXT_LCNT_2(value)		_SET_FV(DW_I3C_EXT_LCNT_2, value)
#define DW_I3C_EXT_LCNT_3_OFFSET		16
#define DW_I3C_EXT_LCNT_3_MASK			REG_8BIT_MASK
#define DW_I3C_EXT_LCNT_3(value)		_SET_FV(DW_I3C_EXT_LCNT_3, value)
#define DW_I3C_EXT_LCNT_4_OFFSET		24
#define DW_I3C_EXT_LCNT_4_MASK			REG_8BIT_MASK
#define DW_I3C_EXT_LCNT_4(value)		_SET_FV(DW_I3C_EXT_LCNT_4, value)
/* 5.2.42 SCL_EXT_TERMN_LCNT_TIMING */
#define DW_I3C_EXT_TERMN_LCNT_OFFSET		0
#define DW_I3C_EXT_TERMN_LCNT_MASK		REG_4BIT_MASK
#define DW_I3C_EXT_TERMN_LCNT(value)		_SET_FV(DW_I3C_EXT_TERMN_LCNT, value)
#define DW_I3C_TS_SKEW_CNT_OFFSET		16
#define DW_I3C_TS_SKEW_CNT_MASK			REG_4BIT_MASK
#define DW_I3C_TS_SKEW_CNT(value)		_SET_FV(DW_I3C_TS_SKEW_CNT, value)
/* 5.2.43 SCL_HOLD_SWITCH_DLY_TIMING */
#define DW_SDA_OD_PP_SWITCH_DLY_OFFSET		0
#define DW_SDA_OD_PP_SWITCH_DLY_MASK		REG_3BIT_MASK
#define DW_SDA_OD_PP_SWITCH_DLY(value)		_SET_FV(DW_SDA_OD_PP_SWITCH_DLY, value)
#define DW_SDA_PP_OD_SWITCH_DLY_OFFSET		8
#define DW_SDA_PP_OD_SWITCH_DLY_MASK		REG_3BIT_MASK
#define DW_SDA_PP_OD_SWITCH_DLY(value)		_SET_FV(DW_SDA_PP_OD_SWITCH_DLY, value)
#define DW_SDA_TX_HOLD_OFFSET			16
#define DW_SDA_TX_HOLD_MASK			REG_3BIT_MASK
#define DW_SDA_TX_HOLD(value)			_SET_FV(DW_SDA_TX_HOLD, value)
/* 5.2.44 BUS_FREE_AVAIL_TIMING */
#define DW_BUS_FREE_TIME_OFFSET			0
#define DW_BUS_FREE_TIME_MASK			REG_16BIT_MASK
#define DW_BUS_FREE_TIME(value)			_SET_FV(DW_BUS_FREE_TIME, value)
#define DW_BUS_AVAILABLE_TIME_OFFSET		16
#define DW_BUS_AVAILABLE_TIME_MASK		REG_16BIT_MASK
#define DW_BUS_AVAILABLE_TIME(value)		_SET_FV(DW_BUS_AVAILABLE_TIME, value)
/* 5.2.45 BUS_IDLE_TIMING */
#define DW_BUS_IDLE_TIME_OFFSET			0
#define DW_BUS_IDLE_TIME_MASK			REG_20BIT_MASK
#define DW_BUS_IDLE_TIME(value)			_SET_FV(DW_BUS_IDLE_TIME, value)
/* 5.2.49 QUEUE_SIZE_CAPABILITY */
#define DW_TX_BUF_SIZE_OFFSET			0
#define DW_TX_BUF_SIZE_MASK			REG_4BIT_MASK
#define DW_TX_BUF_SIZE(value)			_GET_FV(DW_TX_BUF_SIZE, value)
#define DW_RX_BUF_SIZE_OFFSET			4
#define DW_RX_BUF_SIZE_MASK			REG_4BIT_MASK
#define DW_RX_BUF_SIZE(value)			_GET_FV(DW_RX_BUF_SIZE, value)
#define DW_CMD_BUF_SIZE_OFFSET			8
#define DW_CMD_BUF_SIZE_MASK			REG_4BIT_MASK
#define DW_CMD_BUF_SIZE(value)			_GET_FV(DW_CMD_BUF_SIZE, value)
#define DW_RESP_BUF_SIZE_OFFSET			12
#define DW_RESP_BUF_SIZE_MASK			REG_4BIT_MASK
#define DW_RESP_BUF_SIZE(value)			_GET_FV(DW_RESP_BUF_SIZE, value)
#define DW_IBI_BUF_SIZE_OFFSET			16
#define DW_IBI_BUF_SIZE_MASK			REG_4BIT_MASK
#define DW_IBI_BUF_SIZE(value)			_GET_FV(DW_IBI_BUF_SIZE, value)
/* 5.2.55 DEV_ADDR_TABLE1_LOC1 */
/* 5.2.56 DEV_ADDR_TABLE_LOC1 */
#define DW_DEV_DYNAMIC_ADDR_OFFSET		16
#define DW_DEV_DYNAMIC_ADDR_MASK		REG_8BIT_MASK
#define DW_DEV_DYNAMIC_ADDR(value)		_SET_FV(DW_DEV_DYNAMIC_ADDR, value)
/* 5.2.55 DEV_ADDR_TABLE1_LOC1 */
#define DW_STATIC_ADDRESS			_BV(0)
#define DW_IBI_PEC_EN				_BV(11)
#define DW_SIR_REJECT				_BV(13)
#define DW_MR_REJECT				_BV(14)
#define DW_DEVICE				_BV(31)
/* 5.2.56 DEV_ADDR_TABLE_LOC1 */
#define DW_DEV_STATIC_ADDR_OFFSET		0
#define DW_DEV_STATIC_ADDR_MASK			REG_7BIT_MASK
#define DW_DEV_STATIC_ADDR(value)		_SET_FV(DW_DEV_STATIC_ADDR, value)
#define DW_DEV_NACK_RETRY_CNT_OFFSET		29
#define DW_DEV_NACK_RETRY_CNT_MASK		REG_2BIT_MASK
#define DW_DEV_NACK_RETRY_CNT(value)		_SET_FV(DW_DEV_NACK_RETRY_CNT, value)
#define DW_LEGACY_I2C_DEVICE			_BV(31)

#define dw_mipi_i3c_cmd_fifo_depth(n)		\
	DW_CMD_QUEUE_EMPTY_LOC(__raw_readl(QUEUE_STATUS_LEVEL(n)))
#define dw_mipi_i3c_data_fifo_depth(n)		\
	DW_TX_BUF_EMPTY_LOC(__raw_readl(DATA_BUFFER_STATUS_LEVEL(n)))
#define dw_mipi_i3c_data_start_addr(n)		\
	DW_P_DEV_ADDR_TABLE_START_ADDR(__raw_readl(DEVICE_ADDR_TABLE_POINTER(n)))
#define dw_mipi_i3c_max_devs(n)			\
	DW_DEV_ADDR_TABLE_DEPTH(__raw_readl(DEVICE_ADDR_TABLE_POINTER(n)))
#define dw_mipi_i3c_config_sa(n, addr)					\
	dw_i3c_writel_mask(DW_STATIC_ADDR(addr) |			\
			   DW_STATIC_ADDR_VALID,			\
			   DW_STATIC_ADDR(DW_STATIC_ADDR_MASK) |	\
			   DW_STATIC_ADDR_VALID,			\
			   DEVICE_ADDR(n))
#define dw_mipi_i3c_config_da(n, addr)					\
	dw_i3c_writel_mask(DW_DYNAMIC_ADDR(addr) |			\
			   DW_DYNAMIC_ADDR_VALID,			\
			   DW_DYNAMIC_ADDR(DW_DYNAMIC_ADDR_MASK) |	\
			   DW_DYNAMIC_ADDR_VALID,			\
			   DEVICE_ADDR(n))
#define dw_mipi_i3c_disable_all_irqs(n)					\
	dw_i3c_writel(0, INTR_STATUS_EN(n))
#define dw_mipi_i3c_enable_all_irqs(n)					\
	dw_i3c_writel(INTR_ALL, INTR_STATUS_EN(n))
#define dw_mipi_i3c_mask_all_irqs(n)					\
	dw_i3c_writel(0, INTR_SIGNAL_EN(dw_i3cd))
#define dw_mipi_i3c_unmask_all_irqs(n)					\
	dw_i3c_writel(INTR_ALL, INTR_SIGNAL_EN(dw_i3cd))
#define dw_mipi_i3c_enable_irq(n, irq)		dw_i3c_setl(irq, INTR_STATUS_EN(n))
#define dw_mipi_i3c_disable_irq(n, irq)		dw_i3c_clearl(irq, INTR_STATUS_EN(n))
#define dw_mipi_i3c_unmask_irq(n, irq)		dw_i3c_setl(irq, INTR_SIGNAL_EN(n))
#define dw_mipi_i3c_mask_irq(n, irq)		dw_i3c_clearl(irq, INTR_SIGNAL_EN(n))
#define dw_mipi_i3c_irq_pending(n, irq)		(!!(dw_i3c_readl(INTR_STATUS(n)) & (irq)))
#define dw_mipi_i3c_dev_enable(n)		dw_i3c_setl(DW_ENABLE, DEVICE_CTRL(n))
#define dw_mipi_i3c_dev_disable(n)		dw_i3c_clearl(DW_ENABLE, DEVICE_CTRL(n))

struct dw_mipi_i3c_cmd {
	uint32_t cmd_hi;
	uint32_t cmd_lo;
	uint16_t tx_len;
	uint8_t *tx_buf;
	uint16_t rx_len;
	uint8_t *rx_buf;
	uint8_t error;
};

struct dw_mipi_i3c_xfer {
	struct list_head node;
	int ret;
        uint8_t ncmds;
        struct dw_mipi_i3c_cmd cmds[]; /* __counted_by(ncmds) */
};

struct dw_mipi_i3c_dat_entry {
	uint8_t addr;
	bool is_i2c_addr;
};

struct dw_mipi_i3c_ctx {
	uint16_t dat_base;
	uint16_t maxdevs;
	uint32_t free_pos;
	unsigned int ncmds;
	struct dw_mipi_i3c_cmd cmds[1];
	struct list_head xfer_list;
	struct dw_mipi_i3c_xfer *xfer_curr;
	struct dw_mipi_i3c_xfer xfer_pool[DW_MIPI_I3C_MAX_XFERS];
	struct dw_mipi_i3c_dat_entry devs[DW_MIPI_I3C_MAX_DEVS];
};

#if NR_DW_I3CS > 1
extern i3c_t dw_i3cd;
void dw_mipi_i3c_master_select(i3c_t i3c);
#else
#define dw_i3cd						0
#endif

#define dw_i3c_setl(v,a)				\
	do {						\
		uint32_t __v = dw_i3c_readl(a);		\
		__v |= (v);				\
		dw_i3c_writel(__v, (a));		\
	} while (0)
#define dw_i3c_clearl(v,a)				\
	do {						\
		uint32_t __v = dw_i3c_readl(a);		\
		__v &= ~(v);				\
		dw_i3c_writel(__v, (a));		\
	} while (0)
#define dw_i3c_writel_mask(v,m,a)			\
	do {						\
		uint32_t __v = dw_i3c_readl(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		dw_i3c_writel(__v, (a));		\
	} while (0)
uint32_t dw_i3c_readl(caddr_t reg);
void dw_i3c_writel(uint32_t val, caddr_t reg);

void dw_mipi_i3c_ctrl_init(clk_freq_t core_rate);
void dw_mipi_i3c_set_speed(bool od_normal);
void dw_mipi_i3c_submit_ccc(struct i3c_ccc *ccc);
void dw_mipi_i3c_handle_irq(void);
void dw_mipi_i3c_transfer_reset(void);
void dw_mipi_i3c_start_condition(bool sr);
void dw_mipi_i3c_stop_condition(void);
void dw_mipi_i3c_write_byte(uint8_t byte);
uint8_t dw_mipi_i3c_read_byte(void);

#endif /* __DW_MIPI_I3C_H_INCLUDE__ */
