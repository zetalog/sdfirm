#ifndef __ESPI_H_INCLUDE__
#define __ESPI_H_INCLUDE__

#include <target/generic.h>

#ifdef CONFIG_ESPI_DEBUG
#define espi_debug_state(state)					\
	do {							\
		if (state != espi_state_get()) {		\
			espi_debug(ESPI_DEBUG_STATE, state);	\
		}						\
	} while (0)
#define espi_debug_event(event)					\
	do {							\
		espi_debug(ESPI_DEBUG_EVENT, event);		\
	} while (0)
#define espi_debug_op(op)					\
	do {							\
		espi_debug(ESPI_DEBUG_OP, op);			\
	} while (0)
#define espi_debug_reg(reg)					\
	do {							\
		espi_debug(ESPI_DEBUG_REG, reg);			\
	} while (0)
#else
#define espi_debug_state(state)
#define espi_debug_event(event)
#define espi_debug_op(op)
#define espi_debug_reg(reg)
#endif

/* No commands */
#define ESPI_CMD_NONE				0xffff
#define ESPI_CMD(cmd)				(HIBYTE(cmd) == 0xff ? ESPI_CMD_NONE : LOBYTE(cmd))
/* No responses */
#define ESPI_RSP_NONE				0xffff
#define ESPI_RSP(rsp)				(HIBYTE(rsp) == 0xff ? ESPI_RSP_NONE : LOBYTE(rsp))

/* eSPI Interface Base Specification - Document # 327432-004 Revision 1.0 */
/* 4.2 Command phase */
#define ESPI_CMD_PUT_PC				0x00
#define ESPI_CMD_GET_PC				0x01
#define ESPI_CMD_PUT_NP				0x02
#define ESPI_CMD_GET_NP				0x03
#define ESPI_CMD_PUT_IORD_SHORT(c)		(0x40 | (c))
#define ESPI_CMD_PUT_IOWR_SHORT(c)		(0x44 | (c))
#define ESPI_CMD_PUT_MEMRD32_SHORT(c)		(0x48 | (c))
#define ESPI_CMD_PUT_MEMWR32_SHORT(c)		(0x4C | (c))
#define ESPI_CMD_PUT_VWIRE			0x04
#define ESPI_CMD_GET_VWIRE			0x05
#define ESPI_CMD_PUT_OOB			0x06
#define ESPI_CMD_GET_OOB			0x07
#define ESPI_CMD_PUT_FLASH_C			0x08
#define ESPI_CMD_GET_FLASH_NP			0x09
#define ESPI_CMD_GET_STATUS			0x25
#define ESPI_CMD_SET_CONFIGURATION		0x22
#define ESPI_CMD_GET_CONFIGURATION		0x21
#define ESPI_CMD_RESET				0xFF

#define ESPI_CMD_C_1BYTE			0x00
#define ESPI_CMD_C_2BYTES			0x01
#define ESPI_CMD_C_4BYTES			0x03
#define ESPI_CMD_PUT_IORD8			ESPI_CMD_PUT_IORD_SHORT(ESPI_CMD_C_1BYTE)
#define ESPI_CMD_PUT_IORD16			ESPI_CMD_PUT_IORD_SHORT(ESPI_CMD_C_2BYTES)
#define ESPI_CMD_PUT_IORD32			ESPI_CMD_PUT_IORD_SHORT(ESPI_CMD_C_4BYTES)
#define ESPI_CMD_PUT_IOWR8			ESPI_CMD_PUT_IOWR_SHORT(ESPI_CMD_C_1BYTE)
#define ESPI_CMD_PUT_IOWR16			ESPI_CMD_PUT_IOWR_SHORT(ESPI_CMD_C_2BYTES)
#define ESPI_CMD_PUT_IOWR32			ESPI_CMD_PUT_IOWR_SHORT(ESPI_CMD_C_4BYTES)
#define ESPI_CMD_PUT_MEMRD8			ESPI_CMD_PUT_MEMRD32_SHORT(ESPI_CMD_C_1BYTE)
#define ESPI_CMD_PUT_MEMRD16			ESPI_CMD_PUT_MEMRD32_SHORT(ESPI_CMD_C_2BYTES)
#define ESPI_CMD_PUT_MEMRD32			ESPI_CMD_PUT_MEMRD32_SHORT(ESPI_CMD_C_4BYTES)
#define ESPI_CMD_PUT_MEMWR8			ESPI_CMD_PUT_MEMWR32_SHORT(ESPI_CMD_C_1BYTE)
#define ESPI_CMD_PUT_MEMWR16			ESPI_CMD_PUT_MEMWR32_SHORT(ESPI_CMD_C_2BYTES)
#define ESPI_CMD_PUT_MEMWR32			ESPI_CMD_PUT_MEMWR32_SHORT(ESPI_CMD_C_4BYTES)

/* 4.4 Response phase */
#define ESPI_RESPONSE_MODIFIER_OFFSET		6
#define ESPI_RESPONSE_MODIFIER_MASK		REG_2BIT_MASK
#define ESPI_RESPONSE_MODIFIER(mod)		_SET_FV(ESPI_RESPONSE_MODIFIER, mod)
#define ESPI_RSP_NRESPONSE			ESPI_RESPONSE_MODIFIER(0x3)
#define ESPI_RSP_RESPONSE			ESPI_RESPONSE_MODIFIER(0x0)
#define ESPI_RSP_ACCEPT(r)			(ESPI_RESPONSE_MODIFIER(r) | 0x08)
#define ESPI_RSP_DEFER				(ESPI_RSP_RESPONSE | 0x01)
#define ESPI_RSP_NON_FATAL_ERROR		(ESPI_RSP_RESPONSE | 0x02)
#define ESPI_RSP_FATAL_ERROR			(ESPI_RSP_RESPONSE | 0x03)
#define ESPI_RSP_WAIT_STATE			(ESPI_RSP_RESPONSE | 0x0f)
#define ESPI_RSP_NO_RESPONSE			(ESPI_RSP_NRESPONSE | 0x0f)

#define ESPI_RSP_APPEND_NONE			0x00
#define ESPI_RSP_APPEND_PERI			0x01
#define ESPI_RSP_APPEND_VWIRE			0x02
#define ESPI_RSP_APPEND_FLASH			0x03

