/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)bmu.h: K1MXLite bus monitor unit (BMU) implementation
 * $Id: bmu.h,v 1.279 2023-12-01 14:19:18 zhenglv Exp $
 */

#ifndef __BMU_K1MXLITE_H_INCLUDE__
#define __BMU_K1MXLITE_H_INCLUDE__

#include <target/amba.h>
#include <target/barrier.h>

#define NR_BMUS				1

#define BMU_REG(n, offset)		(BMU_BASE(n) + (offset))

#define BMU_IRQ				IRQ_BMU

#define BMU_INTR_MASK(n)		BMU_REG(n, 0x00)
#define BMU_INTR_STAT(n)		BMU_REG(n, 0x04)
#define BMU_INTR_STAT_MASK(n)		BMU_REG(n, 0x08)

#define BMU_CTRL_X(n)			BMU_REG(n, 0x0C)
#define BMU_M_INTERNAL(n)		BMU_REG(n, 0x10)

/* Counters */
#define BMU_COUNTER(n, c)		BMU_REG(n, 0x50 + ((c) << 2))
#define BMU_RD_TRAN_CNT			0
#define BMU_RD_DATA_CNT			1
#define BMU_RD_DURATION_CNT		2
#define BMU_RD_DRT_OVTHR_CNT		3
#define BMU_WR_TRAN_CNT			4
#define BMU_WR_DATA_CNT			5
#define BMU_WR_DURATION_CNT		6
#define BMU_WR_DRT_OVTHR_CNT		7
#define BMU_FILT_RD_TRAN_CNT		8
#define BMU_FILT_WR_TRAN_CNT		9

/* Counter 0-9 supports ID filter */
#define BMU_RD_ID_FILTER(n)		BMU_REG(n, 0xA4)
#define BMU_RD_ID_FILTER_MASK(n)	BMU_REG(n, 0xA8)
#define BMU_WR_ID_FILTER(n)		BMU_REG(n, 0xAC)
#define BMU_WR_ID_FILTER_MASK(n)	BMU_REG(n, 0xB0)

/* Counter 8-9 supports address range filter */
#define BMU_FILT_UP_ADDRL(n, i)		BMU_REG(n, 0xB4 + (0x24 * ((i) - 8)))
#define BMU_FILT_UP_ADDRH(n, i)		BMU_REG(n, 0xB8 + (0x24 * ((i) - 8)))
#define BMU_FILT_LOW_ADDRL(n, i)	BMU_REG(n, 0xBC + (0x24 * ((i) - 8)))
#define BMU_FILT_LOW_ADDRH(n, i)	BMU_REG(n, 0xC0 + (0x24 * ((i) - 8)))

/* Counter 8-9 supports burst length */
#define BMU_FILT_TARGET_LEN(n, i)	BMU_REG(n, 0xD4 + (0x24 * ((i) - 8)))

/* Counter 8-9 supports address alignment filter */
#define BMU_FILT_TGT_ADDRL(n, i)	BMU_REG(n, 0xC4 + (0x24 * ((i) - 8)))
#define BMU_FILT_TGT_ADDRH(n, i)	BMU_REG(n, 0xC8 + (0x24 * ((i) - 8)))
#define BMU_FILT_ADDR_MASKL(n, i)	BMU_REG(n, 0xCC + (0x24 * ((i) - 8)))
#define BMU_FILT_ADDR_MASKH(n, i)	BMU_REG(n, 0xD0 + (0x24 * ((i) - 8)))

/* Data/addr match */
#define BMU_DATA_HIT_DATA(n, i)		BMU_REG(n, 0x80 + ((i) << 2))
#define BMU_DATA_HIT_MASK(n)		BMU_REG(n, 0x90)
#define BMU_ADDR_HIT_ADDRL(n)		BMU_REG(n, 0x94) /* TGT_HIT_ADDRL */
#define BMU_ADDR_HIT_ADDRH(n)		BMU_REG(n, 0x98) /* TGT_HIT_ADDRH */
#define BMU_ADDR_HIT_MASKL(n)		BMU_REG(n, 0x9C) /* ADDR_HIT_ADDR_MASKL */
#define BMU_ADDR_HIT_MASKH(n)		BMU_REG(n, 0xA0) /* ADDR_HIT_ADDR_MASKH */

/* error response:
 *  AXI_RESP_SLVERR
 *  AXI_RESP_DECERR
 */
#define BMU_RD_ERROR_RES(n)		BMU_REG(n, 0x78)
#define BMU_WR_ERROR_RES(n)		BMU_REG(n, 0x7C)

/* AWREADY/ARREADY overtime threshold */
#define BMU_ARREADY_TMO_THR(n)		BMU_REG(n, 0x104)
#define BMU_AWREADY_TMO_THR(n)		BMU_REG(n, 0x108)

/* response overtime threshold */
#define BMU_OVER_TIME_THR(n)		BMU_REG(n, 0x14)

/* duration counter */
#define BMU_RD_DURATION_THR(n)		BMU_REG(n, 0xFC)
#define BMU_WR_DURATION_THR(n)		BMU_REG(n, 0x100)

