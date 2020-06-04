/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <target/console.h>
#ifdef CONFIG_NS87308
#include <ns87308.h>
#endif

/*
 * Divide positive or negative dividend by positive divisor and round
 * to closest integer. Result is undefined for negative divisors and
 * for negative dividends if the divisor variable type is unsigned.
 */
#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(x) __x = x;				\
	typeof(divisor) __d = divisor;			\
	(((typeof(x))-1) > 0 ||				\
	 ((typeof(divisor))-1) > 0 || (__x) > 0) ?	\
		(((__x) + ((__d) / 2)) / (__d)) :	\
		(((__x) - ((__d) / 2)) / (__d));	\
}							\
)

#if !defined(CONFIG_CONS_INDEX)
#elif (CONFIG_CONS_INDEX < 1) || (CONFIG_CONS_INDEX > 6)
#error	"Invalid console index value."
#endif

#if CONFIG_CONS_INDEX == 1 && !defined(CONFIG_SYS_NS16550_COM1)
#error	"Console port 1 defined but not configured."
#elif CONFIG_CONS_INDEX == 2 && !defined(CONFIG_SYS_NS16550_COM2)
#error	"Console port 2 defined but not configured."
#elif CONFIG_CONS_INDEX == 3 && !defined(CONFIG_SYS_NS16550_COM3)
#error	"Console port 3 defined but not configured."
#elif CONFIG_CONS_INDEX == 4 && !defined(CONFIG_SYS_NS16550_COM4)
#error	"Console port 4 defined but not configured."
#elif CONFIG_CONS_INDEX == 5 && !defined(CONFIG_SYS_NS16550_COM5)
#error	"Console port 5 defined but not configured."
#elif CONFIG_CONS_INDEX == 6 && !defined(CONFIG_SYS_NS16550_COM6)
#error	"Console port 6 defined but not configured."
#endif

/* Note: The port number specified in the functions is 1 based.
 *	 the array is 0 based.
 */
static NS16550_t serial_ports[6] = {
#ifdef CONFIG_SYS_NS16550_COM1
	(NS16550_t)CONFIG_SYS_NS16550_COM1,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM2
	(NS16550_t)CONFIG_SYS_NS16550_COM2,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM3
	(NS16550_t)CONFIG_SYS_NS16550_COM3,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM4
	(NS16550_t)CONFIG_SYS_NS16550_COM4,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM5
	(NS16550_t)CONFIG_SYS_NS16550_COM5,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM6
	(NS16550_t)CONFIG_SYS_NS16550_COM6
#else
	NULL
#endif
};

#define PORT	serial_ports[port-1]

/* Multi serial device functions */
#define DECLARE_ESERIAL_FUNCTIONS(port) \
	static int  eserial##port##_init(void) \
	{ \
		int clock_divisor; \
		clock_divisor = ns16550_calc_divisor(serial_ports[port-1], \
				CONFIG_SYS_NS16550_CLK, UART_CON_BAUDRATE); \
		NS16550_init(serial_ports[port-1], clock_divisor); \
		return 0 ; \
	} \
	static void eserial##port##_setbrg(void) \
	{ \
		serial_setbrg_dev(port); \
	} \
	static int  eserial##port##_getc(void) \
	{ \
		return serial_getc_dev(port); \
	} \
	static int  eserial##port##_tstc(void) \
	{ \
		return serial_tstc_dev(port); \
	} \
	static void eserial##port##_putc(const char c) \
	{ \
		serial_putc_dev(port, c); \
	} \
	static void eserial##port##_puts(const char *s) \
	{ \
		serial_puts_dev(port, s); \
	}

/* Serial device descriptor */
#define INIT_ESERIAL_STRUCTURE(port, __name) {	\
	.name	= __name,			\
	.start	= eserial##port##_init,		\
	.stop	= NULL,				\
	.setbrg	= eserial##port##_setbrg,	\
	.getc	= eserial##port##_getc,		\
	.tstc	= eserial##port##_tstc,		\
	.putc	= eserial##port##_putc,		\
	.puts	= eserial##port##_puts,		\
}

static void _serial_putc(const char c, const int port)
{
	NS16550_putc(PORT, c);
}

static void _serial_puts(const char *s, const int port)
{
	while (*s) {
		_serial_putc(*s++, port);
	}
}

static int _serial_getc(const int port)
{
	return NS16550_getc(PORT);
}

static int _serial_tstc(const int port)
{
	return NS16550_tstc(PORT);
}

static void _serial_setbrg(const int port)
{
	int clock_divisor;

	clock_divisor = ns16550_calc_divisor(PORT, CONFIG_SYS_NS16550_CLK,
					     UART_CON_BAUDRATE);
	NS16550_reinit(PORT, clock_divisor);
}

static inline void
serial_putc_dev(unsigned int dev_index,const char c)
{
	_serial_putc(c,dev_index);
}

static inline void
serial_puts_dev(unsigned int dev_index,const char *s)
{
	_serial_puts(s,dev_index);
}

static inline int
serial_getc_dev(unsigned int dev_index)
{
	return _serial_getc(dev_index);
}

static inline int
serial_tstc_dev(unsigned int dev_index)
{
	return _serial_tstc(dev_index);
}

static inline void
serial_setbrg_dev(unsigned int dev_index)
{
	_serial_setbrg(dev_index);
}