/* 4.4.2 Status */
#define ESPI_STATUS_PC_FREE			_BV(0)
#define ESPI_STATUS_NP_FREE			_BV(1)
#define ESPI_STATUS_VWIRE_FREE			_BV(2) /* Always 1 */
#define ESPI_STATUS_OOB_FREE			_BV(3)
#define ESPI_STATUS_PC_AVAIL			_BV(4)
#define ESPI_STATUS_NP_AVAIL			_BV(5)
#define ESPI_STATUS_VWIRE_AVAIL			_BV(6)
#define ESPI_STATUS_OOB_AVAIL			_BV(7)
#define ESPI_STATUS_FLASH_C_FREE		_BV(8) /* Always 1 */
#define ESPI_STATUS_FLASH_NP_FREE		_BV(9)
#define ESPI_STATUS_FLASH_C_AVAIL		_BV(11)
#define ESPI_STATUS_FLASH_NP_AVAIL		_BV(12)

/* 5.5.1 Cycle Types */
#define ESPI_CYCLE_MEMRD32			0x00
#define ESPI_CYCLE_MEMRD64			0x02
#define ESPI_CYCLE_MEMWR32			0x01
#define ESPI_CYCLE_MEMWR64			0x03
#define ESPI_CYCLE_MESSAGE(r)			(0x10 | ((r) << 1))
#define ESPI_CYCLE_MESSAGE_DATA(r)		(0x11 | ((r) << 1))
#define ESPI_CYCLE_SUCCESS			0x06
#define ESPI_CYCLE_SUCCESS_DATA(p)		(0x09 | ((p) << 1))
#define ESPI_CYCLE_UNSUCCESS(p)			(0x08 | ((p) << 1))
#define ESPI_CYCLE_OOB				0x21
#define ESPI_CYCLE_FLASH_READ			0x00
#define ESPI_CYCLE_FLASH_WRITE			0x01
#define ESPI_CYCLE_FLASH_ERASE			0x02

#define ESPI_PERI_SHORT_MEM32_HDR_LEN		4
#define ESPI_PERI_SHORT_IO32_HDR_LEN		2
#define ESPI_PERI_MEM32_HDR_LEN			7
#define ESPI_PERI_MEM64_HDR_LEN			11
#define ESPI_PERI_MESSAGE_HDR_LEN		8
#define ESPI_PERI_COMPLETION_HDR_LEN		3
#define ESPI_OOB_MESSAGE_HDR_LEN		3
#define ESPI_FLASH_ACCESS_REQUEST_HDR_LEN	7
#define ESPI_FLASH_ACCESS_COMPLETION_HDR_LEN	3

#define ESPI_TAG_OFFSET				4
#define ESPI_TAG_MASK				REG_4BIT_MASK
#define ESPI_TXHDR_TAG(value)			_SET_FV(ESPI_TAG, value)
#define ESPI_RXHDR_TAG(value)			_GET_FV(ESPI_TAG, value)
#define ESPI_LEN_OFFSET				0
#define ESPI_LEN_MASK				REG_4BIT_MASK
#define ESPI_TXHDR_LEN(value)			_SET_FV(ESPI_LEN, value)
#define ESPI_RXHDR_LEN(value)			_GET_FV(ESPI_LEN, value)
#define ESPI_TXHDR_LENGTH(hdr, len)				\
	do {							\
		hdr[1] &= ~ESPI_TXHDR_LEN(REG_4BIT_MASK);	\
		hdr[1] |= ESPI_TXHDR_LEN(HIBYTE(len));		\
		hdr[2] = LOBYTE(len);				\
	} while (0)
#define ESPI_RXHDR_LENGTH(hdr)					\
	MAKEWORD(hdr[2], ESPI_RXHDR_LEN(hdr[1]))

#define ESPI_MESSAGE_LTR			0x01
#define ESPI_LTR_RQ				_BV(31)
#define ESPI_LTR_LS_OFFSET			26
#define ESPI_LTR_LS_MASK			REG_3BIT_MASK
#define ESPI_LTR_LS(value)			_SET_FV(ESPI_LTR_LS, value)
#define ESPI_LTR_LV_OFFSET			16
#define ESPI_LTR_LV_MASK			REG_10BIT_MASK
#define ESPI_LTR_LV(value)			_SET_FV(ESPI_LTR_LV, value)
#define ESPI_LTR(rq, ls, lv)			\
	(((rq) ? ESPI_LTR_RQ : 0) | ESPI_LTR_LS(ls) | ESPI_LTR_LV(lv))

/* 7.2 Capabilities and Configuration Registers */
/* 7.2.1.2 Offset 04h: Device Identification */
#define ESPI_SLAVE_DEV_ID			0x04
#define ESPI_SLAVE_VERSION_ID_OFFSET		0
#define ESPI_SLAVE_VERSION_ID_MASK		REG_4BIT_MASK
#define ESPI_SLAVE_VERSION_ID(value)		_GET_FV(ESPI_SLAVE_VERSION_ID)

