#ifndef __UART_H_INCLUDE__
#define __UART_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/bulk.h>

struct uart_port {
	void (*startup)(void);
	void (*cleanup)(void);
	void (*config)(uint8_t params, uint32_t baudrate);
};
__TEXT_TYPE__(const struct uart_port, uart_port_t);

typedef uint8_t uart_pid_t;

#ifdef CONFIG_UART_115200
#define UART_BAUDRATE		115200
#endif
#ifdef CONFIG_UART_57600
#define UART_BAUDRATE		57600
#endif
#ifdef CONFIG_UART_38400
#define UART_BAUDRATE		38400
#endif
#ifdef CONFIG_UART_19200
#define UART_BAUDRATE		19200
#endif
#ifdef CONFIG_UART_9600
#define UART_BAUDRATE		9600
#endif

#ifndef UART_BAUDRATE
#define UART_BAUDRATE		115200
#endif

#define UART_BITS_MASK		0x0F
#define UART_PARITY_MASK	0x30
#define UART_STOPB_MASK		0x40
#define uart_bits(p)		(p&UART_BITS_MASK)
#define uart_parity(p)		(p&UART_PARITY_MASK)
#define uart_stopb(p)		(p&UART_STOPB_MASK)

#define UART_PARITY_NONE	0x00
#define UART_PARITY_ODD		0x10
#define UART_PARITY_EVEN	0x20

#define UART_STOPB_ONE		0x00
#define UART_STOPB_TWO		0x40

#include <driver/uart.h>

/* always use this to communicate with PC: bits = 8, parity = N, stopb = 1 */
#define UART_PARAMS		(8 | UART_PARITY_NONE | UART_STOPB_ONE)

#define INVALID_UART_VALUE	-1

#ifdef CONFIG_UART
#ifdef CONFIG_UART_METERING
/* frequency measured by the oscilloscope will be half of the baudrate */
#define uart_putchar(byte)	uart_hw_sync_write(0x55)
#else
#define uart_putchar(byte)	uart_hw_sync_write(byte)
#endif
#define uart_getchar()		uart_hw_sync_read()
#else
#define uart_putchar(byte)
#define uart_getchar()		(0)
#endif

#ifdef CONFIG_UART_MAX_PORT
#define NR_UART_PORTS		CONFIG_UART_MAX_PORT
#else
#define NR_UART_PORTS		1
#endif

struct uart_state {
	bulk_cid_t bulk_tx;
	bulk_cid_t bulk_rx;
};

extern uart_pid_t uart_pid;

/* Asynchronous UART */
int uart_put_char(uint8_t c);
int uart_write(const uint8_t *buf, int count);
void uart_config_port(uint8_t params, uint32_t baudrate);

void uart_port_restore(uart_pid_t pid);
uart_pid_t uart_port_save(uart_pid_t pid);
#define uart_port_select(pid)	uart_port_restore(pid)

uart_pid_t uart_startup(bulk_user_t *txusr, uint8_t *txbuf, int txlen,
			bulk_user_t *rxusr, uint8_t *rxbuf, int rxlen);
void uart_cleanup(uart_pid_t pid);
bulk_cid_t uart_bulk_tx(void);
bulk_cid_t uart_bulk_rx(void);

uart_pid_t uart_register_port(uart_port_t *port);

#endif /* __UART_H_INCLUDE__ */
