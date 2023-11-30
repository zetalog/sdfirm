#ifndef __BMU_K1MATRIX_H_INCLUDE__
#define __BMU_K1MATRIX_H_INCLUDE__

#define BMU_REG(n, offset)		(BMU_BASE(n) + (offset))

#define BMU_INTR_MASK(n)		BMU_REG(n, 0x00)
#define BMU_INTR_STAT(n)		BMU_REG(n, 0x04)
#define BMU_INTR_STAT_MASK(n)		BMU_REG(n, 0x08)

#define BMU_CTRL_X(n)			BMU_REG(n, 0x0C)
#define BMU_M_INTERNAL(n)		BMU_REG(n, 0x10)

#define BMU_OVER_TIME_THR(n)		BMU_REG(n, 0x14)
#define BMU_RD_OVTIME_ADDRL(n)		BMU_REG(n, 0x18)
#define BMU_RD_OVTIME_ADDRH(n)		BMU_REG(n, 0x1C)
#define BMU_RD_OVTIME_ADDR_VLD(n)	BMU_REG(n, 0x20)
#define BMU_RD_OVTIME_VECTOR(n, i)	BMU_REG(n, 0x24 + ((i) << 2))
#define BMU_WR_OVTIME_ADDRL(n)		BMU_REG(n, 0x34)
#define BMU_WR_OVTIME_ADDRH(n)		BMU_REG(n, 0x34)
#define BMU_WR_OVTIME_ADDR_VLD(n)	BMU_REG(n, 0x3C)
#define BMU_WR_OVTIME_VECTOR(n, i)	BMU_REG(n, 0x40 + ((i) << 2))

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

/* error responce */
#define BMU_RD_ERROR_RES(n)		BMU_REG(n, 0x78)
#define BMU_WR_ERROR_RES(n)		BMU_REG(n, 0x7C)
#define BMU_SLVERR			0
#define BMU_DECERR			1

#define BMU_DATA_HIT_DATA(n, i)		BMU_REG(n, 0x80 + ((i) << 2)
#define BMU_DATA_HIT_MASK(n)		BMU_REG(n, 0x90)
#define BMU_TGT_HIT_ADDRL(n)		BMU_REG(n, 0x94)
#define BMU_TGT_HIT_ADDRH(n)		BMU_REG(n, 0x98)
#define BMU_ADDR_HIT_ADDR_MASKL(n)	BMU_REG(n, 0x9C)
#define BMU_ADDR_HIT_ADDR_MASKH(n)	BMU_REG(n, 0xA0)

/* counter 0-9 supports ID filter */
#define BMU_RD_ID_FILTER(n)		BMU_REG(n, 0xA4)
#define BMU_RD_ID_FILTER_MASK(n)	BMU_REG(n, 0xA8)
#define BMU_WR_ID_FILTER(n)		BMU_REG(n, 0xAC)
#define BMU_WR_ID_FILTER_MASK(n)	BMU_REG(n, 0xB0)

/* counter 8-9 supports address range filter */
#define BMU_FILT_UP_ADDRL(n, i)		BMU_REG(n, 0xB4 + (0x24 * ((i) - 8)))
#define BMU_FILT_UP_ADDRH(n, i)		BMU_REG(n, 0xB8 + (0x24 * ((i) - 8)))
#define BMU_FILT_LOW_ADDRL(n, i)	BMU_REG(n, 0xBC + (0x24 * ((i) - 8)))
#define BMU_FILT_LOW_ADDRH(n, i)	BMU_REG(n, 0xC0 + (0x24 * ((i) - 8)))

/* counter 8-9 supports burst length */
#define BMU_FILT_TARGET_LEN(n)		BMU_REG(n, 0xD4 + (0x24 * ((i) - 8)))

/* counter 8-9 supports address alignment filter */
#define BMU_RD_FILT_TGT_ADDRL(n)	BMU_REG(n, 0xC4)
#define BMU_RD_FILT_TGT_ADDRH(n)	BMU_REG(n, 0xC8)
#define BMU_RD_FILT_ADDR_MASKL(n)	BMU_REG(n, 0xCC)
#define BMU_RD_FILT_ADDR_MASKH(n)	BMU_REG(n, 0xD0)
#define BMU_WR_FILT_TGT_ADDRL(n)	BMU_REG(n, 0xE8)
#define BMU_WR_FILT_TGT_ADDRH(n)	BMU_REG(n, 0xEC)
#define BMU_WR_FILT_ADDR_MASKL(n)	BMU_REG(n, 0xF0)
#define BMU_WR_FILT_ADDR_MASKH(n)	BMU_REG(n, 0xF4)