/* 7.2.1.3 Offset 08h: General Capabilities and Configurations */
#define ESPI_SLAVE_GEN_CFG			0x08
#define ESPI_GEN_CRC_ENABLE			_BV(31)
#define ESPI_GEN_RESP_MOD_ENABLE		_BV(30)
#define ESPI_GEN_ALERT_MODE_PIN			_BV(28)
#define ESPI_GEN_ALERT_MODE_IO1			0
#define ESPI_GEN_IO_MODE_SEL_OFFSET		26
#define ESPI_GEN_IO_MODE_SEL_MASK		REG_2BIT_MASK
#define ESPI_GEN_IO_MODE_SEL(value)		_SET_FV(ESPI_GEN_IO_MODE_SEL, value)
#define ESPI_GEN_IO_MODE_SUP_OFFSET		24
#define ESPI_GEN_IO_MODE_SUP_MASK		REG_2BIT_MASK
#define ESPI_GEN_IO_MODE_SUP(value)		_GET_FV(ESPI_GEN_IO_MODE_SUP, value)
#define espi_gen_io_mode_sup(value)		_SET_FV(ESPI_GEN_IO_MODE_SUP, value)
#define ESPI_GEN_IO_MODE_SINGLE			0x0
#define ESPI_GEN_IO_MODE_DUAL			0x1
#define ESPI_GEN_IO_MODE_QUAD			0x2
#define ESPI_GEN_OPEN_DRAIN_ALERT_SEL		_BV(23)
#define ESPI_GEN_OPEN_DRAIN_ALERT_SUP		_BV(19)
#define ESPI_GEN_ALERT_TYPE_PP			0
#define ESPI_GEN_ALERT_TYPE_OD			ESPI_GEN_OPEN_DRAIN_ALERT_SEL
#define ESPI_GEN_OP_FREQ_SEL_OFFSET		20
#define ESPI_GEN_OP_FREQ_SEL_MASK		REG_3BIT_MASK
#define ESPI_GEN_OP_FREQ_SEL(value)		_SET_FV(ESPI_GEN_OP_FREQ_SEL, value)
#define ESPI_GEN_OP_FREQ_SUP_OFFSET		16
#define ESPI_GEN_OP_FREQ_SUP_MASK		REG_3BIT_MASK
#define ESPI_GEN_OP_FREQ_SUP(value)		_GET_FV(ESPI_GEN_OP_FREQ_SUP, value)
#define espi_gen_op_freq_sup(value)		_SET_FV(ESPI_GEN_OP_FREQ_SUP, value)
#define ESPI_GEN_OP_FREQ_20MHZ			0
#define ESPI_GEN_OP_FREQ_25MHZ			1
#define ESPI_GEN_OP_FREQ_33MHZ			2
#define ESPI_GEN_OP_FREQ_50MHZ			3
#define ESPI_GEN_OP_FREQ_66MHZ			4
#define ESPI_GEN_MAX_WAIT_STATE_OFFSET		12
#define ESPI_GEN_MAX_WAIT_STATE_MASK		REG_4BIT_MASK
#define ESPI_GEN_MAX_WAIT_STATE(value)		_SET_FV(ESPI_GEN_MAX_WAIT_STATE, value)
#define ESPI_GEN_FLASH_CHAN_SUP			_BV(3)
#define ESPI_GEN_OOB_CHAN_SUP			_BV(2)
#define ESPI_GEN_VWIRE_CHAN_SUP			_BV(1)
#define ESPI_GEN_PERI_CHAN_SUP			_BV(0)
#define ESPI_GEN_CAP_MASK					\
	(espi_gen_io_mode_sup(ESPI_GEN_IO_MODE_SUP_MASK) |	\
	 ESPI_GEN_OPEN_DRAIN_ALERT_SUP |			\
	 espi_gen_op_freq_sup(ESPI_GEN_OP_FREQ_SUP_MASK) |	\
	 ESPI_FLASH_CHAN | ESPI_OOB_CHAN |			\
	 ESPI_VWIRE_CHAN | ESPI_PERI_CHAN)
#define ESPI_GEN_CFG_MASK					\
	(ESPI_GEN_CRC_ENABLE |					\
	 ESPI_GEN_RESP_MOD_ENABLE |				\
	 ESPI_GEN_ALERT_MODE_PIN |				\
	 ESPI_GEN_IO_MODE_SEL(ESPI_GEN_IO_MODE_SEL_MASK) |	\
	 ESPI_GEN_OPEN_DRAIN_ALERT_SEL |			\
	 ESPI_GEN_OP_FREQ_SEL(ESPI_GEN_OP_FREQ_SEL_MASK) |	\
	 ESPI_GEN_MAX_WAIT_STATE(ESPI_GEN_MAX_WAIT_STATE_MASK))

/* Peripheral Read Request Size
 * Peripheral Payload Size
 * VWire Payload Size
 * Flash Read Request Size
 * Flash Payload Size
 * 1: 64Bytes
 * 2: 128Bytes
 * 3: 256Bytes
 * 4: 512Bytes
 * 5: 1024Bytes
 * 6: 2048Bytes
 * 7: 4096Bytes
 */
#define ESPI_PAYLOAD_SIZE(nbytes)		(ilog2_const(nbytes) - 5)
#define ESPI_PERI_READ_REQ_SIZE_MAX		4096
#define ESPI_PERI_PAYLOAD_SIZE_MAX		256
#define ESPI_OOB_PAYLOAD_SIZE_MAX		256
#define ESPI_FLASH_READ_REQ_SIZE_MAX		4096
#define ESPI_FLASH_PAYLOAD_SIZE_MAX		256
/* 7.2.1.4 Offset 10h: Channel 0 Capabilities and Configurations */
#define ESPI_SLAVE_PERI_CFG			0x10
#define ESPI_PERI_MAX_READ_REQ_SIZE_OFFSET	12
#define ESPI_PERI_MAX_READ_REQ_SIZE_MASK	REG_3BIT_MASK
#define ESPI_PERI_MAX_READ_REQ_SIZE(value)	_SET_FV(ESPI_PERI_MAX_READ_REQ_SIZE, __ilog2_u32(value)-5)
#define ESPI_PERI_MAX_PAYLOAD_SIZE_SEL_OFFSET	8
#define ESPI_PERI_MAX_PAYLOAD_SIZE_SEL_MASK	REG_3BIT_MASK
#define ESPI_PERI_MAX_PAYLOAD_SIZE_SEL(value)	_SET_FV(ESPI_PERI_MAX_PAYLOAD_SIZE_SEL, (value)+1)
#define ESPI_PERI_MAX_PAYLOAD_SIZE_SUP_OFFSET	4
#define ESPI_PERI_MAX_PAYLOAD_SIZE_SUP_MASK	REG_3BIT_MASK
#define ESPI_PERI_MAX_PAYLOAD_SIZE_SUP(value)	_GET_FV(ESPI_PERI_MAX_PAYLOAD_SIZE_SUP, (value)-1)
#define espi_peri_max_payload_size_sup(value)	_SET_FV(ESPI_PERI_MAX_PAYLOAD_SIZE_SUP, value)
#define ESPI_PERI_BUS_MASTER_ENABLE		_BV(2)
#define ESPI_PERI_CAP_MASK			\
	(ESPI_SLAVE_CHANNEL_READY |		\
	 espi_peri_max_payload_size_sup(ESPI_PERI_MAX_PAYLOAD_SIZE_SUP_MASK))

