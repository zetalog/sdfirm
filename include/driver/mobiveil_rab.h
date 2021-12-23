/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)mobiveil_rab.h: MobiVeil RapidIO to AXI Bridge (RAB) interface
 * $Id: mobiveil_rab.h,v 1.0 2021-11-02 17:05:00 zhenglv Exp $
 */

#ifndef __MOBIVEIL_RAB_H_INCLUDE__
#define __MOBIVEIL_RAB_H_INCLUDE__

#define RAB_NUM_RIO_PIO			8
#define RAB_NUM_AXI_PIO			8
#define RAB_NUM_ASLV			4
#define RAB_NUM_AMST			8
#define RAB_NUM_A2P_AMAP_WIN		32
#define RAB_NUM_MB_OB			15
#define RAB_NUM_MB_IB			32
#define RAB_NUM_WDMA			8
#define RAB_NUM_RDMA			8
#define RAB_NUM_LFC			64

#ifdef CONFIG_RAB_PAGE_2KB
#define RAB_PAGE_SHIFT			11
#endif /* CONFIG_RAB_PAGE_2KB */
#ifdef CONFIG_RAB_PAGE_4KB
#define RAB_PAGE_SHIFT			12
#endif /* CONFIG_RAB_PAGE_4KB */
#define RAB_PAGE_SIZE			_BV(RAB_PAGE_SHIFT)
#define RAB_PAGE_MASK			(RAB_PAGE_SIZE - 1)
#define RAB_ACCESS_SIZE			_BV(RAB_PAGE_SHIFT + 1)
#define RAB_ACCESS_MASK			(RAB_ACCESS_SIZE - 1)
#define RAB_ACCESS(addr)				\
	((RIO_PTR2INT(addr) & ~RAB_ACCESS_MASK) +	\
	 (RIO_PTR2INT(addr) & RAB_PAGE_MASK) +		\
	 RAB_PAGE_SIZE)

#define RAB_REG(offset)			(RAB_BASE + (offset))

#define RAB_GLOBAL_CSR(offset)		RAB_REG(0x20000 + (offset))
#define RAB_INTR_CSR(offset)		RAB_REG(0x20040 + (offset))
#define RAB_RPIO_CSR(n, offset)		RAB_REG(0x20080 + (offset) + 0x08 * (n))
#define RAB_RIO_AMAP_CSR(in, offset)	RAB_REG(0x20100 + (offset) + 0x08 * (n))
#define RAB_APIO_CSR(n, offset)		RAB_REG(0x20180 + (offset) + 0x08 * (n))
#define RAB_ASLV_CSR(n, offset)		RAB_REG(0x201C0 + (offset) + 0x08 * (n))
#define RAB_AMST_CSR(n, offset)		RAB_REG(0x201E0 + (offset) + 0x08 * (n))
#define RAB_AXI_AMAP_CSR(n, offset)	RAB_REG(0x20200 + (offset) + 0x10 * (n))
#define RAB_DB_CSR(offset)		RAB_REG(0x20400 + (offset))
#define RAB_OB_DME_CSR(n, offset)	RAB_REG(0x20500 + (offset) + 0x10 * (n))
#define RAB_IB_DME_CSR(n, offset)	RAB_REG(0x20600 + (offset) + 0x10 * (n))
#define RAB_WDMA_CSR(n, offset)		RAB_REG(0x20800 + (offset) + 0x10 * (n))
#define RAB_RDMA_CSR(n, offset)		RAB_REG(0x20880 + (offset) + 0x10 * (n))
#define RAB_DMA_CSR(offset)		RAB_REG(0x20900 + (offset))
#define RAB_LFC_CSR(n)			RAB_REG(0x20920 + 0x04 * (n))
#define RAB_SRDS_CSR(offset)		RAB_REG(0x20980 + (offset) + 0x04 * (n))
#define RAB_USR_CSR(n)			RAB_REG(0x209A0 + 0x04 * (n))
#define RAB_LPM_CSR(offset)		RAB_REG(0x22800 + (offset))
#define RAB_50BIT_CSR(n, offset)	RAB_REG(0x22900 + (offset) + 0x04 * (n))

