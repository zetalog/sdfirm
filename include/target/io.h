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

#endif /* __IO_H_INCLUDE__ */