/* 7.2.1.5 Offset 20h: Channel 1 Capabilities and Configurations */
#define ESPI_SLAVE_VWIRE_CFG			0x20
#define ESPI_VWIRE_MAX_COUNT_SEL_OFFSET		16
#define ESPI_VWIRE_MAX_COUNT_SEL_MASK		REG_6BIT_MASK
/* 0-based where 0 indicates 1 virtual wire */
#define ESPI_VWIRE_MAX_COUNT_SEL(value)		_SET_FV(ESPI_VWIRE_MAX_COUNT_SEL, (value)-1)
#define ESPI_VWIRE_MAX_COUNT_SUP_OFFSET		8
#define ESPI_VWIRE_MAX_COUNT_SUP_MASK		REG_6BIT_MASK
#define ESPI_VWIRE_MAX_COUNT_SUP(value)		(_GET_FV(ESPI_VWIRE_MAX_COUNT_SUP, (value))+1)
#define espi_vwire_max_count_sup(value)		_SET_FV(ESPI_VWIRE_MAX_COUNT_SUP, value)
#define ESPI_VWIRE_CAP_MASK			\
	(ESPI_SLAVE_CHANNEL_READY |		\
	 espi_vwire_max_count_sup(ESPI_VWIRE_MAX_COUNT_SUP_MASK))

/* 7.2.1.6 Offset 30h: Channel 2 Capabilities and Configurations */
#define ESPI_SLAVE_OOB_CFG			0x30
#define ESPI_OOB_MAX_PAYLOAD_SIZE_SEL_OFFSET	8
#define ESPI_OOB_MAX_PAYLOAD_SIZE_SEL_MASK	REG_3BIT_MASK
#define ESPI_OOB_MAX_PAYLOAD_SIZE_SEL(value)	_SET_FV(ESPI_OOB_MAX_PAYLOAD_SIZE_SEL, (value)+1)
#define ESPI_OOB_MAX_PAYLOAD_SIZE_SUP_OFFSET	4
#define ESPI_OOB_MAX_PAYLOAD_SIZE_SUP_MASK	REG_3BIT_MASK
#define ESPI_OOB_MAX_PAYLOAD_SIZE_SUP(value)	(_GET_FV(ESPI_OOB_MAX_PAYLOAD_SIZE_SUP, value)-1)
#define espi_oob_max_payload_size_sup(value)	_SET_FV(ESPI_OOB_MAX_PAYLOAD_SIZE_SUP, value)
#define ESPI_OOB_CAP_MASK			\
	(ESPI_SLAVE_CHANNEL_READY |		\
	 espi_oob_max_payload_size_sup(ESPI_OOB_MAX_PAYLOAD_SIZE_SUP_MASK))

/* 7.2.1.7 Offset 40h: Channel 3 Capabilities and Configurations */
#define ESPI_SLAVE_FLASH_CFG			0x40
#define ESPI_FLASH_MAX_READ_REQ_SIZE_OFFSET	12
#define ESPI_FLASH_MAX_READ_REQ_SIZE_MASK	REG_3BIT_MASK
#define ESPI_FLASH_MAX_READ_REQ_SIZE(value)	_SET_FV(ESPI_FLASH_MAX_READ_REQ_SIZE, __ilog2_u32(value)-5)
#define ESPI_FLASH_SHARING_MODE_MAF		_BV(11)
#define ESPI_FLASH_MAX_PAYLOAD_SIZE_SEL_OFFSET	8
#define ESPI_FLASH_MAX_PAYLOAD_SIZE_SEL_MASK	REG_3BIT_MASK
#define ESPI_FLASH_MAX_PAYLOAD_SIZE_SEL(value)	_SET_FV(ESPI_FLASH_MAX_PAYLOAD_SIZE_SEL, (value)+1)
#define ESPI_FLASH_MAX_PAYLOAD_SIZE_SUP_OFFSET	5
#define ESPI_FLASH_MAX_PAYLOAD_SIZE_SUP_MASK	REG_3BIT_MASK
#define ESPI_FLASH_MAX_PAYLOAD_SIZE_SUP(value)	(_GET_FV(ESPI_FLASH_MAX_PAYLOAD_SIZE_SUP, value)-1)
#define espi_flash_max_payload_size_sup(value)	_SET_FV(ESPI_FLASH_MAX_PAYLOAD_SIZE_SUP, value)
#define ESPI_FLASH_BLOCK_ERASE_SIZE_OFFSET	2
#define ESPI_FLASH_BLOCK_ERASE_SIZE_MASK	REG_3BIT_MASK
#define ESPI_FLASH_BLOCK_ERASE_SIZE(value)	_SET_FV(ESPI_FLASH_BLOCK_ERASE_SIZE, value)
#define ESPI_FLASH_BLOCK_ERASE_4KB		0x01
#define ESPI_FLASH_BLOCK_ERASE_64KB		0x02
#define ESPI_FLASH_BLOCK_ERASE_4KB_64KB		(ESPI_FLASH_BLOCK_ERASE_4KB | ESPI_FLASH_BLOCK_ERASE_64KB)
#define ESPI_FLASH_BLOCK_ERASE_128KB		0x04
#define ESPI_FLASH_BLOCK_ERASE_256KB		0x05
#define ESPI_FLASH_CAP_MASK			\
	(ESPI_SLAVE_CHANNEL_READY |		\
	 ESPI_FLASH_SHARING_MODE_MAF |		\
	 espi_flash_max_payload_size_sup(ESPI_FLASH_MAX_PAYLOAD_SIZE_SUP_MASK))

/* All channels -- peripheral, virtual wire, OOB, and flash access use the
 * same bits for channel ready and channel enable.
 */
#define ESPI_SLAVE_CHANNEL_READY		_BV(1)
#define ESPI_SLAVE_CHANNEL_ENABLE		_BV(0)
#define ESPI_CHANNEL(n)				_BV(n)
#define ESPI_CHANNEL_PERI			0
#define ESPI_CHANNEL_VWIRE			1
#define ESPI_CHANNEL_OOB			2
#define ESPI_CHANNEL_FLASH			3
#define ESPI_CHANNEL_INVALID			4

