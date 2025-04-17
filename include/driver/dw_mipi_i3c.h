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
/* 5.2.13 IBI_MR_REQ_REJECT
 * 5.2.14 IBI_SIR_REQ_REJECT
 */
#define IBI_REQ_REJECT_ALL			GENMASK(31, 0)
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
#define INTR_ALL					\
	(INTR_BUS_RESET_DONE | INTR_BUSOWNER_UPDATED |	\
	 INTR_IBI_UPDATED | INTR_READ_REQ_RECV |	\
	 INTR_DEFSLV | INTR_TRANSFER_ERR |		\
	 INTR_DYN_ADDR_ASSIGN | INTR_CCC_UPDATED |	\
	 INTR_TRANSFER_ABORT | INTR_RESP_READY |	\
	 INTR_CMD_QUEUE_READY | INTR_IBI_THLD |		\
	 INTR_RX_THLD | INTR_TX_THLD)
#define INTR_MST				\
	(INTR_TRANSFER_ERR | INTR_RESP_READY)
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
/* 5.2.25 DEVICE_ADDR_TABLE_POINTER */
#define DW_P_DEV_ADDR_TABLE_START_ADDR_OFFSET	0
#define DW_P_DEV_ADDR_TABLE_START_ADDR_MASK	REG_16BIT_MASK
#define DW_P_DEV_ADDR_TABLE_START_ADDR(value)	_GET_FV(DW_P_DEV_ADDR_TABLE_START_ADDR, value)
#define DW_DEV_ADDR_TABLE_DEPTH_OFFSET		16
#define DW_DEV_ADDR_TABLE_DEPTH_MASK		REG_16BIT_MASK
#define DW_DEV_ADDR_TABLE_DEPTH(value)		_GET_FV(DW_DEV_ADDR_TABLE_DEPTH, value)
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
/* 5.2.44 BUS_FREE_AVAIL_TIMING */
#define DW_BUS_FREE_TIME_OFFSET			0
#define DW_BUS_FREE_TIME_MASK			REG_16BIT_MASK
#define DW_BUS_FREE_TIME(value)			_SET_FV(DW_BUS_FREE_TIME, value)
#define DW_BUS_AVAILABLE_TIME_OFFSET		16
#define DW_BUS_AVAILABLE_TIME_MASK		REG_16BIT_MASK
#define DW_BUS_AVAILABLE_TIME(value)		_SET_FV(DW_BUS_AVAILABLE_TIME, value)

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
#define dw_mipi_i3c_disable_all_irqs(n)		\
	dw_i3c_writel(INTR_ALL, INTR_STATUS(n))
#define dw_mipi_i3c_enable_mst_irqs(n)					\
	do {								\
		dw_i3c_writel(INTR_MST, INTR_STATUS_EN(dw_i3cd));	\
		dw_i3c_writel(INTR_MST, INTR_SIGNAL_EN(dw_i3cd));	\
	} while (0)
#define dw_mipi_i3c_enable_irq(n, irq)		dw_i3c_setl(irq, INTR_STATUS_EN(n))
#define dw_mipi_i3c_disable_irq(n, irq)		dw_i3c_clearl(irq, INTR_STATUS_EN(n))
#define dw_mipi_i3c_unmask_irq(n, irq)		dw_i3c_setl(irq, INTR_SIGNAL_EN(n))
#define dw_mipi_i3c_mask_irq(n, irq)		dw_i3c_clearl(irq, INTR_SIGNAL_EN(n))
#define dw_mipi_i3c_irq_pending(n, irq)		(!!(dw_i3c_readl(INTR_STATUS(n)) & (irq)))

struct dw_mipi_i3c_ctx {
	uint8_t addr_mode;
	uint8_t last_tx_byte;
	int state;
	uint8_t status;
};

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

void dw_mipi_i3c_ctrl_init(i3c_bus_t bus, clk_freq_t core_rate);
void dw_mipi_i3c_handle_irq(void);
void dw_mipi_i3c_transfer_reset(void);

#endif /* __DW_MIPI_I3C_H_INCLUDE__ */
