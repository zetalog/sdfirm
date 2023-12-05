/* Reference,  https, //spacemit.feishu.cn/wiki/I5OBwa11Ei7Ycrk5QdMcEvrdndg */

#ifndef __SPACEMIT_LPC_H_INCLUDE__
#define __SPACEMIT_LPC_H_INCLUDE__

#ifndef SPACEMIT_LPC_BASE
#define SPACEMIT_LPC_BASE			0x02010000
#endif
#ifndef SPACEMIT_LPC_REG
#define SPACEMIT_LPC_REG(offset)		(SPACEMIT_LPC_BASE + (offset))
#endif

/* Register offset */
#define LPC_CFG 				SPACEMIT_LPC_REG(0x00)
#define LPC_CMD_OP				SPACEMIT_LPC_REG(0x04)
#define LPC_STATUS				SPACEMIT_LPC_REG(0x08)
#define LPC_INT_MASK				SPACEMIT_LPC_REG(0x0c)
#define LPC_INT_STATUS  			SPACEMIT_LPC_REG(0x10)
#define LPC_INT_RAW_STATUS			SPACEMIT_LPC_REG(0x14)
#define LPC_INT_CLR				SPACEMIT_LPC_REG(0x18)
#define LPC_WAIT_COUNT				SPACEMIT_LPC_REG(0x1c)
#define LPC_ADDR				SPACEMIT_LPC_REG(0x20)
#define LPC_WDATA				SPACEMIT_LPC_REG(0x24)
#define LPC_RDATA				SPACEMIT_LPC_REG(0x28)
#define LPC_DEBUG				SPACEMIT_LPC_REG(0x2c)
#define SERIRQ_CFG				SPACEMIT_LPC_REG(0x30)
#define SERIRQ_OP				SPACEMIT_LPC_REG(0x34)
#define SERIRQ_SLOT_MASK			SPACEMIT_LPC_REG(0x38)
#define SERIRQ_SLOT_IRQ				SPACEMIT_LPC_REG(0x3c)
#define SERIRQ_SLOT_CLR				SPACEMIT_LPC_REG(0x40)
#define SERIRQ_DEBUG				SPACEMIT_LPC_REG(0x44)
#define LPC_MEM_CFG				SPACEMIT_LPC_REG(0x50)
#define LPC_ERR_ADDR				SPACEMIT_LPC_REG(0x54)

/* 8.1 LPC_CFG */
#define LPC_CFG_SERIRQ_EN			_BV(16)
#define LPC_CFG_LONG_WAIT_EN			_BV(12)
#define LPC_CFG_FW_BYTE_LEN_OFFSET		8
#define LPC_CFG_FW_BYTE_LEN_MASK		REG_2BIT_MASK
#define LPC_CFG_FW_BYTE_LEN(value)		_SET_FV(LPC_CFG_FW_BYTE_LEN, value)
#define LPC_CFG_CYCLE_TYPE_OFFSET		4
#define LPC_CFG_CYCLE_TYPE_MASK			REG_2BIT_MASK
#define LPC_CFG_CYCLE_TYPE(value)		_SET_FV(LPC_CFG_CYCLE_TYPE, value)
#define LPC_CFG_LFRAME_WIDE_OFFSET		0
#define LPC_CFG_LFRAME_WIDE_MASK		REG_2BIT_MASK
#define LPC_CFG_LFRAME_WIDE(value)		_SET_FV(LPC_CFG_LFRAME_WIDE, value)

/* 8.2 LPC_CMD_OP */
#define LPC_CMD_OP				_BV(0)

/* 8.3 LPC_STATUS */
#define LPC_STATUS_SERIRQ_BUSY			_BV(4)
#define LPC_STATUS_LPC_BUSY			_BV(0)

/* 8.4 LPC_INT_MASK */
#define LPC_INT_SYNC_ERR_MASK			_BV(24)
#define LPC_INT_NO_SYNC_MASK			_BV(20)
#define LPC_INT_LWAIT_TIMEOUT_MASK		_BV(16)
#define LPC_INT_SWAIT_TIMEOUT_MASK		_BV(12)
#define LPC_INT_SERIRQ_INT_MASK			_BV(8)
#define LPC_INT_SERIRQ_DONE_MASK		_BV(4)
#define LPC_INT_OP_DONE_MASK			_BV(0)

/* 8.5 LPC_INT_STATUS */
#define LPC_INT_STATUS_SYNC_ERR			_BV(24)
#define LPC_INT_STATUS_NO_SYNC			_BV(20)
#define LPC_INT_STATUS_LWAIT_TIMEOUT		_BV(16)
#define LPC_INT_STATUS_SWAIT_TIMEOUT		_BV(12)
#define LPC_INT_STATUS_SERIRQ_INT		_BV(8)
#define LPC_INT_STATUS_SERIRQ_DONE		_BV(4)
#define LPC_INT_STATUS_OP_DONE			_BV(0)