#define BMU_RD_OVTIME_ADDRL(n)		BMU_REG(n, 0x18)
#define BMU_RD_OVTIME_ADDRH(n)		BMU_REG(n, 0x1C)
#define BMU_RD_OVTIME_ADDR_VLD(n)	BMU_REG(n, 0x20)
#define BMU_RD_OVTIME_VECTOR(n, i)	BMU_REG(n, 0x24 + ((i) << 2))
#define BMU_WR_OVTIME_ADDRL(n)		BMU_REG(n, 0x34)
#define BMU_WR_OVTIME_ADDRH(n)		BMU_REG(n, 0x34)
#define BMU_WR_OVTIME_ADDR_VLD(n)	BMU_REG(n, 0x3C)
#define BMU_WR_OVTIME_VECTOR(n, i)	BMU_REG(n, 0x40 + ((i) << 2))

#define BMU_RSV_RW_REG0(n)		BMU_REG(n, 0x10C)
#define BMU_RSV_RW_REG1(n)		BMU_REG(n, 0x110)

/* INTR_MASK */
#define bmu_intr_mask			_BV(0)
/* INTR_STAT/INTR_STAT_MASK */
#define bmu_wr_res_time_out_int		_BV(0)
#define bmu_rd_res_time_out_int		_BV(1)
#define bmu_wr_data_hit_int		_BV(2)
#define bmu_rd_data_hit_int		_BV(3)
#define bmu_wr_addr_hit_int		_BV(4)
#define bmu_rd_addr_hit_int		_BV(5)
#define bmu_wr_error_res_int		_BV(6)
#define bmu_rd_error_res_int		_BV(7)
#define bmu_task_done_int		_BV(8)
#define bmu_awready_tmo_int		_BV(9)
#define bmu_arready_tmo_int		_BV(10)
/* BMU_CTRL_X */
#define bmu_start			_BV(0)
#define bmu_status_req_clr		_BV(1)
#define bmu_wr_filter_target_length_en	_BV(2)
#define bmu_wr_filter_address_range_en	_BV(3)
#define bmu_rd_filter_target_length_en	_BV(4)
#define bmu_rd_filter_address_range_en	_BV(5)
#define bmu_monitoring_mode		_BV(6)
#define bmu_start_once			_BV(7)
#define bmu_addr_match_en		_BV(8)
#define bmu_data_match_en		_BV(9)
/* RD_ERROR_RES/WR_ERROR_RES */
#define bmu_err_res_rresp_OFFSET	0
#define bmu_err_res_rresp_MASK		REG_2BIT_MASK
#define bmu_err_res_rresp(value)	_GET_FV(bmu_err_res_rresp, value)

#define bmu_ctrl_start(n, once)						\
	do {								\
		__raw_writel_mask(bmu_start |				\
				  (once) ? bmu_start_once : 0,		\
				  bmu_start | bmu_start_once,		\
				  BMU_CTRL_X(n));			\
	} while (0)
#define bmu_ctrl_stop(n)						\
	__raw_clearl(bmu_start | bmu_start_once, BMU_CTRL_X(n))
#define bmu_ctrl_enter_monitor(n)					\
	__raw_setl(bmu_monitoring_mode, BMU_CTRL_X(n))
#define bmu_ctrl_leave_monitor(n)					\
	__raw_clearl(bmu_monitoring_mode, BMU_CTRL_X(n))

#define bmu_set_internal(n, val)		__raw_writel((val), BMU_M_INTERNAL(n))

#define bmu_mask_irq(n, irq)		__raw_setl(irq, BMU_INTR_STAT_MASK(n))
#define bmu_unmask_irq(n, irq)		__raw_clearl(irq, BMU_INTR_STAT_MASK(n))
#define bmu_mask_all_irqs(n)						\
	do {								\
		__raw_setl(0xffffffff, BMU_INTR_STAT_MASK(n));		\
		__raw_setl(0xffffffff, BMU_INTR_MASK(n));		\
	} while (0)

#define bmu_status_clr(n)		__raw_setl(bmu_status_req_clr, BMU_CTRL_X(n))

/* ===========================================================================
 * Filters
 * =========================================================================== */