/* RAB Global CSR */
#define RAB_VER				RAB_GLOBAL_CSR(0x00)
#define RAB_CAPA			RAB_GLOBAL_CSR(0x04)
#define RAB_CTRL			RAB_GLOBAL_CSR(0x08)
#define RAB_STAT			RAB_GLOBAL_CSR(0x0C)
#define RAB_AXI_TIMEOUT			RAB_GLOBAL_CSR(0x10)
#define RAB_DME_TIMEOUT			RAB_GLOBAL_CSR(0x14)
#define RAB_RST_CTRL			RAB_GLOBAL_CSR(0x18)
#define RAB_COOP_LOCK			RAB_GLOBAL_CSR(0x1C)
#define RAB_STAT_ESEL			RAB_GLOBAL_CSR(0x20)
#define RAB_STAT_STAT			RAB_GLOBAL_CSR(0x24)
#define RAB_IB_PW_CSR			RAB_GLOBAL_CSR(0x28)
#define RAB_IB_PW_Data			RAB_GLOBAL_CSR(0x2C)
#define RAB_APB_CSR			RAB_GLOBAL_CSR(0x30)
#define RAB_ARB_TIMEOUT			RAB_GLOBAL_CSR(0x34)
#define RAB_DESC_RDY_TIMEOUT		RAB_GLOBAL_CSR(0x38)

/* Interrupt CSR */
#define RAB_INTR_ENAB_GNRL		RAB_INTR_CSR(0x00)
#define RAB_INTR_ENAB_APIO		RAB_INTR_CSR(0x04)
#define RAB_INTR_ENAB_RPIO		RAB_INTR_CSR(0x08)
#define RAB_INTR_ENAB_WDMA		RAB_INTR_CSR(0x0C)
#define RAB_INTR_ENAB_RDMA		RAB_INTR_CSR(0x10)
#define RAB_INTR_ENAB_IDME		RAB_INTR_CSR(0x14)
#define RAB_INTR_ENAB_ODME		RAB_INTR_CSR(0x18)
#define RAB_INTR_ENAB_MISC		RAB_INTR_CSR(0x1C)
#define RAB_INTR_STAT_GNRL		RAB_INTR_CSR(0x20)
#define RAB_INTR_STAT_APIO		RAB_INTR_CSR(0x24)
#define RAB_INTR_STAT_RPIO		RAB_INTR_CSR(0x28)
#define RAB_INTR_STAT_WDMA		RAB_INTR_CSR(0x2C)
#define RAB_INTR_STAT_RDMA		RAB_INTR_CSR(0x30)
#define RAB_INTR_STAT_IDME		RAB_INTR_CSR(0x34)
#define RAB_INTR_STAT_ODME		RAB_INTR_CSR(0x38)
#define RAB_INTR_STAT_MISC		RAB_INTR_CSR(0x3C)

/* RIO PIO Engine CSR (N = 0~RAB_NUM_RIO_PIO [max 8]) */
#define RAB_RIO_PIO_CTRL(n)		RAB_RPIO_CSR(n, 0x00)
#define RAB_RIO_PIO_STAT(n)		RAB_RPIO_CSR(n, 0x04)
#define RAB_RPIO_CTRL(n)		RAB_RIO_PIO_CTRL(n)
#define RAB_RPIO_STAT(n)		RAB_RIO_PIO_STAT(n)

/* RIO Address Mapping CSR */
#define RAB_RIO_AMAP_LUT(n)		RAB_RIO_AMAP_CSR(n, 0x00)
#define RAB_RIO_AMAP_IDSL		RAB_REG(0x20140)
#define RAB_RIO_AMAP_BYPS		RAB_REG(0x20144)
#define RAB_RPIO_AMAP_LUT(n)		RAB_RIO_AMAP_LUT(n)
#define RAB_RPIO_AMAP_IDSL		RAB_RIO_AMAP_IDSL
#define RAB_RPIO_AMAP_BYPS		RAB_RIO_AMAP_BYPS

/* AXI PIO Engine CSR (N = 0~RAB_NUM_AXI_PIO [max 8]) */
#define RAB_AXI_PIO_CTRL(n)		RAB_APIO_CSR(n, 0x00)
#define RAB_AXI_PIO_STAT(n)		RAB_APIO_CSR(n, 0x04)
#define RAB_APIO_CTRL(n)		RAB_AXI_PIO_CTRL(n)
#define RAB_APIO_STAT(n)		RAB_AXI_PIO_STAT(n)