/* 8.6 LPC_INT_RAW_STATUS */
#define LPC_INT_RAW_STATUS_SYNCEER		_BV(24)
#define LPC_INT_RAW_STATUS_NO_SYNC		_BV(20)
#define LPC_INT_RAW_STATUS_LWAIT_TIMEOUT	_BV(16)
#define LPC_INT_RAW_STATUS_SWAIT_TIMEOUT	_BV(12)
#define LPC_INT_RAW_STATUS_SERIRQ_INT		_BV(8)
#define LPC_INT_RAW_STATUS_SERIRQ_DONE		_BV(4)
#define LPC_INT_RAW_STATUS_OP_DONE		_BV(0)

/* 8.7 LPC_INT_CLR */
#define LPC_INT_SYNCERR_CLR			_BV(24)
#define LPC_INT_NO_SYNC_CLR			_BV(20)
#define LPC_INT_LWAIT_TIMEOUT_CLR		_BV(16)
#define LPC_INT_SWAIT_TIMEOUT_CLR		_BV(12)
#define LPC_INT_SERIRQ_DONE_CLR			_BV(4)
#define LPC_INT_OP_DONE_CLR			_BV(0)

/* 8.8 LPC_WAIT_COUNT */
#define LPC_WAIT_ABORT_COUNT_OFFSET		24
#define LPC_WAIT_ABORT_COUNT_MASK		REG_4BIT_MASK
#define LPC_WAIT_ABORT_COUNT(value)		_SET_FV(LPC_WAIT_ABORT_COUNT, value)
#define LPC_WAIT_SWAIT_COUNT_OFFSET		20
#define LPC_WAIT_SWAIT_COUNT_MASK		REG_4BIT_MASK
#define LPC_WAIT_SWAIT_COUNT(value)		_SET_FV(LPC_WAIT_SWAIT_COUNT, value)
#define LPC_WAIT_LWAIT_COUNT_OFFSET		0
#define LPC_WAIT_LWAIT_COUNT_MASK		REG_16BIT_MASK
#define LPC_WAIT_LWAIT_COUNT(value)		_SET_FV(LPC_WAIT_LWAIT_COUNT, value)

/* 8.9 LPC_ADDR */
#define LPC_ADDR_REG_OFFSET			0
#define LPC_ADDR_REG_MASK			REG_32BIT_MASK
#define LPC_ADDR_REG(value)			_SET_FV(LPC_ADDR_REG, value)

/* 8.10 LPC_WDATA */
#define LPC_WDATA_REG_OFFSET			0
#define LPC_WDATA_REG_MASK			REG_32BIT_MASK
#define LPC_WDATA_REG(value)			_SET_FV(LPC_WDATA_REG, value)

/* 8.11 LPC_RDATA */
#define LPC_RDATA_REG_OFFSET			0
#define LPC_RDATA_REG_MASK			REG_32BIT_MASK
#define LPC_RDATA_REG(value)			_GET_FV(LPC_RDATA_REG, value)

/* 8.12 LPC_DEBUG */
#define LPC_DEBUG_LAD_I_OFFSET			28
#define LPC_DEBUG_LAD_I_MASK			REG_4BIT_MASK
#define LPC_DEBUG_LAD_I(value)			_GET_FV(LPC_DEBUG_LAD_I, value)
#define LPC_DEBUG_LPC_COUNT_OFFSET		24
#define LPC_DEBUG_LPC_COUNT_MASK		REG_4BIT_MASK
#define LPC_DEBUG_LPC_COUNT(value)		_GET_FV(LPC_DEBUG_LPC_COUNT, value)
#define LPC_DEBUG_LPC_SYNC_COUNT_OFFSET		8
#define LPC_DEBUG_LPC_SYNC_COUNT_MASK		REG_16BIT_MASK
#define LPC_DEBUG_LPC_SYNC_COUNT(value)		_GET_FV(LPC_DEBUG_LPC_SYNC_COUNT, value)
#define LPC_DEBUG_LPC_FSM_OFFSET		0
#define LPC_DEBUG_LPC_FSM_MASK			REG_6BIT_MASK
#define LPC_DEBUG_LPC_FSM(value)		_GET_FV(LPC_DEBUG_LPC_FSM, value)

