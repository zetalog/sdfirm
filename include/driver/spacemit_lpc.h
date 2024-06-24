#ifndef __SPACEMIT_LPC_H_INCLUDE__
#define __SPACEMIT_LPC_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/lpc.h>

#ifndef SPACEMIT_LPC_CFG_BASE
#define SPACEMIT_LPC_CFG_BASE			0x02010000
#endif
#ifndef SPACEMIT_LPC_REG
#define SPACEMIT_LPC_REG(offset)		(SPACEMIT_LPC_CFG_BASE + (offset))
#endif

#define LPC_IRQ					IRQ_LPC

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
#define LPC_AXI_DEBUG				SPACEMIT_LPC_REG(0x58)

/* 8.1 LPC_CFG */
#define LPC_CFG_SERIRQ_EN			_BV(16)
#define LPC_CFG_LONG_WAIT_EN			_BV(12)
#define LPC_CFG_FW_BYTE_LEN_OFFSET		8
#define LPC_CFG_FW_BYTE_LEN_MASK		REG_2BIT_MASK
#define LPC_CFG_FW_BYTE_LEN(value)		_SET_FV(LPC_CFG_FW_BYTE_LEN, value)
#define LPC_CFG_FW_BYTE_LEN_8			0x00
#define LPC_CFG_FW_BYTE_LEN_16			0x01
#define LPC_CFG_FW_BYTE_LEN_32			0x02
#define LPC_CFG_CYCLE_TYPE_OFFSET		4
#define LPC_CFG_CYCLE_TYPE_MASK			REG_2BIT_MASK
#define LPC_CFG_CYCLE_TYPE(value)		_SET_FV(LPC_CFG_CYCLE_TYPE, value)
#define LPC_CFG_CYCLE_IO			0x00
#define LPC_CFG_CYCLE_MEM			0x01
#define LPC_CFG_CYCLE_FIRM			0x02
#define LPC_CFG_LFRAME_WIDE_OFFSET		0
#define LPC_CFG_LFRAME_WIDE_MASK		REG_2BIT_MASK
#define LPC_CFG_LFRAME_WIDE(value)		_SET_FV(LPC_CFG_LFRAME_WIDE, value)

/* 8.2 LPC_CMD_OP */
#define LPC_CMD_OP_READ				0
#define LPC_CMD_OP_WRITE			1

/* 8.3 LPC_STATUS */
#define LPC_STATUS_SERIRQ_BUSY			_BV(4)
#define LPC_STATUS_LPC_BUSY			_BV(0)

/* 8.4 LPC_INT_MASK */
/* 8.5 LPC_INT_STATUS */
/* 8.6 LPC_INT_RAW_STATUS */
/* 8.7 LPC_INT_CLR */
#define LPC_INT_SYNC_ERR			_BV(24)
#define LPC_INT_NO_SYNC				_BV(20)
#define LPC_INT_LWAIT_TIMEOUT			_BV(16)
#define LPC_INT_SWAIT_TIMEOUT			_BV(12)
#define LPC_INT_SERIRQ_INT			_BV(8)
#define LPC_INT_SERIRQ_DONE			_BV(4)
#define LPC_INT_OP_DONE				_BV(0)
#define LPC_INT_OP_ERR				(LPC_INT_SYNC_ERR | \
						 LPC_INT_NO_SYNC | \
						 LPC_INT_LWAIT_TIMEOUT | \
						 LPC_INT_SWAIT_TIMEOUT)
#define LPC_INT_OP_STATUS			(LPC_INT_OP_DONE | LPC_INT_OP_ERR)

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

/* 8.13 SERIRQ_CFG */
#define SERIRQ_CFG_SERIRQ_NUM_OFFSET		12
#define SERIRQ_CFG_SERIRQ_NUM_MASK		REG_4BIT_MASK
#define SERIRQ_CFG_SERIRQ_NUM(value)		_SET_FV(SERIRQ_CFG_SERIRQ_NUM, value)
#define SERIRQ_NUM_MIN				17
#define SERIRQ_NUM_MAX				32
#define SERIRQ_NUM(num)				((num)-SERIRQ_NUM_MIN)
#define SERIRQ_CFG_SERIRQ_IDLE_WIDE_OFFSET	8
#define SERIRQ_CFG_SERIRQ_IDLE_WIDE_MASK	REG_2BIT_MASK
#define SERIRQ_CFG_SERIRQ_IDLE_WIDE(value)	_SET_FV(SERIRQ_CFG_SERIRQ_NUM, value)
#define SERIRQ_CFG_SERIRQ_START_WIDE_OFFSET	4
#define SERIRQ_CFG_SERIRQ_START_WIDE_MASK	REG_2BIT_MASK
#define SERIRQ_CFG_SERIRQ_START_WIDE(value)	_SET_FV(SERIRQ_CFG_SERIRQ_NUM, value)
#define SERIRQ_CFG_SERIRQ_MODE			_BV(0)
#define SERIRQ_MODE_CONTINUOUS			0
#define SERIRQ_MODE_QUIET			SERIRQ_CFG_SERIRQ_MODE