#define BMU_RD_DURATION_THR(n)		BMU_REG(n, 0xFC)
#define BMU_WR_DURATION_THR(n)		BMU_REG(n, 0x100)
#define BMU_ARREADY_TMO_THR(n)		BMU_REG(n, 0x104)
#define BMU_AWREADY_TMO_THR(n)		BMU_REG(n, 0x108)

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
/* RD_ERROR_RES */
#define bmu_rd_err_res_rresp_OFFSET	0
#define bmu_rd_err_res_rresp_MASK	REG_2BIT_MASK
#define bmu_rd_err_res_rresp(value)	_GET_FV(bmu_rd_err_res_rresp, value)
/* WR_ERROR_RES */
#define bmu_wr_err_res_rresp_OFFSET	0
#define bmu_wr_err_res_rresp_MASK	REG_2BIT_MASK
#define bmu_wr_err_res_rresp(value)	_GET_FV(bmu_wr_err_res_rresp, value)

#define bmu_ctrl_start(n, once)				\
	do {						\
		__raw_setl_mask(bmu_start | (once) ?	\
				bmu_start_once : 0,	\
				bmu_start |		\
				bmu_start_once,		\
				BMU_CTRL_X(n));		\
	} while (0)
#define bmu_ctrl_stop(n)		\
	__raw_clearl(bmu_start | bmu_start_once, BMU_CTRL_X(n))

#define bmu_mask_irq(n, irq)		__raw_setl(irq, BMU_INTR_STAT_MASK(n))
#define bmu_unmask_irq(n, irq)		__raw_clearl(irq, BMU_INTR_STAT_MASK(n))
#define bmu_unmask_all_irqs(n)		__raw_setl(0xffffffff, BMU_INTR_STAT_MASK(n))

#define bmu_status_clr(n)		\
	__raw_setl(bmu_status_req_clr, BMU_CTRL_X(n))

#define bmu_read_counter(n, c)		__raw_readl(BMU_COUNTER(n, c))

/* address range filter */
#define bmu_config_address_range(n, c, lower, upper)	\
	do {						\
		__raw_writel(LODWORD(lower),		\
			     BMU_FILT_LOW_ADDRL(n, c);	\
		__raw_writel(HIDWORD(lower),		\
			     BMU_FILT_LOW_ADDRH(n, c);	\
		__raw_writel(LODWORD(upper),		\
			     BMU_FILT_UP_ADDRL(n, c);	\
		__raw_writel(HIDWORD(upper),		\
			     BMU_FILT_UP_ADDRH(n, c);	\
	} while (0)
#define bmu_filter_address_range(n, c)			\
	__raw_setl(19 - (c) * 2, BMU_CTRL_X(n))
#define bmu_unfilter_address_range(n, c)		\
	__raw_clearl(19 - (c) * 2, BMU_CTRL_X(n))
/* target length filter */
#define bmu_config_target_length(n, c, len)		\
	__raw_writel(len, BMU_FILT_TARGET_LEN(n, c))
#define bmu_filter_target_length(n, c)			\
	__raw_setl(20 - (c) * 2, BMU_CTRL_X(n))
#define bmu_unfilter_target_length(n, c)		\
	__raw_clearl(20 - (c) * 2, BMU_CTRL_X(n))
/* data/addr match */
#define bmu_match_data(n)		__raw_setl(bmu_data_match, BMU_CTRL_X(n))
#define bmu_unmatch_data(n)		__raw_clearl(bmu_data_match, BMU_CTRL_X(n))
#define bmu_match_addr(n)		__raw_setl(bmu_addr_match, BMU_CTRL_X(n))
#define bmu_unmatch_addr(n)		__raw_clearl(bmu_addr_match, BMU_CTRL_X(n))

#endif /* __BMU_K1MATRIX_H_INCLUDE__ */
