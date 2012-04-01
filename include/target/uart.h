#ifndef __UART_H_INCLUDE__
#define __UART_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/bulk.h>

#define UART_BITS_MASK		0x0F
#define UART_PARITY_MASK	0x30
#define UART_STOPB_MASK		0x40
#define uart_bits(p)		((p)&UART_BITS_MASK)
#define uart_parity(p)		((p)&UART_PARITY_MASK)
#define uart_stopb(p)		((p)&UART_STOPB_MASK)

#define UART_PARITY_NONE	0x00
#define UART_PARITY_ODD		0x10
#define UART_PARITY_EVEN	0x20

#define UART_STOPB_ONE		0x00
#define UART_STOPB_TWO		0x40

#ifdef CONFIG_UART_MAX_PORTS
#define NR_UART_PORTS		CONFIG_UART_MAX_PORTS
#else
#define NR_UART_PORTS		1
#endif

typedef uint8_t uart_pid_t;

#include <driver/uart.h>

/* always use this to communicate with PC: bits = 8, parity = N, stopb = 1 */
#define UART_DEF_PARAMS		(8 | UART_PARITY_NONE | UART_STOPB_ONE)
#ifdef CONFIG_UART_115200
#define UART_DEF_BAUDRATE	115200
#endif
#ifdef CONFIG_UART_57600
#define UART_DEF_BAUDRATE	57600
#endif
#ifdef CONFIG_UART_38400
#define UART_DEF_BAUDRATE	38400
#endif
#ifdef CONFIG_UART_19200
#define UART_DEF_BAUDRATE	19200
#endif
#ifdef CONFIG_UART_9600
#define UART_DEF_BAUDRATE	9600
#endif
#ifdef CONFIG_UART_4800
#define UART_DEF_BAUDRATE	4800
#endif
#ifdef CONFIG_UART_2400
#define UART_DEF_BAUDRATE	2400
#endif
#ifndef UART_DEF_BAUDRATE
#define UART_DEF_BAUDRATE	115200
#endif

typedef boolean (*uart_sync_cb)(uint8_t *);

struct uart_user {
	uint8_t params;
	uint32_t baudrate;
	uint8_t *txbuf;
	uint8_t *rxbuf;
	bulk_size_t txlen;
	bulk_size_t rxlen;
	bulk_user_t *txusr;
	bulk_user_t *rxusr;
	uart_sync_cb sync_func;
	uint8_t *sync_buf;
	bulk_size_t sync_size;
};
__TEXT_TYPE__(const struct uart_user, uart_user_t);

struct uart_state {
	bulk_cid_t bulk_tx;
	bulk_cid_t bulk_rx;
	uint8_t *sync_buf;
	uart_sync_cb sync_func;
	bulk_size_t sync_size;
	bulk_size_t sync_len;
};

extern struct uart_state uart_states[NR_UART_PORTS];
extern uart_pid_t uart_pid;

#define uart_bulk_tx(pid)	(uart_states[pid].bulk_tx)
#define uart_bulk_rx(pid)	(uart_states[pid].bulk_rx)

/* Asynchronous UART */
void uart_startup(uart_pid_t pid, uart_user_t *user);
void uart_cleanup(uart_pid_t pid);
uart_pid_t uart_register_port(uart_port_t *port);
void uart_select_port(uart_pid_t pid);

void uart_async_read(uint8_t *byte);
void uart_async_start(void);
void uart_async_stop(void);
boolean uart_async_poll(void);
void uart_async_halt(void);
void uart_async_select(void);

void uart_read_submit(uart_pid_t pid, bulk_size_t size);
void uart_read_byte(uart_pid_t pid);
void uart_write_byte(uart_pid_t pid);

#endif /* __UART_H_INCLUDE__ */