/* AXI Slave CSR */
#define RAB_ASLV_STAT_CMD(n)		RAB_ASLV_CSR(n, 0x00)
#define RAB_ASLV_STAT_ADDR(n)		RAB_ASLV_CSR(n, 0x04)

/* AXI Master CSR */
#define RAB_AMST_STAT(n)		RAB_AMST_CSR(n, 0x00)

/* AXI Address Mapping CSR (N = 0~RAB_NUM_A2P_AMAP_WIN [max 32] */
#define RAB_AXI_AMAP_CTRL(n)		RAB_AXI_AMAP_CSR(n, 0x00)
#define RAB_AXI_AMAP_SIZE(n)		RAB_AXI_AMAP_CSR(n, 0x04)
#define RAB_AXI_AMAP_ABAR(n)		RAB_AXI_AMAP_CSR(n, 0x08)
#define RAB_AXI_AMAP_RBAR(n)		RAB_AXI_AMAP_CSR(n, 0x0C)
#define RAB_APIO_AMAP_CTRL(n)		RAB_AXI_AMAP_CTRL(n)
#define RAB_APIO_AMAP_SIZE(n)		RAB_AXI_AMAP_SIZE(n)
#define RAB_APIO_AMAP_ABAR(n)		RAB_AXI_AMAP_ABAR(n)
#define RAB_APIO_AMAP_RBAR(n)		RAB_AXI_AMAP_RBAR(n)

/* Doorbell Message CSR */
#define RAB_OB_DB_CSR(n)		RAB_DB_CSR(0x08 * (n))
#define RAB_OB_DB_INFO(n)		RAB_DB_CSR(0x04 + (0x08 * (n)))
#define RAB_OB_IDB_CSR			RAB_DB_CSR(0x78)
#define RAB_OB_IDB_INFO			RAB_DB_CSR(0x7C)
#define RAB_IB_DB_CSR			RAB_DB_CSR(0x80)
#define RAB_IB_DB_INFO			RAB_DB_CSR(0x84)
#define RAB_IB_DB_CHK(n)		RAB_DB_CSR(0x88 + (0x04 * (n)))

/* Outbound DME CSR */
#define RAB_OB_DME_CTRL(n)		RAB_OB_DME_CSR(n, 0x00)
#define RAB_OB_DME_ADDR(n)		RAB_OB_DME_CSR(n, 0x04)
#define RAB_OB_DME_STAT(n)		RAB_OB_DME_CSR(n, 0x08)
#define RAB_OB_DME_TIDMSK		RAB_REG(0x205F0)

/* Inbound DME CSR */
#define RAB_IB_DME_CTRL(n)		RAB_IB_DME_CSR(n, 0x00)
#define RAB_IB_DME_ADDR(n)		RAB_IB_DME_CSR(n, 0x04)
#define RAB_IB_DME_STAT(n)		RAB_IB_DME_CSR(n, 0x08)
#define RAB_IB_DME_DESC(n)		RAB_IB_DME_CSR(n, 0x0C)

/* Write DMA CSR */
#define RAB_WDMA_CTRL(n)		RAB_WDMA_CSR(n, 0x00)
#define RAB_WDMA_ADDR(n)		RAB_WDMA_CSR(n, 0x04)
#define RAB_WDMA_STAT(n)		RAB_WDMA_CSR(n, 0x08)
#define RAB_WDMA_ADDR_EXT(n)		RAB_WDMA_CSR(n, 0x0C)

/* Read DMA CSR */
#define RAB_RDMA_CTRL(n)		RAB_RDMA_CSR(n, 0x00)
#define RAB_RDMA_ADDR(n)		RAB_RDMA_CSR(n, 0x04)
#define RAB_RDMA_STAT(n)		RAB_RDMA_CSR(n, 0x08)
#define RAB_RDMA_ADDR_EXT(n)		RAB_RDMA_CSR(n, 0x0C)

/* DMA Descriptor CSR */
#define RAB_DMA_IADDR_DESC_SEL		RAB_DMA_CSR(0x00)
#define RAB_DMA_IADDR_DESC_CTRL		RAB_DMA_CSR(0x04)
#define RAB_DMA_IADDR_DESC_SRC_ADDR	RAB_DMA_CSR(0x08)
#define RAB_DMA_IADDR_DESC_DEST_ADDR	RAB_DMA_CSR(0x0C)
#define RAB_DMA_IADDR_DESC_NEXT_ADDR	RAB_DMA_CSR(0x10)

