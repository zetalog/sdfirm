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

#ifdef CONFIG_ARCH_HAS_DW_MIPI_I3C_HCI
#include <target/i3c_hci.h>
#endif /* CONFIG_ARCH_HAS_DW_MIPI_I3C_HCI */

#ifdef CONFIG_ARCH_HAS_DW_MIPI_I3C_SNPS
#ifndef CONFIG_DW_MIPI_I3C_SLAVE_LITE
#define DEVICE_CTRL(n)				DW_MIPI_I3C_REG(n, 0x000)
#define DEVICE_ADDR(n)				DW_MIPI_I3C_REG(n, 0x004)
#define COMMAND_QUEUE_PORT(n)			DW_MIPI_I3C_REG(n, 0x00C)
#define RESPONSE_QUEUE_PORT(n)			DW_MIPI_I3C_REG(n, 0x010)
#define RX_DATA_QUEUE_PORT(n)			DW_MIPI_I3C_REG(n, 0x014)
#define TX_DATA_QUEUE_PORT(n)			DW_MIPI_I3C_REG(n, 0x014)
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
#endif
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
#define SCL_I3C_FM_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0BC)
#define SCL_I3C_FMP_TIMING(n)			DW_MIPI_I3C_REG(n, 0x0C0)
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
#endif /* CONFIG_ARCH_HAS_DW_MIPI_I3C_SNPS */

#endif /* __DW_MIPI_I3C_H_INCLUDE__ */