/* 8.13 SEIRQ_CFG */
#define SEIRQ_CFG_SERIRQ_NUM_OFFSET		12
#define SEIRQ_CFG_SERIRQ_NUM_MASK		REG_4BIT_MASK
#define SEIRQ_CFG_SERIRQ_NUM(value)		_SET_FV(SEIRQ_CFG_SERIRQ_NUM, value)
#define SEIRQ_CFG_SERIRQ_IDLE_WIDE_OFFSET	8
#define SEIRQ_CFG_SERIRQ_IDLE_WIDE_MASK		REG_2BIT_MASK
#define SEIRQ_CFG_SERIRQ_IDLE_WIDE(value)	_SET_FV(SEIRQ_CFG_SERIRQ_NUM, value)
#define SEIRQ_CFG_SERIRQ_START_WIDE_OFFSET	4
#define SEIRQ_CFG_SERIRQ_START_WIDE_MASK	REG_2BIT_MASK
#define SEIRQ_CFG_SERIRQ_START_WIDE(value)	_SET_FV(SEIRQ_CFG_SERIRQ_NUM, value)
#define SEIRQ_CFG_SERIRQ_MODE			_BV(0)

/* 8.14 SERIRQ_OP */
#define SERIRQ_OP_REG				_BV(0)

/* 8.15 SERIRQ_SLOT_MASK */
#define SERIRQ_SLOT_MASK_REG_OFFSET		REG32_1BIT_OFFSET(n)
#define SERIRQ_SLOT_MASK_REG_MASK		REG_1BIT_MASK
#define SERIRQ_SLOT_MASK_REG(n, value)		_SET_FVn(n, SERIRQ_SLOT_MASK_REG, value)

/* 8.16 SERIRQ_SLOT_IRQ */
#define SERIRQ_SLOT_IRQ_REG_OFFSET		REG32_1BIT_OFFSET(n)
#define SERIRQ_SLOT_IRQ_REG_MASK		REG_1BIT_MASK
#define SERIRQ_SLOT_IRQ_REG(n, value)		_GET_FVn(n, SERIRQ_SLOT_IRQ_REG, value)

/* 8.17 SERIRQ_SLOT_CLR */
#define SERIRQ_SLOT_CLR_REG_OFFSET		REG32_1BIT_OFFSET(n)
#define SERIRQ_SLOT_CLR_REG_MASK		REG_1BIT_MASK
#define SERIRQ_SLOT_CLR_REG(n, value)		_SET_FVn(n, SERIRQ_SLOT_CLR_REG, value)

/* 8.18 SERIRQ_DEBUG */
#define SERIRQ_DEBUG_SERIRQ_I			_BV(29)
#define SERIRQ_DEBUG_CURR_SERIRQ_MODE		_BV(28)
#define SERIRQ_DEBUG_STOP_COUNT_OFFSET		24
#define SERIRQ_DEBUG_STOP_COUNT_MASK		REG_2BIT_MASK
#define SERIRQ_DEBUG_STOP_COUNT(value)		_GET_FV(SERIRQ_DEBUG_STOP_COUNT, value)
#define SERIRQ_DEBUG_STOP_IDLE_COUNT_OFFSET	20
#define SERIRQ_DEBUG_STOP_IDLE_COUNT_MASK	REG_2BIT_MASK
#define SERIRQ_DEBUG_STOP_IDLE_COUNT(value)	_GET_FV(SERIRQ_DEBUG_STOP_IDLE_COUNT, value)
#define SERIRQ_DEBUG_CYCLE3_COUNT_OFFSET	16
#define SERIRQ_DEBUG_CYCLE3_COUNT_MASK		REG_2BIT_MASK
#define SERIRQ_DEBUG_CYCLE3_COUNT(value)	_GET_FV(SERIRQ_DEBUG_CYCLE3_COUNT, value)
#define SERIRQ_DEBUG_SERIRQ_COUNT_OFFSET	8
#define SERIRQ_DEBUG_SERIRQ_COUNT_MASK		REG_5BIT_MASK
#define SERIRQ_DEBUG_SERIRQ_COUNT(value)	_GET_FV(SERIRQ_DEBUG_SERIRQ_COUNT, value)
#define SERIRQ_DEBUG_SERIRQ_FSM_OFFSET		0
#define SERIRQ_DEBUG_SERIRQ_FSM_MASK		REG_7BIT_MASK
#define SERIRQ_DEBUG_SERIRQ_FSM(value)		_GET_FV(SERIRQ_DEBUG_SERIRQ_FSM, value)

/* 8.19 LPC_MEM_CFG */
#define LPC_MEM_CFG_TRANS_OFFSET		16
#define LPC_MEM_CFG_TRANS_MASK			REG_7BIT_MASK
#define LPC_MEM_CFG_TRANS(value)		_SET_FV(LPC_MEM_CFG_TRANS, value)
#define LPC_MEM_CFG_TRANS_SEL			_BV(8)
#define LPC_MEM_CFG_CYCLE			_BV(4)
#define LPC_MEM_CFG_RESERVED			_BV(0)

