#ifndef __IO_H_INCLUDE__
#define __IO_H_INCLUDE__

#include <target/types.h>
#include <target/jiffies.h>
#include <target/delay.h>

#define O_RDONLY	0x01
#define O_WRONLY	0x02
#define O_RDWR		0x03
#define O_DIRECT	0x08

/* Following flags are not used for the IO user, but for the IO channels.
 */
#define O_BUFFER	0x00
#define O_CMPL		0x80
#define O_AVAL		0x00
#define O_WRCMPL	(O_CMPL | O_WRONLY)
#define O_WRAVAL	(O_AVAL | O_WRONLY)
#define O_REAP		0x40
#define O_RDREAP	(O_AVAL | O_RDONLY | O_REAP)
#define O_RDAVAL	(O_AVAL | O_RDONLY)

typedef uint8_t (*iord_cb)(void);
typedef void (*iowr_cb)(uint8_t);
typedef void (*io_cb)(void);
typedef boolean (*iotest_cb)(void);
typedef void (*iordwr_cb)(uint8_t *c);
typedef boolean (*iobyte_cb)(uint8_t *c);

#define __raw_read_poll(op, a, v, cond, delay_us, timeout_ms)	\
({								\
	tick_t timeout = tick_get_counter() + timeout_ms;	\
	while (1) {						\
		v = __raw_read##op(a);				\
		if (cond)					\
			break;					\
		if (timeout_ms &&				\
		    time_before(timeout, tick_get_counter())) {	\
			v = __raw_read##op(a);			\
			break;					\
		}						\
		if (delay_us)					\
			udelay(delay_us);			\
	}							\
	(cond) ? true : false;					\
})

#define __raw_readw_le(a)	le16_to_cpu(__raw_readw(a))
#define __raw_writew_le(v, a)	__raw_writew(cpu_to_le16(v), a)
#define __raw_setw_le(v, a)					\
	do {							\
		uint16_t __v = __raw_readw_le(a);		\
		__v |= (v);					\
		__raw_writew_le(__v, (a));			\
	} while (0)
#define __raw_clearw_le(v, a)					\
	do {							\
		uint16_t __v = __raw_readw_le(a);		\
		__v &= ~(v);					\
		__raw_writew_le(__v, (a));			\
	} while (0)
#define __raw_readl_le(a)	le32_to_cpu(__raw_readl(a))
#define __raw_writel_le(v, a)	__raw_writel(cpu_to_le32(v), a)
#define __raw_setl_le(v, a)					\
	do {							\
		uint32_t __v = __raw_readl_le(a);		\
		__v |= (v);					\
		__raw_writel_le(__v, (a));			\
	} while (0)
#define __raw_clearl_le(v, a)					\
	do {							\
		uint32_t __v = __raw_readl_le(a);		\
		__v &= ~(v);					\
		__raw_writel_le(__v, (a));			\
	} while (0)
#define __raw_readq_le(a)	le64_to_cpu(__raw_readl(a))
#define __raw_writeq_le(v, a)	__raw_writel(cpu_to_le64(v), a)
#define __raw_setq_le(v, a)					\
	do {							\
		uint64_t __v = __raw_readq_le(a);		\
		__v |= (v);					\
		__raw_writeq_le(__v, (a));			\
	} while (0)
#define __raw_clearq_le(v, a)					\
	do {							\
		uint64_t __v = __raw_readq_le(a);		\
		__v &= ~(v);					\
		__raw_writeq_le(__v, (a));			\
	} while (0)

#define __raw_readw_be(a)	be16_to_cpu(__raw_readw(a))
#define __raw_writew_be(v, a)	__raw_writew(cpu_to_be16(v), a)
#define __raw_setw_be(v, a)					\
	do {							\
		uint16_t __v = __raw_readw_be(a);		\
		__v |= (v);					\
		__raw_writew_be(__v, (a));			\
	} while (0)
#define __raw_clearw_be(v, a)					\
	do {							\
		uint16_t __v = __raw_readw_be(a);		\
		__v &= ~(v);					\
		__raw_writew_be(__v, (a));			\
	} while (0)
#define __raw_readl_be(a)	be32_to_cpu(__raw_readl(a))
#define __raw_writel_be(v, a)	__raw_writel(cpu_to_be32(v), a)
#define __raw_setl_be(v, a)					\
	do {							\
		uint32_t __v = __raw_readl_be(a);		\
		__v |= (v);					\
		__raw_writel_be(__v, (a));			\
	} while (0)
#define __raw_clearl_be(v, a)					\
	do {							\
		uint32_t __v = __raw_readl_be(a);		\
		__v &= ~(v);					\
		__raw_writel_be(__v, (a));			\
	} while (0)
#define __raw_readq_be(a)	be64_to_cpu(__raw_readl(a))
#define __raw_writeq_be(v, a)	__raw_writel(cpu_to_be64(v), a)
#define __raw_setq_be(v, a)					\
	do {							\
		uint64_t __v = __raw_readq_be(a);		\
		__v |= (v);					\
		__raw_writeq_be(__v, (a));			\
	} while (0)
#define __raw_clearq_be(v, a)					\
	do {							\
		uint64_t __v = __raw_readq_be(a);		\
		__v &= ~(v);					\
		__raw_writeq_be(__v, (a));			\
	} while (0)

#endif /* __IO_H_INCLUDE__ */