/* 5.2.2.1 Virtual Wire Index */
#define ESPI_VWIRE_INTERRUPT			0
#define ESPI_VWIRE_SYSTEM			1
#define ESPI_VWIRE_GPIO				2
#define ESPI_VWIRE_TYPE_OFFSET			11
#define ESPI_VWIRE_TYPE_MASK			REG_2BIT_MASK
#define espi_vwire_type(value)			_SET_FV(ESPI_VWIRE_TYPE, value)
#define ESPI_VWIRE_TYPE(value)			_GET_FV(ESPI_VWIRE_TYPE, value)

/* Interrupt events */
#define ESPI_VWIRE_INTERRUPT_LEVEL_HIGH		_BV(7)
#define ESPI_VWIRE_IS_INTERRUPT_EVENT(group)	((group) < 2)
#define ESPI_VWIRE_INTERRUPT_GROUP(event)	(((event) & _BV(7)) >> 7)
#define ESPI_VWIRE_INTERRUPT_LINE(event)	((event) & ~_BV(7))
#define ESPI_VWIRE_INTERRUPT_EVENT(irq)		\
	(ESPI_VWIRE_TYPE(ESPI_VWIRE_INTERRUPT) | (irq))

/* System events */
#define ESPI_VWIRE_SYSTEM_LEVEL_HIGH(evt)	_BV(evt)
#define ESPI_VWIRE_SYSTEM_VALID(evt)		_BV((evt) + 4)
#define ESPI_VWIRE_SYSTEM_EVENT_HIGH(x)		(ESPI_VWIRE_SYSTEM_VALID(x) | ESPI_VWIRE_SYSTEM_LEVEL_HIGH(x))
#define ESPI_VWIRE_SYSTEM_EVENT_LOW(x)		(ESPI_VWIRE_SYSTEM_VALID(x))
#define ESPI_VWIRE_SYSTEM_EVENT_GROUP_OFFSET	4
#define ESPI_VWIRE_SYSTEM_EVENT_GROUP_MASK	REG_7BIT_MASK
#define ESPI_VWIRE_SYSTEM_EVENT_GROUP_MIN	2
#define ESPI_VWIRE_SYSTEM_EVENT_GROUP_MAX	7
#define espi_vwire_system_group(value)		_SET_FV(ESPI_VWIRE_SYSTEM_EVENT_GROUP, value)
#define ESPI_VWIRE_SYSTEM_GROUP(value)		_GET_FV(ESPI_VWIRE_SYSTEM_EVENT_GROUP, value)
#define ESPI_VWIRE_SYSTEM_EVENT_VWIRE_OFFSET	0
#define ESPI_VWIRE_SYSTEM_EVENT_VWIRE_MASK	REG_4BIT_MASK
#define espi_vwire_system_vwire(value)		_SET_FV(ESPI_VWIRE_SYSTEM_EVENT_VWIRE, value)
#define ESPI_VWIRE_SYSTEM_VWIRE(value)		_GET_FV(ESPI_VWIRE_SYSTEM_EVENT_VWIRE, value)
#define ESPI_VWIRE_SYSTEM_EVENT(group, vwire)	\
	(espi_vwire_type(ESPI_VWIRE_SYSTEM) |	\
	 espi_vwire_system_group(group) |	\
	 espi_vwire_system_vwire(vwire))
#define ESPI_VWIRE_SYSTEM_SLP_S5		ESPI_VWIRE_SYSTEM_EVENT(2, 2)
#define ESPI_VWIRE_SYSTEM_SLP_S4		ESPI_VWIRE_SYSTEM_EVENT(2, 1)
#define ESPI_VWIRE_SYSTEM_SLP_S3		ESPI_VWIRE_SYSTEM_EVENT(2, 0)
#define ESPI_VWIRE_SYSTEM_OOB_RST_WARN		ESPI_VWIRE_SYSTEM_EVENT(3, 2)
#define ESPI_VWIRE_SYSTEM_PLTRST		ESPI_VWIRE_SYSTEM_EVENT(3, 1)
#define ESPI_VWIRE_SYSTEM_SUS_STAT		ESPI_VWIRE_SYSTEM_EVENT(3, 0)
#define ESPI_VWIRE_SYSTEM_PME			ESPI_VWIRE_SYSTEM_EVENT(4, 3)
#define ESPI_VWIRE_SYSTEM_WAKE			ESPI_VWIRE_SYSTEM_EVENT(4, 2)
#define ESPI_VWIRE_SYSTEM_OOB_RST_ACK		ESPI_VWIRE_SYSTEM_EVENT(4, 0)
#define ESPI_VWIRE_SYSTEM_SLV_BOOT_LOAD_STATUS	ESPI_VWIRE_SYSTEM_EVENT(5, 3)
#define ESPI_VWIRE_SYSTEM_ERROR_NONFATAL	ESPI_VWIRE_SYSTEM_EVENT(5, 2)
#define ESPI_VWIRE_SYSTEM_ERROR_FATAL		ESPI_VWIRE_SYSTEM_EVENT(5, 1)
#define ESPI_VWIRE_SYSTEM_SLV_BOOT_LOAD_DONE	ESPI_VWIRE_SYSTEM_EVENT(5, 0)
#define ESPI_VWIRE_SYSTEM_HOST_RST_ACK		ESPI_VWIRE_SYSTEM_EVENT(6, 3)
#define ESPI_VWIRE_SYSTEM_RCIN			ESPI_VWIRE_SYSTEM_EVENT(6, 2)
#define ESPI_VWIRE_SYSTEM_SMI			ESPI_VWIRE_SYSTEM_EVENT(6, 1)
#define ESPI_VWIRE_SYSTEM_SCI			ESPI_VWIRE_SYSTEM_EVENT(6, 0)
#define ESPI_VWIRE_SYSTEM_NMIOUT		ESPI_VWIRE_SYSTEM_EVENT(7, 2)
#define ESPI_VWIRE_SYSTEM_SMIOUT		ESPI_VWIRE_SYSTEM_EVENT(7, 1)
#define ESPI_VWIRE_SYSTEM_HOST_RST_WARN		ESPI_VWIRE_SYSTEM_EVENT(7, 0)
#define ESPI_VWIRE_SYSTEM_UNKNOWN		ESPI_VWIRE_SYSTEM_EVENT(0, 0)
#define ESPI_VWIRE_IS_SYSTEM_EVENT(group)			\
	(((group) <= ESPI_VWIRE_SYSTEM_EVENT_GROUP_MAX) &	\
	 !ESPI_VWIRE_IS_INTERRUPT_EVENT(group))