/* 8.20 LPC_ERR_ADDR */
#define LPC_ERR_ADDR_REG_OFFSET			0
#define LPC_ERR_ADDR_REG_MASK			REG_32BIT_MASK
#define LPC_ERR_ADDR_REG(value)			_SET_FV(LPC_ERR_ADDR_REG, value)


#define spacemit_lpc_get_config			__raw_readl(LPC_CFG)
#define spacemit_lpc_set_config(byte)		__raw_writel(byte, LPC_CFG)
#define spacemit_lpc_read_start			__raw_writel(0, LPC_CMD_OP)
#define spacemit_lpc_write_start		__raw_writel(1, LPC_CMD_OP)
#define spacemit_lpc_get_status			__raw_read(LPC_STATUS)
#define spacemit_lpc_get_int_mask		__raw_read(LPC_INT_MASK)
#define spacemit_lpc_set_int_mask(byte)		__raw_writel(byte, LPC_INT_MASK)
#define spacemit_get_int_status			__raw_read(LPC_INT_STATUS)
#define spacemit_get_int_raw_status		__raw_read(LPC_INT_RAW_STATUS)
#define spacemit_lpc_int_clear(byte)		__raw_writel(byte, LPC_INT_CLR)
#define spacemit_lpc_get_wait_count		__raw_read(LPC_WAIT_COUNT)
#define spacemit_lpc_set_wait_count(byte)	__raw_writel(byte, LPC_WAIT_COUNT)
#define spacemit_lpc_get_addr			__raw_read(LPC_ADDR)
#define spacemit_lpc_set_addr(byte)		__raw_writel(byte, LPC_ADDR)
#define spacemit_lpc_write_data(byte)		__raw_writel(byte, LPC_WDATA)
#define spacemit_lpc_read_data			__raw_read(LPC_RDATA)
#define spacemit_lpc_get_debug			__raw_read(LPC_DEBUG)
#define spacemit_lpc_get_serirq_cfg		__raw_read(SERIRQ_CFG)
#define spacemit_lpc_set_serirq_cfg(byte)	__raw_writel(byte, SERIRQ_CFG)
#define spacemit_lpc_set_serirq_op(byte)	__raw_writel(byte, SERIRQ_OP)
#define spacemit_lpc_get_serirq_slot_mask	__raw_read(SERIRQ_SLOT_MASK)
#define spacemit_lpc_set_serirq_slot_mask(byte)	__raw_writel(byte, SERIRQ_SLOT_MASK)
#define spacemit_lpc_get_serirq_slot_irq	__raw_read(SERIRQ_SLOT_IRQ)
#define spacemit_lpc_set_serirq_slot_clr(byte)	__raw_writel(byte, SERIRQ_SLOT_CLR)
#define spacemit_lpc_get_serirq_debug		__raw_read(SERIRQ_DEBUG)
#define spacemit_lpc_get_mem_cfg		__raw_read(SERIRQ_CFG)
#define spacemit_lpc_set_mem_cfg(byte)		__raw_writel(byte, LPC_MEM_CFG)
#define spacemit_lpc_get_err_addr		__raw_read(LPC_ERR_ADDR)

uint8_t lpc_get_config(void);
uint8_t lpc_set_config(uint8_t byte);
uint8_t lpc_read_start(void);
uint8_t lpc_write_start(void);
uint8_t lpc_get_status(void);
uint8_t lpc_get_int_mask(void);
uint8_t lpc_set_int_mask(uint8_t byte);
uint8_t lpc_get_int_status(void);
uint8_t lpc_get_int_raw_status(void);
uint8_t lpc_int_clear(uint8_t byte);
uint8_t lpc_set_wait_count(uint8_t byte);
uint8_t lpc_get_addr(void);
uint8_t lpc_set_addr(uint8_t byte);
uint8_t lpc_write_data(uint8_t byte);
uint8_t lpc_read_data(void);
uint8_t lpc_get_debug(void);
uint8_t lpc_get_serirq_cfg(void);
uint8_t lpc_set_serirq_cfg(uint8_t byte);
uint8_t lpc_set_serirq_op(uint8_t byte);
uint8_t lpc_get_serirq_slot_mask(void);
uint8_t lpc_set_serirq_slot_mask(uint8_t byte);
uint8_t lpc_get_serirq_slot_irq(void);
uint8_t lpc_set_serirq_slot_clr(uint8_t byte);
uint8_t lpc_get_serirq_debug(void);
uint8_t lpc_get_mem_cfg(void);
uint8_t lpc_set_mem_cfg(uint8_t byte);
uint8_t lpc_get_err_addr(void);

#endif /* __SPACEMIT_LPC_H_INCLUDE__ */