#if defined(CONFIG_SYS_NS16550_COM1)
DECLARE_ESERIAL_FUNCTIONS(1);
struct serial_device eserial1_device =
	INIT_ESERIAL_STRUCTURE(1, "eserial0");
#endif
#if defined(CONFIG_SYS_NS16550_COM2)
DECLARE_ESERIAL_FUNCTIONS(2);
struct serial_device eserial2_device =
	INIT_ESERIAL_STRUCTURE(2, "eserial1");
#endif
#if defined(CONFIG_SYS_NS16550_COM3)
DECLARE_ESERIAL_FUNCTIONS(3);
struct serial_device eserial3_device =
	INIT_ESERIAL_STRUCTURE(3, "eserial2");
#endif
#if defined(CONFIG_SYS_NS16550_COM4)
DECLARE_ESERIAL_FUNCTIONS(4);
struct serial_device eserial4_device =
	INIT_ESERIAL_STRUCTURE(4, "eserial3");
#endif
#if defined(CONFIG_SYS_NS16550_COM5)
DECLARE_ESERIAL_FUNCTIONS(5);
struct serial_device eserial5_device =
	INIT_ESERIAL_STRUCTURE(5, "eserial4");
#endif
#if defined(CONFIG_SYS_NS16550_COM6)
DECLARE_ESERIAL_FUNCTIONS(6);
struct serial_device eserial6_device =
	INIT_ESERIAL_STRUCTURE(6, "eserial5");
#endif

struct serial_device *default_serial_console(void)
{
#if CONFIG_CONS_INDEX == 1
	return &eserial1_device;
#elif CONFIG_CONS_INDEX == 2
	return &eserial2_device;
#elif CONFIG_CONS_INDEX == 3
	return &eserial3_device;
#elif CONFIG_CONS_INDEX == 4
	return &eserial4_device;
#elif CONFIG_CONS_INDEX == 5
	return &eserial5_device;
#elif CONFIG_CONS_INDEX == 6
	return &eserial6_device;
#else
#error "Bad CONFIG_CONS_INDEX."
#endif
}

#define UART_LCRVAL UART_LCR_8N1		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL (UART_MCR_DTR | \
		     UART_MCR_RTS)		/* RTS/DTR */

#if defined(CONFIG_SYS_NS16550_MEM32) && (CONFIG_SYS_NS16550_REG_SIZE < 0)
#define serial_out(x, y)	__raw_writel(x, y)
#define serial_in(y)		__raw_readl(y)
#else
#define serial_out(x, y)	__raw_writeb(x, (uintptr_t)y)
#define serial_in(y)		__raw_readb((uintptr_t)y)
#endif

#ifndef CONFIG_SYS_NS16550_IER
#define CONFIG_SYS_NS16550_IER  0x00
#endif /* CONFIG_SYS_NS16550_IER */

static uint32_t ns16550_getfcr(NS16550_t port)
{
	return UART_FCR_DEFVAL;
}

int ns16550_calc_divisor(NS16550_t port, int clock, int baudrate)
{
	const unsigned int mode_x_div = 16;

	return DIV_ROUND_CLOSEST(clock, mode_x_div * baudrate);
}

static void NS16550_setbrg(NS16550_t com_port, int baud_divisor)
{
	serial_out(UART_LCR_BKSE | UART_LCRVAL, &com_port->lcr);
	serial_out(baud_divisor & 0xff, &com_port->dll);
	serial_out((baud_divisor >> 8) & 0xff, &com_port->dlm);
	serial_out(UART_LCRVAL, &com_port->lcr);
}

void NS16550_init(NS16550_t com_port, int baud_divisor)
{
	while (!(serial_in(&com_port->lsr) & UART_LSR_TEMT))
		;

	serial_out(CONFIG_SYS_NS16550_IER, &com_port->ier);
	serial_out(UART_MCRVAL, &com_port->mcr);
	serial_out(ns16550_getfcr(com_port), &com_port->fcr);
	if (baud_divisor != -1)
		NS16550_setbrg(com_port, baud_divisor);
}

void NS16550_reinit(NS16550_t com_port, int baud_divisor)
{
	serial_out(CONFIG_SYS_NS16550_IER, &com_port->ier);
	NS16550_setbrg(com_port, 0);
	serial_out(UART_MCRVAL, &com_port->mcr);
	serial_out(ns16550_getfcr(com_port), &com_port->fcr);
	NS16550_setbrg(com_port, baud_divisor);
}

void NS16550_putc(NS16550_t com_port, char c)
{
	while ((serial_in(&com_port->lsr) & UART_LSR_THRE) == 0)
		;
	serial_out(c, &com_port->thr);
}

char NS16550_getc(NS16550_t com_port)
{
	while ((serial_in(&com_port->lsr) & UART_LSR_DR) == 0) {
	}
	return serial_in(&com_port->rbr);
}

int NS16550_tstc(NS16550_t com_port)
{
	return (serial_in(&com_port->lsr) & UART_LSR_DR) != 0;
}

void ns16550_con_init(void)
{
	default_serial_console()->start();
}

void ns16550_con_write(uint8_t byte)
{
	default_serial_console()->putc(byte);
}

uint8_t ns16550_con_read(void)
{
	return (uint8_t)default_serial_console()->getc();
}

bool ns16550_con_poll(void)
{
	return !!default_serial_console()->tstc();
}