/* GPIO expander */
#define ESPI_VWIRE_GPIO_EXPANDER_GROUP_OFFSET	4
#define ESPI_VWIRE_GPIO_EXPANDER_GROUP_MASK	REG_7BIT_MASK
#define espi_vwire_gpio_group(value)		_SET_FV(ESPI_VWIRE_GPIO_EXPANDER_GROUP, (value) - 128)
#define ESPI_VWIRE_GPIO_GROPU(value)		(_GET_FV(ESPI_VWIRE_GPIO_EXPANDER_GROUP, value) + 128)
#define ESPI_VWIRE_GPIO_EXPANDER_VWIRE_OFFSET	0
#define ESPI_VWIRE_GPIO_EXPANDER_VWIRE_MASK	REG_4BIT_MASK
#define espi_vwire_gpio_vwire(value)		_SET_FV(ESPI_VWIRE_GPIO_EXPANDER_VWIRE, value)
#define ESPI_VWIRE_GPIO_VWIRE(value)		_GET_FV(ESPI_VWIRE_GPIO_EXPANDER_VWIRE, value)
#define ESPI_VWIRE_GPIO_EXPANDER(group, vwire)	\
	(espi_vwire_type(ESPI_VWIRE_GPIO) |	\
	 espi_vwire_gpio_group(group) |	\
	 espi_vwire_gpio_vwire(vwire))

#ifdef CONFIG_ESPI_DEBUG
void espi_show_slave_general_configuration(uint32_t config);
void espi_show_slave_peripheral_channel_configuration(uint32_t config);
#else
static inline void espi_show_slave_general_configuration(uint32_t config) {}
static inline void espi_show_slave_peripheral_channel_configuration(uint32_t config) {}
#endif

#define espi_slave_io_mode_sup_quad(caps)		\
	(ESPI_GEN_IO_MODE_SUP(caps) == ESPI_GEN_IO_MODE_QUAD)
#define espi_slave_io_mode_sup_dual(caps)		\
	(ESPI_GEN_IO_MODE_SUP(caps) == ESPI_GEN_IO_MODE_DUAL)
#define espi_slave_op_freq_sup_66mhz(caps)		\
	(ESPI_GEN_OP_FREQ_SUP(caps) == ESPI_GEN_OP_FREQ_66MHZ)
#define espi_slave_op_freq_sup_50mhz(caps)		\
	(ESPI_GEN_OP_FREQ_SUP(caps) == ESPI_GEN_OP_FREQ_50MHZ)
#define espi_slave_op_freq_sup_33mhz(caps)		\
	(ESPI_GEN_OP_FREQ_SUP(caps) == ESPI_GEN_OP_FREQ_33MHZ)
#define espi_slave_op_freq_sup_25mhz(caps)		\
	(ESPI_GEN_OP_FREQ_SUP(caps) == ESPI_GEN_OP_FREQ_25MHZ)
#define espi_slave_op_freq_sup_20mhz(caps)		\
	(ESPI_GEN_OP_FREQ_SUP(caps) == ESPI_GEN_OP_FREQ_20MHZ)
#define espi_slave_op_freq_sup_max(caps)		\
	(espi_slave_op_freq_sup_66mhz(caps) ? 66 :	\
	 espi_slave_op_freq_sup_50mhz(caps) ? 50 :	\
	 espi_slave_op_freq_sup_33mhz(caps) ? 33 :	\
	 espi_slave_op_freq_sup_25mhz(caps) ? 25 : 20)
#define espi_slave_peri_chan_sup(caps)			\
	(!!((caps) & ESPI_GEN_PERI_CHAN_SUP))
#define espi_slave_vw_chan_sup(caps)			\
	(!!((caps) & ESPI_GEN_VWIRE_CHAN_SUP))
#define espi_slave_oob_chan_sup(caps)			\
	(!!((caps) & ESPI_GEN_OOB_CHAN_SUP))
#define espi_slave_flash_chan_sup(caps)			\
	(!!((caps) & ESPI_GEN_FLASH_CHAN_SUP))

#ifdef CONFIG_ESPI_CRC
#define ESPI_CRC_CHECKING		ESPI_GEN_CRC_ENABLE
#else
#define ESPI_CRC_CHECKING		0
#endif
#ifdef CONFIG_ESPI_RSP_MOD
#define ESPI_RSP_MODIFIER		ESPI_GEN_RESP_MOD_ENABLE
#else
#define ESPI_RSP_MODIFIER		0
#endif
#ifdef CONFIG_ESPI_ALERT_IO1
#define ESPI_ALERT_PIN			ESPI_GEN_ALERT_MODE_IO1
#define ESPI_ALERT_TYPE			0
#endif
#ifdef CONFIG_ESPI_ALERT_PP
#define ESPI_ALERT_PIN			ESPI_GEN_ALERT_MODE_PIN
#define ESPI_ALERT_TYPE			0
#endif
#ifdef CONFIG_ESPI_ALERT_OD
#define ESPI_ALERT_PIN			ESPI_GEN_ALERT_MODE_PIN
#define ESPI_ALERT_TYPE			ESPI_GEN_ALERT_TYPE_OD
#endif
#ifdef CONFIG_ESPI_IO_SINGLE
#define ESPI_IO_MODE			ESPI_GEN_IO_MODE_SINGLE
#endif
#ifdef CONFIG_ESPI_IO_DUAL
#define ESPI_IO_MODE			ESPI_GEN_IO_MODE_DUAL
#endif
#ifdef CONFIG_ESPI_IO_QUAD
#define ESPI_IO_MODE			ESPI_GEN_IO_MODE_QUAD
#endif
#ifdef CONFIG_ESPI_FREQ_66MHZ
#define ESPI_OP_FREQ			ESPI_GEN_OP_FREQ_66MHZ
#endif
#ifdef CONFIG_ESPI_FREQ_50MHZ
#define ESPI_OP_FREQ			ESPI_GEN_OP_FREQ_50MHZ
#endif
#ifdef CONFIG_ESPI_FREQ_33MHZ
#define ESPI_OP_FREQ			ESPI_GEN_OP_FREQ_33MHZ
#endif
#ifdef CONFIG_ESPI_FREQ_25MHZ
#define ESPI_OP_FREQ			ESPI_GEN_OP_FREQ_25MHZ
#endif
#ifdef CONFIG_ESPI_FREQ_20MHZ
#define ESPI_OP_FREQ			ESPI_GEN_OP_FREQ_20MHZ
#endif
#define ESPI_PERI_CHAN			ESPI_GEN_PERI_CHAN_SUP
#define ESPI_VWIRE_CHAN			ESPI_GEN_VWIRE_CHAN_SUP
#ifdef CONFIG_ESPI_OOB
#define ESPI_OOB_CHAN			ESPI_GEN_OOB_CHAN_SUP
#else
#define ESPI_OOB_CHAN			0
#endif
#ifdef CONFIG_ESPI_FLASH
#define ESPI_FLASH_CHAN			ESPI_GEN_FLASH_CHAN_SUP
#else
#define ESPI_FLASH_CHAN			0
#endif
#define ESPI_ALL_CHAN			\
	(ESPI_PERI_CHAN | ESPI_VW_CHAN | ESPI_OOB_CHAN | ESPI_FLASH_CHAN)

