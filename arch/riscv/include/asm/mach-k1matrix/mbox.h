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

#endif /* __MBOX_K1MATRIX_H_INCLUDE__ */