/* Logical Flow Control */
/* RAB_LFC_CSR already defined */
#define RAB_LFC_TIMEOUT			RAB_REG(0x20960)
#define RAB_LFC_BLOCKED			RAB_REG(0x20964)

/* SERDES Control & Status */
#define RAB_SRDS_CTRL(n)		RAB_SRDS_CSR(n, 0x00)
#define RAB_SRDS_STAT(n)		RAB_SRDS_CSR(n, 0x04)

/* USR CSR Register */
/* RAB_USR_CSR already defined */

/* Low Power Management */
#define RAB_PIO_ENGINE_STAT		RAB_LPM_CSR(0x00)
#define RAB_DMA_ENGINE_STAT		RAB_LPM_CSR(0x04)
#define RAB_IDME_ENGINE_STAT		RAB_LPM_CSR(0x08)
#define RAB_ODME_ENGINE_STAT		RAB_LPM_CSR(0x0C)
#define RAB_AMBA_MISC_STAT		RAB_LPM_CSR(0x10)
#define RAB_IDSE_ENGINE_STAT		RAB_LPM_CSR(0x14)
#define RAB_ODSE_ENGINE_STAT		RAB_LPM_CSR(0x18)

/* 50-bit Addressing */
#define RAB_APIO_RIO_UPPER16_ADDR(n)	RAB_50BIT_CSR(n, 0x00)
#define RAB_WDMA_RIO_UPPER16_ADDR(n)	RAB_50BIT_CSR(n, 0x20)
#define RAB_RDMA_RIO_UPPER16_ADDR(n)	RAB_50BIT_CSR(n, 0x40)

/* ======================================================================
 * 19.3 Global Registers
 * ====================================================================== */
/* 19.3.3 Bridge Control Register */
#define RAB_AXIPIOEnable				_BV(0)
#define RAB_RIOPIOEnable				_BV(1)
#define RAB_WriteDMAEnable				_BV(2)
#define RAB_ReadDMAEnable				_BV(3)
#define RAB_AXIByteSwappingSelect_OFFSET		4
#define RAB_AXIByteSwappingSelect_MASK			REG_2BIT_MASK
#define RAB_AXIByteSwappingSelect(value)		\
	_SET_FV(RAB_AXIByteSwappingSelect, value)
#define RAB_OBMessageArbitration_OFFSET			6
#define RAB_OBMessageArbitration_MASK			REG_2BIT_MASK
#define RAB_OBMessageArbitration(value)			\
	_SET_FV(RAB_OBMessageArbitration, value)
#define RAB_PrefetchSizeOfDMADescriptorsArray_OFFSET	8
#define RAB_PrefetchSizeOfDMADescriptorsArray_MASK	REG_2BIT_MASK
#define RAB_PrefetchSizeOfDMADescriptorsArray(value)	\
	_SET_FV(RAB_PrefetchSizeOfDMADescriptorsArray, value)
#define RAB_OBResponsePriorityControl_OFFSET		10
#define RAB_OBResponsePriorityControl_MASK		REG_2BIT_MASK
#define RAB_OBResponsePriorityControl(value)		\
	_SET_FV(RAB_OBResponsePriorityControl, value)
#define RAB_MessageDescriptorMemoryEnable		_BV(12)
/* 19.3.13 APB Control & Status Register */
#define RAB_APBKey				_BV(0)
#define RAB_APB_KEY_DEFAULT			0x3A2B
#define RAB_APBPageSelect_OFFSET		16
#ifdef CONFIG_RAB_PAGE_2KB
#define RAB_APBPageSelect_MASK			REG_13BIT_MASK
#endif /* CONFIG_RAB_PAGE_2KB */
#ifdef CONFIG_RAB_PAGE_4KB
#define RAB_APBPageSelect_MASK			REG_12BIT_MASK
#endif /* CONFIG_RAB_PAGE_4KB */
#define RAB_APBPageSelect(value)		\
	_SET_FV(RAB_APBPageSelect, value)