#ifdef CONFIG_ESPI_PERI_READ_64
#define ESPI_PERI_READ_SIZE		64
#endif
#ifdef CONFIG_ESPI_PERI_READ_128
#define ESPI_PERI_READ_SIZE		128
#endif
#ifdef CONFIG_ESPI_PERI_READ_256
#define ESPI_PERI_READ_SIZE		256
#endif
#ifdef CONFIG_ESPI_FLASH_READ_64
#define ESPI_FLASH_READ_SIZE		64
#endif
#ifdef CONFIG_ESPI_FLASH_READ_128
#define ESPI_FLASH_READ_SIZE		128
#endif
#ifdef CONFIG_ESPI_FLASH_READ_256
#define ESPI_FLASH_READ_SIZE		256
#endif
#ifdef CONFIG_ESPI_FLASH_READ_512
#define ESPI_FLASH_READ_SIZE		512
#endif
#ifdef CONFIG_ESPI_FLASH_READ_1024
#define ESPI_FLASH_READ_SIZE		1024
#endif
#ifdef CONFIG_ESPI_FLASH_READ_2048
#define ESPI_FLASH_READ_SIZE		2048
#endif
#ifdef CONFIG_ESPI_FLASH_READ_4096
#define ESPI_FLASH_READ_SIZE		4096
#endif
#if defined(CONFIG_ESPI_FLASH_ERASE_256K)
#define ESPI_FLASH_ERASE_SIZE		ESPI_FLASH_BLOCK_ERASE_256KB
#elif defined(CONFIG_ESPI_FLASH_ERASE_128K)
#define ESPI_FLASH_ERASE_SIZE		ESPI_FLASH_BLOCK_ERASE_128KB
#elif defined(CONFIG_ESPI_FLASH_ERASE_64K) && defined(CONFIG_ESPI_FLASH_ERASE_4K)
#define ESPI_FLASH_ERASE_SIZE		ESPI_FLASH_BLOCK_ERASE_4KB_64KB
#elif defined(CONFIG_ESPI_FLASH_ERASE_64K)
#define ESPI_FLASH_ERASE_SIZE		ESPI_FLASH_BLOCK_ERASE_64KB
#else
#define ESPI_FLASH_ERASE_SIZE		ESPI_FLASH_BLOCK_ERASE_4KB
#endif

typedef uint16_t espi_event_t;
typedef uint8_t espi_slave_t;
typedef uint8_t espi_op_t;
typedef void (*espi_cmpl_cb)(espi_slave_t slave, uint8_t op, bool result);

#define ESPI_STATE_INIT			0x00
#define ESPI_STATE_RESET		0x01
#define ESPI_STATE_GET_GEN		0x02
#define ESPI_STATE_SET_GEN		0x03
#define ESPI_STATE_GET_VWIRE		0x04
#define ESPI_STATE_SET_VWIRE		0x05
#define ESPI_STATE_VWIRE_READY		0x06
#define ESPI_STATE_GET_OOB		0x07
#define ESPI_STATE_SET_OOB		0x08
#define ESPI_STATE_OOB_READY		0x09
#define ESPI_STATE_GET_FLASH		0x0A
#define ESPI_STATE_SET_FLASH		0x0B
#define ESPI_STATE_FLASH_READY		0x0C
#define ESPI_STATE_DEASSERT_SUS_STAT	0x0D
#define ESPI_STATE_G3_EXIT		0x0E
#define ESPI_STATE_EARLY_INIT		ESPI_STATE_G3_EXIT
#define ESPI_STATE_HOST_RST_WARN	0x0F
#define ESPI_STATE_HOST_RST_ACK		0x10
#define ESPI_STATE_ASSERT_PLTRST	0x11
#define ESPI_STATE_DEASSERT_PLTRST	0x12
#define ESPI_STATE_GET_PERI		0x13
#define ESPI_STATE_SET_PERI		0x14
#define ESPI_STATE_PERI_READY		0x15
#define ESPI_STATE_LATE_INIT		ESPI_STATE_PERI_READY
#define ESPI_STATE_INVALID		0xFF

#define ESPI_EVENT_INIT			_BV(0x00)
#define ESPI_EVENT_ACCEPT		_BV(0x01)
#define ESPI_EVENT_DEFER		_BV(0x02)
#define ESPI_EVENT_WAIT_STATE		_BV(0x03)
/* fatal/non-fatal error */
#define ESPI_EVENT_REJECT		_BV(0x04)
#define ESPI_EVENT_NO_RESPONSE		_BV(0x05)
#define ESPI_EVENT_RESPONSE		_BV(0x06)
#define ESPI_EVENT_PROBE		_BV(0x07)
#define ESPI_EVENT_BOOT			_BV(0x08)
#define ESPI_EVENT_VWIRE_SYS		_BV(0x09)

#define ESPI_OP_NONE			0x00
#define ESPI_OP_PROBE			0x01
#define ESPI_OP_RESET			0x02

