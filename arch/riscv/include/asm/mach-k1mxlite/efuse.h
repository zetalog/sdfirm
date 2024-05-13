#ifndef __EFUSE_K1MXLITE_H_INCLUDE__
#define __EFUSE_K1MXLITE_H_INCLUDE__


#define EFUSE_REG(offset)			(EFUSE_BASE + (offset))
#define EFUSE_IRQ				IRQ_EFUSE

#define EFUSE_CTRL				EFUSE_REG(0x0400)
#define EFUSE_EXTRA				EFUSE_REG(0x0404)
#define EFUSE_EXTRA_WDS				EFUSE_REG(0x0408)
#define EFUSE_LOCK				EFUSE_REG(0x040c)
#define EFUSE_STATUS				EFUSE_REG(0x0410)
#define EFUSE_REPAIR				EFUSE_REG(0x0414)
#define EFUSE_REPAIR_RF				EFUSE_REG(0x0418)
#define EFUSE_REPAIR_I(i)			EFUSE_REG(0x041c + (0x04 * (i)))
#define EFUSE_TEST_WADDR			EFUSE_REG(0x042c)
#define EFUSE_TEST_RADDR			EFUSE_REG(0x0430)
#define EFUSE_TEST_RDATA			EFUSE_REG(0x0434)
#define EFUSE_TIMING(i)				EFUSE_REG(0x0440 + (0x04 * (i)))
#define EFUSE_DEBUG				EFUSE_REG(0x0460)
#define EFUSE_ERR_COUNT				EFUSE_REG(0x0464)
#define EFUSE_ERR_ADDR(i)			EFUSE_REG(0x0470 + (0x08 * (i)))
#define EFUSE_ERR_BIT(i)			EFUSE_REG(0x0474 + (0x08 * (i)))

/* 1 EFUSE_CTRL */
#define EFUSE_CTRL_MARGIN_EN			_BV(12)
#define EFUSE_CTRL_POWER_DOWN_REQ		_BV(8)
#define EFUSE_CTRL_IDEL_REQ			_BV(4)
#define EFUSE_CTRL_A_READ_START			_BV(0)

/* 2 EFUSE_EXTRA */
#define EFUSE_EXTRA_EFUSE_AT_OFFSET		8
#define EFUSE_EXTRA_EFUSE_AT_MASK		REG_2BIT_MASK
#define EFUSE_EXTRA_EFUSE_AT(value)		_SET_FV(EFUSE_EXTRA_EFUSE_AT, value)
#define EFUSE_EXTRA_TEST_MODE			_BV(4)
#define EFUSE_EXTRA_REPAIR_MODE			_BV(0)

/* 3 EFUSE_EXTRA_WDS */
#define EFUSE_EXTRA_WDS_TEST_MODE_DISABLE	_BV(4)
#define EFUSE_EXTRA_WDS_REPAIR_MODE_DISABLE	_BV(0)

/* 4 EFUSE_LOCK */
#define EFUSE_LOCK_STS				_BV(0)

/* 5 EFUSE_STATUS */
#define EFUSE_STATUS_POWER_DOWN			_BV(16)
#define EFUSE_STATUS_IDLE			_BV(12)
#define EFUSE_STATUS_PGM			_BV(8)
#define EFUSE_STATUS_READ			_BV(4)
#define EFUSE_STATUS_BUSY			_BV(0)

/* 6 EFUSE_REPAIR */

/* 7 EFUSE_REPAIR_RF */

/* 8-11 EFUSE_REPAIR_I */

/* 12 EFUSE_TEST_WADDR */

/* 13 EFUSE_TEST_RADDR */

/* 14 EFUSE_TEST_RDATA */