/* 8.15 SERIRQ_SLOT_MASK */
/* 8.16 SERIRQ_SLOT_IRQ */
/* 8.17 SERIRQ_SLOT_CLR */
#define SERIRQ_SLOT(slot)			_BV(slot)

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
#define LPC_MEM_TRANS1_OFFSET			23
#define LPC_MEM_TRANS1_MASK			REG_8BIT_MASK
#define LPC_MEM_TRANS1(value)			_SET_FV(LPC_MEM_TRANS1, value)
#define LPC_MEM_TRANS0_OFFSET			16
#define LPC_MEM_TRANS0_MASK			REG_8BIT_MASK
#define LPC_MEM_TRANS0(value)			_SET_FV(LPC_MEM_TRANS0, value)
#define LPC_MEM_TRANS_SEL			_BV(4)
#define SEL_FROM_MEM_TRANS			0
#define SEL_FROM_MEM_HADDR			1
#define LPC_MEM_CYCLE				_BV(0)
#define LPC_MEM_FIRM_CYCLE			0
#define LPC_MEM_MEM_CYCLE			1

/* 8.20 LPC_ERR_ADDR */
#define LPC_ERR_ADDR_REG_OFFSET			0
#define LPC_ERR_ADDR_REG_MASK			REG_32BIT_MASK
#define LPC_ERR_ADDR_REG(value)			_SET_FV(LPC_ERR_ADDR_REG, value)

/* 8.21 LPC_AXI_DEBUG */
#define LPC_AXI_DEBUG_CURR_START_OFFSET		0
#define LPC_AXI_DEBUG_CURR_START_MASK		REG_8BIT_MASK
#define LPC_AXI_DEBUG_CURR_START(value)		_SET_FV(LPC_AXI_DEBUG_CURR_START, value)

#define lpc_get_serirq_status()			(!!(__raw_readl(LPC_STATUS) & LPC_STATUS_SERIRQ_BUSY))
#define lpc_get_lpc_status()			(!!(__raw_readl(LPC_STATUS) & LPC_STATUS_LPC_BUSY))

#define lpc_get_int_status()			(__raw_readl(LPC_INT_RAW_STATUS))

#define lpc_serirq_enable()			__raw_setl(LPC_CFG_SERIRQ_EN, LPC_CFG)
#define lpc_serirq_disable()			__raw_clearl(LPC_CFG_SERIRQ_EN, LPC_CFG)

#ifdef CONFIG_SPACEMIT_LPC_BRIDGE
#define lpc_io_read8(a)				__raw_readb(SPACEMIT_LPC_IO_BASE + (a))
#define lpc_io_write8(v, a)			__raw_writeb(v, SPACEMIT_LPC_IO_BASE + (a))
#define lpc_mem_read8(a)			__raw_readb(SPACEMIT_LPC_MEM_BASE + (a))
#define lpc_mem_write8(v, a)			__raw_writeb(v, SPACEMIT_LPC_MEM_BASE + (a))
#define lpc_mem_init()				__raw_setl(LPC_MEM_CYCLE, LPC_MEM_CFG)
#else
#define __lpc_io_read8(a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_IO),		\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel(a, LPC_ADDR);					\
		__raw_writel(LPC_CMD_OP_READ, LPC_CMD_OP);			\
	} while (0);
#define __lpc_io_write8(v, a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_IO),		\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
			LPC_CFG);						\
		__raw_writel((a), LPC_ADDR);					\
		__raw_writel((v), LPC_WDATA);					\
		__raw_writel(LPC_CMD_OP_WRITE, LPC_CMD_OP);			\
	} while (0)

void lpc_io_write8(uint8_t v, uint16_t a);
uint8_t lpc_io_read8(uint16_t a);

#define __lpc_mem_read8(a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_MEM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel(a, LPC_ADDR);					\
		__raw_writel(LPC_CMD_OP_READ, LPC_CMD_OP);			\
	} while (0)
#define __lpc_mem_write8(v, a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_MEM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel((a), LPC_ADDR);					\
		__raw_writel((v), LPC_WDATA);					\
		__raw_writel(LPC_CMD_OP_WRITE, LPC_CMD_OP);			\
	} while (0)

void lpc_mem_write8(uint8_t v, uint32_t a);
uint8_t lpc_mem_read8(uint32_t a);

#define lpc_mem_init()				do {} while (0)
#endif /* CONFIG_SPACEMIT_LPC_BRIDGE */

#define __lpc_firm_read8(a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_FIRM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel_mask(LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_8),	\
				  LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_MASK),\
				  LPC_CFG);					\
		__raw_writel(a, LPC_ADDR);					\
		__raw_writel(LPC_CMD_OP_READ, LPC_CMD_OP);			\
	} while (0)