#define RAB_APBGRIOLocked			_BV(29)
#define RAB_APBByteSwappingSelect_OFFSET	30
#define RAB_APBByteSwappingSelect_MASK		REG_2BIT_MASK
#define RAB_APBByteSwappingSelect(value)	\
	_SET_FV(RAB_APBByteSwappingSelect, value)
#define RAB_APB_NO_SWAPPING			0
#define RAB_APB_SWAPPING_WORD			2
#define RAB_APB_SWAPPING_HALF_WORD		3

/* ======================================================================
 * 19.9 AXI PIO Registers (APIO)
 * ====================================================================== */
/* 19.9.1 AXI PIO Control Register */
#define RAB_APIO_Enable				_BV(0)
#define RAB_APIO_MemoryMappingEnable		_BV(1)
#define RAB_APIO_CCPEnable			_BV(2)
/* 19.9.2 AXI PIO Status Register */
#define RAB_APIO_EngineNotEnabled		_BV(0)
#define RAB_APIO_MemoryMappingNotEnabled	_BV(1)
#define RAB_APIO_MaintenanceMappingNotEnabled	_BV(2)
#define RAB_APIO_InvalidAXIAddressMapping	_BV(3)
#define RAB_APIO_RIOResponseError		_BV(4)
#define RAB_APIO_RIOTimeoutError		_BV(5)
#define RAB_APIO_AXIRequestError		_BV(6)

/* ======================================================================
 * 19.10 AXI Address Mapping Registers (APIO_AMAP)
 * ====================================================================== */
/* 19.10.1 AXI Address Mapping Control Register */
#define RAB_AXI_AMAP_Enable			_BV(0)
#define RAB_AXI_AMAP_Type_OFFSET		1
#define RAB_AXI_AMAP_Type_MASK			REG_2BIT_MASK
#define RAB_AXI_AMAP_Type(value)		_SET_FV(RAB_AXI_AMAP_Type, value)
#define RAB_AXI_AMAP_MAINT_READ_WRITE		0
#define RAB_AXI_AMAP_NREAD_NWRITE		1
#define RAB_AXI_AMAP_NREAD_NWRITE_R		2
#define RAB_AXI_AMAP_NREAD_SWRITE		3
#define RAB_AXI_AMAP_Priority_OFFSET		3
#define RAB_AXI_AMAP_Priority_MASK		REG_2BIT_MASK
#define RAB_AXI_AMAP_Priority(value)		_SET_FV(RAB_AXI_AMAP_Priority, value)
#define RAB_AXI_AMAP_PRIORITY_INVALID		0x3
#define RAB_AXI_AMAP_X_Address_OFFSET		5
#define RAB_AXI_AMAP_X_Address_MASK		REG_2BIT_MASK
#define RAB_AXI_AMAP_X_Address(value)		_SET_FV(RAB_AXI_AMAP_X_Address, value)
#define RAB_AXI_AMAP_CRF			_BV(7)
#define RAB_AXI_AMAP_DestinationID_OFFSET	16
#define RAB_AXI_AMAP_DestinationID_MASK		REG_16BIT_MASK
#define RAB_AXI_AMAP_DestinationID(value)	_SET_FV(RAB_AMAP_DestinationID, value)
/* 19.10.2 AXI Address Mapping Size Register */
/* Due to the design of RAM_AXI_AMAP_SIZE register, the maximum NAOPT size
 * supported by RAB is 0x8000000.
 */
#define RAB_AXI_AMAP_SIZE_MAX			ULL(0x80000000)
#define RAB_AXI_AMAP_MASK(size)			ALIGN(size, 0x400)
/* 19.10.3 AXI Address Mapping AXI Base Register
 * 19.10.4 AXI Address Mapping RIO Base Register
 */
#define RAB_AXI_AMAP_BASE(base)			(((uint64_t)(base)) >> 10)
#define RAB_AXI_AMAP_UPPER(base)		(((uint64_t)(base)) >> 34)
/* 19.10.4 AXI Address Mapping RIO Base Register */
#define RAB_AXI_AMAP_Hopcount_OFFSET		14
#define RAB_AXI_AMAP_Hopcount_MASK		REG_8BIT_MASK
#define RAB_AXI_AMAP_Hopcount(value)		_SET_FV(RAB_AXI_AMAP_Hopcount, value)