/* target id filter */
#define bmu_config_target_id(n, rdwr, id, msk)				\
	do {								\
		__raw_writel((id), BMU_##rdwr##_ID_FILTER(n));		\
		__raw_writel((msk), BMU_##rdwr##_ID_FILTER_MASK(n));	\
	} while (0)
/* address range filter */
#define bmu_config_address_range(n, c, lower, upper)			\
	do {								\
		__raw_writel(LODWORD(lower), BMU_FILT_LOW_ADDRL(n, c));	\
		__raw_writel(HIDWORD(lower), BMU_FILT_LOW_ADDRH(n, c));	\
		__raw_writel(LODWORD(upper), BMU_FILT_UP_ADDRL(n, c));	\
		__raw_writel(HIDWORD(upper), BMU_FILT_UP_ADDRH(n, c));	\
	} while (0)
#define bmu_filter_address_range(n, c)					\
	__raw_setl(19 - (c) * 2, BMU_CTRL_X(n))
#define bmu_unfilter_address_range(n, c)				\
	__raw_clearl(19 - (c) * 2, BMU_CTRL_X(n))
/* address alignment filter */
#define bmu_filter_address_align(n, c, addr, mask)			\
	do {								\
		__raw_writel(LODWORD(addr), BMU_FILT_TGT_ADDRL(n, c));	\
		__raw_writel(HIDWORD(addr), BMU_FILT_TGT_ADDRH(n, c));	\
		__raw_writel(LODWORD(mask), BMU_FILT_ADDR_MASKL(n, c));	\
		__raw_writel(HIDWORD(addr), BMU_FILT_ADDR_MASKH(n, c));	\
	} while (0)
/* target length filter */
#define bmu_config_target_length(n, c, len)				\
	__raw_writel(len, BMU_FILT_TARGET_LEN(n, c))
#define bmu_filter_target_length(n, c)					\
	__raw_setl(20 - (c) * 2, BMU_CTRL_X(n))
#define bmu_unfilter_target_length(n, c)				\
	__raw_clearl(20 - (c) * 2, BMU_CTRL_X(n))

/* ===========================================================================
 * Counters
 * =========================================================================== */
#define bmu_read_counter(n, c)		__raw_readl(BMU_COUNTER(n, c))

/* read/write duration counter */
#define bmu_config_duration_threshold(n, rdwr, thr)			\
	__raw_writel((thr), BMU_##rdwr##_DURATION_THR(n))
#define bmu_vector_size(n, rdwr, nr_vects)				\
	do {								\
		nr_vects = 0;						\
		nr_vects += hweight32(					\
			__raw_readl(BMU_##rdwr##_OVTIME_VECTOR(n, 0)));	\
		nr_vects += hweight32(					\
			__raw_readl(BMU_##rdwr##_OVTIME_VECTOR(n, 1)));	\
		nr_vects += hweight32(					\
			__raw_readl(BMU_##rdwr##_OVTIME_VECTOR(n, 2)));	\
		nr_vects += hweight32(					\
			__raw_readl(BMU_##rdwr##_OVTIME_VECTOR(n, 3)));	\
	} while (0)
#define bmu_vector_next(n, rdwr, addr)					\
	do {								\
		uint32_t addrl, addrh;					\
		while (!__raw_readl(BMU_##rdwr##_OVTIME_ADDR_VLD(n)));	\
		addrl = __raw_readl(BMU_##rdwr##_OVTIME_ADDRL(n));	\
		rmb();							\
		addrh = __raw_readl(BMU_##rdwr##_OVTIME_ADDRH(n));	\
		addr = MAKELLONG(addrl, addrh);				\
	} while (0)

/* ===========================================================================
 * Debugging
 * =========================================================================== */
/* data match */
#define bmu_config_data_match(n, data_array, data_mask)			\
	do {								\
		for (int __i = 0; __i < 4; __i++) {			\
			__raw_writel((data_array)[__i],			\
				     BMU_DATA_HIT_DATA(n, __i));	\
		}							\
		__raw_writel((data_mask), BMU_DATA_HIT_MASK(n));	\
	} while (0)
#define bmu_enable_data_match(n)	__raw_setl(bmu_data_match, BMU_CTRL_X(n))
#define bmu_disable_data_match(n)	__raw_clearl(bmu_data_match, BMU_CTRL_X(n))
/* addr match */
#define bmu_config_addr_match(n, addr, mask)				\
	do {								\
		__raw_writel(LODWORD(addr), BMU_ADDR_HIT_ADDRL(n));	\
		__raw_writel(HIDWORD(addr), BMU_ADDR_HIT_ADDRH(n));	\
		__raw_writel(LODWORD(mask), BMU_ADDR_HIT_MASKL(n));	\
		__raw_writel(HIDWORD(mask), BMU_ADDR_HIT_MASKH(n));	\
	} while (0)
#define bmu_enable_addr_match(n)	__raw_setl(bmu_addr_match, BMU_CTRL_X(n))
#define bmu_disable_addr_match(n)	__raw_clearl(bmu_addr_match, BMU_CTRL_X(n))
/* response timeout threshold */
#define bmu_config_ovtime_threshold(n, thr)				\
	__raw_writel((thr), BMU_OVER_TIME_THR(n))
/* error resp */
#define bmu_error_res(n, rdwr)						\
	bmu_err_res_rresp(__raw_readl(BMU_##rdwr##_ERROR_RES(n)))
/* awready/arready timeout threshold */
#define bmu_config_awready_threshold(n, thr)				\
	__raw_writel((thr), BMU_AWREADY_TMO_THR(n))
#define bmu_config_arready_threshold(n, thr)				\
	__raw_writel((thr), BMU_ARREADY_TMO_THR(n))

#endif /* __BMU_K1MXLITE_H_INCLUDE__ */
