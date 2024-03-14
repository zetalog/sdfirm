#ifndef __MBOX_K1MATRIX_H_INCLUDE__
#define __MBOX_K1MATRIX_H_INCLUDE__

#define MBOX_REG(offset)		(MBOX_BASE + (offset))
#define MBOX_1BIT_REG(offset, n)	REG_1BIT_ADDR(MBOX_REG(offset), n)
#define MBOX_2BIT_REG(offset, n)	REG_2BIT_ADDR(MBOX_REG(offset), n)
#define MBOX_8BIT_REG(offset, n)	REG_8BIT_ADDR(MBOX_REG(offset), n)

#define MBOX_REVISION			MBOX_REG(0x000)
#define MBOX_SYSCONFIG(m)		MBOX_1BIT_REG(0x010, m)
#define MBOX_MESSAGE(m)			MBOX_REG(0x040 + ((m) << 2))
#define MBOX_FIFOSTATUS(m)		MBOX_REG(0x050 + ((m) << 2))
#define MBOX_MSGSTATUS(m)		MBOX_REG(0x080 + ((m) << 2))
#define MBOX_IRQSTATUS_RAW(m)		MBOX_2BIT_REG(0x100, m)
#define MBOX_IRQSTATUS_CLR(m)		MBOX_2BIT_REG(0x104, m)
#define MBOX_IRQENABLE_SET(m)		MBOX_2BIT_REG(0x108, m)
#define MBOX_IRQENABLE_CLR(m)		MBOX_2BIT_REG(0x10C, m)
#define MBOX_IRQTHR(m)			MBOX_8BIT_REG(0x180, m)

/* MBOX_FIFOSTATUS */
#define mbox_fifo_empty			_BV(29)
#define mbox_fifo_full			_BV(28)
#define mbox_fifo_is_empty		_BV(1)
#define mbox_fifo_is_full		_BV(0)

/* MBOX_MSGSTATUS */
#define mbox_msg_read_empty		_BV(29)
#define mbox_msg_write_full		_BV(28)
#define mbox_msg_num_OFFSET		0
#define mbox_msg_num_MASK		REG_4BIT_MASK
#define mbox_msg_num(value)		_GET_FV(mbox_msg_num, value)

#define mbox_fifo_clear(m)						\
	do {								\
		__raw_setl(_BV(m), MBOX_SYSCONFIG(m));			\
		while (__raw_readl(MBOX_SYSCONFIG(m)) & _BV(m));	\
	} while (0)
#define mbox_fifo_ack_empty(m)						\
	do {								\
		if (__raw_readl(MBOX_FIFOSTATUS(m)) &			\
		    mbox_fifo_empty) {					\
			__raw_setl(mbox_fifo_empty,			\
				   MBOX_FIFOSTATUS(m));			\
		}							\
	} while (0)
#define mbox_fifo_ack_full(m)						\
	do {								\
		if (__raw_readl(MBOX_FIFOSTATUS(m)) &			\
		    mbox_fifo_full) {					\
			__raw_setl(mbox_fifo_full,			\
				   MBOX_FIFOSTATUS(m));			\
		}							\
	} while (0)

#endif /* __MBOX_K1MATRIX_H_INCLUDE__ */