/* ======================================================================
 * 19.11 AMBA Slave Registers (ASLV)
 * ====================================================================== */
/* 19.11.1 AMBA Slave Status Register */
#define RAB_ASLV_UnsupportedCommand		_BV(0)
#define RAB_ASLV_ErrorLogCommand_OFFSET		1
#define RAB_ASLV_ErrorLogCommand_MASK		REG_24BIT_MASK
#define RAB_ASLV_ErrorLogCommand(value)		_GET_FV(RAB_ASLV_ErrorLogCommand, value)
#define RAB_ASLV_ErrorLogUpperAddress_OFFSET	26
#define RAB_ASLV_ErrorLogUpperAddress_MASK	REG_6BIT_MASK
#define RAB_ASLV_ErrorLogUpperAddress(value)	_GET_FV(RAB_ASLV_ErrorLogUpperAddress, value)

/* ======================================================================
 * 19.12 AMBA Master Registers (AMST)
 * ====================================================================== */

/* IP integration */
/* Table 117: Minimum Frequency of ulk_clk */
#ifdef CONFIG_RAB_SRIO_1_25G
#define RAB_ULI_FREQ_4X		62500
#define RAB_ULI_FREQ_2X		31250
#define RAB_ULI_FREQ_1X		31250
#endif /* CONFIG_RAB_SRIO_1_25G */
#ifdef CONFIG_RAB_SRIO_2_5G
#define RAB_ULI_FREQ_4X		125000
#define RAB_ULI_FREQ_2X		62500
#define RAB_ULI_FREQ_1X		62500
#endif /* CONFIG_RAB_SRIO_2_5G */
#ifdef CONFIG_RAB_SRIO_3_125G
#define RAB_ULI_FREQ_4X		156250
#define RAB_ULI_FREQ_2X		78125
#define RAB_ULI_FREQ_1X		78125
#endif /* CONFIG_RAB_SRIO_3_125G */
#ifdef CONFIG_RAB_SRIO_5G
#define RAB_ULI_FREQ_4X		250000
#define RAB_ULI_FREQ_2X		125000
#define RAB_ULI_FREQ_1X		125000
#endif /* CONFIG_RAB_SRIO_5G */
#ifdef CONFIG_RAB_SRIO_6_25G
#define RAB_ULI_FREQ_4X		312500
#define RAB_ULI_FREQ_2X		156250
#define RAB_ULI_FREQ_1X		156250
#endif /* CONFIG_RAB_SRIO_6_25G */
#ifdef CONFIG_RAB_SRIO_4X
#define RAB_ULI_FREQ		RAB_ULI_FREQ_4X
#endif /* CONFIG_RAB_SRIO_4X */
#ifdef CONFIG_RAB_SRIO_2X
#define RAB_ULI_FREQ		RAB_ULI_FREQ_2X
#endif /* CONFIG_RAB_SRIO_2X */
#ifdef CONFIG_RAB_SRIO_1X
#define RAB_ULI_FREQ		RAB_ULI_FREQ_1X
#endif /* CONFIG_RAB_SRIO_1X */

/* ======================================================================
 * 3.2 APB Register Access
 * ====================================================================== */
void rab_writel(uint32_t value, caddr_t addr);
uint32_t rab_readl(caddr_t addr);
#define rab_setl(v,a)					\
	do {						\
		uint32_t __v = rab_readl(a);		\
		__v |= (v);				\
		rab_writel(__v, (a));			\
	} while (0)
#define rab_clearl(v,a)					\
	do {						\
		uint32_t __v = rab_readl(a);		\
		__v &= ~(v);				\
		rab_writel(__v, (a));			\
	} while (0)
#define rab_writel_mask(v,m,a)				\
	do {						\
		uint32_t __v = rab_readl(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		rab_writel(__v, (a));			\
	} while (0)

/* ======================================================================
 * 3.3 Cooperative Lock CSR Write Access
 * ====================================================================== */

void rab_enable_axi_amap(int apio, int win, uint8_t type, uint8_t prio,
			 caddr_t axi_base, uint64_t axi_size,
			 uint64_t rio_base);

void rab_axi_testcase(void);
void rab_init_port(void);

#endif /* __MOBIVEIL_RAB_H_INCLUDE__ */