#define __lpc_firm_write8(v, a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_FIRM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel_mask(LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_8),	\
				  LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_MASK),\
			LPC_CFG);						\
		__raw_writel((a), LPC_ADDR);					\
		__raw_writel(v, LPC_WDATA);					\
		__raw_writel(LPC_CMD_OP_WRITE, LPC_CMD_OP);			\
	} while (0)
#define __lpc_firm_read16(a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_FIRM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel_mask(LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_16),	\
				  LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_MASK),\
				  LPC_CFG);					\
		__raw_writel(a, LPC_ADDR);					\
		__raw_writel(LPC_CMD_OP_READ, LPC_CMD_OP);			\
	} while (0)
#define __lpc_firm_write16(v, a)						\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_FIRM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel_mask(LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_16),	\
				  LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_MASK),\
				  LPC_CFG);					\
		__raw_writel((a), LPC_ADDR);					\
		__raw_writel(v, LPC_WDATA);					\
		__raw_writel(LPC_CMD_OP_WRITE, LPC_CMD_OP);			\
	} while (0)
#define __lpc_firm_read32(a)							\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_FIRM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel_mask(LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_32),	\
				  LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_MASK),\
				  LPC_CFG);					\
		__raw_writel(a, LPC_ADDR);					\
		__raw_writel(LPC_CMD_OP_READ, LPC_CMD_OP);			\
	} while (0)
#define __lpc_firm_write32(v, a)						\
	do {									\
		__raw_writel_mask(LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_FIRM),	\
				  LPC_CFG_CYCLE_TYPE(LPC_CFG_CYCLE_TYPE_MASK),	\
				  LPC_CFG);					\
		__raw_writel_mask(LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_32),	\
				  LPC_CFG_FW_BYTE_LEN(LPC_CFG_FW_BYTE_LEN_MASK),\
				  LPC_CFG);					\
		__raw_writel((a), LPC_ADDR);					\
		__raw_writel(v, LPC_WDATA);					\
		__raw_writel(LPC_CMD_OP_WRITE, LPC_CMD_OP);			\
	} while (0)

#define lpc_clear_int(irq)				__raw_setl(irq, LPC_INT_CLR)
#define lpc_get_irq(irq)				(!!(__raw_readl(LPC_INT_STATUS) & _BV(irq)))
#define lpc_mask_irq(irq)				__raw_setl(_BV(irq), LPC_INT_MASK)
#define lpc_unmask_irq(irq)				__raw_clearl(_BV(irq), LPC_INT_MASK)
#define lpc_mask_all_irqs()				__raw_writel(0xffffffff, LPC_INT_MASK)
#define lpc_get_raw_irq(irq)				(!!(__raw_readl(LPC_INT_RAW_STATUS) & _BV(irq)))
#define lpc_count_init()							\
	__raw_writel_mask(LPC_WAIT_ABORT_COUNT(0),				\
		LPC_WAIT_ABORT_COUNT(LPC_WAIT_ABORT_COUNT_MASK), 		\
		LPC_WAIT_COUNT)

#define lpc_serirq_config(quiet)					\
	do {								\
		if (mode)						\
			__raw_setl(SERIRQ_MODE_QUIET, SERIRQ_CFG);	\
		else							\
			__raw_clearl(SERIRQ_MODE_QUIET, SERIRQ_CFG);	\
	} while (0)
#define lpc_serirq_start()			__raw_writel(1, SERIRQ_OP)
#define lpc_mask_serirq(slot)			__raw_setl(_BV(slot), SERIRQ_SLOT_MASK)
#define lpc_mask_all_serirqs()			__raw_setl(0xffffffff, SERIRQ_SLOT_MASK)
#define lpc_unmask_serirq(slot)			__raw_clearl(_BV(slot), SERIRQ_SLOT_MASK)
#define lpc_get_serirq(slot)			(!!(__raw_readl(SERIRQ_SLOT_IRQ) & _BV(slot)))
#define lpc_clear_serirq(slot)			__raw_setl(_BV(slot), SERIRQ_SLOT_CLR)

#define lpc_mem_cfg(sel, address0, address1, cycle)					\
	do {										\
		__raw_writel((sel), LPC_MEM_TRANS_SEL | LPC_MEM_CFG);			\
		__raw_writel((cycle), LPC_MEM_CYCLE | LPC_MEM_CFG);			\
		__raw_writel_mask(LPC_MEM_TRANS0(address0), 				\
			LPC_MEM_TRANS0(LPC_MEM_TRANS0_MASK), 				\
			LPC_MEM_CFG);							\
		__raw_writel_mask(LPC_MEM_TRANS1(address1), 				\
			LPC_MEM_TRANS1(LPC_MEM_TRANS1_MASK), 				\
			LPC_MEM_CFG);							\
	} while (0)

void spacemit_lpc_init(void);

#endif /* __SPACEMIT_LPC_H_INCLUDE__ */