/* 15-19 EFUSE_TIMING */
#define EFUSE_TIMING_TRD_M_OFFSET		24
#define EFUSE_TIMING_TRD_M_MASK			REG_8BIT_MASK
#define EFUSE_TIMING_TRD_M(value)		_SET_FV(EFUSE_TIMING_TRD_M, value)
#define EFUSE_TIMING_TRD_OFFSET			16
#define EFUSE_TIMING_TRD_MASK			REG_8BIT_MASK
#define EFUSE_TIMING_TRD(value)			_SET_FV(EFUSE_TIMING_TRD, value)
#define EFUSE_TIMING_TPGM_OFFSET		0
#define EFUSE_TIMING_TPGM_MASK			REG_12BIT_MASK
#define EFUSE_TIMING_TPGM(value)		_SET_FV(EFUSE_TIMING_TPGM, value)

/* 20 EFUSE_DEBUG */
#define EFUSE_DEBUG_CURR_CNT_OFFSET		16
#define EFUSE_DEBUG_CURR_CNT_MASK		REG_13BIT_MASK
#define EFUSE_DEBUG_CURR_CNT(value)		_SET_FV(EFUSE_DEBUG_CURR_CNT, value)
#define EFUSE_DEBUG_CURR_STATE_OFFSET		0
#define EFUSE_DEBUG_CURR_STATE_MASK		REG_5BIT_MASK
#define EFUSE_DEBUG_CURR_STATE			_SET_FV(EFUSE_DEBUG_CURR_STATE, value)

/* 21 EFUSE_ERR_COUNT */

/* 22-37 EFUSE_ERR_ADDR EFUSE_ERR_BIT */

#define efuse_write(addr, value)							\
	do {										\
		while ((!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_IDLE)) && 		\
			(!(__raw_readl(EFUSE_EXTRA) & EFUSE_EXTRA_REPAIR_MODE)) && 	\
			(!(__raw_readl(EFUSE_EXTRA) & EFUSE_EXTRA_TEST_MODE)) &&	\
			(!(__raw_readl(EFUSE_LOCK) & EFUSE_LOCK_STS)));			\
		__raw_writel((addr), (value));						\
		while((!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_PGM)) || 		\
			(!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_BUSY)));		\
	} while (0)

static inline uint32_t efuse_read(uint16_t addr, uint16_t mode)
{
	uint32_t v;
	while ((!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_IDLE)) &&
		(!(__raw_readl(EFUSE_EXTRA) & EFUSE_EXTRA_REPAIR_MODE)) && 
		(!(__raw_readl(EFUSE_EXTRA) & EFUSE_EXTRA_TEST_MODE)));
	__raw_writel(EFUSE_CTRL_MARGIN_EN | EFUSE_CTRL, (mode));
	__raw_setl(EFUSE_CTRL_A_READ_START, EFUSE_CTRL);
	while ((!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_READ)) || 
		(!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_BUSY)));
	v = __raw_readl(addr);
	return v;
}

#define efuse_read_repair_rf()			__raw_readl(EFUSE_REPAIR_RF)

#define efuse_repair_prepare()								\
	do {										\
		while (!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_IDLE));		\
		__raw_setl(EFUSE_EXTRA_REPAIR_MODE, EFUSE_EXTRA);			\
	} while (0)

#define efuse_repair_single(addr)		__raw_writel(EFUSE_REPAIR, (addr))

#define efuse_repair_finish()			__raw_clearl(EFUSE_EXTRA_REPAIR_MODE, EFUSE_EXTRA);

static inline uint32_t efuse_test_func(uint16_t waddr, uint16_t raddr, uint16_t mode)
{
	uint32_t v;
	while (!(__raw_readl(EFUSE_STATUS) & EFUSE_STATUS_IDLE));
	__raw_setl(EFUSE_EXTRA_TEST_MODE, EFUSE_EXTRA);
	__raw_writel_mask(EFUSE_EXTRA_EFUSE_AT(mode), EFUSE_EXTRA_EFUSE_AT(EFUSE_EXTRA_EFUSE_AT_MASK), EFUSE_EXTRA);
	__raw_writel(EFUSE_TEST_WADDR, (waddr));
	__raw_writel(EFUSE_TEST_RADDR, (raddr));
	__raw_clearl(EFUSE_EXTRA_TEST_MODE, EFUSE_EXTRA);
}