#define espi_op_busy()		(!!(espi_op != ESPI_OP_NONE))
#define espi_op_is(op)		(espi_op == (op))
#define espi_cmd_is(cmd)	(ESPI_CMD(espi_cmd) == (cmd))
#define espi_rsp_is(rsp)	(ESPI_CMD(espi_rsp) == (rsp))
#define espi_cmd_is_get(addr)				\
	(espi_cmd_is(ESPI_CMD_GET_CONFIGURATION) &&	\
	 espi_addr == (addr))
#define espi_cmd_is_set(addr)				\
	(espi_cmd_is(ESPI_CMD_SET_CONFIGURATION) &&	\
	 espi_addr == (addr))
#define espi_cmd_is_assert(vwire)			\
	(espi_cmd_is(ESPI_CMD_PUT_VWIRE) &&		\
	 espi_vwire_is_asserting(vwire))
#define espi_cmd_is_deassert(vwire)			\
	(espi_cmd_is(ESPI_CMD_PUT_VWIRE) &&		\
	 espi_vwire_is_deasserting(vwire))

#define espi_op_success()		espi_op_complete(true)
#define espi_op_failure()		espi_op_complete(false)

#define espi_auto_G3_exit()		espi_start_op(ESPI_OP_PROBE, NULL)
#define espi_auto_reset()		espi_start_op(ESPI_OP_RESET, NULL)

#define espi_get_gen_cfg()		espi_get_config(ESPI_SLAVE_GEN_CFG)
#define espi_get_peri_cfg()		espi_get_config(ESPI_SLAVE_PERI_CFG)
#define espi_get_vwire_cfg()		espi_get_config(ESPI_SLAVE_VWIRE_CFG)
#define espi_get_oob_cfg()		espi_get_config(ESPI_SLAVE_OOB_CFG)
#define espi_get_flash_cfg()		espi_get_config(ESPI_SLAVE_FLASH_CFG)
#define espi_set_gen_cfg()		espi_set_config(ESPI_SLAVE_GEN_CFG)
#define espi_set_peri_cfg()		espi_set_config(ESPI_SLAVE_PERI_CFG)
#define espi_set_vwire_cfg()		espi_set_config(ESPI_SLAVE_VWIRE_CFG)
#define espi_set_oob_cfg()		espi_set_config(ESPI_SLAVE_OOB_CFG)
#define espi_set_flash_cfg()		espi_set_config(ESPI_SLAVE_FLASH_CFG)

#define espi_assert_vwire(vwire)				\
	do {							\
		if (espi_sys_event_is_active_low(vwire))	\
			espi_put_vwire(vwire, false);		\
		else						\
			espi_put_vwire(vwire, true);		\
	} while (0)
#define espi_deassert_vwire(vwire)				\
	do {							\
		if (espi_sys_event_is_active_low(vwire))	\
			espi_put_vwire(vwire, true);		\
		else						\
			espi_put_vwire(vwire, false);		\
	} while (0)

#include <driver/espi.h>

#define lpc_hw_io_read8(a)		espi_hw_io_read8(a)
#define lpc_hw_io_write8(v, a)		espi_hw_io_write8(v, a)
#define lpc_hw_mem_read8(a)		espi_hw_mem_read8(a)
#define lpc_hw_mem_write8(v, a)		espi_hw_mem_write8(v, a)
#define lpc_hw_mem_read16(a)		espi_hw_mem_read16(a)
#define lpc_hw_mem_write16(v, a)	espi_hw_mem_write16(v, a)
#define lpc_hw_mem_read32(a)		espi_hw_mem_read32(a)
#define lpc_hw_mem_write32(v, a)	espi_hw_mem_write32(v, a)

#define lpc_hw_irq_clear(irq)		espi_hw_vwire_clear(irq)
#define lpc_hw_irq_status(irq)		espi_hw_vwire_status(irq)
#define lpc_hw_irq_mask(irq)		espi_hw_vwire_mask(irq)
#define lpc_hw_irq_unmask(irq)		espi_hw_vwire_unmask(irq)

extern uint16_t espi_cmd;
extern uint16_t espi_rsp;
extern uint8_t espi_op;
extern uint16_t espi_addr;

int espi_start_op(espi_op_t op, espi_cmpl_cb cb);
void espi_cmd_complete(uint8_t rsp);
void espi_op_complete(bool result);
uint8_t espi_state_get(void);
void espi_state_set(uint8_t state);
void espi_enter_state(uint8_t state);
void espi_raise_event(espi_event_t event);
espi_event_t espi_event_save(void);
void espi_event_restore(espi_event_t event);
void espi_sync(void);
void espi_seq_handler(void);

void espi_inband_reset(void);
void espi_get_configuration(uint16_t address);
void espi_set_configuration(uint16_t address, uint32_t config);
void espi_put_message(uint8_t r, uint8_t code, uint32_t msg,
		      uint16_t len, uint8_t *buf);
void espi_put_ltr(bool rq, uint8_t ls, uint16_t lv);
//void espi_get_vwire(void);
void espi_put_vwire(uint16_t vwire, bool state);
void espi_put_vwires(uint8_t count, uint16_t *vwire, bool *state);
void espi_put_oob(uint16_t len, uint8_t *buf);
void espi_put_flash(bool s, uint8_t p, uint32_t len, uint8_t *buf);
void espi_get_oob(void);
void espi_get_flash(void);

void espi_write_cmd_async(uint8_t opcode,
			  uint8_t hlen, uint8_t *hbuf,
			  uint8_t dlen, uint8_t *dbuf);
uint8_t espi_read_rsp(uint8_t opcode,
		      uint8_t hlen, uint8_t *hbuf,
		      uint8_t dlen, uint8_t *dbuf);
int espi_write_cmd(uint8_t opcode,
		   uint8_t hlen, uint8_t *hbuf,
		   uint8_t dlen, uint8_t *dbuf);
void espi_get_config(uint16_t address);
void espi_set_config(uint16_t address);
void espi_set_sys_event(uint16_t event);
void espi_clear_sys_event(uint16_t event);
bool espi_sys_event_is_set(uint16_t event);
bool espi_sys_event_is_active_low(uint16_t event);
bool espi_vwire_is_asserting(uint16_t vwire);
bool espi_vwire_is_deasserting(uint16_t vwire);

#endif /* __ESPI_H_INCLUDE__ */
